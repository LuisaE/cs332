#ifndef _PIPE_H_
#define _PIPE_H_

#include <kernel/bbq.h>

err_t pipe_init(struct file *read_file, struct file *write_file);

struct pipe {
    bool write_end_status;  // whether write end is open
    bool read_end_status; // whether read end is open
    struct condvar wait_cv;
    struct spinlock pipe_lock;
    struct bbq bbq;
};

#endif /* _PIPE_H_ */
