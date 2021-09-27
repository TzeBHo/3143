/* Gets the neighbors in a cartesian communicator
 * Orginally written by Mary Thomas
 * - Updated Mar, 2015
 * Link: https://edoras.sdsu.edu/~mthomas/sp17.605/lectures/MPICart-Comms-and-Topos.pdf
 * Minor modifications to fix bugs and to revise print output
 */
#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define SHIFT_ROW 0
#define SHIFT_COL 1
#define DISP 1
// Set to a small value so we get more collisions
#define PRIME_RANGE 10
#define REPEAT 10

/**
 * Check if an integer is a prime
 *
 * @param p Integer
 * @return int 1 if is prime, 0 otherwise
 */
int is_prime(int p) {
    for (int i = 2; i <= ((int) sqrt(p)); i++) {
        if (p % i == 0) return 0;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    int ndims = 2, size, my_rank, reorder, my_cart_rank, ierr;
    int nrows, ncols;
    int nbr_i_lo, nbr_i_hi;
    int nbr_j_lo, nbr_j_hi;
    MPI_Comm comm2D;
    int dims[ndims], coord[ndims];
    int wrap_around[ndims];

    /* start up initial MPI environment */
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    /* process command line arguments*/
    if (argc == 3) {
        nrows = atoi(argv[1]);
        ncols = atoi(argv[2]);
        dims[0] = nrows; /* number of rows */
        dims[1] = ncols; /* number of columns */
        if ((nrows * ncols) != size) {
            if (my_rank == 0)
                printf(
                    "ERROR: nrows*ncols)=%d *%d = %d != %d\n", nrows, ncols, nrows * ncols, size);
            MPI_Finalize();
            return 0;
        }
    } else {
        nrows = ncols = (int) sqrt(size);
        dims[0] = dims[1] = 0;
    }
    /************************************************************
     */
    /* create cartesian topology for processes */
    /************************************************************
     */
    MPI_Dims_create(size, ndims, dims);
    if (my_rank == 0)
        printf(
            "Root Rank: %d. Comm Size: %d: Grid Dimension =[%d x %d] \n",
            my_rank,
            size,
            dims[0],
            dims[1]);
    /* create cartesian mapping */
    wrap_around[0] = wrap_around[1] = 0; /* periodic shift is.false. */
    reorder = 1;
    ierr = 0;
    ierr = MPI_Cart_create(MPI_COMM_WORLD, ndims, dims, wrap_around, reorder, &comm2D);
    if (ierr != 0) printf("ERROR[%d] creating CART\n", ierr);

    /* find my coordinates in the cartesian communicator group */
    MPI_Cart_coords(comm2D, my_rank, ndims, coord);
    /* use my cartesian coordinates to find my rank in cartesian
    group*/
    MPI_Cart_rank(comm2D, coord, &my_cart_rank);

    /* get my neighbors; axis is coordinate dimension of shift */
    /* axis=0 ==> shift along the rows: P[my_row-1]: P[me] :
    P[my_row+1] */
    /* axis=1 ==> shift along the columns P[my_col-1]: P[me] :
    P[my_col+1] */
    MPI_Cart_shift(comm2D, SHIFT_ROW, DISP, &nbr_i_lo, &nbr_i_hi);
    MPI_Cart_shift(comm2D, SHIFT_COL, DISP, &nbr_j_lo, &nbr_j_hi);

    /* Process communication */

    // Open file
    char filename[16];
    sprintf(filename, "%02d.out", my_rank);
    FILE *fid = fopen(filename, "w");

    // Seed rng
    // xor to reduce collisions based on time e.g. time = 10 will collide with time = 8 + 2 rank
    srand((time(NULL) ^ 6700417) + my_rank);

    for (int i = 0; i < REPEAT; i++) {
        // Find a prime
        int randomPrime;
        while (1) {
            randomPrime = (rand() % PRIME_RANGE) + 2;  // +2 to skip { 0, 1 }
            if (is_prime(randomPrime)) break;
        }

        // MPI outputs
        MPI_Request send_request[4];
        MPI_Request receive_request[4];
        MPI_Status send_status[4];
        MPI_Status receive_status[4];

        // Send primes to adjacent
        MPI_Isend(&randomPrime, 1, MPI_INT, nbr_i_lo, 0, comm2D, &send_request[0]);
        MPI_Isend(&randomPrime, 1, MPI_INT, nbr_i_hi, 0, comm2D, &send_request[1]);
        MPI_Isend(&randomPrime, 1, MPI_INT, nbr_j_lo, 0, comm2D, &send_request[2]);
        MPI_Isend(&randomPrime, 1, MPI_INT, nbr_j_hi, 0, comm2D, &send_request[3]);

        // Receive primes from adjacent
        int recvValL = -1, recvValR = -1, recvValT = -1, recvValB = -1;
        MPI_Irecv(&recvValT, 1, MPI_INT, nbr_i_lo, 0, comm2D, &receive_request[0]);
        MPI_Irecv(&recvValB, 1, MPI_INT, nbr_i_hi, 0, comm2D, &receive_request[1]);
        MPI_Irecv(&recvValL, 1, MPI_INT, nbr_j_lo, 0, comm2D, &receive_request[2]);
        MPI_Irecv(&recvValR, 1, MPI_INT, nbr_j_hi, 0, comm2D, &receive_request[3]);

        MPI_Waitall(4, send_request, send_status);
        MPI_Waitall(4, receive_request, receive_status);

        /* Check and write primes to file */
        if (recvValT == randomPrime) {
            printf("Rank %d == Rank %d on %d\n", my_rank, nbr_i_lo, randomPrime);
            fprintf(fid, "Rank %d == Rank %d on %d\n", my_rank, nbr_i_lo, randomPrime);
        }
        if (recvValB == randomPrime) {
            printf("Rank %d == Rank %d on %d\n", my_rank, nbr_i_hi, randomPrime);
            fprintf(fid, "Rank %d == Rank %d on %d\n", my_rank, nbr_i_hi, randomPrime);
        }
        if (recvValL == randomPrime) {
            printf("Rank %d == Rank %d on %d\n", my_rank, nbr_j_lo, randomPrime);
            fprintf(fid, "Rank %d == Rank %d on %d\n", my_rank, nbr_j_lo, randomPrime);
        }
        if (recvValR == randomPrime) {
            printf("Rank %d == Rank %d on %d\n", my_rank, nbr_j_hi, randomPrime);
            fprintf(fid, "Rank %d == Rank %d on %d\n", my_rank, nbr_j_hi, randomPrime);
        }
    }

    fclose(fid);
    fflush(stdout);
    MPI_Comm_free(&comm2D);
    MPI_Finalize();
    return 0;
}
