#define GL_SILENCE_DEPRECATION
#include "core.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <cctype>
#include <cstring>

struct Config {
    std::string nick = "Player";
    std::string version;
    bool snapshots = false;
};

static std::filesystem::path configPath() { return core::rootDir() / "config.json"; }

static Config loadConfig() {
    Config c;
    try {
        std::ifstream f(configPath());
        if (f) {
            auto j = core::json::parse(f);
            c.nick = j.value("nick", c.nick);
            c.version = j.value("version", c.version);
            c.snapshots = j.value("snapshots", c.snapshots);
        }
    } catch (...) {}
    return c;
}

static void saveConfig(const Config& c) {
    try {
        std::filesystem::create_directories(configPath().parent_path());
        core::json j = {{"nick", c.nick}, {"version", c.version}, {"snapshots", c.snapshots}};
        std::ofstream(configPath()) << j.dump(2);
    } catch (...) {}
}

struct App {
    Config cfg;
    char nick[32] = {0};
    std::vector<core::VersionInfo> versions;
    std::mutex vmtx;
    std::atomic<int> versionState{0};  // 0 - завантажується, 1 - готово, 2 - помилка
    core::Status st;
    std::thread worker;
};

// Лише літери, цифри та підкреслення в нікнеймі
static int nickFilter(ImGuiInputTextCallbackData* d) {
    ImWchar c = d->EventChar;
    return (c < 128 && (std::isalnum(static_cast<int>(c)) || c == '_')) ? 0 : 1;
}

static void loadVersionsAsync(App& app) {
    app.versionState = 0;
    std::thread([&app] {
        try {
            auto v = core::fetchVersions();
            std::lock_guard<std::mutex> lk(app.vmtx);
            app.versions = std::move(v);
            app.versionState = 1;
        } catch (const std::exception& e) {
            app.st.log(std::string("Не вдалося отримати список версій: ") + e.what());
            app.versionState = 2;
        }
    }).detach();
}

static void loadFont(ImGuiIO& io, float scale) {
    const char* candidates[] = {
        "/System/Library/Fonts/Supplemental/Arial.ttf",
        "/Library/Fonts/Arial.ttf",
        "/System/Library/Fonts/Helvetica.ttc",
    };
    for (const char* path : candidates) {
        if (!std::filesystem::exists(path)) continue;
        io.Fonts->AddFontFromFileTTF(path, 16.0f * scale, nullptr,
                                     io.Fonts->GetGlyphRangesCyrillic());
        io.FontGlobalScale = 1.0f / scale;  // чіткий текст на Retina
        return;
    }
}

int main() {
    App& app = *new App;  // навмисно не видаляється: фонові потоки можуть жити до виходу
    app.cfg = loadConfig();
    std::snprintf(app.nick, sizeof app.nick, "%s", app.cfg.nick.c_str());

    if (!glfwInit()) return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    GLFWwindow* window = glfwCreateWindow(720, 560, "Minecraft Launcher", nullptr, nullptr);
    if (!window) { glfwTerminate(); return 1; }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    float xscale = 1.f, yscale = 1.f;
    glfwGetWindowContentScale(window, &xscale, &yscale);
    loadFont(io, xscale);
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 150");

    loadVersionsAsync(app);
    size_t lastLines = 0;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Якщо версія не обрана, беремо найновіший реліз
        if (app.versionState == 1 && app.cfg.version.empty()) {
            std::lock_guard<std::mutex> lk(app.vmtx);
            for (auto& v : app.versions)
                if (v.type == "release") { app.cfg.version = v.id; break; }
        }

        ImGuiViewport* vp = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(vp->WorkPos);
        ImGui::SetNextWindowSize(vp->WorkSize);
        ImGui::Begin("main", nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                         ImGuiWindowFlags_NoSavedSettings);

        ImGui::Text("Minecraft Launcher");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Нікнейм");
        ImGui::SetNextItemWidth(-1);
        ImGui::InputText("##nick", app.nick, sizeof app.nick,
                         ImGuiInputTextFlags_CallbackCharFilter, nickFilter);

        ImGui::Spacing();
        ImGui::Text("Версія");
        ImGui::SetNextItemWidth(-1);
        const char* preview = app.versionState == 0   ? "Завантажую список..."
                              : app.cfg.version.empty() ? "Оберіть версію"
                                                        : app.cfg.version.c_str();
        if (ImGui::BeginCombo("##ver", preview)) {
            std::lock_guard<std::mutex> lk(app.vmtx);
            for (auto& v : app.versions) {
                if (!app.cfg.snapshots && v.type != "release") continue;
                bool sel = (v.id == app.cfg.version);
                std::string label = v.id;
                if (v.type != "release") label += "  (" + v.type + ")";
                if (ImGui::Selectable(label.c_str(), sel)) app.cfg.version = v.id;
                if (sel) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::Checkbox("Показувати снапшоти", &app.cfg.snapshots);
        if (app.versionState == 2) {
            ImGui::SameLine();
            if (ImGui::Button("Спробувати ще раз")) loadVersionsAsync(app);
        }

        ImGui::Spacing();
        bool canPlay = app.versionState == 1 && !app.cfg.version.empty() &&
                       !app.st.busy && !app.st.gameRunning && std::strlen(app.nick) > 0;
        ImGui::BeginDisabled(!canPlay);
        if (ImGui::Button("Грати", ImVec2(-1, 48))) {
            app.cfg.nick = app.nick;
            saveConfig(app.cfg);
            core::VersionInfo chosen;
            {
                std::lock_guard<std::mutex> lk(app.vmtx);
                for (auto& v : app.versions)
                    if (v.id == app.cfg.version) chosen = v;
            }
            if (app.worker.joinable()) app.worker.join();
            app.st.busy = true;
            app.st.progress = 0.f;
            std::string nick = app.cfg.nick;
            app.worker = std::thread([&app, chosen, nick] {
                try {
                    core::installAndRun(chosen, nick, app.st);
                } catch (const std::exception& e) {
                    app.st.log(std::string("Помилка: ") + e.what());
                }
                app.st.busy = false;
            });
        }
        ImGui::EndDisabled();

        ImGui::Spacing();
        ImGui::ProgressBar(app.st.progress, ImVec2(-1, 0));
        {
            std::lock_guard<std::mutex> lk(app.st.m);
            ImGui::TextWrapped("%s", app.st.text.c_str());
        }
        if (app.st.gameRunning) ImGui::Text("Minecraft запущено");

        ImGui::Spacing();
        ImGui::Text("Журнал");
        ImGui::BeginChild("log", ImVec2(0, 0), ImGuiChildFlags_Borders);
        {
            std::lock_guard<std::mutex> lk(app.st.m);
            for (auto& l : app.st.lines) ImGui::TextWrapped("%s", l.c_str());
            if (app.st.lines.size() != lastLines) {
                ImGui::SetScrollHereY(1.0f);
                lastLines = app.st.lines.size();
            }
        }
        ImGui::EndChild();

        ImGui::End();
        ImGui::Render();

        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        glClearColor(0.10f, 0.10f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Закриття вікна: зберігаємо налаштування, перериваємо завантаження
    app.cfg.nick = app.nick;
    saveConfig(app.cfg);
    core::g_cancel = true;
    if (app.worker.joinable()) app.worker.join();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
