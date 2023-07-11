#ifndef RQUEUE_H
#define RQUEUE_H
/*
    rqueue - A queue of values implemented with a circular buffer.
*/

#include <stdint.h>

typedef struct {
    void* values;
    uint32_t avail;
    uint32_t head;
    uint32_t used;
} RQueue;

#define RQUEUE_DECLARE(TYPE) \
    TYPE* rqueue_append_ ## TYPE(RQueue*); \
    TYPE* rqueue_removeHead_ ## TYPE(RQueue*);

#ifdef __cplusplus
extern "C" {
#endif

void  rqueue_init(RQueue*, int reserve, size_t elemSize);
void  rqueue_clear(RQueue*);
void  rqueue_free(RQueue*);

#ifdef __cplusplus
}
#endif

#endif  // RQUEUE_H
