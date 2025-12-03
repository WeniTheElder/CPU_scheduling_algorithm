#include <iomanip>
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <math.h>

using namespace std;

#define TIME_QUANTUM 5
#define TIME_UNIT .1
#define EPSILON .000001

struct Process{
    string name;
    double arrivalTime;
    double burstTime;
    int priority = 0;
    double remainingTime = burstTime;
    double waitTime = 0;
    double completetionTime = 0;
    double turnAroundTime = 0;
};


struct GanttCharttEntry{
    string name;
    double startTime;
    double endTime;
};

vector<GanttCharttEntry> GanttChart;



void FCFS(vector<Process> &processes){
    double Time{};

    // Sort the processes according to their arrivale time
    sort(processes.begin(),processes.end(),[](const Process&p1, const Process&p2){
        return p1.arrivalTime<p2.arrivalTime;
    });

    // Loop through the processes and calculate their stats
    for(auto&p:processes){
        GanttChart.push_back({p.name,Time,Time+p.burstTime});
        Time += p.burstTime;
        p.completetionTime = Time;
        p.turnAroundTime = p.completetionTime - p.arrivalTime;
        p.waitTime = p.turnAroundTime - p.burstTime;
        p.remainingTime = 0;
    }
}


void Priority(vector<Process> &processes, bool preemtionFlag){
    double Time{};
    int completedProcessCount{}, processIndex{};

    // Sort the processes according to their arrivale time
    sort(processes.begin(),processes.end(),[](const Process&p1, const Process&p2){
        return p1.arrivalTime < p2.arrivalTime;
    });

    // Initializing priority queue, the front element is the one with highest priority
    auto pq_compare = [](const Process* p1, const Process* p2){
        if (p1->priority != p2->priority) {
            return p1->priority > p2->priority;
        }
        return p1->arrivalTime > p2->arrivalTime;
    };
    priority_queue<Process*,vector<Process*>,decltype(pq_compare)> readyQueue(pq_compare);
    

    // Algorithm simulation
    while(completedProcessCount < processes.size()){
        // Add arrived processes to ready queue
        while(processIndex<processes.size() && processes[processIndex].arrivalTime <= Time+ EPSILON){
            readyQueue.push(&processes[processIndex]);
            ++processIndex;
        }

        if(readyQueue.empty()){
            // If there are still processes, fast-forward to the next process arrival time
            if(processIndex < processes.size()){
                double idleStartTime = Time;
                Time = processes[processIndex].arrivalTime;
                // Add the idle time stampt to Gantt chart vector
                if (Time > idleStartTime) {
                    GanttChart.push_back({"IDLE", idleStartTime, Time});
                }
                // Skip the current loop to add the arrived process/es
                continue;
            }
            // No more processes to run
            else return;
        }

        // Get the next process to run
        Process* currentPrcoess = readyQueue.top();
        readyQueue.pop();

        if(preemtionFlag){
            double runDuration = currentPrcoess->remainingTime;

            // Determine the time until the next process arrives.
            if (processIndex < processes.size()) {
                double timeToNextArrival = processes[processIndex].arrivalTime - Time;
                // The process can only run until the next one arrives or it's finished.
                if (timeToNextArrival >= 0 && timeToNextArrival < runDuration) {
                    runDuration = timeToNextArrival;
                }
            }

            // If runDuration is 0 or less, we need to re-evaluate.
            // Pushing the current process back and continuing the loop will allow
            // the newly arrived process to be added to the ready queue before a decision is made.
            if (runDuration <= 0) {
                readyQueue.push(currentPrcoess);
                continue;
            }

            // Gantt Chart logic
            if (GanttChart.empty() || GanttChart.back().name != currentPrcoess->name || Time > GanttChart.back().endTime) {
                GanttChart.push_back({currentPrcoess->name, Time, Time + runDuration});
            } else {
                GanttChart.back().endTime = Time + runDuration;
            }

            // Run the process for runDuration
            currentPrcoess->remainingTime -= runDuration;
            Time += runDuration;

            // If process is done, measure its stats
            if(currentPrcoess->remainingTime <= EPSILON){
                currentPrcoess->completetionTime = Time;
                currentPrcoess->turnAroundTime = currentPrcoess->completetionTime - currentPrcoess->arrivalTime;
                currentPrcoess->waitTime = currentPrcoess->turnAroundTime - currentPrcoess->burstTime;
                ++completedProcessCount;
            }
            // The process is not done, push it back to ready queue
            else {
                 readyQueue.push(currentPrcoess);
            }
        }
        else{ 
            // Run the process until it's completed and measure its stats
            GanttChart.push_back({currentPrcoess->name,Time,Time+currentPrcoess->burstTime});
            Time += currentPrcoess->burstTime;
            currentPrcoess->completetionTime = Time;
            currentPrcoess->turnAroundTime = currentPrcoess->completetionTime - currentPrcoess->arrivalTime;
            currentPrcoess->waitTime = currentPrcoess->turnAroundTime - currentPrcoess->burstTime;
            currentPrcoess->remainingTime = 0;
            ++completedProcessCount;
        }
    }
}



