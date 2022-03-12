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
#include <lib/string.h>


// Helper funtions 

#define BUFF_SIZE 32
#define LOOP 80000
#define DELAY 1

int idle_thread(void *args) {
    for (int i = 0; i < LOOP; i++) { }
    return 0;
}

int thread_lower_its_priority(void *args) {
    for (int i = 0; i < LOOP; i++) { }
    thread_set_priority(PRI_MIN, NULL);
    for (int i = 0; i < LOOP; i++) { }
    return 0;
}

int low_thread_acquire_lock(void *lock) {
    spinlock_acquire((struct spinlock*) lock);

    thread_set_priority(DEFAULT_PRI-1, NULL);
    for (int i = 0; i < LOOP; i++) { }

    spinlock_release((struct spinlock*) lock);
    return 0;
}

int high_thread_acquire_lock(void *lock) {
    spinlock_acquire((struct spinlock*) lock);
    for (int i = 0; i < LOOP; i++) { }
    spinlock_release((struct spinlock*) lock);

    kprintf("PASS: inversion_priority_sched_test\n");
    return 0;
}

int write_to_file_thread(void *f) {
#if DELAY
    for (int i = 0; i < LOOP; i++) { }
#endif
    char *buf = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
    offset_t offset = 0;
    fs_write_file((struct file *) f, buf, BUFF_SIZE, &offset);
#if DELAY
    for (int i = 0; i < LOOP; i++) { }
#endif
    return 0;
}

int write_to_file_thread_alt(void *f) {
    char *buf = "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB";
    offset_t offset = 0;
    fs_write_file((struct file *) f, buf, BUFF_SIZE, &offset);
    return 0;
}

int read_file_and_write(void *f) {
#if DELAY
    for (int i = 0; i < LOOP; i++) { }
#endif
    char *read = kmalloc(BUFF_SIZE);
    offset_t offset = 0;
    fs_read_file(f, read, BUFF_SIZE, &offset);
    
    kassert(strcmp(read, "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA") == 0);
    kfree(read);

    char *buf = "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB";
    offset = 0;
    fs_write_file((struct file *) f, buf, BUFF_SIZE, &offset);
#if DELAY
    for (int i = 0; i < LOOP; i++) { }
#endif
    return 0;
}

int read_file(void *f) {
#if DELAY
    for (int i = 0; i < LOOP; i++) { }
#endif
    char *read = kmalloc(BUFF_SIZE);
    offset_t offset = 0;
    fs_read_file(f, read, BUFF_SIZE, &offset);
    
    kassert(strcmp(read, "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB") == 0);
    kfree(read);
#if DELAY
    for (int i = 0; i < LOOP; i++) { }
#endif
    return 0;
}

// Testing functions

int simple_priority_sched_test() {
    // create three threads with different priorities
    struct thread *high_thread = thread_create("simple_priority/thread high", NULL, DEFAULT_PRI + 15);
    struct thread *medium_thread = thread_create("simple_priority/thread medium", NULL, DEFAULT_PRI + 10);
    struct thread *low_thread = thread_create("simple_priority/thread low", NULL, DEFAULT_PRI + 5);

    struct file *f;

    // create a file with read and write permissions
    fs_open_file("/test.txt", FS_RDWR | FS_CREAT, FMODE_R | FMODE_W, &f);

    // write something to the file
    char *buf = "*";
    offset_t offset = 0;
    fs_write_file((struct file *) f, buf, 1, &offset);

    // There is some donation happening here when thread_start_context -> sched_ready
    // We fix it by adding a loop to delay the threads from starting in between
    // start the threads and verify that high wrote 32s A
    thread_start_context(high_thread, write_to_file_thread, f);
    // medium read 32s A and write 32s B 
    thread_start_context(medium_thread, read_file_and_write, f);
    // low reads 32s Bs
    thread_start_context(low_thread, read_file, f);
    
    fs_close_file(f);
    kprintf("PASS: simple_priority_sched_test\n");
    return 0;
}

int tie_priority_sched_test() {
   
    struct thread *thread_a = thread_create("simple_priority/thread high", proc_current(), DEFAULT_PRI + 10);
    struct thread *thread_b = thread_create("simple_priority/thread high", proc_current(), DEFAULT_PRI + 10);
    
    struct file *f;
    // create a file with read and write permissions
    fs_open_file("/test_tie.txt", FS_RDWR | FS_CREAT, FMODE_R | FMODE_W, &f);

    // write something to the file
    char *buf = "*";
    offset_t offset = 0;
    fs_write_file((struct file *) f, buf, 1, &offset);

    thread_start_context(thread_a, write_to_file_thread, NULL);
    thread_start_context(thread_b, write_to_file_thread_alt, NULL);

    char *read = kmalloc(64);
    offset = 0;
    fs_read_file(f, read, BUFF_SIZE, &offset);
    
    // Make sure that the string written to file is interleved between A's and B's
    kassert(strcmp(read, "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB") != 0);
    kassert(strcmp(read, "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA") != 0);
    kassert(sizeof(read) == 64);
    kfree(read);

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
    struct thread *t = thread_create("set_invalid_priority_test/thread test", NULL, DEFAULT_PRI);

    // try to set a priority more than max
    int desired_priority = PRI_MAX + 1;
    thread_set_priority(desired_priority, t);
    kassert(desired_priority != thread_get_priority(t));

    // try to set a priority less than min
    desired_priority = PRI_MIN - 1;
    thread_set_priority(desired_priority, t);
    kassert(desired_priority != thread_get_priority(t));

    kprintf("PASS: set_invalid_priority_test\n");
    
    return 0;
}