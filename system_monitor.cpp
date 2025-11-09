/*
 * A simple, read-only system monitor tool for Linux.
 *
 * This program mimics some of the functionality of 'top' by reading
 * data from the /proc filesystem. It is Linux-specific.
 *
 * Features:
 * - Displays total and free memory.
 * - Displays system load averages.
 * - Lists running processes with PID, state, memory usage (VmRSS), and command.
 * - Sorts processes by memory usage (descending).
 * - Refreshes the display every 2 seconds.
 *
 * Limitations (for simplicity):
 * - No real-time CPU % calculation (which requires diffing values over time).
 * - No user input or interactivity (like killing processes or changing sort order).
 *   A full implementation would use a library like ncurses.
 * - User name is not included (requires parsing /etc/passwd).
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <algorithm>
#include <unistd.h>     // For sleep()
#include <stdlib.h>     // For system()
#include <dirent.h>     // For reading /proc
#include <iomanip>      // For std::setw
#include <cctype>       // For isdigit

// Holds basic system-wide information
struct SystemInfo {
    long total_mem_kb = 0;
    long free_mem_kb = 0;
    std::string load_avg = "0.0 0.0 0.0";
};

// Holds information for a single process
struct ProcessInfo {
    int pid = 0;
    std::string name = "N/A";
    char state = '?';
    long vmrss_kb = 0; // Virtual Memory Resident Set Size
    std::string cmdline = "[kernel]";
};

// Helper function to parse a line from /proc/meminfo
// Example line: "MemTotal:       16301584 kB"
long getMemValue(const std::string& line) {
    std::stringstream ss(line);
    std::string key;
    long value = 0;
    ss >> key >> value;
    return value;
}

// Fetches system-wide info from /proc/meminfo and /proc/loadavg
SystemInfo getSystemInfo() {
    SystemInfo sys;
    std::string line;

    // Read memory info
    std::ifstream meminfo("/proc/meminfo");
    if (meminfo.is_open()) {
        while (std::getline(meminfo, line)) {
            if (line.rfind("MemTotal:", 0) == 0) {
                sys.total_mem_kb = getMemValue(line);
            } else if (line.rfind("MemFree:", 0) == 0) {
                sys.free_mem_kb = getMemValue(line);
            }
        }
        meminfo.close();
    }

    // Read load average
    std::ifstream loadavg("/proc/loadavg");
    if (loadavg.is_open()) {
        std::getline(loadavg, line); // Read the single line
        std::stringstream ss(line);
        std::string load1, load5, load15;
        ss >> load1 >> load5 >> load15;
        sys.load_avg = load1 + " " + load5 + " " + load15;
        loadavg.close();
    }

    return sys;
}

// Fetches specific info for a single process from its /proc/[pid] directory
ProcessInfo getProcessInfo(const std::string& pid) {
    ProcessInfo proc;
    proc.pid = std::stoi(pid);
    std::string line;

    // Read /proc/[pid]/status for Name, State, and VmRSS
    std::ifstream status_file("/proc/" + pid + "/status");
    if (status_file.is_open()) {
        while (std::getline(status_file, line)) {
            if (line.rfind("Name:", 0) == 0) {
                proc.name = line.substr(6); // Skip "Name: \t"
                // Trim leading whitespace
                proc.name.erase(0, proc.name.find_first_not_of(" \t"));
            } else if (line.rfind("State:", 0) == 0) {
                std::stringstream ss(line);
                std::string key;
                char state_char;
                ss >> key >> state_char;
                proc.state = state_char;
            } else if (line.rfind("VmRSS:", 0) == 0) {
                proc.vmrss_kb = getMemValue(line);
            }
        }
        status_file.close();
    }

    // Read /proc/[pid]/cmdline for the full command
    std::ifstream cmdline_file("/proc/" + pid + "/cmdline");
    if (cmdline_file.is_open()) {
        std::getline(cmdline_file, line);
        // Arguments are separated by null characters, replace them with spaces
        std::replace(line.begin(), line.end(), '\0', ' ');
        if (!line.empty()) {
            proc.cmdline = line;
        }
        cmdline_file.close();
    }

    return proc;
}

// Comparison function for sorting processes by memory usage (descending)
bool compareByMem(const ProcessInfo& a, const ProcessInfo& b) {
    return a.vmrss_kb > b.vmrss_kb;
}

// Enhanced UI display function
void display(const SystemInfo& sys, std::vector<ProcessInfo>& processes) {
    system("clear");  // Clear screen

    // Top border
    std::cout << "+" << std::string(86, '-') << "+" << std::endl;

    // Title, centered
    std::string title = "--- System Monitor (Linux) ---";
    int pad = (86 - title.length()) / 2;
    std::cout << "|" << std::string(pad, ' ') << title << std::string(86 - pad - title.length(), ' ') << "|" << std::endl;

    // Empty line
    std::cout << "|" << std::string(86, ' ') << "|" << std::endl;

    // System Summary
    std::cout << "| Memory: "
        << std::fixed << std::setprecision(2)
        << std::setw(7) << ((sys.total_mem_kb - sys.free_mem_kb)/1024.0/1024.0) << "G / "
        << std::setw(7) << (sys.total_mem_kb/1024.0/1024.0) << "G Used"
        << " (" << std::setw(6) << (sys.free_mem_kb/1024.0/1024.0) << "G Free)"
        << std::setw(23) << " "
        << "Load Avg (1,5,15 min): " << std::setw(12) << sys.load_avg
        << " |" << std::endl;

    std::cout << "| Total Processes: " << std::setw(67) << processes.size() << " |" << std::endl;

    // Empty line
    std::cout << "|" << std::string(86, ' ') << "|" << std::endl;

    // Table header for processes
    std::cout << "| "
        << std::setw(8) << std::left << "PID"
        << std::setw(20) << std::left << "NAME"
        << std::setw(4) << std::left << "S"
        << std::setw(12) << std::right << "MEM (MB)"
        << "  " << std::setw(36) << std::left << "COMMAND"
        << " |" << std::endl;
    std::cout << "|" << std::string(86, '-') << "|" << std::endl;

    // Sort and display
    std::sort(processes.begin(), processes.end(), compareByMem);
    int count = 0;
    for (const auto& proc : processes) {
        if (count++ >= 25) break;
        std::string name_short = proc.name.length() > 18 ? proc.name.substr(0, 18) + ".." : proc.name;
        std::string cmd_short = proc.cmdline.length() > 34 ? proc.cmdline.substr(0, 34) + "..." : proc.cmdline;

        std::cout << "| "
            << std::setw(8) << std::left << proc.pid
            << std::setw(20) << std::left << name_short
            << std::setw(4) << std::left << proc.state
            << std::setw(11) << std::right << std::fixed << std::setprecision(1) << (proc.vmrss_kb / 1024.0) << "M"
            << "  " << std::setw(36) << std::left << cmd_short
            << " |" << std::endl;
    }

    while (count++ < 25) {
        std::cout << "| " << std::setw(84) << " " << " |" << std::endl;
    }

    // Bottom border
    std::cout << "+" << std::string(86, '-') << "+" << std::endl;
}

// Helper to check if a string is all digits
bool isNumeric(const std::string& s) {
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

int main() {
    while (true) {
        SystemInfo sys = getSystemInfo();
        std::vector<ProcessInfo> processes;

        // Read /proc directory for process IDs
        DIR* proc_dir = opendir("/proc");
        if (proc_dir == NULL) {
            std::cerr << "Error: Could not open /proc" << std::endl;
            return 1;
        }

        struct dirent* entry;
        while ((entry = readdir(proc_dir)) != NULL) {
            // Check if the directory name is a number (a PID)
            if (entry->d_type == DT_DIR && isNumeric(entry->d_name)) {
                processes.push_back(getProcessInfo(entry->d_name));
            }
        }
        closedir(proc_dir);

        // Display all collected information
        display(sys, processes);

        // Wait for 2 seconds before refreshing
        sleep(2);
    }

    return 0;
    
}
