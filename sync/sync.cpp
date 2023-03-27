#include <iostream>

#include <filesystem>

#include <exception>

#include <thread>

#include <chrono>

#include <sstream>

#include <fstream>

namespace fs = std::filesystem;

void sync_folders(const fs::path& source,
    const fs::path& destination, std::ofstream& logFile) {
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
            else if (fs::is_regular_file(entry_path)) {
                if (fs::exists(destination_path)) {
                    if (fs::last_write_time(entry_path) > fs::last_write_time(destination_path)) {
                        bool works = fs::copy_file(entry_path, destination_path, fs::copy_options::overwrite_existing);
                        if (works) {
                            logFile << "File copied: " << entry_path.string() << " -> " << destination_path.string() << std::endl;

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
bool is_path_valid(const fs::path& path) {
    try {
        const auto status = fs::status(path);
        return true;
    }
    catch (const std::filesystem::filesystem_error& ex) {
        std::cerr << ex.what() << std::endl;
        return false;
    }
}
void print_files_in_directory(const fs::path& directory) {
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
int main(int argc, char** argv) {
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
        logFile << "Logfile does not exist" << std::endl;
        return 1;
    }
    else {

        logFile.open(logFile_path);
    }

    std::cout << "Syncing folders every " << interval_seconds << " seconds" << std::endl;
    if (is_path_valid(source_folder) == true) {
        std::cout << "valid path" << std::endl;
        std::cout << "files in source folder are:" << std::endl;
        print_files_in_directory(source_folder);
        std::cout << "files in destination folder are:" << std::endl;
        print_files_in_directory(destination_folder);
        std::cout << "starting sync" << std::endl;
        sync_folders(source_folder, destination_folder, logFile);
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
                logFile << "Logfile does not exist" << std::endl;
                return 1;
            }

            auto start_time = std::chrono::high_resolution_clock::now();
            std::cout << "starting sync" << std::endl;
            sync_folders(source_folder, destination_folder, logFile);
            std::cout << "sync succesful" << std::endl;
            auto end_time = std::chrono::high_resolution_clock::now();
            auto elapsed_time = std::chrono::duration_cast <std::chrono::milliseconds> (end_time - start_time); // Calculate duration
            auto current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()); // Get current time

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