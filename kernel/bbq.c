// bbq.c
// thread-safe blocking queue

#include <kernel/kmalloc.h>
#include <kernel/bbq.h>

// static prevents this variable from being visible outside this file
static struct kmem_cache *bbq_allocator;

// Wait until there is room and
// then insert an item.
void
bbq_insert(bbq *q, int item) {
    spinlock_acquire(&q->lock);
    while ((q->next_empty - q->front) == MAX) {
        condvar_wait(&q->item_removed, &q->lock);
    }
    q->items[q->next_empty % MAX] = item;
    q->next_empty++;
    condvar_signal(&q->item_added);
    spinlock_release(&q->lock);
}

// Wait until there is an item and 
// then remove an item.
int
bbq_remove(bbq *q) {
    int item;

    spinlock_acquire(&q->lock);
    while (q->front == q->next_empty) {
        condvar_wait(&q->item_added, &q->lock);
    }
    item = q->items[q->front % MAX];
    q->front++;
    condvar_signal(&q->item_removed);
    spinlock_release(&q->lock);
    return item;
}

// Initialize the queue to empty,
// the lock to free, and the
// condition variables to empty.
bbq* bbq_init() {
  bbq *q;

  // if the allocator has not been created yet, do so now
  if (bbq_allocator == NULL) {
    // ERROR HERE, check with Aaron
    // if ((bbq_allocator = kmem_cache_alloc(sizeof(bbq))) == NULL) {
    //   return NULL;
    // }
  }

  // allocate the BBQ struct
  if ((q = kmem_cache_alloc(bbq_allocator)) == NULL) {
    return NULL;
  }

  q->front = 0;
  q->next_empty = 0;
  spinlock_init(&q->lock);
  condvar_init(&q->item_added);
  condvar_init(&q->item_removed);

  return q;
}

void bbq_free(bbq *q) {
  kmem_cache_free(bbq_allocator, q);
}