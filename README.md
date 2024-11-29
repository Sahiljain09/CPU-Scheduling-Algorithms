# CPU-Scheduling-Algorithms
In this project, a number of CPU Scheduling Algorithms found in most operating systems are implemented. It considers the scheduling of processes, turnaround time and also the normalized turnaround time metrics, and illustrates the process timeline. In addition, it deepens the concept of scheduling, which is based on the following algorithms: FCFS, RR, SRT and Aging.

## Table of Contents
- [CPU-Scheduling-Algorithms](#cpu-scheduling-algorithms)
  - [Algorithms](#algorithms)
    - [First Come First Serve (FCFS)](#first-come-first-serve-fcfs)
    - [Shortest Remaining Time (SRT)](#shortest-remaining-time-srt)
    - [Round Robin with varying time quantum (RR)](#round-robin-with-varying-time-quantum-rr)
    - [Aging](#aging)
  - [Input Format](#input-format)

## Algorithms

### First Come First Serve (FCFS)
 <strong>Description:</strong> Processes are scheduled according to their arrival times. It is non-preemptive, meaning once a process starts execution, it runs to completion.

<strong>Implementation:</strong><br>
            Start with the first process in the list (sorted by arrival time).<br>
            For each process:<br>
            - Calculate its finish time as the current time plus its service time.<br>
            - Calculate its turnaround time (finish time - arrival time) and normalized turnaround time (turnaround time/service time).<br>
            - Mark its execution timeline with * during its execution and . for waiting.<br>
            - Update the current time after executing each process.
            - Function: <code>firstComeFirstServe()</code>

### Round Robin with varying time quantum (RR)
<b>Description:</b> Processes are executed in a circular order for a fixed quantum. If a process doesn’t complete within the quantum, it returns to the ready queue.

<b>Implementation:</b>
Use a queue to manage processes. Enqueue processes as they arrive.
For each time unit:
Dequeue a process and execute it for the quantum or until completion.
Update the remaining service time for the process.
If the process is not completed, re-enqueue it with the updated service time.
Mark the timeline with * during execution and handle arrivals dynamically.
Calculate metrics such as finish time, turnaround time, and normalized turnaround time once the process is completed.
Function: roundRobin(int originalQuantum)
  
### Shortest Remaining Time (SRT)
<b>Description:</b> The process with the shortest remaining service time is executed first. This is a preemptive version of Shortest Job Next (SJN).

<b>Implementation:</b>
Use a priority queue to keep track of processes, sorted by their remaining service time.
For each time unit:
Add newly arrived processes to the priority queue.
Execute the process with the shortest remaining service time.
Update its remaining time and mark the timeline with *.
If a process completes (remaining time becomes 0), calculate its finish time, turnaround time, and normalized turnaround time.
Repeat until all processes are scheduled.
Function: shortestRemainingTime()

### Aging

<b>Description:</b> Aging prevents starvation by gradually increasing the priority of waiting processes. Higher priority processes are executed first.

<b>Implementation:</b>
Use a vector of tuples to store each process's priority level, index, and total waiting time.
For each time unit:
Add newly arrived processes to the vector.
Increment the priority level of waiting processes (aging) and reset the priority for the currently executing process.
Sort the processes by priority (and waiting time for ties).
Execute the process with the highest priority for the given quantum or until completion, marking the timeline with *.
Update waiting times and priorities dynamically.
Fill in waiting times for visualization.
Function: aging(int originalQuantum)

## Input Format
- Line 1: "trace" or "stats"
- Line 2: a comma-separated list telling which CPU scheduling policies to be analyzed/visualized along with
their parameters, if applicable. Each algorithm is represented by a number as listed in the
introduction section and as shown in the attached testcases.
Round Robin and Aging have a parameter specifying the quantum q to be used. Therefore, a policy
entered as 2-4 means Round Robin with q=4. Also, policy 8-1 means Aging with q=1.
 1. FCFS (First Come First Serve)
 2. RR (Round Robin)
 3. SRT (Shortest Remaining Time)
 4. Aging
- Line 3: An integer specifying the last instant to be used in your simulation and to be shown on the timeline.
- Line 4: An integer specifying the number of processes to be simulated.
- Line 5: Start of description of processes. Each process is to be described on a separate line. For algorithms 1 through 7, each process is described using a comma-separated list specifying:
