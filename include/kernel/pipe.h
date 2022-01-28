#ifndef _PIPE_H_
#define _PIPE_H_

#include <kernel/console.h>
#include <kernel/trap.h>
#include <lib/stdarg.h>
#include <kernel/cga.h>
#include <kernel/uart.h>
#include <kernel/keyboard.h>
#include <kernel/bbq.h>

ssize_t pipe_read(struct file *file, void *buf, size_t count, offset_t *ofs);
ssize_t pipe_write(struct file *file, const void *buf, size_t count, offset_t *ofs);
void pipe_close(struct file *p);

struct pipe {
    bool write_end_status;  // whether write end is open
    bool read_end_status; // whether read end is open
    struct condvar wait_cv;
    struct spinlock pipe_lock;
    bbq bbq;
};

#endif /* _PIPE_H_ */
