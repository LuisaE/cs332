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
    thread_set_priority(PRI_MIN);
    for (int i = 0; i < 5000; i++) { }
    return 0;
}

int write_to_file_thread(void *args) {
    char *buf = "A";
    offset_t offset = 0;
    for (int i = 0; i < 100; i++) { 
        fs_write_file((struct file *)args, (void *) buf, 1, &offset);
    }
    return 0;
}

// Testing functions

int simple_priority_sched_test() {
    struct thread *high_thread = thread_create("sched/testing thread 1", NULL, DEFAULT_PRI + 3);
    struct thread *medium_thread = thread_create("sched/testing thread 2", NULL, DEFAULT_PRI + 2);
    struct thread *low_thread = thread_create("sched/testing thread 3", NULL, DEFAULT_PRI + 1);

    struct file *f;

    // ASK Aaron!
    fs_open_file("/", FS_RDWR, 0, &f);
    kprintf("file size: %d\n", f->f_inode->i_size);
    kprintf("In test: %d\n", __LINE__);

    thread_start_context(high_thread, write_to_file_thread, (void *) f);

    kprintf("In test: %d\n", __LINE__);

    char *read = kmalloc(101);
    offset_t offset = 0;

    kprintf("file size: %d\n", f->f_inode->i_size);
    kprintf("In test: %d\n", __LINE__);

    fs_read_file(f, (void *) read, 1, &offset);

    kprintf("In test: %d\n", __LINE__);

    kprintf("%s\n", read);

    kprintf("In test: %d\n", __LINE__);
    thread_start_context(medium_thread, idle_thread, NULL);
    thread_start_context(low_thread, idle_thread, NULL);

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

    kprintf("PASS: inversion_priority_sched_test\n");
    return 0;
}

int add_higher_thread_test() {
    int switch_count = get_thread_switch_count();

    struct thread *high_thread = thread_create("sched/high thread", NULL, PRI_MAX);
    thread_start_context(high_thread, idle_thread, NULL);

    // 2 switches: from testing thread -> high thread and then high thread -> testing thread
    // when high thread is done
    int switch_after = get_thread_switch_count();

    // when this code is running, the high_thread should be done
    kassert(high_thread->state == ZOMBIE);
    kassert(switch_count + 2 == switch_after);

    kprintf("PASS: add_higher_thread_test\n");
    return 0;
}


int lower_thread_priority_should_yield() {
    int switch_count = get_thread_switch_count();

    struct thread *high_thread = thread_create("sched/high thread", NULL, PRI_MAX);
    thread_start_context(high_thread, thread_lower_its_priority, NULL);

    // assert the originally high_thread did not finish running
    kassert(high_thread->state == READY);
    int switch_after = get_thread_switch_count();

    // this code is running before high_thread finished running, so switches twice
    kassert(switch_count + 2 == switch_after);
    
    kprintf("PASS: lower_thread_priority_should_yield\n");
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