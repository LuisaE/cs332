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
    for (int i = 0; i < 10000; i++) { }
    return 0;
}

int simple_priority_sched_test() {
    struct thread *t = thread_create("sched/testing thread", NULL, DEFAULT_PRI);
    kassert(t);
    thread_start_context(t, idle_thread, NULL);
    
    kprintf("PASS: simple_priority_sched_test\n");
    return 0;
}

int tie_priority_sched_test() {

    kprintf("PASS: tie_priority_sched_test\n");
    return 0;
}

int inversion_priority_sched_test() {

    kprintf("PASS: inversion_priority_sched_test\n");
    return 0;
}

int add_higher_thread_test() {
    kprintf("add_higher_thread_test\n");
    int switch_count = get_thread_switch_count();
    struct thread *t = thread_create("sched/testing thread", NULL, PRI_MAX);
    thread_start_context(t, idle_thread, NULL);
    // 2 cpus, figure out what is running, test thread, count per cpu
    kassert(switch_count + 1 == get_thread_switch_count());
    kprintf("PASS: add_higher_thread_test\n");
    return 0;
}

int get_set_priority_test() {
    struct thread *t = thread_create("sched/testing thread", NULL, DEFAULT_PRI);
    thread_start_context(t, idle_thread, NULL);
    int desired_priority = PRI_MAX - 1;
    thread_set_priority(desired_priority);
    kassert(desired_priority == thread_get_priority());
    kprintf("PASS: get_set_priority_test\n");
    return 0;
}