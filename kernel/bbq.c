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
// Write
void
bbq_insert(struct bbq *q, char* item, int count) {
  spinlock_acquire(&q->lock);
  while ((q->next_empty - q->front) == MAX_SIZE) {
      condvar_wait(&q->item_removed, &q->lock);
  }

  for (int i = 0; i < count; i++) {
      q->items[q->next_empty++] = item[i];
      q->next_empty = q->next_empty % MAX_SIZE;
      if (q->front == q->next_empty) {
        break;
      }
  }
  condvar_signal(&q->item_added);
  spinlock_release(&q->lock);
}

// Wait until there is an item and 
// then remove an item.
// Read
int
bbq_remove(struct bbq *q, int count, char* item) {
    spinlock_acquire(&q->lock);
    while (q->front == q->next_empty) {
        condvar_wait(&q->item_added, &q->lock);
    }
    //char* item = kmalloc(count + 1);
    // for (int i = 0; i < count; i++) {
    //   item[i] = q->items[(q->front + i) % MAX_SIZE];
    // }
    int i = 0;
    // i < count?
    while (q->front != q->next_empty) {
      item[i++] = q->items[q->front++];
      q->front = q->front % MAX_SIZE;
    }
    condvar_signal(&q->item_removed);
    spinlock_release(&q->lock);
    return i;
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
  spinlock_init(&q->lock);
  condvar_init(&q->item_added);
  condvar_init(&q->item_removed);

  return q;
}

void bbq_free(struct bbq *q) {
  kmem_cache_free(bbq_allocator, q);
}

void print_string(void* buf) {
  kprintf("Start buf: ");
  for (int i = 0; i < sizeof(buf); i++) {
    kprintf("%x ", ((char*) buf)[i]);
  }
  kprintf("\n");
}

void bbq_print(struct bbq *q) {
  kprintf("Printing BBQ states: \n");
  kprintf("Front: %d \n", q->front);
  kprintf("Next empty: %d \n", q->next_empty);
  print_string((char*) q->items);
}