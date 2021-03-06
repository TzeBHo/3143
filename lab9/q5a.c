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
#define SHIFT_ROW 0
#define SHIFT_COL 1
#define SHIFT_DEP 2
#define DISP 1
int main(int argc, char *argv[]) {
    int ndims = 3, size, my_rank, reorder, my_cart_rank, ierr;
    int nrows, ncols, ndepth;
    int nbr_i_lo, nbr_i_hi;
    int nbr_j_lo, nbr_j_hi;
    int nbr_k_lo, nbr_k_hi;
    MPI_Comm comm3D;
    int dims[ndims], coord[ndims];
    int wrap_around[ndims];
    /* start up initial MPI environment */
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    /* process command line arguments*/
    if (argc == 4) {
        nrows = atoi(argv[1]);
        ncols = atoi(argv[2]);
        ndepth = atoi(argv[3]);
        dims[0] = nrows;  /* number of rows */
        dims[1] = ncols;  /* number of columns */
        dims[2] = ndepth; /* magnitude of depth */
        if ((nrows * ncols * ndepth) != size) {
            if (my_rank == 0)
                printf(
                    "ERROR: nrows*ncols)=%d *%d = %d != %d\n", nrows, ncols, nrows * ncols, size);
            MPI_Finalize();
            return 0;
        }
    } else {
        nrows = ncols = ndepth = (int) cbrt(size);
        dims[0] = dims[1] = dims[2] = 0;
    }
    /************************************************************
     */
    /* create cartesian topology for processes */
    /************************************************************
     */
    MPI_Dims_create(size, ndims, dims);
    if (my_rank == 0)
        printf(
            "Root Rank: %d. Comm Size: %d: Grid Dimension =[%d x %d x %d] \n",
            my_rank,
            size,
            dims[0],
            dims[1],
            dims[2]);
    /* create cartesian mapping */
    wrap_around[0] = wrap_around[1] = wrap_around[2] = 0; /* periodic shift is.false. */
    reorder = 1;
    ierr = 0;
    ierr = MPI_Cart_create(MPI_COMM_WORLD, ndims, dims, wrap_around, reorder, &comm3D);
    if (ierr != 0) printf("ERROR[%d] creating CART\n", ierr);
    /* find my coordinates in the cartesian communicator group */
    MPI_Cart_coords(comm3D, my_rank, ndims, coord);

    /* use my cartesian coordinates to find my rank in cartesian
    group*/
    MPI_Cart_rank(comm3D, coord, &my_cart_rank);

    /* get my neighbors; axis is coordinate dimension of shift */
    MPI_Cart_shift(comm3D, SHIFT_ROW, DISP, &nbr_i_lo, &nbr_i_hi);
    MPI_Cart_shift(comm3D, SHIFT_COL, DISP, &nbr_j_lo, &nbr_j_hi);
    MPI_Cart_shift(comm3D, SHIFT_DEP, DISP, &nbr_k_lo, &nbr_k_hi);

    // Print out neighbours, ordered to match the diagram
    printf(
        "Global rank: %d. Cart rank: %d. Coord: (%d, %d, %d).Left:% d.Right:% d.Top:% d.Bottom:% "
        "d.Front:% d.Back% d\n ",
        my_rank,
        my_cart_rank,
        coord[0],
        coord[1],
        coord[2],
        nbr_k_lo,
        nbr_k_hi,
        nbr_i_lo,
        nbr_i_hi,
        nbr_j_hi,
        nbr_j_lo);

    fflush(stdout);
    MPI_Comm_free(&comm3D);
    MPI_Finalize();
    return 0;
}
