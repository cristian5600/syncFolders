/*
#include <array>
#include <iostream>
#include <string_view>
#include <tuple>
#include <type_traits>

namespace a::b::c
{
    inline constexpr std::string_view str{ "hello" };
}

template <class... T>
std::tuple<std::size_t, std::common_type_t<T...>> sum(T... args)
{
    return { sizeof...(T), (args + ...) };
}

int main()
{
    auto [iNumbers, iSum] { sum(1, 2, 3) };
    std::cout << a::b::c::str << ' ' << iNumbers << ' ' << iSum << '\n';

    std::array arr{ 1, 2, 3 };

    std::cout << std::size(arr) << '\n';

    return 0;
}
*/

#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

void sync_folders(const fs::path& source, const fs::path& destination)
{
    for (const auto& entry : fs::directory_iterator(source)) {
        const auto entry_path = entry.path();
        const auto destination_path = destination / entry_path.filename();
        if (fs::is_directory(entry_path)) {
            fs::create_directory(destination_path);
            sync_folders(entry_path, destination_path);
        }
        else {
            fs::copy_file(entry_path, destination_path, fs::copy_options::overwrite_existing);
        }
    }

    for (const auto& entry : fs::directory_iterator(destination)) {
        const auto entry_path = entry.path();
        const auto source_path = source / entry_path.filename();
        if (fs::is_directory(entry_path)) {
            if (!fs::exists(source_path)) {
                fs::remove_all(entry_path);
            }
        }
        else {
            if (!fs::exists(source_path)) {
                fs::remove(entry_path);
            }
        }
    }
}
bool is_path_valid(const fs::path& path)
{
    try {
        const auto status = fs::status(path);
        return true;
    }
    catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        return false;
    }
}
void print_files_in_directory(const fs::path& directory)
{
    for (const auto& entry : fs::directory_iterator(directory)) {
        const auto& path = entry.path();
        if (fs::is_regular_file(path)) {
            std::cout << path.filename() << std::endl;
        }
    }
}
int main()
{
    const fs::path source_folder = "C:\\Users\\Cristi\\source\\repos\\sync\\sursa";
    const fs::path destination_folder = "C:\\Users\\Cristi\\source\\repos\\sync\\destinatie";
    if (is_path_valid(source_folder) == true) {
        std::cout << "valid path" << std::endl;
        std::cout << "files in folder are:" << std::endl;
        print_files_in_directory(source_folder);
        std::cout << "starting sync" << std::endl;
        sync_folders(source_folder, destination_folder);
        std::cout << "sync succesful";
    }
    else
        std::cout << "invalid path";
    /*std::cout << "Current path is " << fs::current_path() << '\n'; // (1)
    fs::current_path(fs::temp_directory_path()); // (3)
    std::cout << "Current path is " << fs::current_path() << '\n'; */
    return 0;
}
