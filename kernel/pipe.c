#include <kernel/console.h>
#include <kernel/trap.h>
#include <lib/stdarg.h>
#include <kernel/cga.h>
#include <kernel/uart.h>
#include <kernel/keyboard.h>


ssize_t pipe_read(struct file *file, void *buf, size_t count, offset_t *ofs) {
    return NULL;
}

ssize_t pipe_write(struct file *file, const void *buf, size_t count, offset_t *ofs) {
    return NULL;
}

void pipe_close(struct file *p) {

}