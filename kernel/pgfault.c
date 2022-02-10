#include <kernel/proc.h>
#include <kernel/console.h>
#include <kernel/trap.h>
#include <kernel/vpmap.h>

size_t user_pgfault = 0;


void
handle_page_fault(vaddr_t fault_addr, int present, int write, int user) {
    if (user) {
        __sync_add_and_fetch(&user_pgfault, 1);
    }
    // turn on interrupt now that we have the fault address 
    intr_set_level(INTR_ON);

    if (!present) {
        return;
    }

    // list stack
    // as_map_findmemregion -> give address and return 
    struct proc *p = proc_current();
    if (write && (p->as.heap->perm == MEMPERM_R || p->as.heap->perm == MEMPERM_UR)) {
        return;
    }

    /* Your Code Here. */
    if (user) {
        proc_exit(-1);
    }
}
