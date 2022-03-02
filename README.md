# CS 332, Final project
David Chu and Luisa Escosteguy

## The features from proposal successfully implemented

Simple priority scheduling: 

- thread_set_priority and thread_get_priority
- yield

## Any non-functional features you attempted to implement

## The files you added or modified, and how they relate to the features above

- thread.h: added header for thread_get_priority and thread_set_priority
- thread.c: implement thread_get_priority and thread_set_priority
- sched.h: added header for yield
- sched.c: implemented yield
- kernel/sched-test: all test cases for this lab

## What aspects of the implementation each test case tests

To run the tests run make qemu-shed-test and note the print statements

- simple_priority_sched_test: 
- tie_priority_sched_test: 
- inversion_priority_sched_test:
- add_higher_thread_test: 
- get_set_priority_test: test if the priority of a thread is correctly set

## Features or edge cases the test cases do not address

## Known bugs


## Anything interesting you would like to share
