# OS CPU Scheduling Simulator

A **C++17 command-line simulator** that demonstrates how CPU scheduling algorithms work by simulating processes with arrival time, burst time, and priority.

## 🚀 Features

* FCFS (First Come First Serve)
* SJF (Shortest Job First)
* Priority Scheduling
* Round Robin
* Gantt Chart generation
* Waiting Time & Turnaround Time
* CPU Utilization
* Idle CPU handling
* Input validation
* Automated test suite

## 🛠️ Tech Stack

* **C++17**
* **STL**
* **Makefile**
* **Git**

## 📂 Project Structure

```text
├── include/
│   ├── Task.h
│   ├── Scheduler.h
│   └── GanttChart.h
├── src/
│   ├── main.cpp
│   ├── Scheduler.cpp
│   └── GanttChart.cpp
├── tests/
├── examples/
├── docs/
├── Makefile
└── README.md
```

## ▶️ Run

### Windows

```bash
g++ -std=c++17 -Iinclude src/main.cpp src/Scheduler.cpp src/GanttChart.cpp -o scheduler.exe
.\scheduler.exe
```

### Linux / macOS

```bash
g++ -std=c++17 -Iinclude src/main.cpp src/Scheduler.cpp src/GanttChart.cpp -o scheduler
./scheduler
```

## 📝 Example Input

```text
5
P1 0 8 2
P2 1 4 1
P3 2 2 3
P4 3 6 2
P5 4 3 1
```

Format:

```text
Task_ID  Arrival_Time  Burst_Time  Priority
```

For Round Robin, enter a time quantum such as:

```text
2
```

## 📊 Metrics

The simulator calculates:

```text
Turnaround Time = Completion Time - Arrival Time

Waiting Time = Turnaround Time - Burst Time
```

It also displays average waiting time, average turnaround time, and CPU utilization.

## 🧪 Testing

Run the automated test suite using:

```bash
make test
```

The project includes tests for all four scheduling algorithms and idle CPU scenarios.

## 🔮 Future Improvements

* Preemptive SJF / SRTF
* Preemptive Priority Scheduling
* Multilevel Queue Scheduling
* Context-switching simulation
* GUI-based visualization

## OUTPUT 
<img width="1920" height="1080" alt="Screenshot 2026-09-30 015303" src="https://github.com/user-attachments/assets/2dec1e44-7e8e-4ddc-b214-63b244797450" />



## 👨‍💻 Author

**Harshal Vyawhare**
B.E. Information Technology | D. Y. Patil College of Engineering, Akurdi
