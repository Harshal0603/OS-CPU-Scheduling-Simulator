#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include "Task.h"

// Each simulate function takes the task list by const reference and works on
// its own copy, so running one algorithm never affects the next one.
ScheduleResult simulateFCFS(const std::vector<Task>& tasks);
ScheduleResult simulateSJF(const std::vector<Task>& tasks);                      // non-preemptive
ScheduleResult simulateRoundRobin(const std::vector<Task>& tasks, int timeQuantum);
ScheduleResult simulatePriority(const std::vector<Task>& tasks);                 // non-preemptive, 1 = highest

void printResult(const ScheduleResult& result);
void printComparison(const std::vector<ScheduleResult>& results);

#endif
