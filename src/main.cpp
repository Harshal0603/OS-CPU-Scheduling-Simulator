#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "Scheduler.h"
#include "Task.h"

namespace {

// ---------- Input helpers (every value is validated; bad input never crashes the program) ----------

// Reads one line. If input has ended (Ctrl+D or piped input finished) we exit cleanly
// instead of looping forever on a closed stream.
std::string readLine() {
    std::string line;
    if (!std::getline(std::cin, line)) {
        std::cout << "\nInput ended. Exiting.\n";
        std::exit(0);
    }
    return line;
}

// Asks until the user types a whole number >= minValue.
// We read a full line and parse it ourselves so text like "abc" or "5x" is rejected cleanly.
int readInt(const std::string& prompt, int minValue) {
    while (true) {
        std::cout << prompt;
        const std::string line = readLine();
        try {
            size_t parsedChars = 0;
            const int value = std::stoi(line, &parsedChars);
            if (line.find_first_not_of(" \t\r", parsedChars) != std::string::npos) {
                throw std::invalid_argument("extra characters");
            }
            if (value < minValue) {
                std::cout << "Value must be at least " << minValue << ". Try again.\n";
                continue;
            }
            return value;
        } catch (const std::exception&) {
            std::cout << "Please enter a valid whole number.\n";
        }
    }
}

bool idAlreadyUsed(const std::vector<Task>& tasks, const std::string& id) {
    for (const Task& task : tasks) {
        if (task.id == id) {
            return true;
        }
    }
    return false;
}

std::string readTaskId(const std::vector<Task>& existingTasks, int taskNumber) {
    while (true) {
        std::cout << "Task " << taskNumber << " ID (no spaces): ";
        std::istringstream words(readLine());
        std::string id;
        std::string extra;
        if (!(words >> id) || (words >> extra)) {
            std::cout << "Please enter one word with no spaces.\n";
        } else if (idAlreadyUsed(existingTasks, id)) {
            std::cout << "That ID is already used. Choose another.\n";
        } else {
            return id;
        }
    }
}

void printTasks(const std::vector<Task>& tasks) {
    std::cout << "\n" << std::left << std::setw(8) << "Task" << std::right << std::setw(9) << "Arrival"
              << std::setw(7) << "Burst" << std::setw(10) << "Priority" << "\n";
    for (const Task& task : tasks) {
        std::cout << std::left << std::setw(8) << task.id << std::right << std::setw(9) << task.arrivalTime
                  << std::setw(7) << task.burstTime << std::setw(10) << task.priority << "\n";
    }
}

// ---------- Menu actions ----------

void enterTasks(std::vector<Task>& tasks) {
    const int count = readInt("Number of tasks: ", 1);
    std::vector<Task> newTasks;  // only replaces the old list once every value has been entered

    for (int i = 1; i <= count; ++i) {
        Task task;
        task.id = readTaskId(newTasks, i);
        task.arrivalTime = readInt("  Arrival time (>= 0): ", 0);
        task.burstTime = readInt("  Burst time (>= 1): ", 1);
        task.priority = readInt("  Priority (>= 1, 1 = highest): ", 1);
        task.remainingTime = task.burstTime;
        newTasks.push_back(task);
    }

    tasks = newTasks;
    printTasks(tasks);
}

// File format: one task per line -> id arrival burst priority
// Blank lines and text after '#' are ignored. If anything is wrong we report the
// line number and keep the previously loaded tasks unchanged.
void loadTasksFromFile(std::vector<Task>& tasks) {
    std::cout << "File path: ";
    const std::string path = readLine();

    std::ifstream file(path);
    if (!file) {
        std::cout << "Could not open file: " << path << "\n";
        return;
    }

    std::vector<Task> newTasks;
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
        const size_t commentStart = line.find('#');
        if (commentStart != std::string::npos) {
            line = line.substr(0, commentStart);
        }
        if (line.find_first_not_of(" \t\r") == std::string::npos) {
            continue;  // blank line
        }

        std::istringstream words(line);
        Task task;
        std::string extra;
        if (!(words >> task.id >> task.arrivalTime >> task.burstTime >> task.priority) || (words >> extra)) {
            std::cout << "Line " << lineNumber << ": expected 'id arrival burst priority'. File not loaded.\n";
            return;
        }
        if (task.arrivalTime < 0 || task.burstTime < 1 || task.priority < 1) {
            std::cout << "Line " << lineNumber << ": arrival must be >= 0, burst and priority >= 1. File not loaded.\n";
            return;
        }
        if (idAlreadyUsed(newTasks, task.id)) {
            std::cout << "Line " << lineNumber << ": duplicate task ID '" << task.id << "'. File not loaded.\n";
            return;
        }
        task.remainingTime = task.burstTime;
        newTasks.push_back(task);
    }

    if (newTasks.empty()) {
        std::cout << "The file contains no tasks. File not loaded.\n";
        return;
    }

    tasks = newTasks;
    printTasks(tasks);
}

void runAll(const std::vector<Task>& tasks) {
    const int quantum = readInt("Time quantum (>= 1): ", 1);

    std::vector<ScheduleResult> results;
    results.push_back(simulateFCFS(tasks));
    results.push_back(simulateSJF(tasks));
    results.push_back(simulateRoundRobin(tasks, quantum));
    results.push_back(simulatePriority(tasks));

    for (const ScheduleResult& result : results) {
        printResult(result);
    }
    printComparison(results);
}

void printMenu() {
    std::cout << "\n========================================\n";
    std::cout << " Embedded OS Task Scheduler Simulator\n";
    std::cout << "========================================\n\n";
    std::cout << "1. Enter Tasks\n";
    std::cout << "2. Load Tasks from File\n";
    std::cout << "3. Run FCFS\n";
    std::cout << "4. Run SJF\n";
    std::cout << "5. Run Round Robin\n";
    std::cout << "6. Run Priority Scheduling\n";
    std::cout << "7. Run All Algorithms\n";
    std::cout << "8. Exit\n\n";
}

}  // namespace

int main() {
    std::vector<Task> tasks;

    while (true) {
        printMenu();
        const int choice = readInt("Choice: ", 1);

        if (choice == 8) {
            std::cout << "Goodbye.\n";
            return 0;
        }
        if (choice > 8) {
            std::cout << "Invalid choice. Please pick a number from 1 to 8.\n";
            continue;
        }

        if (choice == 1) {
            enterTasks(tasks);
        } else if (choice == 2) {
            loadTasksFromFile(tasks);
        } else if (tasks.empty()) {
            // Choices 3-7 need tasks.
            std::cout << "No tasks loaded yet. Use option 1 or 2 first.\n";
        } else if (choice == 3) {
            printResult(simulateFCFS(tasks));
        } else if (choice == 4) {
            printResult(simulateSJF(tasks));
        } else if (choice == 5) {
            const int quantum = readInt("Time quantum (>= 1): ", 1);
            printResult(simulateRoundRobin(tasks, quantum));
        } else if (choice == 6) {
            printResult(simulatePriority(tasks));
        } else if (choice == 7) {
            runAll(tasks);
        }
    }
}