void SJF(vector<Process> &processes, bool preemtionFlag){
    double Time{};
    int completedProcessCount{}, processIndex{};

    // Sort the processes according to their arrivale time
    sort(processes.begin(),processes.end(),[](const Process&p1, const Process&p2){
        return p1.arrivalTime < p2.arrivalTime;
    });

    // Initializing priority queue, the front element is the one with least reamining time
    auto pq_compare = [](const Process* p1, const Process* p2){
        return p1->remainingTime > p2->remainingTime;
    };
    priority_queue<Process*,vector<Process*>,decltype(pq_compare)> readyQueue(pq_compare);
    

    // Algorithm simulation
    while(completedProcessCount < processes.size()){
        // Add arrived processes to ready queue
        while(processIndex<processes.size() && processes[processIndex].arrivalTime <= Time+ EPSILON){
            readyQueue.push(&processes[processIndex]);
            ++processIndex;
        }

        if(readyQueue.empty()){
            // If there are still processes, fast-forward to the next process arrival time
            if(processIndex < processes.size()){
                double idleStartTime = Time;
                Time = processes[processIndex].arrivalTime;
                if (Time > idleStartTime) {
                    GanttChart.push_back({"IDLE", idleStartTime, Time});
                }
                // Skip the current loop to add the arrived process/es
                continue;
            }
            // No more processes to run, terminate
            else return;
        }

        // Get the next process to run
        Process* currentPrcoess = readyQueue.top();
        readyQueue.pop();

        if(preemtionFlag){
            // GanttChart logic
            if (GanttChart.empty() || GanttChart.back().name != currentPrcoess->name) {
                GanttChart.push_back({currentPrcoess->name, Time, Time + TIME_UNIT});
            } else {
                GanttChart.back().endTime = Time + TIME_UNIT;
            }

            // Run the process for one time unit
            currentPrcoess->remainingTime -= TIME_UNIT;
            Time += TIME_UNIT;

            // If process is done, measure its stats
            if(currentPrcoess->remainingTime <= EPSILON){
                currentPrcoess->completetionTime = Time;
                currentPrcoess->turnAroundTime = currentPrcoess->completetionTime - currentPrcoess->arrivalTime;
                currentPrcoess->waitTime = currentPrcoess->turnAroundTime - currentPrcoess->burstTime;
                ++completedProcessCount;
            }
            // The process is not done, push it back to ready queue (preemtion step)
            else  readyQueue.push(currentPrcoess);
        }
        else{ 
            // Run the process until it's completed and measure its stats
            GanttChart.push_back({currentPrcoess->name,Time,Time+currentPrcoess->burstTime});
            Time += currentPrcoess->burstTime;
            currentPrcoess->completetionTime = Time;
            currentPrcoess->turnAroundTime = currentPrcoess->completetionTime - currentPrcoess->arrivalTime;
            currentPrcoess->waitTime = currentPrcoess->turnAroundTime - currentPrcoess->burstTime;
            currentPrcoess->remainingTime = 0;
            ++completedProcessCount;
        }
    }
} 


