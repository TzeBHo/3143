#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/node.h"
#include "include/reading.h"
#include "include/satellite.h"
#include "include/station.h"

#define QUEUE_SIZE 16

reading_t** readings;
int r_count = 0;

int main(int argc, char** argv) {
    /* Variables for all processes  */

    int size, rank;

    /* Start up initial MPI environment */

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        /* Allocate queue */

        reading_t* r;

        readings = q_create(QUEUE_SIZE);

        r = create_reading(0, 0, 1000);
        q_push(readings, r_count++, QUEUE_SIZE, r);
        r = create_reading(0, 1, 2000);
        q_push(readings, r_count++, QUEUE_SIZE, r);
        r = create_reading(0, 2, 3000);
        q_push(readings, r_count++, QUEUE_SIZE, r);

        q_print(readings, r_count);

        /* Main process is the station */

        printf("Root initialised\n");
    } else {
        /* All other processes are nodes */

        printf("Node %d initialised\n", rank);
    }

    /* Clean up */

    MPI_Finalize();
    q_clear(readings, r_count);
    free(readings);
    return 0;
}
