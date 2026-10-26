#pragma once
// Логіка лаунчера: список версій, завантаження файлів, запуск Java.
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#endif
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#ifdef _WIN32
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif
#endif
#include <atomic>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <filesystem>
#include <functional>
#include <fstream>
#include <map>
#include <mutex>
#include <set>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace core {

namespace fs = std::filesystem;
using json = nlohmann::json;

inline std::atomic<bool> g_cancel{false};

// ---------- Платформа ----------

#if defined(_WIN32)
inline constexpr bool kIsWindows = true;
inline constexpr const char* kOsName = "windows";  // назва ОС у правилах Mojang
inline constexpr char kPathSep = ';';              // роздільник classpath
#elif defined(__APPLE__)
inline constexpr bool kIsWindows = false;
inline constexpr const char* kOsName = "osx";
inline constexpr char kPathSep = ':';
#else
inline constexpr bool kIsWindows = false;
inline constexpr const char* kOsName = "linux";
inline constexpr char kPathSep = ':';
#endif

#if defined(__aarch64__) || defined(_M_ARM64)
inline constexpr const char* kArch = "arm64";
#elif defined(__x86_64__) || defined(_M_X64)
inline constexpr const char* kArch = "x86_64";
#else
inline constexpr const char* kArch = "x86";
#endif

// Шлях у UTF-8 (на Windows path::string() дає локальну кодову сторінку, а Java чекає UTF-8)
inline std::string u8(const std::filesystem::path& p) {
    auto s = p.u8string();  // std::string у C++17, std::u8string у C++20
    return std::string(s.begin(), s.end());
}

