# CS 332, Final project
David Chu and Luisa Escosteguy

## The features from proposal successfully implemented

Simple priority scheduling: 

- thread_set_priority and thread_get_priority
- yield

## Any non-functional features you attempted to implement

- priority donation

## The files you added or modified, and how they relate to the features above

- thread.h: added header for thread_get_priority and thread_set_priority
- thread.c: implement thread_get_priority and thread_set_priority
- sched.h: added header for yield and get_max_priority_thread helper function
- sched.c: implemented yield, changed sched_ready and sched to take priority into consideration
        and added get_max_priority_thread helper 
- kernel/sched_test: all test cases for this lab
- include/kernel/sched_test: header file for sched_test file
- Makefile: added option to run osv in test mode to run priority scheduling tests in kernel mode
- kernel/main: either runs regular osv or in test mode

## What aspects of the implementation each test case tests

To run the tests run `TEST=true make qemu` and note the print statements. We are running qemu with 1 CPU to facilitate testing. 

- simple_priority_sched_test: 
- tie_priority_sched_test: 
- inversion_priority_sched_test:
- add_higher_thread_test: add a thread with higher priority to the ready list - it should start running. 
- lower_thread_priority_should_yield: create a high priority thread which should start
running and then lower its own priority - it should yield to the other threads (running to ready)
and other thread should take over. 
- get_set_priority_test: test if the priority of a thread is correctly set

## Features or edge cases the test cases do not address

## Known bugs


## Anything interesting you would like to share
