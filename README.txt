DEPS : scheduler.h list.h process.h
scheduler.h: header file for scheduler.c
List.h list.c: a singly linked list to implement the ready queue, getPID() method added, to get the PID of the first process on the queue without deleting it.
scheduler.c: a scheduler program simulating FCFS, RR, and SJF scheduling algorithm.