// Папка, де лежить тека game.
//  1) Збірка на машині розробника: LAUNCHER_ROOT з CMake (діє, лише якщо ця тека існує тут).
//  2) Windows (zip з релізу): тека з exe, тож усе портативне.
//  3) macOS (.app): ~/Library/Application Support/CyberLauncher (поруч з .app писати не можна).
inline std::filesystem::path projectDir() {
    namespace sfs = std::filesystem;
#ifdef LAUNCHER_ROOT
    {
        std::error_code ec;
        sfs::path r(LAUNCHER_ROOT);
        if (sfs::exists(r / "CMakeLists.txt", ec)) return r;
    }
#endif
#if defined(_WIN32)
    std::wstring buf(32768, L'\0');
    DWORD n = GetModuleFileNameW(nullptr, buf.data(), static_cast<DWORD>(buf.size()));
    if (n == 0 || n >= buf.size()) return sfs::current_path();
    buf.resize(n);
    sfs::path dir = sfs::path(buf).parent_path();
    const std::string leaf = dir.filename().string();
    // build/Release/CyberLauncher.exe (Visual Studio) -> корінь проєкту
    if (leaf == "Release" || leaf == "Debug" || leaf == "RelWithDebInfo" || leaf == "MinSizeRel")
        return dir.parent_path().parent_path();
    if (leaf == "build") return dir.parent_path();
    return dir;  // розпакований реліз: все лежить поруч з exe
#elif defined(__APPLE__)
    uint32_t size = 0;
    _NSGetExecutablePath(nullptr, &size);
    std::string buf(size, '\0');
    if (_NSGetExecutablePath(buf.data(), &size) != 0) return sfs::current_path();
    sfs::path exe = sfs::canonical(sfs::path(buf.c_str()));
    if (exe.parent_path().filename() == "MacOS") {  // CyberLauncher.app/Contents/MacOS/CyberLauncher
        const char* home = std::getenv("HOME");
        if (home) return sfs::path(home) / "Library" / "Application Support" / "CyberLauncher";
    }
    return exe.parent_path().parent_path();  // build/launcher -> проєкт
#else
    std::error_code ec;
    auto exe = sfs::read_symlink("/proc/self/exe", ec);
    if (ec) return sfs::current_path();
    return exe.parent_path().parent_path();
#endif
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
    curl_easy_setopt(c, CURLOPT_USERAGENT, "CyberLauncher/1.0");
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
            if (os.contains("name") && os["name"] != kOsName) match = false;
            // "arch": "x86" означає 32-бітну систему, "arm64" - ARM
            if (os.contains("arch")) {
                const std::string a = os["arch"].get<std::string>();
                if (a != kArch) match = false;
            }
            if (os.contains("version")) match = false;
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

#ifdef __APPLE__
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
#endif

inline fs::path javaExeIn(const fs::path& home) {
    return home / "bin" / (kIsWindows ? "java.exe" : "java");
}

// Мажорна версія JDK за файлом "release" (JAVA_VERSION="17.0.9" або "1.8.0_392"). 0 - невідомо.
inline int javaMajorOfHome(const fs::path& home) {
    std::ifstream f(home / "release");
    std::string line;
    while (std::getline(f, line)) {
        if (line.rfind("JAVA_VERSION=", 0) != 0) continue;
        std::string v = line.substr(13);
        v.erase(std::remove(v.begin(), v.end(), '"'), v.end());
        v.erase(std::remove(v.begin(), v.end(), '\r'), v.end());
        int a = std::atoi(v.c_str());
        if (a == 1) {  // 1.8.0 -> 8
            size_t dot = v.find('.');
            if (dot != std::string::npos) return std::atoi(v.c_str() + dot + 1);
        }
        return a;
    }
    return 0;
}

// Рекурсивно шукає у root папки, що виглядають як JDK/JRE (є bin/java і файл release)
inline void scanJavaHomes(const fs::path& root, int depth, std::vector<fs::path>& out) {
    std::error_code ec;
    if (depth < 0 || !fs::is_directory(root, ec)) return;
    if (fs::exists(javaExeIn(root), ec) && fs::exists(root / "release", ec)) {
        out.push_back(root);
        return;
    }
    if (fs::exists(javaExeIn(root / "Contents" / "Home"), ec)) {  // macOS .jdk
        out.push_back(root / "Contents" / "Home");
        return;
    }
    for (const auto& e : fs::directory_iterator(root, ec))
        if (e.is_directory(ec)) scanJavaHomes(e.path(), depth - 1, out);
}

// Шукає встановлену Java потрібної версії
inline std::string findJava(int major) {
#ifdef __APPLE__
    for (std::string v : {std::to_string(major), std::to_string(major) + "+"}) {
        std::string home = runCapture("/usr/libexec/java_home -v " + v + " 2>/dev/null");
        if (!home.empty() && fs::exists(home + "/bin/java")) return home + "/bin/java";
    }
    return "";
#else
    std::vector<fs::path> homes;
    auto env = [](const char* name) -> fs::path {
        const char* v = std::getenv(name);
        return v ? fs::path(v) : fs::path();
    };
    std::error_code ec;
    if (fs::path jh = env("JAVA_HOME"); !jh.empty() && fs::exists(javaExeIn(jh), ec)) homes.push_back(jh);

    std::vector<fs::path> roots;
    if (kIsWindows) {
        for (const char* var : {"ProgramFiles", "ProgramW6432", "ProgramFiles(x86)"}) {
            fs::path pf = env(var);
            if (pf.empty()) continue;
            for (const char* d : {"Eclipse Adoptium", "Java", "Microsoft", "Zulu", "Amazon Corretto",
                                  "BellSoft", "Semeru", "AdoptOpenJDK", "OpenJDK"})
                roots.push_back(pf / d);
        }
        if (fs::path ad = env("APPDATA"); !ad.empty())  // Java з офіційного лаунчера Minecraft
            roots.push_back(ad / ".minecraft" / "runtime");
        if (fs::path up = env("USERPROFILE"); !up.empty()) roots.push_back(up / ".jdks");
    } else {
        roots = {"/usr/lib/jvm", "/usr/lib64/jvm", "/opt/java", "/opt/jdk"};
        if (fs::path h = env("HOME"); !h.empty()) {
            roots.push_back(h / ".jdks");
            roots.push_back(h / ".sdkman" / "candidates" / "java");
        }
    }
    for (const auto& r : roots) scanJavaHomes(r, 4, homes);

    // Спершу точний збіг мажорної версії, інакше найменша з новіших
    fs::path best;
    int bestMajor = 0;
    for (const auto& h : homes) {
        int m = javaMajorOfHome(h);
        if (m == major) return u8(javaExeIn(h));
        if (m > major && (bestMajor == 0 || m < bestMajor)) { best = h; bestMajor = m; }
    }
    return best.empty() ? std::string() : u8(javaExeIn(best));
#endif
}

// ---------- Завантажувачі модів, збірки і Modrinth ----------

enum class Loader { Vanilla = 0, Fabric = 1 };

inline const char* loaderSlug(Loader l) { return l == Loader::Fabric ? "fabric" : ""; }

// Окрема папка гри для кожної версії з Fabric (щоб моди не змішувались)
inline fs::path instanceDir(const std::string& versionId, Loader l) {
    if (l == Loader::Fabric) return rootDir() / "instances" / (versionId + "-fabric");
    return rootDir() / "instance";
}

inline std::string urlEncode(const std::string& s) {
    static const char* hex = "0123456789ABCDEF";
    std::string out;
    for (unsigned char c : s) {
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            out += static_cast<char>(c);
        } else {
            out += '%';
            out += hex[c >> 4];
            out += hex[c & 15];
        }
    }
    return out;
}

