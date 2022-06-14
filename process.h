/**
 *  process.h
 *
 *  Full Name: Jiayue Zhang
 *  Course section: LE/EECS3221 M - Operating System Fundamentals (Winter 2021-2022)
 *  Representation of a process in the system.
 *  
 */


#ifndef PROCESS_H
#define PROCESS_H

// representation of a a process
typedef struct process {
    int  pid;
    int  cpu_time;
    int  io_time;  
    int  arrival_time;
    int  state;
    int  advance_time;
    int  start_time;
    int  finish_time;
} Process;

#endif
