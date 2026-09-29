# Interview Questions and Answers

Simple answers you can say in your own words. Read your code once more before the interview - you should be able to point to the function that does each thing.

**1. What is CPU scheduling?**
The CPU can run only one task at a time (per core). CPU scheduling is the OS deciding which ready task gets the CPU next, and for how long. A good scheduling policy keeps the CPU busy and keeps waiting times reasonable.

**2. Why did you implement FCFS?**
It is the simplest algorithm and works as a baseline. Every other algorithm can be compared against it, and it clearly shows the convoy effect (short tasks stuck behind a long one).

**3. What is the difference between FCFS and SJF?**
FCFS picks the task that arrived first. SJF picks the ready task with the smallest burst time. SJF usually gives a lower average waiting time, but it needs to know burst times in advance and can starve long tasks.

**4. Why is Round Robin useful?**
It gives every task a fair turn on the CPU, so no task waits forever and the system responds quickly to everyone. That is why time-sharing systems use RR-style scheduling.

**5. What is a time quantum?**
The maximum time a task can run before the scheduler switches to the next task. In my program the user enters it, and each slice runs for `min(remaining time, quantum)`.

**6. What happens when a task's remaining time becomes zero?**
The task is finished. I record its completion time at that moment, count it as done, and do NOT put it back in the queue. (Function: `simulateRoundRobin` in `Scheduler.cpp`.)

**7. What is turnaround time?**
The total time from when a task arrives until it finishes: `TAT = Completion Time - Arrival Time`.

**8. What is waiting time?**
The time a task spends waiting in the ready queue instead of running: `WT = TAT - Burst Time`.

**9. What is CPU utilization?**
The percentage of time the CPU was busy: `busy time / total time * 100`. If tasks arrive late and the CPU has gaps with nothing to run, utilization drops below 100%. I count total time from 0 to the last completion.

**10. What is preemptive scheduling?**
The OS can take the CPU away from a running task before it finishes (for example, when its time quantum ends or a higher-priority task arrives). Round Robin is preemptive in my project.

**11. Which algorithms in this project are non-preemptive?**
FCFS, SJF and Priority. Once they start a task, it runs until it finishes.

**12. Why is Round Robin implemented using a queue?**
Round Robin means "take turns in order". A queue is first-in-first-out, so a task that just used its slice goes to the back and everyone else gets a turn before it runs again.

**13. How is CPU idle time handled?**
If no task has arrived yet, the CPU cannot run anything. I jump the clock to the next arrival time and record an `IDLE` block in the Gantt chart. Idle time counts in total time, so it lowers CPU utilization.

**14. What data structures did you use?**
`vector` for the task list and Gantt chart segments, `queue` for the Round Robin ready queue, `struct`s for `Task`, `GanttSegment` and `ScheduleResult`, and `stable_sort` on a vector of indices to order tasks by arrival while keeping input order for ties.

**15. How could this simulator be extended toward an embedded system?**
Honest answer: it is only a simulation today. Possible next steps: preemptive priority scheduling (like an RTOS), simulating interrupts as events that trigger scheduling, adding context-switch overhead, simulating memory allocation for tasks, and porting the scheduling logic to run real tasks on a Linux system or a microcontroller RTOS.

**Bonus 16. What is starvation, and can it happen in your project?**
Starvation is when a task waits forever because others keep getting picked first. It can happen in SJF (long tasks) and Priority (low-priority tasks) if new tasks keep arriving. FCFS and Round Robin do not starve tasks. I did not implement aging, which is the usual fix.

**Bonus 17. How did you make sure your results are correct?**
I hand-calculated the sample workload, checked all algorithms against a separate reference script, and saved the verified outputs as test cases that `make test` compares against.
