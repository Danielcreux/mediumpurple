#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <map>
#include <ctime>
#include <iomanip>

namespace fs = std::filesystem;

const std::string DEFAULT_SIZE_FILE = "file_sizes.txt";
const std::string BACKUP_FOLDER = "backup";
const std::string SCAN_FOLDER = "target_folder";
const std::string LOG_FOLDER = "logs";
const std::string LOG_FILE = LOG_FOLDER + "/integrity_log.txt";

// Function to get current timestamp as string
std::string get_current_time() {
    std::time_t now = std::time(nullptr);
    char buf[80];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d_%H-%M-%S", std::localtime(&now));
    return std::string(buf);
}

// Function to format file modification time
std::string get_last_write_time(const fs::path& file_path) {
    auto ftime = fs::last_write_time(file_path);
    auto cftime = decltype(ftime)::clock::to_time_t(ftime);
    std::tm* tm = std::localtime(&cftime);
    char buf[80];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm);
    return std::string(buf);
}

// Function to log events
void log_event(const std::string& message) {
    if (!fs::exists(LOG_FOLDER)) {
        fs::create_directories(LOG_FOLDER);
    }
    std::ofstream log(LOG_FILE, std::ios::app);
    if (log) {
        log << "[" << std::time(nullptr) << "] " << message << "\n";
        std::cout << message << std::endl;
    }
}

// Function to get file size
std::uintmax_t get_file_size(const fs::path& file_path) {
    return fs::exists(file_path) ? fs::file_size(file_path) : 0;
}

// Function to generate detailed file report
void generate_file_report(const fs::path& directory, const std::string& output_file) {
    if (!fs::exists(LOG_FOLDER)) {
        fs::create_directories(LOG_FOLDER);
    }
    std::string full_output_path = LOG_FOLDER + "/" + output_file;
    
    if (!fs::exists(directory)) {
        fs::create_directories(directory);
        log_event("Directory " + directory.string() + " did not exist and was created.");
    }
    
    std::ofstream file(full_output_path);
    if (!file) {
        log_event("Error: Unable to create " + full_output_path);
        return;
    }

    file << "Report generated on: " << get_current_time() << "\n";
    file << "------------------------------------------------------------\n";
    file << std::left << std::setw(50) << "File Path" << std::setw(15) << "Size (bytes)" << std::setw(25) << "Last Modified" << "\n";
    file << "------------------------------------------------------------\n";

    for (const auto& entry : fs::recursive_directory_iterator(directory)) {
        if (fs::is_regular_file(entry.path())) {
            file << std::left << std::setw(50) << entry.path().string() 
                 << std::setw(15) << get_file_size(entry.path()) 
                 << std::setw(25) << get_last_write_time(entry.path()) << "\n";
        }
    }
    
    log_event("File report generated in " + full_output_path);
}

// Main function to handle user input
int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        std::cout << "Usage: " << argv[0] << " [report] [optional: output filename]\n";
        return 1;
    }

    std::string action = argv[1];
    std::string output_file = (argc == 3) ? argv[2] : "file_report_" + get_current_time() + ".txt";

    if (action == "report") {
        generate_file_report(SCAN_FOLDER, output_file);
    } else {
        std::cout << "Invalid action. Use: report.\n";
        return 1;
    }

    return 0;
}
