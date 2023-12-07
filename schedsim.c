#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>
#include "process.h"
#include "util.h"

// Function to compare processes for sorting by priority
int my_comparer(const void *process1, const void *process2) {
    const ProcessType *p1 = process1;
    const ProcessType *p2 = process2;
    return p2->pri - p1->pri;  // Corrected member names
}

// Function declarations
void findWaitingTimeFCFS(ProcessType plist[], int n);
void findavgTimeFCFS(ProcessType plist[], int n);
void findWaitingTimeSJF(ProcessType plist[], int n);
void findavgTimeSJF(ProcessType plist[], int n);
void findavgTimePriority(ProcessType plist[], int n);
void findWaitingTimeRR(ProcessType plist[], int n, int quantum);
void findavgTimeRR(ProcessType plist[], int n, int quantum);
void printMetrics(ProcessType plist[], int n);


// Function to find the waiting time for all processes (FCFS)
void findWaitingTimeFCFS(ProcessType plist[], int n) {
    // Waiting time for the first process is 0, or arrival time if not
    plist[0].wt = plist[0].art;

    // Calculate waiting time for remaining processes
    for (int i = 1; i < n; i++) {
        plist[i].wt = plist[i - 1].bt + plist[i - 1].wt;
    }
}

// Function to calculate average turnaround and waiting time (FCFS)
void findavgTimeFCFS(ProcessType plist[], int n) {
    // Find waiting time
    findWaitingTimeFCFS(plist, n);

    printf("\n\n*********\nFCFS\n");

    // Calculate turn around time
    for (int i = 0; i < n; i++) {
        plist[i].tat = plist[i].bt + plist[i].wt;
    }

    // Print processes and metrics
    printMetrics(plist, n);
}

// Function to find waiting time for all processes (SJF)
void findWaitingTimeSJF(ProcessType plist[], int n) {
    int completed = 0; // Number of completed processes
    int t = 0;         // Current time

    while (completed != n) {
        int min_index = -1; // Index of process with minimum remaining time
        int min_bt = INT_MAX; // Minimum remaining time

        for (int i = 0; i < n; i++) {
            if (plist[i].art <= t && plist[i].bt < min_bt) {
                min_index = i;
                min_bt = plist[i].bt;
            }
        }

        if (min_index != -1) {
            t += plist[min_index].bt;
            plist[min_index].wt = t - plist[min_index].bt - plist[min_index].art;
            completed++;
        } else {
            t++;
        }
    }
}


// Function to calculate average turnaround and waiting time (SJF)
void findavgTimeSJF(ProcessType plist[], int n) {
    // Find waiting time
    findWaitingTimeSJF(plist, n);

    // Calculate turn around time
    for (int i = 0; i < n; i++) {
        plist[i].tat = plist[i].bt + plist[i].wt;
    }

    // Print processes and metrics
    printf("\n*********\nSJF\n");
    printMetrics(plist, n);
}

// Function to sort processes by priority and apply FCFS
void findavgTimePriority(ProcessType plist[], int n) {
    qsort(plist, n, sizeof(ProcessType), my_comparer);
    findavgTimeFCFS(plist, n);
}

// Function to find waiting time for all processes (RR)
void findWaitingTimeRR(ProcessType plist[], int n, int quantum) {
    int rem_bt[n]; // Remaining burst time for each process
    for (int i = 0; i < n; i++) {
        rem_bt[i] = plist[i].bt;
    }

    int t = 0; // Current time

    while (1) {
        bool done = true; // Check if all processes are completed

        for (int i = 0; i < n; i++) {
            if (rem_bt[i] > 0) {
                done = false;
                if (rem_bt[i] > quantum) {
                    t += quantum;
                    rem_bt[i] -= quantum;
                } else {
                    t += rem_bt[i];
                    plist[i].wt = t - plist[i].bt;
                    rem_bt[i] = 0; // This process is over
                }
            }
        }

        // If all processes are done, break from the loop
        if (done == true) {
            break;
        }
    }
}

// Function to calculate average turnaround and waiting time (RR)
void findavgTimeRR(ProcessType plist[], int n, int quantum) {
    // Find waiting time
    findWaitingTimeRR(plist, n, quantum);

    // Calculate turn around time
    for (int i = 0; i < n; i++) {
        plist[i].tat = plist[i].bt + plist[i].wt;
    }

    // Print processes and metrics
    printf("\n*********\nRR Quantum = %d\n", quantum);
    printMetrics(plist, n);
}

void printMetrics(ProcessType plist[], int n) {
    int total_wt = 0, total_tat = 0;
    float awt, att;

    printf("\tProcesses\tBurst time\tWaiting time\tTurn around time\n");

    // Calculate total waiting time and total turn around time
    for (int i = 0; i < n; i++) {
        total_wt = total_wt + plist[i].wt;
        total_tat = total_tat + plist[i].tat;
        printf("\t%d\t\t%d\t\t%d\t\t%d\n", plist[i].pid, plist[i].bt, plist[i].wt, plist[i].tat);
    }

    awt = ((float)total_wt / (float)n);
    att = ((float)total_tat / (float)n);

    printf("\nAverage waiting time = %.2f", awt);
    printf("\nAverage turn around time = %.2f\n", att);
}

ProcessType *initProc(char *filename, int *n) {
    FILE *input_file = fopen(filename, "r");
    if (!input_file) {
        fprintf(stderr, "Error: Invalid filepath\n");
        fflush(stdout);
        exit(0);
    }

    ProcessType *plist = parse_file(input_file, n);
    fclose(input_file);
    return plist;
}

int main(int argc, char *argv[]) {
    int n;
    int quantum = 2;

    ProcessType *proc_list;

    if (argc < 2) {
        fprintf(stderr, "Usage: ./schedsim <input-file-path>\n");
        fflush(stdout);
        return 1;
    }

    // FCFS
    n = 0;
    proc_list = initProc(argv[1], &n);
    findavgTimeFCFS(proc_list, n);

    // SJF
    n = 0;
    proc_list = initProc(argv[1], &n);
    findavgTimeSJF(proc_list, n);

    // Priority
    n = 0;
    proc_list = initProc(argv[1], &n);
    findavgTimePriority(proc_list, n);

    // RR
    n = 0;
    proc_list = initProc(argv[1], &n);
    findavgTimeRR(proc_list, n, quantum);

    return 0;
}