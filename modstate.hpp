#pragma once
#include <filesystem>
#include <stdexcept>

namespace modstate {
namespace fs = std::filesystem;

inline bool disabled(const fs::path& p) {
    return p.extension() == ".disabled" && p.stem().extension() == ".jar";
}
inline bool supported(const fs::path& p) {
    return p.extension() == ".jar" || disabled(p);
}
// Приймайте лише назву файлу в каталозі вибраних модів; ніколи не перезаписуйте файл.
inline fs::path toggle(const fs::path& dir, const fs::path& name) {
    if (name.empty() || name != name.filename() || !supported(name))
        throw std::runtime_error("Invalid mod filename");
    const auto source = dir / name;
    const auto status = fs::symlink_status(source);
    if (!fs::is_regular_file(status))
        throw std::runtime_error("Mod file not found or not regular");
    auto dest = source;
    if (disabled(source)) dest.replace_extension();
    else dest += ".disabled";
    if (fs::symlink_status(dest).type() != fs::file_type::not_found)
        throw std::runtime_error("Destination already exists");
    fs::rename(source, dest);
    return dest.filename();
}
}
