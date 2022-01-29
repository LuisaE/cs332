#include <kernel/console.h>
#include <kernel/trap.h>
#include <lib/stdarg.h>
#include <kernel/cga.h>
#include <kernel/uart.h>
#include <kernel/keyboard.h>
#include <kernel/pipe.h>
#include <lib/errcode.h>
#include <string.h>

static ssize_t pipe_read(struct file *file, void *buf, size_t count, offset_t *ofs) {
    if (!file->info->write_end_status) {
        return 0;
    }
    char *ret = bbq_remove(&file->info->bbq);
    kprintf("Check ret in pipe_read %s \n", ret);
    // strcpy(, (char*) buf);
    // kprintf("Check buf in pipe_read %s \n", (char*) buf);
    return strlen((char*) buf);
}

static ssize_t pipe_write(struct file *file, const void *buf, size_t count, offset_t *ofs) {
    // kprintf("Write\n");
    if (!file->info->read_end_status) {
        return ERR_END;
    }
    // kprintf("Call bbq\n");
    bbq_insert(&file->info->bbq, (char*) buf);
    // kprintf("Check buf and count %s %d \n", (char*) buf, count);
    return strlen(buf); //Aaron
}

static void pipe_close(struct file *p) {
    // CHECK: Do we dealloc fd?
    spinlock_acquire(&p->info->pipe_lock);
    if (p->oflag == FS_RDONLY) {
        p->info->read_end_status = False;
    } else {
        // close write
        p->info->write_end_status = False;
    }
    spinlock_release(&p->info->pipe_lock);
    if (!p->info->read_end_status && !p->info->write_end_status) {
        // free pipe struct and bbq
        bbq_free(&p->info->bbq);
        kfree(p->info);
    }
}

err_t pipe_init(struct file *read_file, struct file *write_file) {
    // Initialize pipe
    struct pipe *info = kmalloc(sizeof(struct info*));
    if (!info) {
        return ERR_NOMEM;
    }

    condvar_init(&info->wait_cv);
    spinlock_init(&info->pipe_lock);
    info->bbq = *bbq_init();

    static struct file_operations pipe_ops = {
        .read = pipe_read,
        .write = pipe_write,
        .close = pipe_close
    };

    read_file->f_ops = &pipe_ops;
    read_file->oflag = FS_RDONLY;
    info->read_end_status = True;

    write_file->f_ops = &pipe_ops;
    write_file->oflag = FS_WRONLY;
    info->write_end_status = True;

    read_file->info = info;
    write_file->info = info;

    return ERR_OK;
}