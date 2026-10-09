#define GL_SILENCE_DEPRECATION
#include "core.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <cctype>
#include <cstring>

// =====================================================================
//  НАЛАШТУВАННЯ ВИГЛЯДУ: міняй числа й кольори тут
// =====================================================================
namespace layout {
constexpr int   WINDOW_W = 820;                 // ширина вікна
constexpr int   WINDOW_H = 540;                 // висота вікна
constexpr float SIDEBAR_W = 180.f;              // ширина лівого меню
constexpr float MENU_ITEM_H = 40.f;             // висота пункту меню
constexpr float PLAY_BUTTON_H = 56.f;           // висота кнопки "Грати"
constexpr float PLAY_BUTTON_FROM_BOTTOM = 140.f;// наскільки кнопка вище від низу
}
static const ImVec4 COLOR_TITLE(0.25f, 0.90f, 0.80f, 1.0f);      // колір заголовка
static const ImVec4 COLOR_PLAY(0.15f, 0.65f, 0.35f, 1.0f);       // кнопка "Грати"
static const ImVec4 COLOR_PLAY_HOVER(0.20f, 0.78f, 0.42f, 1.0f); // при наведенні
static const ImVec4 COLOR_PLAY_ACTIVE(0.12f, 0.55f, 0.30f, 1.0f);// при натисканні
static const ImVec4 COLOR_BACKGROUND(0.10f, 0.10f, 0.12f, 1.0f); // фон вікна

// Сторінки меню. Щоб додати нову: додай сюди, у drawSidebar і в main (switch)
enum class Page { Play, Settings, Log };

// =====================================================================
//  Налаштування, що зберігаються між запусками
// =====================================================================
struct Config {
    std::string nick = "Player";
    std::string version;
    bool snapshots = false;
    int ramGb = 2;
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
            c.ramGb = j.value("ramGb", c.ramGb);
        }
    } catch (...) {}
    return c;
}

static void saveConfig(const Config& c) {
    try {
        std::filesystem::create_directories(configPath().parent_path());
        core::json j = {{"nick", c.nick}, {"version", c.version},
                        {"snapshots", c.snapshots}, {"ramGb", c.ramGb}};
        std::ofstream(configPath()) << j.dump(2);
    } catch (...) {}
}

// =====================================================================
//  Стан програми
// =====================================================================
struct App {
    Config cfg;
    Page page = Page::Play;
    char nick[32] = {0};
    std::vector<core::VersionInfo> versions;
    std::mutex vmtx;
    std::atomic<int> versionState{0};  // 0 - завантажується, 1 - готово, 2 - помилка
    core::Status st;
    std::thread worker;
};

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

// Те, що відбувається після натискання "Грати"
static void startGame(App& app) {
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
    int ram = app.cfg.ramGb;
    app.worker = std::thread([&app, chosen, nick, ram] {
        try {
            core::installAndRun(chosen, nick, ram, app.st);
        } catch (const std::exception& e) {
            app.st.log(std::string("Помилка: ") + e.what());
        }
        app.st.busy = false;
    });
}

// =====================================================================
//  ЛІВЕ МЕНЮ: кожен пункт це рядок у масиві items
// =====================================================================
static void drawSidebar(App& app) {
    ImGui::TextColored(COLOR_TITLE, "CYBER LAUNCHER");
    ImGui::Separator();
    ImGui::Spacing();

    struct Item { const char* label; Page page; };
    const Item items[] = {
        {"Грати", Page::Play},
        {"Налаштування", Page::Settings},
        {"Журнал", Page::Log},
    };
    for (const Item& it : items)
        if (ImGui::Selectable(it.label, app.page == it.page, 0, ImVec2(0, layout::MENU_ITEM_H)))
            app.page = it.page;
}

