// bbq.h 
// Thread-safe blocking queue.

#ifndef _BBQ_H_
#define _BBQ_H_
#include <kernel/synch.h>

#define MAX 512

typedef struct {
  // Synchronization variables
    struct spinlock lock;
    struct condvar item_added;
    struct condvar item_removed;

  // State variables
    int items[MAX];
    int front;
    int next_empty;
} bbq;

bbq* bbq_init();
void bbq_free(bbq *q);
void bbq_insert(bbq *q, int item);
int bbq_remove(bbq *q);
#endif