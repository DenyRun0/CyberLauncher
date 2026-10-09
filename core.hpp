#pragma once
// Логіка лаунчера: список версій, завантаження файлів, запуск Java.
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <fcntl.h>
#include <mach-o/dyld.h>
#include <sys/wait.h>
#include <unistd.h>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <map>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace core {

namespace fs = std::filesystem;
using json = nlohmann::json;

inline std::atomic<bool> g_cancel{false};

// Папка програми: <проєкт>/build/launcher -> <проєкт>
inline fs::path projectDir() {
    uint32_t size = 0;
    _NSGetExecutablePath(nullptr, &size);
    std::string buf(size, '\0');
    if (_NSGetExecutablePath(buf.data(), &size) != 0) return fs::current_path();
    return fs::canonical(fs::path(buf.c_str())).parent_path().parent_path();
}

// Усі дані гри (версії, бібліотеки, ресурси, світи) лежать у <проєкт>/game
inline fs::path rootDir() { return projectDir() / "game"; }

struct VersionInfo {
    std::string id, type, url;
};

// Стан, який оновлює робочий потік і читає вікно
struct Status {
    std::atomic<float> progress{0.f};
    std::atomic<bool> busy{false};
    std::atomic<bool> gameRunning{false};
    std::mutex m;
    std::string text;
    std::vector<std::string> lines;

    void setText(const std::string& s) {
        std::lock_guard<std::mutex> l(m);
        text = s;
    }
    void log(const std::string& s) {
        std::lock_guard<std::mutex> l(m);
        text = s;
        lines.push_back(s);
    }
};

// ---------- HTTP ----------

struct CurlHandle {
    CURL* h;
    CurlHandle() : h(curl_easy_init()) {}
    ~CurlHandle() { if (h) curl_easy_cleanup(h); }
};
inline CURL* curlHandle() {
    thread_local CurlHandle c;  // свій handle для кожного потоку
    return c.h;
}

inline size_t writeStr(char* p, size_t s, size_t n, void* ud) {
    static_cast<std::string*>(ud)->append(p, s * n);
    return s * n;
}
inline size_t writeFile(char* p, size_t s, size_t n, void* ud) {
    static_cast<std::ofstream*>(ud)->write(p, s * n);
    return s * n;
}

inline void checkCancel() {
    if (g_cancel) throw std::runtime_error("Скасовано");
}

inline void setCommon(CURL* c, const std::string& url) {
    curl_easy_reset(c);
    curl_easy_setopt(c, CURLOPT_URL, url.c_str());
    curl_easy_setopt(c, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(c, CURLOPT_FAILONERROR, 1L);
    curl_easy_setopt(c, CURLOPT_CONNECTTIMEOUT, 15L);
    curl_easy_setopt(c, CURLOPT_LOW_SPEED_LIMIT, 1L);
    curl_easy_setopt(c, CURLOPT_LOW_SPEED_TIME, 30L);
}

inline std::string httpGet(const std::string& url) {
    CURL* c = curlHandle();
    std::string body;
    setCommon(c, url);
    curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, writeStr);
    curl_easy_setopt(c, CURLOPT_WRITEDATA, &body);
    if (curl_easy_perform(c) != CURLE_OK)
        throw std::runtime_error("Помилка мережі: " + url);
    return body;
}

inline bool downloadFile(const std::string& url, const fs::path& dest,
                         std::uintmax_t expectedSize = 0) {
    if (fs::exists(dest) && (expectedSize == 0 || fs::file_size(dest) == expectedSize))
        return true;
    fs::create_directories(dest.parent_path());
    for (int attempt = 0; attempt < 3; ++attempt) {
        checkCancel();
        CURLcode r;
        {
            std::ofstream out(dest, std::ios::binary);
            CURL* c = curlHandle();
            setCommon(c, url);
            curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, writeFile);
            curl_easy_setopt(c, CURLOPT_WRITEDATA, &out);
            r = curl_easy_perform(c);
        }
        if (r == CURLE_OK) return true;
        fs::remove(dest);
    }
    return false;
}

// ---------- Версії ----------

inline const char* kManifestUrl =
    "https://piston-meta.mojang.com/mc/game/version_manifest_v2.json";

