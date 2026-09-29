#ifndef TASK_H
#define TASK_H

#include <string>
#include <vector>

// One task (process) that wants CPU time.
struct Task {
    std::string id;
    int arrivalTime = 0;     // when the task enters the system
    int burstTime = 0;       // total CPU time the task needs
    int remainingTime = 0;   // CPU time still needed (used by Round Robin)
    int priority = 0;        // 1 = highest priority, larger number = lower priority
    int completionTime = 0;  // CT: when the task finishes
    int turnaroundTime = 0;  // TAT = CT - arrival
    int waitingTime = 0;     // WT  = TAT - burst
};

// One block of the Gantt chart: the CPU ran taskId from startTime to endTime.
// taskId is "IDLE" when the CPU had nothing to run.
struct GanttSegment {
    std::string taskId;
    int startTime;
    int endTime;
};

// Everything one algorithm produces, so results can be printed and compared.
struct ScheduleResult {
    std::string algorithmName;
    int timeQuantum = 0;                // only used by Round Robin (0 = not applicable)
    std::vector<Task> tasks;            // tasks with CT / TAT / WT filled in
    std::vector<GanttSegment> gantt;    // in time order
    double avgWaitingTime = 0.0;
    double avgTurnaroundTime = 0.0;
    double cpuUtilization = 0.0;        // percentage
};

#endif
