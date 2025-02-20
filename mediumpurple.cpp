#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <map>
#include <ctime>

namespace fs = std::filesystem;

const std::string DEFAULT_SIZE_FILE = "file_sizes.txt";
const std::string BACKUP_FOLDER = "backup";
const std::string SCAN_FOLDER = "target_folder";
const std::string LOG_FILE = "integrity_log.txt";

// Function to get current timestamp as string
std::string get_current_time() {
    std::time_t now = std::time(nullptr);
    char buf[80];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d_%H-%M-%S", std::localtime(&now));
    return std::string(buf);
}

// Function to log events
void log_event(const std::string& message) {
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

// Function to generate file report with timestamp
void generate_file_report(const fs::path& directory, const std::string& output_file) {
    if (!fs::exists(directory)) {
        fs::create_directories(directory);
        log_event("Directory " + directory.string() + " did not exist and was created.");
    }
    
    std::ofstream file(output_file);
    if (!file) {
        log_event("Error: Unable to create " + output_file);
        return;
    }

    file << "Report generated on: " << get_current_time() << "\n";
    file << "----------------------------------------\n";

    for (const auto& entry : fs::recursive_directory_iterator(directory)) {
        if (fs::is_regular_file(entry.path())) {
            file << entry.path().string() << " " << get_file_size(entry.path()) << " bytes\n";
        }
    }
    log_event("File report generated in " + output_file);
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