// "net.fabricmc:fabric-loader:0.15.11" -> "net/fabricmc/fabric-loader/0.15.11/fabric-loader-0.15.11.jar"
inline std::string mavenPath(const std::string& name) {
    std::vector<std::string> parts;
    size_t start = 0, pos;
    while ((pos = name.find(':', start)) != std::string::npos) {
        parts.push_back(name.substr(start, pos - start));
        start = pos + 1;
    }
    parts.push_back(name.substr(start));
    if (parts.size() < 3) throw std::runtime_error("Некоректна назва бібліотеки: " + name);
    std::string group = parts[0];
    for (char& ch : group)
        if (ch == '.') ch = '/';
    std::string file = parts[1] + "-" + parts[2];
    if (parts.size() > 3) file += "-" + parts[3];
    return group + "/" + parts[1] + "/" + parts[2] + "/" + file + ".jar";
}

// Завантажувати файли дозволено лише з відомих хостів Modrinth
inline bool allowedDownloadUrl(const std::string& u) {
    static const char* prefixes[] = {"https://cdn.modrinth.com/", "https://github.com/",
                                     "https://raw.githubusercontent.com/", "https://gitlab.com/"};
    for (const char* p : prefixes)
        if (u.rfind(p, 0) == 0) return true;
    return false;
}

// Безпечне ім'я файлу чи слаг: лише літери, цифри, '-', '_' (і '.', якщо дозволено)
inline std::string safeName(const std::string& s, bool allowDot) {
    std::string out;
    for (unsigned char c : s)
        out += (std::isalnum(c) || c == '-' || c == '_' || (allowDot && c == '.')) ? static_cast<char>(c) : '_';
    if (out.empty() || out == "." || out == "..") out = "_";
    return out;
}

// Відносний шлях із файлу збірки: без абсолютних шляхів і ".."
inline fs::path safeRelPath(const std::string& rel) {
    fs::path p(rel);
    if (rel.empty() || p.is_absolute() || rel.find('\\') != std::string::npos)
        throw std::runtime_error("Небезпечний шлях у збірці: " + rel);
    for (const auto& part : p)
        if (part == "..") throw std::runtime_error("Небезпечний шлях у збірці: " + rel);
    return p.lexically_normal();
}

