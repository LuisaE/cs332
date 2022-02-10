#include <kernel/proc.h>
#include <kernel/console.h>
#include <kernel/trap.h>
#include <kernel/vpmap.h>
#include <kernel/vm.h>
#include <arch/mmu.h>
#include <lib/string.h>
#include <lib/errcode.h>

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

    struct proc *p = proc_current();
    struct memregion *cur_memregion = as_find_memregion(&p->as, fault_addr, pg_size);
    if (!cur_memregion) { // CHECK PG_SIZE
        return;
    }

    if (write && (cur_memregion->perm == MEMPERM_R || cur_memregion->perm == MEMPERM_UR)) {
        return;
    }
    
    paddr_t paddr;
    if (pmem_alloc(&paddr) != ERR_OK) {
        return;
    }
    memset((void*) kmap_p2v(paddr), 0, pg_size);
    vpmap_map(cur_memregion->as->vpmap, kmap_p2v(paddr), paddr, 1, cur_memregion->perm); //ASK AARON ABOUT THE SIZE


    if (user) {
        proc_exit(-1);
    }
}
