#include <kernel/console.h>
#include <kernel/trap.h>
#include <lib/stdarg.h>
#include <kernel/cga.h>
#include <kernel/uart.h>
#include <kernel/keyboard.h>
#include <kernel/pipe.h>


ssize_t pipe_read(struct file *file, void *buf, size_t count, offset_t *ofs) {
    spinlock_acquire(&file->info->pipe_lock);
    file->info->read_end_status = True;
    //bbq_remove();
    if ((int) count > (int) *ofs) {
        // partial read
    }
    spinlock_release(&file->info->pipe_lock);
    return NULL; // for now
}

ssize_t pipe_write(struct file *file, const void *buf, size_t count, offset_t *ofs) {
    spinlock_acquire(&file->info->pipe_lock);
    file->info->write_end_status = True;
    //bbq_insert(q, *buf);
    spinlock_release(&file->info->pipe_lock);
    return NULL;
}

void pipe_close(struct file *p) {
    // check if other end is open
    kprintf("Test\n");
    spinlock_acquire(&p->info->pipe_lock);
    if (p->oflag == FS_RDONLY) {
        p->info->read_end_status = False;
        if (!p->info->write_end_status) {
            //fs_close_file(p);
        }
    } else {
        // close write
        p->info->write_end_status = False;
        if (!p->info->read_end_status) {
            //fs_close_file(p);
        }
    }
    fs_close_file(p);
    kprintf("Made it out!\n");
    spinlock_release(&p->info->pipe_lock);
}