# CS 332, Final project
David Chu and Luisa Escosteguy

## The features from proposal successfully implemented

- thread_set_priority and thread_get_priority
- yield
- Basic priority scheduling
    - New threads are scheduled based on priority
    - If a thread is added to the ready list and has the highest priority, it starts running immediately
    - If the highest priority thread (currently running) priority is lowered and there is a thread with higher priority, it yields 
    - TODO: Simple priority donation

## Any non-functional features you attempted to implement

- Priority donation
- Ties

## The files you added or modified, and how they relate to the features above

Priority implementation:

- thread.h: added header for thread_get_priority and thread_set_priority
- thread.c: implement thread_get_priority and thread_set_priority
- sched.h: added header for yield and get_max_priority_thread helper function
- sched.c: implemented yield, changed sched_ready and sched to take priority into consideration
        and added get_max_priority_thread helper 
- synch.c: TODO changed spinlock_acquire and spinlock_release to support priority donation

Testing related:

- kernel/sched_test: all test cases for this lab
- include/kernel/sched_test: header file for sched_test file
- Makefile: added option to run osv in test mode to run priority scheduling tests in kernel mode. Changed the number of CPUs
- kernel/main: either runs regular osv or in sched test mode

## What aspects of the implementation each test case tests

To run the tests, run `TEST=true make qemu` and note the print statements. We are running qemu with 1 CPU to facilitate testing. 

- simple_priority_sched_test: TODO - verifies that the three threads with high, medium and low priority finish running in order. 
- tie_priority_sched_test: TODO - several threads with the same priority should take turns to run
- inversion_priority_sched_test: high priority thread (H) needs a lock adquired by low priority thread (L), so it donates its priority to L while L is holding the lock. When L releases the lock, H should continue running. 
- add_higher_thread_test: add a thread with higher priority to the ready list - it should start running
- lower_thread_priority_should_yield: create a high priority thread that should start
running and then lower its own priority - it should yield to the other threads (running to ready)
and other thread should take over. 
- get_set_priority_test: test if the priority of a thread is correctly set

## Features or edge cases the test cases do not address

- Work in progress: ties. 

## Known bugs

None that we know.

## Anything interesting you would like to share