// SHA-1 файлу (для перевірки завантажених модів)
inline std::string sha1File(const fs::path& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return "";
    uint32_t h[5] = {0x67452301u, 0xEFCDAB89u, 0x98BADCFEu, 0x10325476u, 0xC3D2E1F0u};
    unsigned char block[64];
    size_t blen = 0;
    uint64_t total = 0;
    auto rol = [](uint32_t x, int n) { return (x << n) | (x >> (32 - n)); };
    auto process = [&](const unsigned char* b) {
        uint32_t w[80];
        for (int i = 0; i < 16; ++i)
            w[i] = (uint32_t(b[4 * i]) << 24) | (uint32_t(b[4 * i + 1]) << 16) |
                   (uint32_t(b[4 * i + 2]) << 8) | uint32_t(b[4 * i + 3]);
        for (int i = 16; i < 80; ++i) w[i] = rol(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
        uint32_t a = h[0], b2 = h[1], c = h[2], d = h[3], e = h[4];
        for (int i = 0; i < 80; ++i) {
            uint32_t fn, k;
            if (i < 20) { fn = (b2 & c) | (~b2 & d); k = 0x5A827999u; }
            else if (i < 40) { fn = b2 ^ c ^ d; k = 0x6ED9EBA1u; }
            else if (i < 60) { fn = (b2 & c) | (b2 & d) | (c & d); k = 0x8F1BBCDCu; }
            else { fn = b2 ^ c ^ d; k = 0xCA62C1D6u; }
            uint32_t temp = rol(a, 5) + fn + e + k + w[i];
            e = d; d = c; c = rol(b2, 30); b2 = a; a = temp;
        }
        h[0] += a; h[1] += b2; h[2] += c; h[3] += d; h[4] += e;
    };
    std::vector<char> buf(65536);
    while (true) {
        f.read(buf.data(), static_cast<std::streamsize>(buf.size()));
        std::streamsize n = f.gcount();
        if (n <= 0) break;
        total += static_cast<uint64_t>(n);
        for (std::streamsize i = 0; i < n; ++i) {
            block[blen++] = static_cast<unsigned char>(buf[static_cast<size_t>(i)]);
            if (blen == 64) { process(block); blen = 0; }
        }
    }
    block[blen++] = 0x80;
    if (blen > 56) {
        while (blen < 64) block[blen++] = 0;
        process(block);
        blen = 0;
    }
    while (blen < 56) block[blen++] = 0;
    uint64_t bits = total * 8;
    for (int i = 7; i >= 0; --i) block[blen++] = static_cast<unsigned char>((bits >> (8 * i)) & 0xFF);
    process(block);
    static const char* hex = "0123456789abcdef";
    std::string out;
    for (uint32_t v : h)
        for (int i = 7; i >= 0; --i) out += hex[(v >> (4 * i)) & 0xF];
    return out;
}

#ifdef _WIN32
inline std::wstring widen(const std::string& s) {
    if (s.empty()) return std::wstring();
    int n = MultiByteToWideChar(CP_UTF8, 0, s.data(), static_cast<int>(s.size()), nullptr, 0);
    std::wstring w(static_cast<size_t>(n), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.data(), static_cast<int>(s.size()), w.data(), n);
    return w;
}

// Екранування одного аргументу за правилами CommandLineToArgvW
inline std::wstring quoteArg(const std::wstring& a) {
    if (!a.empty() && a.find_first_of(L" \t\n\v\"") == std::wstring::npos) return a;
    std::wstring r = L"\"";
    for (size_t i = 0;; ++i) {
        size_t bs = 0;
        while (i < a.size() && a[i] == L'\\') { ++bs; ++i; }
        if (i == a.size()) { r.append(bs * 2, L'\\'); break; }
        if (a[i] == L'"') { r.append(bs * 2 + 1, L'\\'); r += L'"'; }
        else { r.append(bs, L'\\'); r += a[i]; }
    }
    r += L'"';
    return r;
}

inline std::wstring buildCommandLine(const std::vector<std::string>& args) {
    std::wstring cmd;
    for (size_t i = 0; i < args.size(); ++i) {
        if (i) cmd += L' ';
        cmd += quoteArg(widen(args[i]));
    }
    return cmd;
}
#endif

// Запускає зовнішню програму без оболонки, повертає код завершення
inline int runCommand(std::vector<std::string> args) {
#ifdef _WIN32
    std::wstring cmd = buildCommandLine(args);
    STARTUPINFOW si{};
    si.cb = sizeof si;
    PROCESS_INFORMATION pi{};
    if (!CreateProcessW(nullptr, cmd.data(), nullptr, nullptr, FALSE, CREATE_NO_WINDOW, nullptr,
                        nullptr, &si, &pi))
        return -1;
    WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD code = 1;
    GetExitCodeProcess(pi.hProcess, &code);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return static_cast<int>(code);
#else
    std::vector<char*> argv;
    for (auto& a : args) argv.push_back(a.data());
    argv.push_back(nullptr);
    pid_t pid = fork();
    if (pid < 0) return -1;
    if (pid == 0) {
        execv(argv[0], argv.data());
        _exit(127);
    }
    int status = 0;
    waitpid(pid, &status, 0);
    return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
#endif
}

// Профіль Fabric для версії гри: бібліотеки, головний клас, аргументи
inline json fabricProfile(const std::string& mc, const std::string& loaderVersion = "") {
    std::string lv = loaderVersion;
    if (lv.empty()) {
        json loaders = json::parse(httpGet("https://meta.fabricmc.net/v2/versions/loader/" + urlEncode(mc)));
        if (!loaders.is_array() || loaders.empty())
            throw std::runtime_error("Fabric не підтримує версію " + mc);
        for (auto& e : loaders) {
            if (e["loader"].value("stable", false)) { lv = e["loader"]["version"].get<std::string>(); break; }
        }
        if (lv.empty()) lv = loaders[0]["loader"]["version"].get<std::string>();
    }
    return json::parse(httpGet("https://meta.fabricmc.net/v2/versions/loader/" + urlEncode(mc) +
                               "/" + urlEncode(lv) + "/profile/json"));
}

// ---------- Каталог Modrinth ----------

struct ModHit {
    std::string projectId, title, description, iconUrl;
    long long downloads = 0;
};

struct SearchParams {
    std::string query;
    std::string projectType = "mod";  // "mod" або "modpack"
    std::string loader = "fabric";
    std::string category;             // наприклад "optimization"
    std::string index = "downloads";  // downloads, relevance, newest, updated
    std::vector<std::string> versions;  // версії гри (достатньо збігу з однією)
    int offset = 0;
    int limit = 20;
};

inline std::string buildSearchUrl(const SearchParams& p) {
    json facets = json::array();
    facets.push_back(json::array({"project_type:" + p.projectType}));
    if (!p.versions.empty()) {
        json g = json::array();
        for (const auto& v : p.versions) g.push_back("versions:" + v);
        facets.push_back(g);
    }
    if (!p.loader.empty()) facets.push_back(json::array({"categories:" + p.loader}));
    if (!p.category.empty()) facets.push_back(json::array({"categories:" + p.category}));
    std::string url = "https://api.modrinth.com/v2/search?limit=" + std::to_string(p.limit) +
                      "&offset=" + std::to_string(p.offset) + "&facets=" + urlEncode(facets.dump());
    if (!p.query.empty()) url += "&query=" + urlEncode(p.query);
    url += "&index=" + urlEncode(p.index.empty() ? std::string("relevance") : p.index);
    return url;
}

inline std::vector<ModHit> searchProjects(const SearchParams& p) {
    json j = json::parse(httpGet(buildSearchUrl(p)));
    std::vector<ModHit> out;
    for (auto& h : j["hits"]) {
        ModHit m;
        m.projectId = h["project_id"].get<std::string>();
        m.title = h.value("title", std::string());
        m.description = h.value("description", std::string());
        m.iconUrl = h.value("icon_url", std::string());
        m.downloads = h.value("downloads", 0LL);
        out.push_back(m);
    }
    return out;
}

// Завантажує іконки проєктів у кеш; кожну готову повідомляє через onIcon
inline void fetchIcons(const std::vector<ModHit>& hits,
                       const std::function<void(const std::string&, const std::string&)>& onIcon) {
    for (const auto& h : hits) {
        if (g_cancel) return;
        if (h.iconUrl.empty() || !allowedDownloadUrl(h.iconUrl)) continue;
        std::string ext = "png";
        std::string clean = h.iconUrl.substr(0, h.iconUrl.find('?'));
        size_t dot = clean.rfind('.'), slash = clean.rfind('/');
        if (dot != std::string::npos && slash != std::string::npos && dot > slash) {
            std::string e = clean.substr(dot + 1);
            for (char& ch : e) ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
            if (e == "png" || e == "jpg" || e == "jpeg" || e == "webp" || e == "gif") ext = e;
        }
        fs::path p = rootDir() / "cache" / "icons" / (safeName(h.projectId, false) + "." + ext);
        try {
            if (downloadFile(h.iconUrl, p)) onIcon(h.projectId, u8(p));
        } catch (...) {
        }
    }
}

// Встановлює мод та всі його обов'язкові залежності (наприклад, Fabric API)
inline void installMod(const std::string& projectId, const std::string& mc, Loader loader,
                       const fs::path& modsDir, Status& st, std::set<std::string>& visited,
                       int depth = 0) {
    if (depth > 6 || !visited.insert(projectId).second) return;
    checkCancel();
    std::string url = "https://api.modrinth.com/v2/project/" + urlEncode(projectId) +
                      "/version?loaders=" + urlEncode(std::string("[\"") + loaderSlug(loader) + "\"]") +
                      "&game_versions=" + urlEncode("[\"" + mc + "\"]");
    json versions = json::parse(httpGet(url));
    if (!versions.is_array() || versions.empty())
        throw std::runtime_error("Немає сумісної версії мода для " + mc);
    json& v = versions[0];
    if (!v.contains("files") || v["files"].empty()) throw std::runtime_error("У моді немає файлів");
    json* file = &v["files"][0];
    for (auto& f : v["files"])
        if (f.value("primary", false)) { file = &f; break; }

    std::string filename = (*file)["filename"].get<std::string>();
    if (filename.find('/') != std::string::npos || filename.find('\\') != std::string::npos ||
        filename.find("..") != std::string::npos)
        throw std::runtime_error("Небезпечна назва файлу: " + filename);
    std::string fileUrl = (*file)["url"].get<std::string>();
    if (!allowedDownloadUrl(fileUrl)) throw std::runtime_error("Недозволене джерело файлу: " + fileUrl);

    st.log("Встановлюю " + filename + " ...");
    fs::path dest = modsDir / filename;
    if (!downloadFile(fileUrl, dest, (*file)["size"].get<std::uintmax_t>()))
        throw std::runtime_error("Не вдалося завантажити " + filename);
    if (file->contains("hashes") && (*file)["hashes"].contains("sha1")) {
        std::string want = (*file)["hashes"]["sha1"].get<std::string>();
        if (!want.empty() && sha1File(dest) != want) {
            std::error_code ec;
            fs::remove(dest, ec);
            throw std::runtime_error("Контрольна сума не збігається: " + filename);
        }
    }

    if (v.contains("dependencies")) {
        for (auto& d : v["dependencies"]) {
            if (d.value("dependency_type", std::string()) != "required") continue;
            if (!d.contains("project_id") || d["project_id"].is_null()) continue;
            installMod(d["project_id"].get<std::string>(), mc, loader, modsDir, st, visited, depth + 1);
        }
    }
}

// ---------- Збірки (.mrpack) і профілі ----------

struct Profile {
    std::string slug, name, mcVersion, loaderVersion, versionNumber;
    fs::path dir;  // папка гри цієї збірки
};

inline fs::path profilesDir() { return rootDir() / "profiles"; }

inline bool loadProfile(const std::string& slug, Profile& out) {
    try {
        std::ifstream f(profilesDir() / (safeName(slug, false) + ".json"));
        if (!f) return false;
        json j = json::parse(f);
        out.slug = safeName(slug, false);
        out.name = j.value("name", out.slug);
        out.mcVersion = j.value("mcVersion", std::string());
        out.loaderVersion = j.value("loaderVersion", std::string());
        out.versionNumber = j.value("versionNumber", std::string());
        out.dir = rootDir() / "instances" / ("pack-" + out.slug);
        return !out.mcVersion.empty();
    } catch (...) {
        return false;
    }
}

inline std::vector<Profile> listProfiles() {
    std::vector<Profile> out;
    std::error_code ec;
    if (!fs::exists(profilesDir(), ec)) return out;
    for (const auto& e : fs::directory_iterator(profilesDir(), ec)) {
        if (!e.is_regular_file() || e.path().extension() != ".json") continue;
        Profile p;
        if (loadProfile(e.path().stem().string(), p)) out.push_back(p);
    }
    std::sort(out.begin(), out.end(), [](const Profile& a, const Profile& b) { return a.name < b.name; });
    return out;
}

// Видаляє профіль разом із папкою гри (моди, світи) цієї збірки
inline void deleteProfile(const std::string& slug) {
    Profile p;
    std::error_code ec;
    if (loadProfile(slug, p)) fs::remove_all(p.dir, ec);
    fs::remove(profilesDir() / (safeName(slug, false) + ".json"), ec);
}

// Розпаковує .mrpack у tmp і повертає modrinth.index.json
inline json extractMrpack(const fs::path& mrpack, const fs::path& tmp) {
    std::error_code ec;
    fs::remove_all(tmp, ec);
    fs::create_directories(tmp);
    int code;
    if (kIsWindows) {
        // tar.exe (bsdtar) входить до Windows 10 1803+ і вміє розпаковувати zip
        const char* sr = std::getenv("SystemRoot");
        std::string tar = std::string(sr ? sr : "C:\\Windows") + "\\System32\\tar.exe";
        code = runCommand({tar, "-xf", u8(mrpack), "-C", u8(tmp)});
    } else {
        code = runCommand({"/usr/bin/unzip", "-o", "-q", u8(mrpack), "-d", u8(tmp)});
    }
    if (code != 0 && code != 1) throw std::runtime_error("Не вдалося розпакувати збірку (код " + std::to_string(code) + ")");
    // Символічні посилання у збірці небезпечні
    for (const auto& e : fs::recursive_directory_iterator(tmp, ec))
        if (e.is_symlink()) throw std::runtime_error("Збірка містить символічні посилання, її відхилено");
    std::ifstream f(tmp / "modrinth.index.json");
    if (!f) throw std::runtime_error("У збірці немає modrinth.index.json");
    return json::parse(f);
}

// Копіює файли з overrides та client-overrides у папку гри
inline void applyOverrides(const fs::path& tmp, const fs::path& instance) {
    for (const char* dir : {"overrides", "client-overrides"}) {
        fs::path src = tmp / dir;
        std::error_code ec;
        if (fs::exists(src, ec))
            fs::copy(src, instance, fs::copy_options::recursive | fs::copy_options::overwrite_existing);
    }
}

// Завантажує збірку з Modrinth, ставить її файли й створює профіль. Повертає slug профілю.
inline std::string installModpack(const std::string& projectId, Status& st) {
    fs::path root = rootDir();
    st.progress = 0.f;
    st.log("Отримую інформацію про збірку ...");
    json proj = json::parse(httpGet("https://api.modrinth.com/v2/project/" + urlEncode(projectId)));
    std::string slug = safeName(proj.value("slug", projectId), false);
    std::string title = proj.value("title", slug);

    std::set<std::string> supported;
    for (const auto& v : fetchVersions()) supported.insert(v.id);

    json versions = json::parse(httpGet("https://api.modrinth.com/v2/project/" + urlEncode(projectId) +
                                        "/version?loaders=" + urlEncode("[\"fabric\"]")));
    json* chosen = nullptr;
    json* packFile = nullptr;
    for (auto& v : versions) {
        bool ok = false;
        for (auto& g : v["game_versions"])
            if (supported.count(g.get<std::string>())) ok = true;
        if (!ok) continue;
        for (auto& f : v["files"]) {
            std::string fn = f.value("filename", std::string());
            if (fn.size() > 7 && fn.compare(fn.size() - 7, 7, ".mrpack") == 0) { packFile = &f; break; }
        }
        if (packFile) { chosen = &v; break; }
    }
    if (!chosen) throw std::runtime_error("Немає сумісної версії збірки (потрібні Fabric і Minecraft 1.19+)");
    const std::string versionNumber = chosen->value("version_number", std::string());

    std::string packUrl = (*packFile)["url"].get<std::string>();
    if (!allowedDownloadUrl(packUrl)) throw std::runtime_error("Недозволене джерело файлу збірки");
    fs::path packPath = root / "cache" / "packs" / safeName((*packFile)["filename"].get<std::string>(), true);
    st.log("Завантажую " + title + " " + versionNumber + " ...");
    if (!downloadFile(packUrl, packPath, (*packFile)["size"].get<std::uintmax_t>()))
        throw std::runtime_error("Не вдалося завантажити файл збірки");
    st.progress = 0.04f;

    fs::path tmp = root / "cache" / ("unpack-" + slug);
    std::error_code ec;
    try {
        json idx = extractMrpack(packPath, tmp);
        json deps = idx.value("dependencies", json::object());
        std::string mc = deps.value("minecraft", std::string());
        if (!deps.contains("fabric-loader")) {
            std::string other = "інший завантажувач";
            for (const char* k : {"neoforge", "forge", "quilt-loader"})
                if (deps.contains(k)) { other = k; break; }
            throw std::runtime_error("Ця збірка потребує " + other + ", а лаунчер поки підтримує лише Fabric");
        }
        if (!supported.count(mc))
            throw std::runtime_error("Збірка для Minecraft " + mc + ", який лаунчер не підтримує (потрібна 1.19+)");
        std::string loaderVersion = deps["fabric-loader"].get<std::string>();

        fs::path instance = root / "instances" / ("pack-" + slug);
        fs::create_directories(instance);

        json& files = idx["files"];
        size_t total = files.size(), i = 0;
        for (auto& f : files) {
            checkCancel();
            ++i;
            if (f.contains("env") && f["env"].value("client", std::string("required")) == "unsupported") continue;
            fs::path dest = instance / safeRelPath(f["path"].get<std::string>());
            std::uintmax_t size = f.value("fileSize", static_cast<std::uintmax_t>(0));
            bool done = false;
            for (auto& u : f["downloads"]) {
                std::string url = u.get<std::string>();
                if (!allowedDownloadUrl(url)) continue;
                if (downloadFile(url, dest, size)) { done = true; break; }
            }
            if (!done) throw std::runtime_error("Не вдалося завантажити " + f["path"].get<std::string>());
            if (f.contains("hashes") && f["hashes"].contains("sha1")) {
                std::string want = f["hashes"]["sha1"].get<std::string>();
                if (!want.empty() && sha1File(dest) != want) {
                    fs::remove(dest, ec);
                    throw std::runtime_error("Контрольна сума не збігається: " + f["path"].get<std::string>());
                }
            }
            st.progress = 0.05f + 0.9f * float(i) / float(total);
            if (i % 5 == 0 || i == total)
                st.setText("Файли збірки: " + std::to_string(i) + "/" + std::to_string(total));
        }
        applyOverrides(tmp, instance);

        json prof = {{"slug", slug}, {"name", title}, {"mcVersion", mc}, {"loader", "fabric"},
                     {"loaderVersion", loaderVersion}, {"versionNumber", versionNumber},
                     {"projectId", projectId}};
        fs::create_directories(profilesDir());
        std::ofstream(profilesDir() / (slug + ".json")) << prof.dump(2);
    } catch (...) {
        fs::remove_all(tmp, ec);
        throw;
    }
    fs::remove_all(tmp, ec);
    st.progress = 1.f;
    return slug;
}

// ---------- Встановлення і запуск ----------

// ---------- Запуск процесу гри ----------

// Запускає Java з аргументами args (args[0] - java), вивід пише у logPath.
// Окремий потік чекає на завершення гри й оновлює Status.
inline void spawnGame(const std::vector<std::string>& args, const fs::path& gameDir,
                      const fs::path& logPath, Status& st) {
    const std::string logStr = u8(logPath);
    auto onExit = [&st, logStr](int code) {
        st.gameRunning = false;
        st.log(code == 0 ? "Гру закрито"
                         : "Гра завершилась з кодом " + std::to_string(code) + ". Деталі: " + logStr);
    };
#ifdef _WIN32
    SECURITY_ATTRIBUTES sa{};
    sa.nLength = sizeof sa;
    sa.bInheritHandle = TRUE;
    HANDLE hLog = CreateFileW(logPath.c_str(), GENERIC_WRITE, FILE_SHARE_READ, &sa, CREATE_ALWAYS,
                              FILE_ATTRIBUTE_NORMAL, nullptr);
    HANDLE hNul = CreateFileW(L"NUL", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, &sa,
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    STARTUPINFOW si{};
    si.cb = sizeof si;
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = hNul;
    si.hStdOutput = hLog;
    si.hStdError = hLog;
    PROCESS_INFORMATION pi{};
    std::wstring cmd = buildCommandLine(args);
    const std::wstring wdir = gameDir.wstring();
    const BOOL ok = CreateProcessW(nullptr, cmd.data(), nullptr, nullptr, TRUE, CREATE_NO_WINDOW,
                                   nullptr, wdir.c_str(), &si, &pi);
    if (hLog != INVALID_HANDLE_VALUE) CloseHandle(hLog);
    if (hNul != INVALID_HANDLE_VALUE) CloseHandle(hNul);
    if (!ok) throw std::runtime_error("Не вдалося запустити процес");
    CloseHandle(pi.hThread);
    st.gameRunning = true;
    HANDLE proc = pi.hProcess;
    std::thread([proc, onExit] {
        WaitForSingleObject(proc, INFINITE);
        DWORD code = 1;
        GetExitCodeProcess(proc, &code);
        CloseHandle(proc);
        onExit(static_cast<int>(code));
    }).detach();
#else
    std::vector<std::string> a = args;
    std::vector<char*> argv;
    for (auto& s : a) argv.push_back(s.data());
    argv.push_back(nullptr);
    const std::string gameDirStr = gameDir.string();
    int logfd = open(logPath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    pid_t pid = fork();
    if (pid < 0) {
        if (logfd >= 0) close(logfd);
        throw std::runtime_error("Не вдалося запустити процес");
    }
    if (pid == 0) {
        if (logfd >= 0) { dup2(logfd, 1); dup2(logfd, 2); }
        if (chdir(gameDirStr.c_str()) != 0) _exit(126);
        execv(argv[0], argv.data());
        _exit(127);
    }
    if (logfd >= 0) close(logfd);
    st.gameRunning = true;
    std::thread([pid, onExit] {
        int status = 0;
        waitpid(pid, &status, 0);
        onExit(WIFEXITED(status) ? WEXITSTATUS(status) : -1);
    }).detach();
#endif
}


inline void installAndRun(const VersionInfo& ver, const std::string& player, int ramGb,
                           Loader loader, Status& st,
                           const fs::path& instanceOverride = fs::path(),
                           const std::string& loaderVersion = std::string()) {
    fs::path root = rootDir();
    fs::path vdir = root / "versions" / ver.id;
    fs::path vjson = vdir / (ver.id + ".json");
    fs::path gameDir = instanceOverride.empty() ? instanceDir(ver.id, loader) : instanceOverride;

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
        cp.push_back(u8(p));
        st.progress = 0.05f + 0.25f * float(k + 1) / float(libs.size());
        st.setText("Бібліотеки: " + std::to_string(k + 1) + "/" + std::to_string(libs.size()));
    }
    cp.push_back(u8(jar));
    st.log("Бібліотек: " + std::to_string(libs.size()));

    // Fabric: додаткові бібліотеки, головний клас та аргументи
    std::string mainClass = vj["mainClass"].get<std::string>();
    std::vector<std::string> extraJvm, extraGame;
    if (loader == Loader::Fabric) {
        st.log("Отримую Fabric ...");
        json fabric = fabricProfile(ver.id, loaderVersion);
        std::vector<std::string> fabricCp;
        for (auto& lib : fabric["libraries"]) {
            checkCancel();
            std::string rel = mavenPath(lib["name"].get<std::string>());
            std::string base = lib.value("url", std::string("https://maven.fabricmc.net/"));
            if (!base.empty() && base.back() != '/') base += '/';
            fs::path p = root / "libraries" / rel;
            if (!downloadFile(base + rel, p))
                throw std::runtime_error("Не вдалося завантажити " + rel);
            fabricCp.push_back(u8(p));
        }
        cp.insert(cp.begin(), fabricCp.begin(), fabricCp.end());
        mainClass = fabric["mainClass"].get<std::string>();
        if (fabric.contains("arguments")) {
            for (auto& a : fabric["arguments"].value("jvm", json::array()))
                if (a.is_string()) extraJvm.push_back(a.get<std::string>());
            for (auto& a : fabric["arguments"].value("game", json::array()))
                if (a.is_string()) extraGame.push_back(a.get<std::string>());
        }
        fs::create_directories(gameDir / "mods");
        st.log("Fabric готовий");
    }

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
    fs::create_directories(natives);
    fs::create_directories(gameDir);

    std::string cpStr;
    for (size_t k = 0; k < cp.size(); ++k) cpStr += (k ? std::string(1, kPathSep) : std::string()) + cp[k];

    std::map<std::string, std::string> vars = {
        {"auth_player_name", player},
        {"version_name", ver.id},
        {"game_directory", u8(gameDir)},
        {"assets_root", u8(assets)},
        {"assets_index_name", assetId},
        {"auth_uuid", "00000000-0000-0000-0000-000000000000"},
        {"auth_access_token", "0"},
        {"clientid", "0"},
        {"auth_xuid", "0"},
        {"user_type", "legacy"},
        {"version_type", vj["type"].get<std::string>()},
        {"natives_directory", u8(natives)},
        {"launcher_name", "MyLauncher"},
        {"launcher_version", "0.2"},
        {"classpath", cpStr},
    };

    std::vector<std::string> raw = {javaBin, "-Xmx" + std::to_string(ramGb) + "G"};
    collectArgs(vj["arguments"]["jvm"], raw);
    raw.insert(raw.end(), extraJvm.begin(), extraJvm.end());
    raw.push_back(mainClass);
    collectArgs(vj["arguments"]["game"], raw);
    raw.insert(raw.end(), extraGame.begin(), extraGame.end());

    std::vector<std::string> args;
    for (auto& s : raw) args.push_back(substitute(s, vars));

    // Запуск: вивід гри йде у файл game.log
    const fs::path logPath = root / "game.log";
    st.log("Запускаю Minecraft " + ver.id + " ...");
    spawnGame(args, gameDir, logPath, st);
    st.progress = 1.f;
}

}  // namespace core
