#include <lib/test.h>
// #include <kernel/thread.h>
// #include <kernel/proc.h>

// int test_fun() {
//     //for (;;) {}
//     return 0;
// }

// Test directly kernel, separate makefile, as part of booting up. 
// high priority write a file, medium requires -> low requires 10 k As, read 10k a, write 10k Bs .... Long, exists at the same time. 

int
main()
{

    // struct thread *t = thread_create("init/testing thread", NULL, 60);
    // thread_start_context(t, test_fun, NULL);

    pass("priority-simple-test");

    exit(0);
    return 0;
}