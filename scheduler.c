/**
 *  scheduler.c
 *
 *  Full Name: Jiayue Zhang
 *  Course section: LE/EECS3221 M - Operating System Fundamentals (Winter 2021-2022)
 *  Description of the program: a scheduler program simulating FCFS, RR, and SJF scheduling algorithm.
 *  
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "list.h"
#include "scheduler.h"
#include "process.h"

#define SIZE  100

int main(int argc, char *argv[])
{
	char out[SIZE];
	strncpy(out, argv[1], 10);
	char dash = '-'; 
	strncat(out, &dash, 1);
	strcat(out, argv[2]);
	strcat(out, ".txt");
	freopen(out,"w",stdout);

	FILE *fp;
	int finishing_time = 0;
	int idle_time = 0;
	int num_of_processes;
	struct node *ready = NULL;// ready queue

	fp  = fopen(argv[1],"r");
	fscanf(fp, "%d", &num_of_processes);    
	if (num_of_processes > 0)  {
		Process *queue = malloc(num_of_processes * sizeof(Process));      
		int i;
		for (i=0; i<num_of_processes; i++){
			fscanf(fp, "%d %d %d %d",
					&queue[i].pid, 
					&queue[i].cpu_time, 
					&queue[i].io_time, 
					&queue[i].arrival_time); 
		}                                                                               

		// for(i =0; i<num_of_processes; i++) {
		// 	printf("A: %d B: %d  C: %d D: %d \n",
		// 			queue[i].pid, 
		// 			queue[i].cpu_time, 
		// 			queue[i].io_time, 
		// 			queue[i].arrival_time);    
		// }
		for(i =0; i<num_of_processes; i++) {
					queue[i].state = 0;
					queue[i].advance_time = 0; 
					queue[i].start_time = 0; 
					queue[i].finish_time = 0; 			
		}
		/* Record states of each processes, and the advance time:
		0: not arrived yet
		1: running
		2: blocked
		3: ready
		4: finished */

		int running_pid = -1;	//only one running process
		int alg = atoi(argv[2]);

		/* FCFS */
		if (alg == 0) {
			int finished = 1; // 1 = false, 0 = true(all processes finished)
			for (i=0; finished == 1; i++) {		// Cycle i
				int j;
				for (j=0; j<num_of_processes; j++){
				// Put all the blocked processes that finished I/O back on the ready queue. 
				// Special case: if CPU time = 1, then after being blocked, the program finishes.
					if (queue[j].state == 2 
					&& queue[j].advance_time == queue[j].cpu_time / 2 + queue[j].cpu_time % 2 + queue[j].io_time) {
						if (queue[j].cpu_time == 1) {
							queue[j].state = 4;
							queue[j].finish_time = i;
						}
						else{
							queue[j].state = 3;		// Change to ready
							insert(&ready, &queue[j]);	// Add to ready queue
						}
					}
				}

			/* DoRunningProcesses() */
				if (running_pid != -1) {
					// If process has run for cpu_time/2, change the state to blocked
					int half_cpu_time = queue[running_pid].cpu_time / 2 + queue[running_pid].cpu_time % 2;
					if (half_cpu_time == queue[running_pid].advance_time) {
						queue[running_pid].state = 2;	// Set to blocked.
						running_pid = -1;	// stopped running
					}

					// Change processes with advance time equals their CPU time + I/O time to finished state. 
					if (queue[running_pid].cpu_time + queue[running_pid].io_time == queue[running_pid].advance_time) {
						queue[running_pid].state = 4;	// Set to finished
						queue[running_pid].finish_time = i;
						running_pid = -1;	// stopped running	
					}
				}

			/* DoArrivingProcesses() */

				// Newly arriving processes are added to the end of the queue. 
				for (j=0; j<num_of_processes; j++){
					if (queue[j].arrival_time == i) {
						queue[j].state = 3;
						insert(&ready, &queue[j]);
					}
				}

			/* DoReadyProcesses() */
				// Check to see if all processes finished.
				finished = 0;
				for (j=0; j<num_of_processes; j++){
					if (queue[j].state != 4) {	// If states of all processes is 4, finish.
						finished = 1;
					} 
					if (finished == 0) {
						finishing_time = i;
					}
				}
				// If no process is running and the ready queue is not empty,
				// and it is not the case that all processes have finished,
				//  put a process to run, and delete it from the ready queue 
				if (running_pid == -1 && finished != 0) {
					running_pid = getPID(ready);	// FIFO
					if (running_pid >= 0) {
						if (queue[running_pid].advance_time == 0) {	// if start running first time
							queue[running_pid].start_time = i;
						}
						queue[running_pid].state = 1;	//change state to running.
						if(ready->next == NULL) {
							ready = NULL;
						}
						else {
							delete(&ready, &(queue[running_pid]));	//remove from queue
						}	
					}
				}
				// 	If a process is running or blocked, the advance time adds 1.
				for (j=0; j<num_of_processes; j++){
					if (queue[j].state == 1 || queue[j].state == 2) {
						queue[j].advance_time++;	
					}
				}
				/* If no process is running at this point, 
				then the system is idle. */
				if (running_pid == -1 && finished == 1) {
					idle_time++;
				}

				/* Timing snapshot */
				if(finished == 1) {
					printf("%d ", i);
					for (j=0; j<num_of_processes; j++) {
						if (queue[j].state == 1) {
							printf("%d: running ", j);
						}
						if (queue[j].state == 2) {
							printf("%d: blocked ", j);
						}	
						if (queue[j].state == 3) {
							printf("%d: ready   ", j);
						}
					}
					printf("\n");
				}
			}	// end cycle i
		}	// end if alg == 0
		/* RR */
		if (alg == 1) {	
			int finished = 1; // 1 = false, 0 = true(all processes finished)
			for (i=0; finished == 1; i++) {		// Cycle i
				int j;
				for (j=0; j<num_of_processes; j++){
				// Put all the blocked processes that finished I/O back on the ready queue. 
				// Special case: if CPU time = 1, then after being blocked, the program finishes.
					if (queue[j].state == 2 
					&& queue[j].advance_time == queue[j].cpu_time / 2 + queue[j].cpu_time % 2 + queue[j].io_time) {
						if (queue[j].cpu_time == 1) {
							queue[j].state = 4;
							queue[j].finish_time = i;
						}
						else{
							queue[j].state = 3;		// Change to ready
							insert(&ready, &queue[j]);	// Add to ready queue
						}
					} 
				}

			/* DoArrivingProcesses() */

				// Newly arriving processes are added to the end of the queue. 
				for (j=0; j<num_of_processes; j++){
					if (queue[j].arrival_time == i) {
						queue[j].state = 3;
						insert(&ready, &queue[j]);
					}
				}

			/* DoRunningProcesses() */
			// printf("running pid=%d advance0=%d advance1=%d advance2=%d\n", running_pid, queue[0].advance_time, queue[1].advance_time, queue[2].advance_time);
				if (running_pid != -1) {
					int half_cpu_time = queue[running_pid].cpu_time / 2 + queue[running_pid].cpu_time % 2;
					// Change processes with advance time equals their CPU time + I/O time to finished state. 
					if (queue[running_pid].cpu_time + queue[running_pid].io_time == queue[running_pid].advance_time) {
						queue[running_pid].state = 4;	// Set to finished
						queue[running_pid].finish_time = i;
						running_pid = -1;	// stopped running	
					}					
					
					// If process has run for cpu_time/2, change the state to blocked
					else if (half_cpu_time == queue[running_pid].advance_time) {
						queue[running_pid].state = 2;	// Set to blocked.
						running_pid = -1;	// stopped running
					}
					// If current running process ran for 2 cycles, put it to the ready queue, change its state to ready.
					// check the advance time to see if it it can be divided by 2,
					// and is less than 1/2 CPU time, if this is the case we put it to ready queue.
					// if it is greater than 1/2 CPU time, we check advance time - io_time % 2.
					else if (queue[running_pid].advance_time % 2 == 0
					&& queue[running_pid].advance_time < half_cpu_time){
						queue[running_pid].state = 3;	// Set to ready.
						insert(&ready, &queue[running_pid]);	// Add to ready queue
						running_pid = -1;	// stop running	
					}
					else if (queue[running_pid].advance_time > half_cpu_time 
					&& (queue[running_pid].advance_time - queue[running_pid].io_time % 2) % 2 == 0) {
						queue[running_pid].state = 3;	// Set to ready.
						insert(&ready, &queue[running_pid]);	// Add to ready queue
						running_pid = -1;	// stop running	
					}
				}


			/* DoReadyProcesses() */
				// Check to see if all processes finished.
				finished = 0;
				for (j=0; j<num_of_processes; j++){
					if (queue[j].state != 4) {	// If states of all processes is 4, finish.
						finished = 1;
					} 
					if (finished == 0) {
						finishing_time = i;
					}
				}
				// If no process is running and the ready queue is not empty,
				// and it is not the case that all processes have finished,
				//  put a process to run, and delete it from the ready queue 
				if (running_pid == -1 && finished != 0) {
					running_pid = getPID(ready);	// FIFO
					if (running_pid >= 0) {
						if (queue[running_pid].advance_time == 0) {	// if start running first time
							queue[running_pid].start_time = i;
						}
						queue[running_pid].state = 1;	//change state to running.
						if(ready->next == NULL) {
							ready = NULL;
						}
						else {
							delete(&ready, &(queue[running_pid]));	//remove from queue
						}	
					}
				}
				// 	If a process is running or blocked, the advance time adds 1.
				int m;
				for (m=0; m<num_of_processes; m++){
					if (queue[m].state == 1 || queue[m].state == 2) {
						queue[m].advance_time++;	
					}
				}
				/* If no process is running at this point, 
				then the system is idle. */
				if (running_pid == -1 && finished == 1) {
					idle_time++;
				}

				/* Timing snapshot */
				if(finished == 1) {
					printf("%d ", i);
					for (j=0; j<num_of_processes; j++) {
						if (queue[j].state == 1) {
							printf("%d: running ", j);
						}
						if (queue[j].state == 2) {
							printf("%d: blocked ", j);
						}	
						if (queue[j].state == 3) {
							printf("%d: ready   ", j);
						}
					}
					printf("\n");
				}
				// printf("i=%d running=%d idle time=%d finished=%d advacne0=%d advacne1=%d advacne2=%d\n", i, running_pid, idle_time,finished, queue[0].advance_time, queue[1].advance_time, queue[2].advance_time);
				// traverse(ready);
				// printf("-----------------\n");
			}	// end cycle i
		}	// end if alg == 1


		/* SJF */
		if (alg == 2) {
			int finished = 1; // 1 = false, 0 = true(all processes finished)
			for (i=0; finished == 1; i++) {		// Cycle i
				int j;
				for (j=0; j<num_of_processes; j++){
				// Put all the blocked processes that finished I/O back on the ready queue. 
				// Special case: if CPU time = 1, then after being blocked, the program finishes.
					if (queue[j].state == 2 
					&& queue[j].advance_time == queue[j].cpu_time / 2 + queue[j].cpu_time % 2 + queue[j].io_time) {
						if (queue[j].cpu_time == 1) {
							queue[j].state = 4;
							queue[j].finish_time = i;
						}
						else{
							queue[j].state = 3;		// Change to ready
							insert(&ready, &queue[j]);	// Add to ready queue
						}
					}
				}

			/* DoRunningProcesses() */
				if (running_pid != -1) {
					// If process has run for cpu_time/2, change the state to blocked
					int half_cpu_time = queue[running_pid].cpu_time / 2 + queue[running_pid].cpu_time % 2;
					if (half_cpu_time == queue[running_pid].advance_time) {
						queue[running_pid].state = 2;	// Set to blocked.
						running_pid = -1;	// stopped running
					}

					// Change processes with advance time equals their CPU time + I/O time to finished state. 
					if (queue[running_pid].cpu_time + queue[running_pid].io_time == queue[running_pid].advance_time) {
						queue[running_pid].state = 4;	// Set to finished
						queue[running_pid].finish_time = i;
						running_pid = -1;	// stopped running	
					}
				}

			/* DoArrivingProcesses() */

				// Newly arriving processes are added to the end of the queue. 
				for (j=0; j<num_of_processes; j++){
					if (queue[j].arrival_time == i) {
						queue[j].state = 3;
						insert(&ready, &queue[j]);
					}
				}

			/* DoReadyProcesses() */
				// Check to see if all processes finished.
				finished = 0;
				for (j=0; j<num_of_processes; j++){
					if (queue[j].state != 4) {	// If states of all processes is 4, finish.
						finished = 1;
					} 
					if (finished == 0) {
						finishing_time = i;
					}
				}
				// If no process is running and the ready queue is not empty,
				// and it is not the case that all processes have finished,
				//  put the shortest process to run, and delete it from the ready queue 
				// printf("running_pid=%d queue[running_pid].state=%d queue[0].advance_time=%d queue[0].state=%d\n", running_pid, queue[running_pid].state,queue[0].advance_time, queue[0].state);
				if (running_pid == -1 && finished != 0) {
					int arr[num_of_processes];
					int k;
					k = 0;
					while (arr[k-1] != -1) {	// get all the jobs on the queue
						arr[k] = getPID(ready);
						if (arr[k] >= 0) {
							if(ready->next == NULL) {
								ready = NULL;
							}
							else {
								delete(&ready, &(queue[arr[k]]));	//remove from queue
							}	
						}
						k++;
					}
					int s;
					// Put the one with the shortest remaining time to run, put the rest back on the queue.
					int shortest_time = queue[arr[0]].io_time + queue[arr[0]].cpu_time - queue[arr[0]].advance_time;
					for (s=0; s<k-1; s++) {
						int current_remaining = queue[arr[s]].io_time + queue[arr[s]].cpu_time - queue[arr[s]].advance_time;
						if (current_remaining <= shortest_time) {
							running_pid = arr[s];
							shortest_time = current_remaining;
						}
					}
					queue[running_pid].state = 1;
					if (queue[running_pid].advance_time == 0) {
						queue[running_pid].start_time	= i;				
					}

					for (s=0; s<k; s++) {
						if (arr[s] != running_pid && arr[s] >= 0) {	// insert the rest of the jobs back
							insert(&ready, &queue[arr[s]]);
						}
					}
				}
				// 	If a process is running or blocked, the advance time adds 1.
				for (j=0; j<num_of_processes; j++){
					if (queue[j].state == 1 || queue[j].state == 2) {
						queue[j].advance_time++;	
					}
				}
				/* If no process is running at this point, 
				then the system is idle. */
				if (running_pid == -1 && finished == 1) {
					idle_time++;
				}

				/* Timing snapshot */
				if(finished == 1) {
					printf("%d ", i);
					for (j=0; j<num_of_processes; j++) {
						if (queue[j].state == 1) {
							printf("%d: running ", j);
						}
						if (queue[j].state == 2) {
							printf("%d: blocked ", j);
						}	
						if (queue[j].state == 3) {
							printf("%d: ready   ", j);
						}
					}
					printf("\n");
				}
			}	// for cycle i loop
		}	// if alg == 2


		/* Summary statistics */
		printf("\n");
		printf("Finishing time: %d\n", finishing_time - 1);
		printf("CPU utilization: %.2f\n", (float)(finishing_time - idle_time) / (float)finishing_time);
		for (i=0; i<num_of_processes; i++) {
			printf("Turnaround process %d: %d\n", i, queue[i].finish_time  - queue[i].arrival_time);
			// printf("start=%d finish=%d\n", queue[i].start_time, queue[i].finish_time);
		}
		free(queue);
	}	// if (num_of_processes > 0)
	fclose(fp);
	return 0;
} // main

