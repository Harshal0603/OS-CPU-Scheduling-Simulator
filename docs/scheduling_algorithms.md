# Scheduling Algorithms

This document explains the four algorithms implemented in this simulator, in simple language.
All of them are **simulations**: they calculate what *would* happen, they do not schedule real processes.

**Rules used everywhere in this project**

- Priority: **1 = highest priority**, larger number = lower priority.
- Tie-breaking: if two tasks look equally good, the one that **arrived earlier** goes first. If they also arrived together, the one **entered first** goes first.
- If no task is ready, the CPU is **IDLE** until the next task arrives.

## The metrics

| Term | Meaning | Formula |
|------|---------|---------|
| **CT** (Completion Time) | The time at which the task finishes | read from the schedule |
| **TAT** (Turnaround Time) | Total time from arrival until finish | `TAT = CT - Arrival` |
| **WT** (Waiting Time) | Time spent waiting in the ready queue, not running | `WT = TAT - Burst` |
| **CPU Utilization** | Percentage of time the CPU was doing real work | `Busy time / Total time * 100` |

*Busy time* = sum of all burst times. *Total time* = from time 0 until the last task finishes, so idle time at the start or in the middle counts against utilization.

Lower average WT and TAT are generally better, but **no algorithm is best for every workload** - that is why the simulator shows a comparison table instead of picking a winner.

---

## 1. FCFS (First Come First Serve)

- **What it does:** Runs tasks in the order they arrive, like a queue at a ticket counter.
- **How it works:** Sort tasks by arrival time. Run each one completely. If the next task has not arrived yet, the CPU idles until it does.
- **Preemptive?** No. A running task is never interrupted.
- **Advantages:** Very simple. Fair in the sense of "first come, first served". No starvation.
- **Limitations:** *Convoy effect* - one long task at the front makes all the short tasks behind it wait a long time.
- **Complexity of this implementation:** `O(n log n)` for sorting, then `O(n)` to run.

## 2. SJF (Shortest Job First, non-preemptive)

- **What it does:** Whenever the CPU becomes free, it picks the ready task with the **smallest burst time**.
- **How it works:** At each decision point, look only at tasks that have already arrived and are unfinished. Choose the smallest burst (ties: earlier arrival). If none have arrived, the CPU idles until the next arrival.
- **Preemptive?** No. Once a task starts, it runs to completion, even if a shorter task arrives meanwhile.
- **Advantages:** Gives the lowest average waiting time among non-preemptive algorithms when all tasks are available together.
- **Limitations:** Needs to know burst times in advance (real systems can only estimate them). Long tasks can **starve** if short tasks keep arriving.
- **Complexity of this implementation:** `O(n^2)` - for each of the `n` tasks we scan up to `n` tasks to pick the best.

## 3. Round Robin

- **What it does:** Gives every ready task a fixed slice of CPU time called the **time quantum**, then moves on to the next one in line.
- **How it works:** Keep a **queue** of ready tasks. Take the task at the front, run it for `min(remaining time, quantum)`. Tasks that arrived during that slice join the queue first, then the unfinished task goes to the back. Repeat until all tasks finish. Idle time is recorded when the queue is empty and more tasks are still to arrive.
- **Preemptive?** Yes. A task is interrupted when its quantum ends (this is what the *timer interrupt* does in a real OS).
- **Advantages:** Fair to everyone, no starvation, good response time for interactive systems.
- **Limitations:** Average waiting/turnaround time is often higher than SJF. A very small quantum causes many context switches (wasted time in real systems); a very large quantum makes it behave like FCFS. This simulator does not model context-switch cost.
- **Complexity of this implementation:** `O(n log n + S)`, where `S` is the number of time slices (about the sum of `ceil(burst / quantum)`). Each slice uses `O(1)` queue operations.

## 4. Priority Scheduling (non-preemptive)

- **What it does:** Whenever the CPU becomes free, it picks the ready task with the **highest priority** (the **lowest number**; priority 1 is the highest).
- **How it works:** Same loop as SJF, but the key is the priority number instead of the burst time. Ties: earlier arrival first.
- **Preemptive?** No. A high-priority task that arrives while another is running must wait.
- **Advantages:** Important tasks are served first - useful when some work is more urgent (for example, a sensor-handling task vs. logging).
- **Limitations:** Low-priority tasks can **starve**. Real systems use *aging* (slowly raising the priority of waiting tasks) to avoid this; that is not implemented here.
- **Complexity of this implementation:** `O(n^2)`, same reason as SJF.

---

## Preemptive vs non-preemptive (quick summary)

| Algorithm | Preemptive? | Picks next task by |
|-----------|-------------|--------------------|
| FCFS | No | Arrival time |
| SJF | No | Shortest burst time |
| Round Robin | Yes (time quantum) | Queue order |
| Priority | No | Priority number (1 = highest) |
