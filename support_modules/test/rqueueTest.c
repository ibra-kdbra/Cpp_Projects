#include <stdio.h>

typedef struct
{
    int id;
}
MyData;

#include "rqueue.c"
RQUEUE_IMP(MyData)

int main(int argc, char** argv)
{
    RQueue rb;
    MyData* dat;
    int in, out;
    (void) argc;
    (void) argv;

    rqueue_init(&rb, 4, sizeof(MyData));
    for (in = 0; in < 4; ++in) {
        dat = rqueue_append_MyData(&rb);
        dat->id = in;
    }
    printf("used: %d\n", rb.used);
    for (out = 0; out < 2; ++out) {
        dat = rqueue_removeHead_MyData(&rb);
        printf(" %d", dat->id);
    }
    printf(" (used: %d)\n", rb.used);

    // Exceed reserved amount to test resize.
    for (; in < 8; ++in) {
        dat = rqueue_append_MyData(&rb);
        dat->id = in;
    }
    printf("used: %d\n", rb.used);
    // Empty buffer.
    while ((dat = rqueue_removeHead_MyData(&rb)))
        printf(" %d", dat->id);
    printf(" (used: %d)\n", rb.used);

    rqueue_free(&rb);
    return 0;
}
