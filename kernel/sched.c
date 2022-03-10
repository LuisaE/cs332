#include <arch/cpu.h>
#include <kernel/trap.h>
#include <kernel/sched.h>
#include <kernel/console.h>
#include <kernel/list.h>
#include <lib/errcode.h>
#include <lib/stddef.h>

List _ready_queue;
List* ready_queue = &_ready_queue;
size_t thread_switch_count = 0;

/* 
 * Schedules a new thread, if no thread on the ready queue
 * current cpu's idle thread is scheduled. Returns a thread
 * if the descheduled thread needs to be reclaimed.
 * */
static struct thread* sched(void);

void
sched_sys_init(void)
{
    list_init(ready_queue);
    spinlock_init(&sched_lock);
}

err_t
sched_start()
{
    kassert(intr_get_level() == INTR_OFF);
    cpu_set_idle_thread(mycpu(), thread_current());
    kprintf("cpu %d is up and scheduling \n", mycpu()->lapic_id);
    // turn on interrupt
    intr_set_level(INTR_ON);
    return ERR_OK;
}

err_t
sched_start_ap()
{
    kassert(intr_get_level() == INTR_OFF);
    // initialize idle thread and starts interrupt
    struct thread *t = thread_create("idle thread", NULL, DEFAULT_PRI);
    if (t == NULL) {
       return !ERR_OK;
    }
    cpu_set_idle_thread(mycpu(), t);
    kprintf("cpu %d is up and scheduling \n", mycpu()->lapic_id);
    // turn on interrupt
    intr_set_level(INTR_ON);
    return ERR_OK;
}

void
sched_ready(struct thread *t)
{
    kassert(t);
    spinlock_acquire(&sched_lock);
    t->state = READY;
    // take priority into consideration
    struct thread *curr = thread_current();
    if (curr->priority < t->priority) {
        // t takes over
        // add the current thread to the ready list
        if (curr != cpu_idle_thread(mycpu())) {
            curr->state = READY;
            list_append(ready_queue, &curr->node);
        }
        //list_append(ready_queue, &curr->node);
        t->state = RUNNING;
        thread_switch_count++;
        // schedule the max priority thread to run
        cpu_switch_thread(mycpu(), t);
    } else {
        list_append(ready_queue, &t->node);
    }

    spinlock_release(&sched_lock);
}

void
sched_sched(threadstate_t next_state, void* lock)
{
    struct thread *curr = thread_current();
    spinlock_acquire(&sched_lock);
    
    if (next_state == READY && curr != cpu_idle_thread(mycpu())) {
        list_append(ready_queue, &curr->node);
    }
    curr->state = next_state;
    if (lock) {
        lock_release(lock);
    }

    /* 
     * schedule a new thread and see if any thread needs to be cleaned up
     * newly scheduled thread will release the scheduler lock acquired by the previous thread
     */
    struct thread *dying = sched();
    spinlock_release(&sched_lock);
    if (dying) {
        thread_cleanup(dying);
    }
}

// function to schedule thread, sched_lock must be held before calling this function
static struct thread*
sched(void)
{
    void *cpu = mycpu();
    struct thread *curr = thread_current();
    struct thread *prev = NULL;
    struct thread *next = NULL;

    if (!list_empty(ready_queue)) {
        // schedule the highest priority thread in the ready queue to run
        next = get_max_priority_thread();
        kassert(next->state == READY);
        list_remove(&next->node);
    } else {
        // if current thread is not the idle thread, schedules to idle thread of the cpu
        struct thread *idle = cpu_idle_thread(cpu);
        next = idle;
    }

    next->state = RUNNING;
    thread_switch_count++;
    if (curr != next) {
        prev = cpu_switch_thread(cpu, next);
        kassert(prev);
    }
    return prev && prev->state == ZOMBIE ? prev : NULL;
}


void yield(threadstate_t next_state, void* lock) {
    struct thread *curr = thread_current();
    spinlock_acquire(&sched_lock);

    struct thread *max_priority_thread = NULL;
    void *cpu = mycpu();

    // get the highest priority thread in the ready list
    for (Node *n = list_begin(ready_queue); n != list_end(ready_queue); n = list_next(n)) {
        struct thread *t = (struct thread*) list_entry(n, struct thread, node);
        kassert(t->state == READY);
        if (t->priority > curr->priority) {
            max_priority_thread = t;
        }
    }

    // if there isn't a higher priority thread, return
    if (!max_priority_thread) {
        spinlock_release(&sched_lock);
        return;
    }

    // add the current thread to the ready list
    if (curr != cpu_idle_thread(mycpu())) {
        list_append(ready_queue, &curr->node);
    }
    curr->state = next_state;
    if (lock) {
        lock_release(lock);
    }
    // change the max thread to be running
    // and remove it from the ready list

    kassert(max_priority_thread->state == READY);
    list_remove(&max_priority_thread->node);
    max_priority_thread->state = RUNNING;
    thread_switch_count++;

    // schedule the max priority thread to run
    cpu_switch_thread(cpu, max_priority_thread);
    spinlock_release(&sched_lock);
}

struct thread* get_max_priority_thread() {
    // get the thread with the highest priority from the ready list
    struct thread* max_priority_thread = NULL;
    int max_priority = -1;
    for (Node *n = list_begin(ready_queue); n != list_end(ready_queue); n = list_next(n)) {
        struct thread *t = (struct thread*) list_entry(n, struct thread, node);
        if (max_priority < t->priority) {
            max_priority_thread = t;
            max_priority = t->priority;
        }
    }
    
    return max_priority_thread;
}

int get_thread_switch_count() {
    // helper function, used for testing
    return thread_switch_count;
}