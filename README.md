# CPU Scheduling Algorithms Simulator

This project implements and simulates several classic **CPU scheduling algorithms** using C++. It calculates per-process metrics and generates a Gantt chart to visualize CPU usage over time.

## Supported Algorithms

* **FCFS (First Come, First Served)**
* **SJF (Shortest Job First)**

  * Preemptive and non-preemptive
* **Priority Scheduling**

  * Preemptive and non-preemptive
* **Round Robin** (with a fixed time quantum)

## Features

* Simulates process execution based on arrival time, burst time, and priority
* Generates a **Gantt chart** showing CPU timeline (including idle periods)
* Computes:

  * Completion Time
  * Turnaround Time
  * Waiting Time
  * Average turnaround and waiting times
* Handles preemption, idle CPU gaps, and process arrival events accurately

## How It Works

* Processes are stored in a vector and sorted by arrival time.
* Each scheduling algorithm updates:

  * `remainingTime`
  * `waitTime`
  * `turnAroundTime`
  * `completionTime`
* A global `GanttChart` vector stores CPU timeline segments as:

  ```
  { processName, startTime, endTime }
  ```
* After simulation, the Gantt chart and statistics are printed to the console.

## Example

The default `main()` function demonstrates **preemptive priority scheduling** on a fixed set of processes:

```
p1: arrival=0,  burst=12, priority=3  
p2: arrival=1,  burst=8,  priority=2  
p3: arrival=2,  burst=4,  priority=1  
p4: arrival=3,  burst=10, priority=4  
p5: arrival=4,  burst=5,  priority=5  
```

The program prints:

* The scheduling Gantt chart
* Average waiting time
* Average turnaround time

## Compilation

```bash
g++ -std=c++17 scheduler.cpp -o scheduler
./scheduler
```

## Notes

* `TIME_UNIT` is used for preemptive SJF to simulate continuous time.
* `TIME_QUANTUM` controls Round Robin behavior.
* `EPSILON` avoids floating-point comparison errors.
