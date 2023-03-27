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
#include <thread>
#include <chrono>


#include <openssl/md5.h>
#include <sstream>
#include <fstream>

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
void sync_folders(const fs::path& source, const fs::path& destination , std::ofstream& logFile)
{
    try {
        for (const auto& entry : fs::directory_iterator(source)) {
            const auto entry_path = entry.path();
            const auto destination_path = destination / entry_path.filename();
            if (fs::is_directory(entry_path)) {
                if (!fs::exists(destination_path)) {
                    fs::create_directory(destination_path);
                    logFile << "Created folder: " << destination_path.string() << std::endl;
                    std::cout << "Created folder: " << destination_path.string() << std::endl;
                }
                sync_folders(entry_path, destination_path, logFile);
            }
            else if(fs::is_regular_file(entry_path)){
                if (fs::exists(destination_path)) {
                    if (fs::last_write_time(entry_path) > fs::last_write_time(destination_path)) {
                        bool works = fs::copy_file(entry_path, destination_path, fs::copy_options::overwrite_existing);
                        if (works) {
                            logFile << "File copied: " << entry_path.string() << " -> " << destination_path.string() << std::endl;
                            // logFile << "File copied: " << entry_path.string() << " -> " << destination_path.string() << std::endl;
                            std::cout << "File copied: " << entry_path.string() << " -> " << destination_path.string() << std::endl;
                        }
                        else {
                            throw std::exception("");
                        }
                    }
                }
                else {
                    fs::copy_file(entry_path, destination_path, fs::copy_options::overwrite_existing);
                    logFile << "File copied: " << entry_path.string() << " -> " << destination_path.string() << std::endl;
                    std::cout << "File copied: " << entry_path.string() << " -> " << destination_path.string() << std::endl;
                   // logFile << "File copied: " << entry_path.string() << " -> " << destination_path.string() << std::endl;
                }
            }
        }

        for (const auto& entry : fs::directory_iterator(destination)) {
            const auto entry_path = entry.path();
            const auto source_path = source / entry_path.filename();
            if (fs::is_directory(entry_path)) {
                if (!fs::exists(source_path)) {
                    fs::remove_all(entry_path);
                    logFile << "Removed directory: " << entry_path.string();
                }
            }
            else {
                if (!fs::exists(source_path)) {
                    fs::remove(entry_path);
                    logFile << "Removed file: " << entry_path.string();
                }
            }
        }
    }
    catch (const std::exception& ex) {
        std::cout << ex.what();
        logFile << "Error: " << ex.what() << std::endl;
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
int main(int argc, char** argv)
{
    if (argc != 5) {
        std::cout << "Usage: " << argv[0] << " <source_folder> <destination_folder> <log_file_path> <interval_seconds>" << std::endl;
        return 1;
    }
    std::ofstream logFile;
    const auto source_folder = fs::path(argv[1]);
    const auto destination_folder = fs::path(argv[2]);
    const auto logFile_path = fs::path(argv[3]);
    const auto interval_seconds = std::stoi(argv[4]);

    if (!fs::exists(source_folder)) {
        std::cout << "Source folder does not exist" << std::endl;
        return 1;
    }

    if (!fs::exists(destination_folder)) {
        std::cout << "Destination folder does not exist" << std::endl;
        return 1;
    }
    if (!fs::exists(logFile_path)) {
        std::cout << "Logfile does not exist" << std::endl;
        return 1;
    }
    else {
        
        logFile.open(logFile_path); 
    }

    std::cout << "Syncing folders every " << interval_seconds << " seconds" << std::endl;

    /*const int n_seconds = 10;
    const fs::path source_folder = "C:\\Users\\Cristi\\source\\repos\\sync\\sursa";
    const fs::path destination_folder = "C:\\Users\\Cristi\\source\\repos\\sync\\destinatie";
    std::ofstream logFile;
    logFile.open("C:\\Users\\Cristi\\source\\repos\\sync\\log.txt"); */
    
    // logFile.flush(); 
    if (is_path_valid(source_folder) == true) {
        std::cout << "valid path" << std::endl;
        std::cout << "files in folder are:" << std::endl;
        print_files_in_directory(source_folder);
        std::cout << "starting sync" << std::endl; 
        sync_folders(source_folder, destination_folder,logFile);
        std::cout << "sync succesful";
    }
    else
        logFile << "invalid path";
    while (true) {
        try {
            if (!fs::exists(source_folder)) {
                std::cout << "Source folder does not exist" << std::endl;
                logFile << "Source folder does not exist" << std::endl;
                return 1;
            }

            if (!fs::exists(destination_folder)) {
                std::cout << "Destination folder does not exist" << std::endl;
                logFile << "Destination folder does not exist" << std::endl;
                return 1;
            }
            if (!fs::exists(logFile_path)) {
                std::cout << "Logfile does not exist" << std::endl;
                logFile<< "Logfile does not exist" << std::endl;
                return 1;
            }



            auto start_time = std::chrono::high_resolution_clock::now();
            std::cout << "starting sync" << std::endl;//<< std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << std::endl;
            sync_folders(source_folder, destination_folder, logFile);
            std::cout << "sync succesful" << std::endl;
            auto end_time = std::chrono::high_resolution_clock::now();
            auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time); // Calculate duration
            auto current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()); // Get current time
            //logFile << "Sync completed at " << std::ctime(&current_time) << " Duration: " << elapsed_time.count() << "ms" << std::endl;
           // std::cout<< "Sync completed at " << std::ctime(&current_time) << " Duration: " << elapsed_time.count() << "ms" << std::endl;
            // sleep for remaining time until n_seconds has elapsed

            //std::cout << "Sync completed at " << std::ctime(&end_time) << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds(interval_seconds));
        }
        catch (const std::exception& ex) {
            std::cout << ex.what();
            logFile << "Error: " << ex.what() << std::endl;
        }
    }
    logFile.close();
    return 0;
}