// =====================================================================
//  СТОРІНКА "ГРАТИ": порядок рядків = порядок елементів на екрані
// =====================================================================
static void drawPlayPage(App& app) {
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
    if (app.versionState == 2 && ImGui::Button("Спробувати ще раз")) loadVersionsAsync(app);

    // Кнопка "Грати" стоїть на фіксованій відстані від низу сторінки
    float y = ImGui::GetWindowHeight() - layout::PLAY_BUTTON_FROM_BOTTOM;
    if (y > ImGui::GetCursorPosY()) ImGui::SetCursorPosY(y);

    bool canPlay = app.versionState == 1 && !app.cfg.version.empty() &&
                   !app.st.busy && !app.st.gameRunning && std::strlen(app.nick) > 0;
    ImGui::PushStyleColor(ImGuiCol_Button, COLOR_PLAY);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, COLOR_PLAY_HOVER);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, COLOR_PLAY_ACTIVE);
    ImGui::BeginDisabled(!canPlay);
    if (ImGui::Button("ГРАТИ", ImVec2(-1, layout::PLAY_BUTTON_H))) startGame(app);
    ImGui::EndDisabled();
    ImGui::PopStyleColor(3);

    ImGui::Spacing();
    ImGui::ProgressBar(app.st.progress, ImVec2(-1, 0));
    {
        std::lock_guard<std::mutex> lk(app.st.m);
        ImGui::TextWrapped("%s", app.st.text.c_str());
    }
    if (app.st.gameRunning) ImGui::Text("Minecraft запущено");
}

// =====================================================================
//  СТОРІНКА "НАЛАШТУВАННЯ"
// =====================================================================
static void drawSettingsPage(App& app) {
    ImGui::Text("Пам'ять для гри (ГБ)");
    ImGui::SetNextItemWidth(-1);
    ImGui::SliderInt("##ram", &app.cfg.ramGb, 1, 16, "%d ГБ");

    ImGui::Spacing();
    ImGui::Checkbox("Показувати снапшоти у списку версій", &app.cfg.snapshots);

    ImGui::Spacing();
    if (ImGui::Button("Відкрити папку з грою")) {
        std::filesystem::create_directories(core::rootDir());
        std::string cmd = "open \"" + core::rootDir().string() + "\"";
        if (std::system(cmd.c_str()) != 0) {}
    }
}

// =====================================================================
//  СТОРІНКА "ЖУРНАЛ"
// =====================================================================
static void drawLogPage(App& app) {
    static size_t lastLines = 0;
    std::lock_guard<std::mutex> lk(app.st.m);
    for (auto& l : app.st.lines) ImGui::TextWrapped("%s", l.c_str());
    if (app.st.lines.size() != lastLines) {
        ImGui::SetScrollHereY(1.0f);
        lastLines = app.st.lines.size();
    }
}

// =====================================================================
//  Шрифт із кирилицею
// =====================================================================
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
    GLFWwindow* window = glfwCreateWindow(layout::WINDOW_W, layout::WINDOW_H,
                                          "Cyber Launcher", nullptr, nullptr);
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

        // Одне вікно на весь екран: зліва меню, справа вміст сторінки
        ImGuiViewport* vp = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(vp->WorkPos);
        ImGui::SetNextWindowSize(vp->WorkSize);
        ImGui::Begin("main", nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                         ImGuiWindowFlags_NoSavedSettings);

        ImGui::BeginChild("sidebar", ImVec2(layout::SIDEBAR_W, 0), ImGuiChildFlags_Borders);
        drawSidebar(app);
        ImGui::EndChild();

        ImGui::SameLine();  // SameLine = наступний елемент праворуч, а не нижче

        ImGui::BeginChild("content", ImVec2(0, 0), ImGuiChildFlags_Borders);
        switch (app.page) {
            case Page::Play:     drawPlayPage(app); break;
            case Page::Settings: drawSettingsPage(app); break;
            case Page::Log:      drawLogPage(app); break;
        }
        ImGui::EndChild();

        ImGui::End();
        ImGui::Render();

        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        glClearColor(COLOR_BACKGROUND.x, COLOR_BACKGROUND.y, COLOR_BACKGROUND.z, 1.0f);
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