void roundRobin(vector<Process> &processes){
    double Time{};
    int completedProcessCount{}, processIndex{};

    // sort the processes accroding to their arrive time
    sort(processes.begin(),processes.end(),[](const Process&p1, const Process&p2){
        return p1.arrivalTime < p2.arrivalTime;
    });

    queue<Process*> readyQueue;

    //  Algorithm simulation
    while(completedProcessCount < processes.size()){

        // Add arrived processes to ready queue
        while(processIndex < processes.size() && processes[processIndex].arrivalTime <= Time + EPSILON){
            readyQueue.push(&processes[processIndex]);
            ++processIndex;
        }

        // If the queue is empty add processes to the queue
        if(readyQueue.empty()){
            // If there are still processes, fast-forward to the next process arrival time
            if(processIndex < processes.size()){
                double idleStartTime = Time;
                Time = processes[processIndex].arrivalTime;
                if (Time > idleStartTime) {
                    GanttChart.push_back({"IDLE", idleStartTime, Time});
                }
                // Skip the current loop to add the arrived process/es
                continue;
            }
            // No more processes to run, terminate
            else {
                return;
            }
        }

        // Get the next process to execute
        Process* currentProcess = readyQueue.front();
        readyQueue.pop();

        double timeToRun = min((double)TIME_QUANTUM, currentProcess->remainingTime);
        
        GanttChart.push_back({currentProcess->name,Time,Time+timeToRun});

        currentProcess->remainingTime -= timeToRun;
        Time += timeToRun;

        // Add any processes that arrived during the execution of the current process
        while(processIndex < processes.size() && processes[processIndex].arrivalTime <= Time + EPSILON){
            readyQueue.push(&processes[processIndex]);
            ++processIndex;
        }

        if(currentProcess->remainingTime <= EPSILON){ // Process is completed
            currentProcess->remainingTime = 0;
            currentProcess->completetionTime = Time;
            currentProcess->turnAroundTime = currentProcess->completetionTime - currentProcess->arrivalTime;
            currentProcess->waitTime = currentProcess->turnAroundTime - currentProcess->burstTime;
            completedProcessCount++; // Increment the count of completed processes
        } 
        else { // Process is not completed, put it back in the queue
            readyQueue.push(currentProcess);
        }
    }   
}

void printProcesses(const vector<Process> &processes){
    for(auto & p:processes){
        cout << p.name << ": " << "Arrival Time -> " << setw(2) << p.arrivalTime << " - Burst Time -> "<< setw(2) << p.burstTime 
        << " - Priority -> " << setw(2) << p.priority << endl;
    }
}

void printGanttChart(){
    // Print top part
    cout << "\n|";
    for(int i{};i<GanttChart.size();++i) cout << "----|";
    cout << endl;
    cout << '|';

    // Print process names in each entry
    for(auto &p:GanttChart){
        if (p.name == "IDLE"){
            cout << p.name << '|';
        }
        else {
            cout << ' ' << p.name << " |";
        }
    }

    //  Print bottom part
    cout << "\n|";
    for(int i{};i<GanttChart.size();++i) cout << "----|";
    cout << endl;

    // print start Time for each process/Idle
    for(auto &p: GanttChart) cout << left << setw(5) << round(p.startTime); 
    cout << left << setw(5) << round(GanttChart.back().endTime) << endl;
}



int main(){
    vector<Process> processes{
        {"p1",0,12,3},
        {"p2",1,8,2},
        {"p3",2,4,1},
        {"p4",3,10,4},
        {"p5",4,5,5}
    };
    double totalTurnAroundTime{}, totalWaitTime{};
    printProcesses(processes);
    Priority(processes, true);
    for(auto &p:processes){
        totalTurnAroundTime += p.turnAroundTime;
        totalWaitTime += p.waitTime;
    }
    printGanttChart();
    cout << "Average wait time = " << totalWaitTime/processes.size() << endl;
    cout << "Average turnaround time = " << totalTurnAroundTime/processes.size() << endl;

    return 0;
}
