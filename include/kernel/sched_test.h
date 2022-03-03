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

int idle_thread(void *args);

int simple_priority_sched_test();

int tie_priority_sched_test();

int inversion_priority_sched_test();

int add_higher_thread_test();

int get_set_priority_test();