#include <arch/mp.h>
#include <kernel/vm.h>
#include <kernel/thread.h>
#include <kernel/proc.h>
#include <kernel/console.h>
#include <kernel/arch.h>
#include <kernel/tests.h>
#include <kernel/sched.h>
#include <kernel/trap.h>
#include <kernel/bdev.h>
#include <kernel/fs.h>
#include <kernel/vpmap.h>
#include <kernel/pmem.h>
#include <lib/errcode.h>

/* verifies that the three threads with high, medium and low priority finish running in order */
int simple_priority_sched_test();

/* several threads with the same priority should take turns to run */
int tie_priority_sched_test();

/* high priority thread (H) needs a lock acquired by low priority thread (L), 
 * so it donates its priority to L while L is holding the lock. When L releases the lock, 
 * H should continue running */
int inversion_priority_sched_test();

/* add a thread with higher priority to the ready list - it should start running */
int add_higher_thread_test();

/* test if the priority of a thread is correctly set */
int get_set_priority_test();

/* create a high priority thread that should start */
int lower_thread_priority_should_yield();

/* should not set priority if out of bounds */
int set_invalid_priority_test();