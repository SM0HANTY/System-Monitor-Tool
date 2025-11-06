#💻 Simple Linux System Monitor 🚀

📊 Monitor. Analyze. Optimize. A lightweight, terminal-based system monitor built with C++—featuring real-time stats, process sorting, and zero external dependencies! 🐧⚡

🎯 Objective

The goal of this project is to create a lightweight, terminal-based utility that provides a real-time, at-a-glance overview of system memory, load, and the most resource-intensive processes.

✨ Core Features

📈 Live System Vitals – Real-time memory usage, free memory, and system load averages!

📋 Process List – See PID, Name, State, and Memory (VmRSS) for the top 25 processes!

💾 Memory-First Sorting – Automatically shows the most memory-hungry processes at the top!

⏱️ Real-time Updates – Refreshes every 2 seconds to keep you in the loop!

🛠️ Tech Stack & Tools

🐧 Linux – Built for and runs exclusively on the Linux /proc filesystem.

⚙️ C++ – Written in standard C++11.

📁 ProcFS – All data is read directly from /proc (no external libraries!).

⌨️ Terminal UI – A classic, lightweight console interface.

⚠️ The Fine Print (Limitations)

This is a simplified educational tool. For a full-featured monitor, tools like htop are recommended.

🐧 Linux Only: This tool reads /proc and will not work on Windows or macOS.

👀 Read-Only: You can look, but you can't touch! (No killing processes, changing sorting, or scrolling... yet!)

📉 No Real-time CPU%: This feature is on the future goals list!

👻 Flicker-Warning: Uses system("clear") which can cause flickering. A ncurses UI would fix this!

🚀 Get it Running

Create the file:

nano system_monitor.cpp


(Paste the C++ code from system_monitor.cpp into this file, then save & exit)

Compile it:

g++ system_monitor.cpp -o monitor -std=c++11


Run it:

./monitor


Stop it:
Press Ctrl+C in the terminal.

🔮 Level Up! (Future Goals)

This simple tool could be expanded with more advanced features:

🎨 UI Overhaul – Integrate ncurses for a smooth, flicker-free, and interactive dashboard!

🖱️ Full Interactivity – Add process killing, new sorting options (by CPU, PID), and scrolling!

🧠 CPU Stats – Implement the logic to sample CPU times and display a real-time CPU % for each process.

🧑 User Display – Show which user is running each process.

⚙️ How it Works

The monitor loops every 2 seconds, reading and parsing plain text files:

📄 /proc/meminfo - For global memory stats.

📄 /proc/loadavg - For system load.

📁 /proc/[PID]/ - Scans all process directories for:

/proc/[PID]/status (Name, State, VmRSS)

/proc/[PID]/cmdline (The full command)

📢 Stay Tuned! Feel free to contribute, fork, or suggest improvements!
