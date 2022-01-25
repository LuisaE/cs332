#ifndef _PIPE_H_
#define _PIPE_H_

#include <kernel/console.h>
#include <kernel/trap.h>
#include <lib/stdarg.h>
#include <kernel/cga.h>
#include <kernel/uart.h>
#include <kernel/keyboard.h>

static ssize_t pipe_read(struct file *file, void *buf, size_t count, offset_t *ofs);
static ssize_t pipe_write(struct file *file, const void *buf, size_t count, offset_t *ofs);
static void pipe_close(struct file *p);

static struct file_operations pipe_ops = {
    .read = pipe_read,
    .write = pipe_write,
    .close = pipe_close,
};

#endif /* _PIPE_H_ */