// Лише версії 1.19 і новіші: у них простий формат бібліотек
inline std::vector<VersionInfo> fetchVersions() {
    fs::path cache = rootDir() / "version_manifest.json";
    json manifest;
    try {
        std::string body = httpGet(kManifestUrl);
        manifest = json::parse(body);
        fs::create_directories(cache.parent_path());
        std::ofstream(cache) << body;
    } catch (const std::exception&) {
        if (!fs::exists(cache)) throw;  // ні інтернету, ні кешу
        std::ifstream f(cache);
        manifest = json::parse(f);
    }
    std::vector<VersionInfo> out;
    for (auto& v : manifest["versions"]) {
        if (v["releaseTime"].get<std::string>() < "2022-06-07") continue;
        out.push_back({v["id"].get<std::string>(), v["type"].get<std::string>(),
                       v["url"].get<std::string>()});
    }
    return out;
}

// ---------- Правила та аргументи ----------

inline bool rulesAllow(const json& obj) {
    if (!obj.contains("rules")) return true;
    bool allowed = false;
    for (auto& r : obj["rules"]) {
        bool match = true;
        if (r.contains("features")) match = false;
        if (r.contains("os")) {
            auto& os = r["os"];
            if (os.contains("name") && os["name"] != "osx") match = false;
            if (os.contains("arch") || os.contains("version")) match = false;
        }
        if (match) allowed = (r["action"] == "allow");
    }
    return allowed;
}

inline void collectArgs(const json& arr, std::vector<std::string>& out) {
    for (auto& a : arr) {
        if (a.is_string()) { out.push_back(a.get<std::string>()); continue; }
        if (!rulesAllow(a)) continue;
        auto& v = a["value"];
        if (v.is_string()) out.push_back(v.get<std::string>());
        else for (auto& x : v) out.push_back(x.get<std::string>());
    }
}

inline std::string substitute(std::string s, const std::map<std::string, std::string>& vars) {
    for (auto& kv : vars) {
        std::string key = "${" + kv.first + "}";
        size_t pos;
        while ((pos = s.find(key)) != std::string::npos) s.replace(pos, key.size(), kv.second);
    }
    return s;
}

// ---------- Java ----------

inline std::string runCapture(const std::string& cmd) {
    std::string out;
    FILE* p = popen(cmd.c_str(), "r");
    if (!p) return out;
    char buf[256];
    while (fgets(buf, sizeof buf, p)) out += buf;
    pclose(p);
    while (!out.empty() && (out.back() == '\n' || out.back() == '\r')) out.pop_back();
    return out;
}

// Шукає встановлену Java потрібної версії
inline std::string findJava(int major) {
    for (std::string v : {std::to_string(major), std::to_string(major) + "+"}) {
        std::string home = runCapture("/usr/libexec/java_home -v " + v + " 2>/dev/null");
        if (!home.empty() && fs::exists(home + "/bin/java")) return home + "/bin/java";
    }
    return "";
}

// ---------- Встановлення і запуск ----------

