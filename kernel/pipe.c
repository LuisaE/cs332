#include <kernel/console.h>
#include <kernel/trap.h>
#include <lib/stdarg.h>
#include <kernel/cga.h>
#include <kernel/uart.h>
#include <kernel/keyboard.h>
#include <kernel/pipe.h>


ssize_t pipe_read(struct file *file, void *buf, size_t count, offset_t *ofs) {
    spinlock_acquire(&file->info->pipe_lock);
    if (!file->info->write_end_status) {
        return 0;
    }
    //bbq_remove();
    // if ((int) count > (int) *ofs) {
    //     // partial read
    // }
    spinlock_release(&file->info->pipe_lock);
    return NULL; // for now
}

ssize_t pipe_write(struct file *file, const void *buf, size_t count, offset_t *ofs) {
    spinlock_acquire(&file->info->pipe_lock);
    //bbq_insert(q, *buf);
    if (!file->info->read_end_status) {
        return 0;
    }
    spinlock_release(&file->info->pipe_lock);
    return NULL;
}

void pipe_close(struct file *p) {
    // CHECK: Do we dealloc fd?
    spinlock_acquire(&p->info->pipe_lock);
    if (p->oflag == FS_RDONLY) {
        p->info->read_end_status = False;
    } else {
        // close write
        p->info->write_end_status = False;
    }
    spinlock_release(&p->info->pipe_lock);
}