#include "../core.hpp"
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
}
