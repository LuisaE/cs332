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
#include <kernel/sched_test.h>

int idle_thread(void *args) {
    for (int i = 0; i < 10; i++) { 
        kprintf("CHECK \n");
    }
    return 0;
}

int simple_priority_sched_test() {
    struct thread *t = thread_create("sched/testing thread", NULL, DEFAULT_PRI);
    kassert(t);
    thread_start_context(t, idle_thread, NULL);
    kprintf("simple_priority_sched_test\n");
    return 0;
}

int tie_priority_sched_test() {
    kprintf("tie_priority_sched_test\n");

    return 0;
}

int inversion_priority_sched_test() {
    kprintf("inversion_priority_sched_test\n");

    return 0;
}

int add_higher_thread_test() {
    kprintf("add_higher_thread_test\n");

    return 0;
}

int get_set_priority_test() {
    kprintf("before get_set_priority_test\n");
    struct thread *t = thread_create("sched/testing thread", NULL, DEFAULT_PRI);
    kassert(t);
    kprintf("%d \n", __LINE__);
    thread_start_context(t, idle_thread, NULL);
    kprintf("%d \n", __LINE__);
    int desired_priority = 63;
    thread_set_priority(desired_priority);
    kprintf("%d \n", __LINE__);
    kassert(desired_priority == thread_get_priority());

    kprintf("get_set_priority_test\n");

    return 0;
}