#include "../modstate.hpp"
#include <chrono>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace fs = std::filesystem;

int main() {
    const auto dir = fs::temp_directory_path() / ("cyber-negative-" + std::to_string(
        std::chrono::steady_clock::now().time_since_epoch().count()));
    fs::create_directory(dir);
    struct Cleanup { fs::path dir; ~Cleanup() { std::error_code ec; fs::remove_all(dir, ec); } } cleanup{dir};
    fs::create_directory(dir / "folder.jar");
    std::ofstream(dir / "notes.txt") << "unchanged";
    auto reject = [&](const fs::path& name, const char* label) {
        bool rejected = false;
        try { modstate::toggle(dir, name); }
        catch (const std::exception&) { rejected = true; }
        if (!rejected) throw std::runtime_error(label);
        std::cout << "PASS: " << label << '\n';
    };
    reject("", "empty filename");
    reject("notes.txt", "unsupported extension");
    reject("missing.jar", "missing mod file");
    reject("folder.jar", "directory instead of file");
    reject("../outside.jar", "path outside mods directory");
    if (!fs::is_regular_file(dir / "notes.txt") || !fs::is_directory(dir / "folder.jar") ||
        fs::exists(dir / "notes.txt.disabled") || fs::exists(dir / "missing.jar.disabled") ||
        fs::exists(dir / "folder.jar.disabled")) throw std::runtime_error("unexpected filesystem change");
    return 0;
}
