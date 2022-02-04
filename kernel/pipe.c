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
    spinlock_acquire(&file->info->pipe_lock);
    if (!file->info->write_end_status) {
        spinlock_release(&file->info->pipe_lock);
        // return remaining bytes in buf or 0 if otherwise
        return bbq_remove(file->info->bbq, (int) count, (char *) buf, True);
    }
    spinlock_release(&file->info->pipe_lock);
    return bbq_remove(file->info->bbq, (int) count, (char *) buf, False);
}

static ssize_t pipe_write(struct file *file, const void *buf, size_t count, offset_t *ofs) {
    spinlock_acquire(&file->info->pipe_lock);
    if (!file->info->read_end_status) {
        spinlock_release(&file->info->pipe_lock);
        return ERR_END;
    }
    spinlock_release(&file->info->pipe_lock);
    // write
    bbq_insert(file->info->bbq, (char*) buf, (int) count);
    return count;
}

static void pipe_close(struct file *p) {
    spinlock_acquire(&p->info->pipe_lock);
    if (p->oflag == FS_RDONLY) {
        p->info->read_end_status = False;
    } else {
        // close write
        p->info->write_end_status = False;
    }
    if (!p->info->read_end_status && !p->info->write_end_status) {
        // free pipe struct and bbq
        bbq_free(p->info->bbq);
        spinlock_release(&p->info->pipe_lock);
        kfree(p->info);
        return;
    }
    spinlock_release(&p->info->pipe_lock);
}

err_t pipe_init(struct file *read_file, struct file *write_file) {
    // Initialize pipe
    struct pipe *info = kmalloc(sizeof(struct info*));
    if (!info) {
        return ERR_NOMEM;
    }

    spinlock_init(&info->pipe_lock);
    info->bbq = kmalloc(sizeof(struct bbq));
    spinlock_acquire(&info->pipe_lock);
    info->bbq = bbq_init();

    static struct file_operations pipe_ops = {
        .read = pipe_read,
        .write = pipe_write,
        .close = pipe_close
    };

    // set file metadata
    read_file->f_ops = &pipe_ops;
    read_file->oflag = FS_RDONLY;
    info->read_end_status = True;

    write_file->f_ops = &pipe_ops;
    write_file->oflag = FS_WRONLY;
    info->write_end_status = True;
    spinlock_release(&info->pipe_lock);

    // point file info to the pipe struct
    read_file->info = info;
    write_file->info = info;

    return ERR_OK;
}