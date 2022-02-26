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
- user/final folder: all test cases for this lab (?)

## What aspects of the implementation each test case tests

- donation-test: 
- priority-simple-test: 
- priority-tie-test:
- bad-args: priority should not be set if out of bounds

## Features or edge cases the test cases do not address

## Known bugs


## Anything interesting you would like to share
