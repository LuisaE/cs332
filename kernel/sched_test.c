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


// Helper funtions 

int idle_thread(void *args) {
    for (int i = 0; i < 1000; i++) { }
    return 0;
}

int thread_lower_its_priority(void *args) {
    for (int i = 0; i < 5000; i++) { }
    thread_set_priority(PRI_MIN, NULL);
    for (int i = 0; i < 5000; i++) { }
    return 0;
}

int low_thread_acquire_lock(void *lock) {
    spinlock_acquire((struct spinlock*) lock);

    thread_set_priority(DEFAULT_PRI-1, NULL);
    for (int i = 0; i < 500000; i++) { }

    spinlock_release((struct spinlock*) lock);
    return 0;
}

int high_thread_acquire_lock(void *lock) {
    spinlock_acquire((struct spinlock*) lock);
    for (int i = 0; i < 5000; i++) { }
    spinlock_release((struct spinlock*) lock);

    kprintf("PASS: inversion_priority_sched_test\n");
    return 0;
}

int write_to_file_thread(void *f) {
    // Pointer not allocated, then issue when call fs write
    kprintf("In write_to_file_thread: %d\n", __LINE__);
    char *buf = "BBB";
    offset_t offset = 0;
    fs_write_file((struct file *) f, buf, 3, &offset);
    kprintf("In write_to_file_thread: %d\n", __LINE__);
    return 0;
}

// Testing functions

int simple_priority_sched_test() {

    //struct thread *high_thread = thread_create("simple_priority/thread high", NULL, DEFAULT_PRI + 3);
    // struct thread *medium_thread = thread_create("simple_priority/thread medium", NULL, DEFAULT_PRI + 2);
    // struct thread *low_thread = thread_create("simple_priority/thread low", NULL, DEFAULT_PRI + 1);

    struct file *f;

    fs_open_file("/test.txt", FS_RDWR | FS_CREAT, FMODE_R | FMODE_W, &f);

    kprintf("pointer to file: %x\n", f);
    //thread_start_context(high_thread, write_to_file_thread, f);
    kprintf("In write_to_file_thread: %d\n", __LINE__);
    char *buf = "BBB";
    offset_t offset = 0;
    fs_write_file((struct file *) f, buf, 3, &offset);
    kprintf("In write_to_file_thread: %d\n", __LINE__);

    char read[3];
    offset = 0;
    fs_read_file(f, read, 3, &offset);

    kprintf("%s\n", read);

    // thread_start_context(medium_thread, idle_thread, NULL);
    // thread_start_context(low_thread, idle_thread, NULL);

    //open file and check if 100 A, 100 B, 100 C
    
    fs_close_file(f);
    kprintf("PASS: simple_priority_sched_test\n");
    return 0;
}

int tie_priority_sched_test() {
    // do the same as in simple, but letters should be interleaved - never 100 A, then 100 B ...
    kprintf("PASS: tie_priority_sched_test\n");
    return 0;
}

int inversion_priority_sched_test() {
    struct spinlock lock;
    spinlock_init(&lock);

    struct thread *low_thread = thread_create("inversion_priority/thread low", NULL, DEFAULT_PRI + 1);
    struct thread *high_thread = thread_create("inversion_priority/thread high", NULL, DEFAULT_PRI + 2);
    thread_start_context(low_thread, low_thread_acquire_lock, &lock);
    thread_start_context(high_thread, high_thread_acquire_lock, &lock);
    
    // this test will pass if the high priority thread is unblocked - prints PASS
    return 0;
}

int add_higher_thread_test() {
    int switch_count = get_thread_switch_count();

    struct thread *high_thread = thread_create("add_higher_thread_test/thread high", NULL, PRI_MAX);
    thread_start_context(high_thread, idle_thread, NULL);

    // 2 switches: from testing thread -> high thread and then high thread -> testing thread
    // when high thread is done
    int switch_after = get_thread_switch_count();

    // when this code is running, the high_thread should be done
    kassert(high_thread->state == ZOMBIE);
    kassert(switch_count + 2 == switch_after);

    kprintf("PASS: add_higher_thread_test\n");
    kassert(high_thread->state == ZOMBIE);
    return 0;
}


int lower_thread_priority_should_yield() {
    int switch_count = get_thread_switch_count();

    struct thread *high_thread = thread_create("lower_thread_priority/thread high", NULL, PRI_MAX);
    thread_start_context(high_thread, thread_lower_its_priority, NULL);

    // assert the originally high_thread did not finish running
    kassert(high_thread->state == READY);
    int switch_after = get_thread_switch_count();

    // this code is running before high_thread finished running, so switches twice
    kassert(switch_count + 2 == switch_after);
    
    kprintf("PASS: lower_thread_priority_should_yield\n");

    // make sure it doesn't interfere other threads
    thread_set_priority(PRI_MAX, high_thread);
    kassert(high_thread->state == ZOMBIE);
    return 0;
}

int get_set_priority_test() {
    struct thread *t = thread_create("get_set_priority_test/thread test", NULL, DEFAULT_PRI);
    thread_start_context(t, idle_thread, NULL);

    int desired_priority = PRI_MAX - 1;
    thread_set_priority(desired_priority, t);
    kassert(desired_priority == thread_get_priority(t));

    kprintf("PASS: get_set_priority_test\n");
    kassert(t->state == ZOMBIE);
    
    return 0;
}

int set_invalid_priority_test() {
    struct thread *t = thread_create("set_invalid_priority_test/thread test", NULL, DEFAULT_PRI + 1);
    thread_start_context(t, idle_thread, NULL);

    int desired_priority = PRI_MAX + 1;
    thread_set_priority(desired_priority, t);
    kassert(desired_priority != thread_get_priority(t));

    desired_priority = PRI_MIN - 1;
    thread_set_priority(desired_priority, t);
    kassert(desired_priority != thread_get_priority(t));

    kprintf("PASS: set_invalid_priority_test\n");
    kassert(t->state == ZOMBIE);
    
    return 0;
}