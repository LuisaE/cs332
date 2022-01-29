// bbq.c
// thread-safe blocking queue

#include <kernel/kmalloc.h>
#include <kernel/bbq.h>
#include <string.h>
#include <kernel/console.h>

// static prevents this variable from being visible outside this file
static struct kmem_cache *bbq_allocator;

// Wait until there is room and
// then insert an item.
void
bbq_insert(struct bbq *q, char* item) {
  spinlock_acquire(&q->lock);
  while ((q->next_empty - q->front) == MAX_SIZE) {
      condvar_wait(&q->item_removed, &q->lock);
  }
  strcpy(&q->items[q->next_empty % MAX_SIZE], item);
  q->next_empty += strlen(item);
  condvar_signal(&q->item_added);
  spinlock_release(&q->lock);
}

// Wait until there is an item and 
// then remove an item.
char*
bbq_remove(struct bbq *q) {
    spinlock_acquire(&q->lock);
    while (q->front == q->next_empty) {
        condvar_wait(&q->item_added, &q->lock);
    }
    char* item = kmalloc(sizeof(q->items[q->front % MAX_SIZE]) + 1);
    strcpy(item, &q->items[q->front % MAX_SIZE]);
    q->front += strlen(item);
    condvar_signal(&q->item_removed);
    spinlock_release(&q->lock);
    return item;
}

// Initialize the queue to empty,
// the lock to free, and the
// condition variables to empty.
struct bbq* bbq_init() {
  struct bbq *q;

  // if the allocator has not been created yet, do so now
  if (bbq_allocator == NULL) {
    if ((bbq_allocator = kmem_cache_create(sizeof(struct bbq))) == NULL) {
      return NULL;
    }
  }
  // allocate the BBQ struct
  if ((q = kmem_cache_alloc(bbq_allocator)) == NULL) {
    return NULL;
  }

  q->front = 0;
  q->next_empty = 0;
  q->items[0] = '\0';
  spinlock_init(&q->lock);
  condvar_init(&q->item_added);
  condvar_init(&q->item_removed);

  return q;
}

void bbq_free(struct bbq *q) {
  kprintf("In bbq free\n");
  kmem_cache_free(bbq_allocator, q);
  kprintf("In bbq free 2\n");
}