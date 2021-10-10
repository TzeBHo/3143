/**
 * file: queue.h
 * author: Haotong Wang
 * id: 30635500
 * created: 10-10-2021
 * modified: 10-10-2021
 * desc: Queue data type, inspired from
 * https://stackoverflow.com/questions/22727404/making-a-tuple-in-c/22727432
 */

typedef struct reading {
    int i;
    int j;
    float height;
} reading_t;

reading_t** q_create(int size);

reading_t* create_reading(int i, int j, float height);

reading_t* q_pop(reading_t** queue, int count);

int q_push(reading_t** queue, int count, int maxsize, reading_t* r);

void q_print(reading_t** queue, int count);

void print_reading(reading_t r);

void q_clear(reading_t** queue, int count);
