/*
    rqueue - A queue of values implemented with a circular buffer.
*/
/*
    This is a C template for an expandable queue.

    To generate the source implementation:

        #include "rqueue.c"
        RQUEUE_IMP(MyStruct)

    If rqueue.c is included in more than one file in a project then all other
    inclusions must be preceeded by a "#define RQUEUE_NO_BASE" line.

    To generate header declarations:

        #include "rqueue.h"
        RQUEUE_DECLARE(MyStruct);
*/

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "rqueue.h"

#ifdef RQUEUE_NO_BASE
extern void rqueue_resize(RQueue* rb, int count, size_t elemSize);
#else
void rqueue_resize(RQueue* rb, int count, size_t elemSize)
{
    void* newValues = malloc(count * elemSize);
    assert(newValues);

    if (rb->used) {
        uint8_t* src = ((uint8_t*) rb->values) + rb->head * elemSize;
        if (rb->head + rb->used > rb->avail) {
            size_t len1 = rb->avail - rb->head;
            size_t len2 = rb->used - len1;

            len1 *= elemSize;
            memcpy(newValues, src, len1);
            memcpy(((uint8_t*) newValues) + len1, rb->values, len2 * elemSize);
        } else
            memcpy(newValues, src, rb->used * elemSize);

        rb->head = 0;
    }

    free(rb->values);
    rb->values = newValues;
    rb->avail = count;
}

void rqueue_init(RQueue* rb, int reserve, size_t elemSize)
{
    memset(rb, 0, sizeof(RQueue));
    if (reserve > 0)
        rqueue_resize(rb, reserve, elemSize);
}

void rqueue_clear(RQueue* rb)
{
    rb->head = rb->used = 0;
}

void rqueue_free(RQueue* rb)
{
    free(rb->values);
    rb->values = NULL;
    rb->avail = rb->used = 0;
}
#endif

/*
 * TYPE* rqueue_append_<TYPE>(RQueue*)
 * Return pointer to a value in the buffer which the caller must initialize.
 *
 * TYPE* rqueue_removeHead_<TYPE>(RQueue*)
 * Return pointer to the previous head value.
 */

#define RQUEUE_IMP(T) \
T* rqueue_append_ ## T(RQueue* rb) { \
    uint32_t next;  \
    if (rb->avail == 0)  \
        rqueue_resize(rb, 8, sizeof(T)); \
    else if (rb->used == rb->avail) \
        rqueue_resize(rb, rb->avail * 2, sizeof(T)); \
    next = (rb->head + rb->used) % rb->avail; \
    rb->used++; \
    return ((T*) rb->values) + next; \
} \
T* rqueue_removeHead_ ## T(RQueue* rb) { \
    uint32_t cur, next; \
    if (rb->used) { \
        rb->used--; \
        cur = rb->head; \
        next = cur + 1; \
        rb->head = (next >= rb->avail) ? 0 : next; \
        return ((T*) rb->values) + cur; \
    } \
    return NULL; \
}
