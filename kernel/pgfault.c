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
    } else {
        panic("Kernel error in page fault handler\n");
    }

    // turn on interrupt now that we have the fault address 
    intr_set_level(INTR_ON);
    struct proc *p = proc_current();
    struct memregion *cur_memregion = as_find_memregion(&p->as, fault_addr, 1);

    if (present && write && cur_memregion && 
    (cur_memregion->perm == MEMPERM_RW || cur_memregion->perm == MEMPERM_URW)) {
        // allocate a new physical page
        paddr_t paddr;
        paddr_t pfault_addr;
        if (pmem_alloc(&paddr) != ERR_OK) {
            proc_exit(-1);
        }
        // reset the to 0
        memset((void*) kmap_p2v(paddr), 0, pg_size);
        // copy the data from the copy-on-write page to the newly allocated page
        memcpy((void*) kmap_p2v(paddr), (void*) pg_round_down(fault_addr), pg_size);
        // set permission of new page to be the permission of the current memory region
        vpmap_set_perm(cur_memregion->as->vpmap, kmap_p2v(paddr), pg_size, cur_memregion->perm);
        // get the physical address of the fault address
        vpmap_lookup_vaddr(cur_memregion->as->vpmap, pg_round_down(fault_addr), &pfault_addr, NULL);
        // decrement the count of the physical fault address's page
        struct page *page;
        page = paddr_to_page(pfault_addr);
        kprintf("About to decrement %d \n", page->refcnt);
        pmem_dec_refcnt(pfault_addr);
        return;
    }

    if (present) {
        // is page protection issue, just exit
        proc_exit(-1);
    }

    if (!cur_memregion) {
        // invalid region, exit
        proc_exit(-1);
    }

    if (write && (cur_memregion->perm == MEMPERM_R || cur_memregion->perm == MEMPERM_UR)) {
        // invalid permission
        proc_exit(-1);
    }
    
    paddr_t paddr;
    if (pmem_alloc(&paddr) != ERR_OK) {
        proc_exit(-1);
    }
    memset((void*) kmap_p2v(paddr), 0, pg_size);

    err_t alloc_status = vpmap_map(cur_memregion->as->vpmap, fault_addr, paddr, 1, cur_memregion->perm);

    if (alloc_status != ERR_OK) {
        proc_exit(-1);
    }

    return;
}
