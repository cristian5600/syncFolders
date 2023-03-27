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
#include <exception>


#include <openssl/md5.h>
#include <sstream>

namespace fs = std::filesystem;
/*
std::string calc_md5(const std::string& file_path) {
    std::ifstream file(file_path, std::ios::binary);

    if (!file) {
        throw std::runtime_error("Cannot open file");
    }

    MD5_CTX md5_ctx;
    MD5_Init(&md5_ctx);

    char buffer[1024];
    while (file.read(buffer, sizeof(buffer))) {
        MD5_Update(&md5_ctx, buffer, sizeof(buffer));
    }

    if (file.gcount() > 0) {
        MD5_Update(&md5_ctx, buffer, file.gcount());
    }

    unsigned char hash[MD5_DIGEST_LENGTH];
    MD5_Final(hash, &md5_ctx);

    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (auto b : hash) {
        ss << std::setw(2) << static_cast<int>(b);
    }

    return ss.str();
} */
void sync_folders(const fs::path& source, const fs::path& destination)
{
    try {
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
    catch (const std::exception& ex) {
        std::cout << ex.what();
    }
}
bool is_path_valid(const fs::path& path)
{
    try {
        const auto status = fs::status(path);
        return true;
    }
    catch (const std::filesystem::filesystem_error & ex) {
        std::cerr << ex.what() << std::endl;
        return false;
    }
}
void print_files_in_directory(const fs::path& directory)
{
    try {
        for (const auto& entry : fs::directory_iterator(directory)) {
            const auto& path = entry.path();
            if (fs::is_regular_file(path)) {
                std::cout << path.filename() << std::endl;
            }
        }
    }
    catch (const std::exception& ex) {
        std::cout << ex.what();
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
