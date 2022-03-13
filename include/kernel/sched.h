#ifndef _SCHED_H_
#define _SCHED_H_

#include <kernel/synch.h>
#include <kernel/thread.h>

struct spinlock sched_lock;

/* initialize lists used by scheduler */
void sched_sys_init();

/* start scheduling for the calling AP, interrupt must be off when calling this */
err_t sched_start();

/* start scheduling for the calling AP, interrupt must be off when calling this */
err_t sched_start_ap();

/* Add thread to ready queue */
void sched_ready(struct thread*);

/* 
 * Schedule another thread to run. 
 * Current thread transition to the next state, release lock passed in if not null 
*/
void sched_sched(threadstate_t next_state, void* lock);

/* 
 * Check if the current thread is the highest priority thread
 * If yes, return
 * If not, schedule the highest priority thread to run
 * and current thread transition to the next state, release lock passed in if not null 
*/
void yield(threadstate_t next_state, void* lock);

/*
* Return the ready thread with the highest priority 
*/
struct thread* get_max_priority_thread();

/*
* Return the list of threads with the highest priority from the ready list
* Contains only one element if only one thread has highest priority
*/
void get_max_priority_thread_tie();

/*
* Return the thread switch count for all CPUs for testing purposes 
*/
int get_thread_switch_count();

#endif /* _SCHED_H_ */
