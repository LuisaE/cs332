// bbq.h 
// Thread-safe blocking queue.

#ifndef _BBQ_H_
#define _BBQ_H_
#include <kernel/synch.h>

#define MAX_SIZE 512

struct bbq {
  // Synchronization variables
  struct spinlock lock;
  struct condvar item_added;
  struct condvar item_removed;

  // State variables
  char items[MAX_SIZE];
  int front;
  int next_empty;
};

struct bbq* bbq_init();
void bbq_free(struct bbq *q);
void bbq_insert(struct bbq *q, char* item, int count);
int bbq_remove(struct bbq *q, int count, char *item);
void print_string(void* buf);
void bbq_print(struct bbq* q);

#endif