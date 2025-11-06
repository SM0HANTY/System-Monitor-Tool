Simple Linux System Monitor

This is a basic, read-only system monitoring tool written in C++ for Linux. It mimics some of the core functionality of the top command by reading data directly from the /proc filesystem.

Features

System Overview: Displays real-time information on:

Total and used memory (in Gigabytes).

Free memory (in Gigabytes).

System load averages (1, 5, and 15-minute intervals).

Process List: Lists currently running processes with:

Process ID (PID)

Process Name

State (e.g., 'R' for running, 'S' for sleeping)

Memory Usage (VmRSS in Megabytes)

The full command that launched the process.

Sorting: Automatically sorts the process list by memory usage (VmRSS) in descending order, showing the most memory-intensive processes first.

Real-time Updates: The display automatically refreshes every 2 seconds.

Limitations

This is a simplified educational tool and has some limitations compared to full-featured monitors like top or htop:

Linux Only: It relies on the Linux-specific /proc filesystem and will not compile or run on Windows or macOS.

Read-Only: You cannot interact with the monitor (e.g., kill processes, change sorting, scroll).

No Real-time CPU%: Calculating real-time CPU percentage requires comparing process CPU times between intervals. This functionality is omitted for simplicity.

Basic UI: The tool uses system("clear") to refresh the screen, which can cause flickering. A more advanced tool would use a library like ncurses for a stable terminal UI.

Requirements

A Linux-based operating system.

A C++ compiler (e.g., g++).

How to Compile and Run

Create the file:
Open your terminal and use a text editor like nano to create the file:

nano system_monitor.cpp


Paste the C++ code into this file. Save and exit (Ctrl+O to save, Ctrl+X to exit).

Compile the program:
In the same terminal, run the following command to compile the code. This creates an executable file named monitor.

g++ system_monitor.cpp -o monitor -std=c++11


Run the monitor:
Execute the compiled program:

./monitor


Exit the monitor:
Press Ctrl+C in the terminal to stop the program.

Code Overview

The program works by reading and parsing plain text files from the /proc directory:

/proc/meminfo: Used to get system-wide memory statistics (MemTotal, MemFree).

/proc/loadavg: Used to get the system load averages.

/proc/[PID]/: The program scans the /proc directory for subdirectories named only with numbers. Each number is a Process ID (PID).

/proc/[PID]/status: Read to find the process Name, State, and VmRSS (memory usage).

/proc/[PID]/cmdline: Read to get the full command that started the process.

All data is collected, stored in simple structs, sorted using std::sort, and then printed to the console before the screen is cleared and the loop repeats.
