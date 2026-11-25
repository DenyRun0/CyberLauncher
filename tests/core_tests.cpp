#include "../core.hpp"
#include "../modstate.hpp"
#include <chrono>
#include <fstream>
#include <iostream>
#include <stdexcept>
void check(bool ok, const char* name) {
    if (!ok) throw std::runtime_error(name);
    std::cout << "PASS: " << name << '\n';
}
int main() {
    check(core::urlEncode("abc") == "abc", "plain URL text");
    check(core::urlEncode("a b") == "a%20b", "URL space");
    check(core::mavenPath("org.example:demo:1.0") == "org/example/demo/1.0/demo-1.0.jar", "Maven path");
    check(core::substitute("${name}", {{"name", "Player"}}) == "Player", "known placeholder");
    check(!core::allowedDownloadUrl("http://example.invalid/a"), "reject untrusted HTTP URL");
    namespace fs = std::filesystem;
    const auto dir = fs::temp_directory_path() / ("cyber-tests-" + std::to_string(
        std::chrono::steady_clock::now().time_since_epoch().count()));
    fs::create_directory(dir);
    struct Cleanup { fs::path dir; ~Cleanup() { std::error_code ec; fs::remove_all(dir, ec); } } cleanup{dir};
    std::ofstream(dir / "sample.jar") << "test";
    check(modstate::toggle(dir, "sample.jar") == "sample.jar.disabled" &&
          fs::exists(dir / "sample.jar.disabled") && !fs::exists(dir / "sample.jar"), "disable mod");
    check(modstate::toggle(dir, "sample.jar.disabled") == "sample.jar" &&
          fs::exists(dir / "sample.jar") && !fs::exists(dir / "sample.jar.disabled"), "enable mod");
    std::ofstream(dir / "sample.jar.disabled") << "existing";
    bool rejected = false;
    try { modstate::toggle(dir, "sample.jar"); } catch (const std::exception&) { rejected = true; }
    check(rejected && fs::exists(dir / "sample.jar") && fs::exists(dir / "sample.jar.disabled"), "reject collision");
}
