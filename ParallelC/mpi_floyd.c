/*******************************************************************************
  Title          : mpi_floyd.c
  Author         : Michael J. Quinn, modified by Stewart Weiss
  Created on     : January 20, 2014
  Description    : Parallel version of Floyd's all-pairs, shortest-path
  Purpose        : To introduce various MPI primitives
  Usage          : mpi_floyd <input-matrix>
                       where <input-matrix> is a binary file containing first
                       number of rows, then number of columns, then the data in
                       row major order.
  Build with     : mpicc -Wall  -L../lib -I../include -o mpi_floyd \
                                mpi_floyd.c -lutils
  Modifications  :

*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "utils.h"


typedef int Element_type;
#define MPI_TYPE MPI_INT

void compute_shortest_paths (
          int id,            /* rank of calling process                    */
          int p,             /* number of processes in communicator group  */
          Element_type **a,  /* portion of matrix owned by calling process */
          int n,             /* number of rows and columns (must be square)*/
          MPI_Comm comm);    /* communicator handle                        */


int main (int argc, char *argv[])
{
    Element_type** adjmatrix;      /* Doubly-subscripted array */
    Element_type*  matrix_storage; /* Local portion of array elements */
    int     id;                    /* Process rank */
    int     nrows;                 /* Rows in matrix */
    int     ncols;                 /* Columns in matrix */
    int     p;                     /* Number of processes */
    double  time;
    double  max_time;
    double  total_time;
    int     error;
    char    errstring[127];

    MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &id);
    MPI_Comm_size (MPI_COMM_WORLD, &p);

    if ( argc < 2 ) {
        sprintf(errstring, "Usage: %s filename, where filename contains"
                           " binary adjacency matrix\n", argv[0]);
        terminate(id, errstring);
    }

    /* Read the matrix from the file named on the command line and
       distribute the rows to each process. Check error on return */
    read_and_distribute_matrix_byrows (argv[1],
                       (void *) &adjmatrix,
                       (void *) &matrix_storage,
                       MPI_TYPE, &nrows, &ncols, &error,
                       MPI_COMM_WORLD);

    /* Check if successful */
    if ( SUCCESS != error ) {
        terminate(id, "Error reading or allocating matrix.\n");
    }

    /* Check if the matrix is square and exit if not */
    if (nrows != ncols) {
        terminate(id, "Error: matrix is not square.\n");
    }


    /* Gather the submatrices from all processes onto process 0 and print
       out the entire matrix */
    collect_and_print_matrix_byrows ((void **) adjmatrix, MPI_TYPE, nrows, ncols,
            MPI_COMM_WORLD);

    /* Time how long the longest process takes to do its work.
       Start them at the gate together using barrier synchronization. */
    MPI_Barrier ( MPI_COMM_WORLD);

    /* Get the baseline time */
    time = -MPI_Wtime();

    /* Call the workhorse function */
    compute_shortest_paths (id, p, (Element_type **) adjmatrix,
                            ncols, MPI_COMM_WORLD);

    /* Get the time (each process gets its own local time) */
    time += MPI_Wtime();

    /* Use a reduction with the MAX operator to put the largest time
       on process 0 */
    MPI_Reduce ( &time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0,
                 MPI_COMM_WORLD);

    /* Use a reduction using the SUM operator to get the total time
       for comparing to the total serial time. */
    MPI_Reduce ( &time, &total_time, 1, MPI_DOUBLE, MPI_SUM, 0,
                 MPI_COMM_WORLD);

    if  (0 == id )
        printf ("Floyd, matrix size %d, %d processes. "
                "Elapsed time %6.4f seconds,"
                " Total time %6.4f  seconds\n",
                 ncols, p, max_time,total_time);

    /* Gather the submatrices from all processes onto process 0 and print
       out the entire matrix */
    collect_and_print_matrix_byrows ((void **) adjmatrix, MPI_TYPE, nrows, ncols,
    MPI_COMM_WORLD);

    MPI_Finalize();
    return 0;
}


void compute_shortest_paths (
          int id,            /* rank of calling process                    */
          int p,             /* number of processes in communicator group  */
          Element_type **a,  /* portion of matrix owned by calling process */
          int n,             /* number of rows and columns (must be square)*/
          MPI_Comm comm)     /* communicator handle                        */
{
    int  i, j, k;            /* for general use */
    int  local_index;        /* local index of broadcast row */
    int  root;               /* process controlling row to be bcast */
    Element_type* tmp;       /* holds the broadcast row        */
    int  nlocal_rows;        /* number of rows owned by process */

    /* Allocate a linear array large enough to hold one row of the matrix */
    tmp = (Element_type *) malloc (n * sizeof(Element_type));

    for (k = 0; k < n; k++) {
        /* Determine which process owns the kth row. */
        root = owner(k,p,n);
        /* If the executing process is the owner of the row, it has to send
           a copy to all other processes */
        if ( root == id ) {
            /* Compute row k's index in the submatrix owned by process id
               It is k - first row number assigned to task with rank id,
               which is floor(id*n/p)       */
            local_index = k - (id*n)/p;

            /* Copy the row from the submatrix owned by this process into
               the temporary array to be broadcast. We cannot broadcast the
               actual row because we will be updating it. */
            for (j = 0; j < n; j++)
                tmp[j] =  a[local_index][j];
        }
        /* Broadcast tmp from root to all other processes */
        MPI_Bcast (tmp, n, MPI_TYPE, root, comm);

        /* Compute the shortest paths that do not pass through vertices higher
           than k */
        nlocal_rows = size_of_block(id,n,p);
        for (i = 0; i < nlocal_rows; i++)
            for (j = 0; j < n; j++)
                a[i][j] = MIN(a[i][j],a[i][k]+tmp[j]);
    }
    free (tmp);
}