#include "reading.h"

#include <stdio.h>
#include <stdlib.h>

/**
 * Pop a reading from the queue. Remember to un-allocate memory when done!
 *
 * @param queue Queue
 * @param count Number of elements in the queue
 * @return reading_t* Pointer to reading at the top of the queue
 */
reading_t* q_pop(reading_t** queue, int count) {
    if (count == 0) return NULL;

    // Pointer to return
    reading_t* tmp = queue[0];

    // Shift all elements up
    for (int i = 1; i < count; i++) queue[i - 1] = queue[i];
    queue[count - 1] = 0;

    return tmp;
};

/**
 * Push a reading to the queue. Removes and un-allocates first item if full
 *
 * @param queue Queue
 * @param count Number of elements in the queue
 * @param maxsize Maximum size of the queue
 * @param r Pointer to reading
 * @return int Number of elements in the queue after pushing
 */
int q_push(reading_t** queue, int count, int maxsize, reading_t* r) {
    if (count == maxsize) {
        reading_t* tmp = q_pop(queue, count--);
        free(tmp);
    }
    queue[count] = r;
    return count + 1;
};

void print_reading(reading_t r) { printf("(%i, %i, %.1f)\n", r.i, r.j, r.height); }

void q_print(reading_t** queue, int count) {
    for (int i = 0; i < count; i++) {
        printf("%i. ", i);
        print_reading(*queue[i]);
    }
}

reading_t* create_reading(int i, int j, float height) {
    reading_t* r = malloc(sizeof(reading_t));
    r->i = i;
    r->j = j;
    r->height = height;
    return r;
}

reading_t** q_create(int size) {
    reading_t** r = calloc(size, sizeof(reading_t*));
    return r;
}

void q_clear(reading_t** queue, int count) {
    for (int i = 0; i < count; i++) {
        free(queue[i]);
        queue[i] = 0;
    }
}
