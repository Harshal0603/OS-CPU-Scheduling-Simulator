#include "Scheduler.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <limits>
#include <queue>

#include "GanttChart.h"

namespace {

const std::string IDLE_LABEL = "IDLE";

// Task indices sorted by arrival time. stable_sort keeps the original input
// order for tasks that arrive at the same time, so ties are predictable.
std::vector<int> indicesByArrival(const std::vector<Task>& tasks) {
    std::vector<int> order(tasks.size());
    for (size_t i = 0; i < order.size(); ++i) {
        order[i] = static_cast<int>(i);
    }
    std::stable_sort(order.begin(), order.end(), [&tasks](int a, int b) {
        return tasks[a].arrivalTime < tasks[b].arrivalTime;
    });
    return order;
}

void addSegment(ScheduleResult& result, const std::string& id, int start, int end) {
    result.gantt.push_back({id, start, end});
}

// Computes TAT, WT and the averages once every task has a completion time.
void finalizeResult(ScheduleResult& result) {
    int busyTime = 0;   // time the CPU was actually running tasks
    int totalTime = 0;  // time 0 until the last task finishes (idle time included)
    double totalWaiting = 0.0;
    double totalTurnaround = 0.0;

    for (Task& task : result.tasks) {
        task.turnaroundTime = task.completionTime - task.arrivalTime;
        task.waitingTime = task.turnaroundTime - task.burstTime;
        busyTime += task.burstTime;
        totalTime = std::max(totalTime, task.completionTime);
        totalWaiting += task.waitingTime;
        totalTurnaround += task.turnaroundTime;
    }

    const double count = static_cast<double>(result.tasks.size());
    result.avgWaitingTime = totalWaiting / count;
    result.avgTurnaroundTime = totalTurnaround / count;
    result.cpuUtilization = totalTime > 0 ? (busyTime * 100.0) / totalTime : 0.0;
}

// Is task a a better pick than task b? Lower key wins (shortest burst for SJF,
// lowest priority number for Priority). If keys are equal, the earlier arrival wins.
// If that is also equal, the caller keeps the task that comes first in the input.
bool isBetter(const Task& a, const Task& b, bool chooseByBurst) {
    const int keyA = chooseByBurst ? a.burstTime : a.priority;
    const int keyB = chooseByBurst ? b.burstTime : b.priority;
    if (keyA != keyB) {
        return keyA < keyB;
    }
    return a.arrivalTime < b.arrivalTime;
}

// SJF and Priority differ only in HOW they pick the next task, so they share this loop.
ScheduleResult runNonPreemptive(const std::string& name, const std::vector<Task>& input, bool chooseByBurst) {
    ScheduleResult result;
    result.algorithmName = name;
    result.tasks = input;
    std::vector<Task>& tasks = result.tasks;

    std::vector<bool> finished(tasks.size(), false);
    int finishedCount = 0;
    int currentTime = 0;

    while (finishedCount < static_cast<int>(tasks.size())) {
        // Look only at tasks that have already arrived and are not finished.
        int chosen = -1;
        for (size_t i = 0; i < tasks.size(); ++i) {
            if (finished[i] || tasks[i].arrivalTime > currentTime) {
                continue;
            }
            if (chosen == -1 || isBetter(tasks[i], tasks[chosen], chooseByBurst)) {
                chosen = static_cast<int>(i);
            }
        }

        if (chosen == -1) {
            // Nothing is ready: the CPU idles until the next task arrives.
            int nextArrival = std::numeric_limits<int>::max();
            for (size_t i = 0; i < tasks.size(); ++i) {
                if (!finished[i]) {
                    nextArrival = std::min(nextArrival, tasks[i].arrivalTime);
                }
            }
            addSegment(result, IDLE_LABEL, currentTime, nextArrival);
            currentTime = nextArrival;
            continue;
        }

        // Non-preemptive: once started, the task runs until it is done.
        Task& task = tasks[chosen];
        addSegment(result, task.id, currentTime, currentTime + task.burstTime);
        currentTime += task.burstTime;
        task.remainingTime = 0;
        task.completionTime = currentTime;
        finished[chosen] = true;
        ++finishedCount;
    }

    finalizeResult(result);
    return result;
}

}  // namespace

ScheduleResult simulateFCFS(const std::vector<Task>& input) {
    ScheduleResult result;
    result.algorithmName = "FCFS";
    result.tasks = input;
    std::vector<Task>& tasks = result.tasks;

    int currentTime = 0;
    for (int index : indicesByArrival(tasks)) {
        Task& task = tasks[index];

        // If the CPU is free before this task arrives, it sits idle.
        if (currentTime < task.arrivalTime) {
            addSegment(result, IDLE_LABEL, currentTime, task.arrivalTime);
            currentTime = task.arrivalTime;
        }

        addSegment(result, task.id, currentTime, currentTime + task.burstTime);
        currentTime += task.burstTime;
        task.remainingTime = 0;
        task.completionTime = currentTime;
    }

    finalizeResult(result);
    return result;
}

