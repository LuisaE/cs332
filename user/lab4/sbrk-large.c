#include <lib/test.h>
#include <lib/stddef.h>

int
main()
{
    int pid, ret;
    char *a, *b, *lastaddr, *p;
    size_t amt;

    // try growing heap by 256 pages
    printf("1st\n");
    a = sbrk(0);
    amt = 256 * 4096;
    printf("2\n");
    if ((p = sbrk(amt)) != a) {
        error("sbrk test failed to grow big address space or did not return old bound, return value was %p", p);
    }
    lastaddr = (char *)(a + amt - 1);
    *lastaddr = 99;
    printf("3\n");

    // fork a child and make sure heap is inherited/copied over
    if ((pid = fork()) < 0) {
        error("sbrk test fork failed, return value was %d", pid);
    }
        printf("3\n");

    // child grow heap by two pages and exit
    if (pid == 0) {
        b = sbrk(4096);
        b = sbrk(4096);
        if (b != a + amt + 4096) {
            error("sbrk test failed post-fork, return value was %d", b);
        }
        exit(0);
    }
        printf("4st\n");

    // parent waits
    if ((ret = wait(pid, NULL)) != pid) {
        error("sbrk test wait failed, return value was %d", ret);
    }

    pass("sbrk-large");
    exit(0);
    return 0;
}