inline void installAndRun(const VersionInfo& ver, const std::string& player, int ramGb, Status& st) {
    fs::path root = rootDir();
    fs::path vdir = root / "versions" / ver.id;
    fs::path vjson = vdir / (ver.id + ".json");

    st.progress = 0.f;
    st.log("Отримую опис версії " + ver.id + " ...");
    json vj;
    try {
        vj = json::parse(httpGet(ver.url));
        fs::create_directories(vdir);
        std::ofstream(vjson) << vj.dump(2);
    } catch (const std::exception&) {
        if (!fs::exists(vjson)) throw;
        std::ifstream f(vjson);
        vj = json::parse(f);
        st.log("Немає мережі: використовую збережений опис версії");
    }
    if (!vj.contains("arguments"))
        throw std::runtime_error("Ця версія не підтримується (потрібна 1.19+)");
    st.progress = 0.03f;
    checkCancel();

    // client.jar
    st.log("Завантажую клієнт ...");
    auto& cl = vj["downloads"]["client"];
    fs::path jar = vdir / (ver.id + ".jar");
    if (!downloadFile(cl["url"].get<std::string>(), jar, cl["size"].get<std::uintmax_t>()))
        throw std::runtime_error("Не вдалося завантажити client.jar");
    st.progress = 0.05f;

    // Бібліотеки
    std::vector<const json*> libs;
    for (auto& lib : vj["libraries"]) {
        if (!rulesAllow(lib)) continue;
        if (!lib.contains("downloads") || !lib["downloads"].contains("artifact")) continue;
        libs.push_back(&lib);
    }
    std::vector<std::string> cp;
    for (size_t k = 0; k < libs.size(); ++k) {
        checkCancel();
        auto& a = (*libs[k])["downloads"]["artifact"];
        fs::path p = root / "libraries" / a["path"].get<std::string>();
        if (!downloadFile(a["url"].get<std::string>(), p, a["size"].get<std::uintmax_t>()))
            throw std::runtime_error("Не вдалося завантажити " + a["path"].get<std::string>());
        cp.push_back(p.string());
        st.progress = 0.05f + 0.25f * float(k + 1) / float(libs.size());
        st.setText("Бібліотеки: " + std::to_string(k + 1) + "/" + std::to_string(libs.size()));
    }
    cp.push_back(jar.string());
    st.log("Бібліотек: " + std::to_string(libs.size()));

    // Assets
    auto& ai = vj["assetIndex"];
    std::string assetId = ai["id"].get<std::string>();
    fs::path assets = root / "assets";
    fs::path indexPath = assets / "indexes" / (assetId + ".json");
    if (!downloadFile(ai["url"].get<std::string>(), indexPath, ai["size"].get<std::uintmax_t>()))
        throw std::runtime_error("Не вдалося завантажити assetIndex");
    std::ifstream f(indexPath);
    json idx = json::parse(f);
    size_t total = idx["objects"].size(), i = 0;
    for (auto& el : idx["objects"].items()) {
        checkCancel();
        std::string hash = el.value()["hash"].get<std::string>();
        std::string pre = hash.substr(0, 2);
        if (!downloadFile("https://resources.download.minecraft.net/" + pre + "/" + hash,
                          assets / "objects" / pre / hash,
                          el.value()["size"].get<std::uintmax_t>()))
            throw std::runtime_error("Не вдалося завантажити ресурс " + hash);
        ++i;
        if (i % 25 == 0 || i == total) {
            st.progress = 0.3f + 0.7f * float(i) / float(total);
            st.setText("Ресурси: " + std::to_string(i) + "/" + std::to_string(total));
        }
    }
    st.log("Ресурси готові (" + std::to_string(total) + " файлів)");

    // Java
    int major = vj.contains("javaVersion") ? vj["javaVersion"]["majorVersion"].get<int>() : 17;
    std::string javaBin = findJava(major);
    if (javaBin.empty())
        throw std::runtime_error("Не знайдено Java " + std::to_string(major) +
                                 ". Встанови JDK " + std::to_string(major) + " з adoptium.net");
    st.log("Java " + std::to_string(major) + ": " + javaBin);

    // Аргументи
    fs::path natives = vdir / "natives";
    fs::path gameDir = root / "instance";
    fs::create_directories(natives);
    fs::create_directories(gameDir);

    std::string cpStr;
    for (size_t k = 0; k < cp.size(); ++k) cpStr += (k ? ":" : "") + cp[k];

    std::map<std::string, std::string> vars = {
        {"auth_player_name", player},
        {"version_name", ver.id},
        {"game_directory", gameDir.string()},
        {"assets_root", assets.string()},
        {"assets_index_name", assetId},
        {"auth_uuid", "00000000-0000-0000-0000-000000000000"},
        {"auth_access_token", "0"},
        {"clientid", "0"},
        {"auth_xuid", "0"},
        {"user_type", "legacy"},
        {"version_type", vj["type"].get<std::string>()},
        {"natives_directory", natives.string()},
        {"launcher_name", "MyLauncher"},
        {"launcher_version", "0.2"},
        {"classpath", cpStr},
    };

    std::vector<std::string> raw = {javaBin, "-Xmx" + std::to_string(ramGb) + "G"};
    collectArgs(vj["arguments"]["jvm"], raw);
    raw.push_back(vj["mainClass"].get<std::string>());
    collectArgs(vj["arguments"]["game"], raw);

    std::vector<std::string> args;
    for (auto& s : raw) args.push_back(substitute(s, vars));
    std::vector<char*> argvv;
    for (auto& s : args) argvv.push_back(s.data());
    argvv.push_back(nullptr);

    // Запуск: вивід гри йде у файл game.log
    std::string logPath = (root / "game.log").string();
    int logfd = open(logPath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    st.log("Запускаю Minecraft " + ver.id + " ...");
    pid_t pid = fork();
    if (pid < 0) throw std::runtime_error("Не вдалося запустити процес");
    if (pid == 0) {
        if (logfd >= 0) { dup2(logfd, 1); dup2(logfd, 2); }
        if (chdir(gameDir.c_str()) != 0) _exit(126);
        execv(javaBin.c_str(), argvv.data());
        _exit(127);
    }
    if (logfd >= 0) close(logfd);

    st.progress = 1.f;
    st.gameRunning = true;
    std::thread([pid, &st, logPath] {
        int status = 0;
        waitpid(pid, &status, 0);
        int code = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
        st.gameRunning = false;
        st.log(code == 0 ? "Гру закрито"
                         : "Гра завершилась з кодом " + std::to_string(code) +
                               ". Деталі: " + logPath);
    }).detach();
}

}  // namespace core