ScheduleResult simulateSJF(const std::vector<Task>& input) {
    return runNonPreemptive("SJF", input, true);
}

ScheduleResult simulatePriority(const std::vector<Task>& input) {
    return runNonPreemptive("Priority", input, false);
}

ScheduleResult simulateRoundRobin(const std::vector<Task>& input, int timeQuantum) {
    ScheduleResult result;
    result.algorithmName = "Round Robin";
    result.timeQuantum = timeQuantum;
    result.tasks = input;
    std::vector<Task>& tasks = result.tasks;

    const std::vector<int> arrivalOrder = indicesByArrival(tasks);
    size_t nextArrival = 0;  // position in arrivalOrder of the next task that has not arrived yet
    std::queue<int> readyQueue;
    int finishedCount = 0;
    int currentTime = 0;

    // Moves every task that has arrived by 'currentTime' into the ready queue.
    auto admitArrivals = [&]() {
        while (nextArrival < arrivalOrder.size() &&
               tasks[arrivalOrder[nextArrival]].arrivalTime <= currentTime) {
            readyQueue.push(arrivalOrder[nextArrival]);
            ++nextArrival;
        }
    };

    for (Task& task : tasks) {
        task.remainingTime = task.burstTime;
    }

    while (finishedCount < static_cast<int>(tasks.size())) {
        admitArrivals();

        if (readyQueue.empty()) {
            // Nobody is ready: jump forward to the next arrival and record idle time.
            const int nextTime = tasks[arrivalOrder[nextArrival]].arrivalTime;
            addSegment(result, IDLE_LABEL, currentTime, nextTime);
            currentTime = nextTime;
            continue;
        }

        const int index = readyQueue.front();
        readyQueue.pop();
        Task& task = tasks[index];

        // Run for one quantum, or less if the task needs less than a full quantum.
        const int runTime = std::min(task.remainingTime, timeQuantum);
        addSegment(result, task.id, currentTime, currentTime + runTime);
        currentTime += runTime;
        task.remainingTime -= runTime;

        // Convention: tasks that arrived while this one was running go into the queue
        // BEFORE the unfinished task is put back at the end. This is the standard rule.
        admitArrivals();

        if (task.remainingTime > 0) {
            readyQueue.push(index);
        } else {
            task.completionTime = currentTime;
            ++finishedCount;
        }
    }

    finalizeResult(result);
    return result;
}

void printResult(const ScheduleResult& result) {
    std::cout << "\n========================================\n";
    std::cout << " " << result.algorithmName;
    if (result.timeQuantum > 0) {
        std::cout << " (Time Quantum = " << result.timeQuantum << ")";
    }
    std::cout << "\n========================================\n";

    // Execution order: the order tasks got the CPU (IDLE is left out here, it shows in the Gantt chart).
    std::cout << "Execution order: ";
    bool first = true;
    for (const GanttSegment& seg : result.gantt) {
        if (seg.taskId == IDLE_LABEL) {
            continue;
        }
        if (!first) {
            std::cout << " -> ";
        }
        std::cout << seg.taskId;
        first = false;
    }
    std::cout << "\n\nGantt chart:\n";
    printGantt(result.gantt);

    std::cout << "\n" << std::left << std::setw(8) << "Task" << std::right
              << std::setw(9) << "Arrival" << std::setw(7) << "Burst" << std::setw(10) << "Priority"
              << std::setw(6) << "CT" << std::setw(6) << "TAT" << std::setw(6) << "WT" << "\n";
    for (const Task& task : result.tasks) {
        std::cout << std::left << std::setw(8) << task.id << std::right
                  << std::setw(9) << task.arrivalTime << std::setw(7) << task.burstTime
                  << std::setw(10) << task.priority << std::setw(6) << task.completionTime
                  << std::setw(6) << task.turnaroundTime << std::setw(6) << task.waitingTime << "\n";
    }

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\nAverage Waiting Time    : " << result.avgWaitingTime << "\n";
    std::cout << "Average Turnaround Time : " << result.avgTurnaroundTime << "\n";
    std::cout << "CPU Utilization         : " << result.cpuUtilization << "%\n";
}

void printComparison(const std::vector<ScheduleResult>& results) {
    std::cout << "\n========================================\n";
    std::cout << " Comparison\n";
    std::cout << "========================================\n";
    std::cout << std::left << std::setw(16) << "Algorithm" << std::setw(12) << "Avg WT"
              << std::setw(12) << "Avg TAT" << "CPU Utilization\n";
    std::cout << "---------------------------------------------------------\n";
    std::cout << std::fixed << std::setprecision(2);
    for (const ScheduleResult& r : results) {
        std::cout << std::left << std::setw(16) << r.algorithmName << std::setw(12) << r.avgWaitingTime
                  << std::setw(12) << r.avgTurnaroundTime << r.cpuUtilization << "%\n";
    }
}
