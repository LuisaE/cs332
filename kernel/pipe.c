#include <kernel/console.h>
#include <kernel/trap.h>
#include <lib/stdarg.h>
#include <kernel/cga.h>
#include <kernel/uart.h>
#include <kernel/keyboard.h>
#include <kernel/pipe.h>


ssize_t pipe_read(struct file *file, void *buf, size_t count, offset_t *ofs) {
    // spinlock_acquire(&((struct pipe*) file->info)->pipe_lock); -> CHECK w AARON
    return NULL; // for now
    // spinlock_release(&((struct pipe*) file->info)->pipe_lock);
}

ssize_t pipe_write(struct file *file, const void *buf, size_t count, offset_t *ofs) {
    return NULL;
}

void pipe_close(struct file *p) {
    fs_close_file(p);
}