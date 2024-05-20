/*******************************************************************************
  Title          : utilities_basic.h
  Author         : Stewart Weiss, based on code by Michael Quinn
  Created on     : February 10, 2014
  Description    : Header file for functions used non-MPI programs
  Purpose        : Provides definitions for utility functions
  Build with     : no build
  License        : Copyright 2019 Stewart Weiss

      This code is free software: you can redistribute it and/or modify
      it under the terms of the GNU General Public License as published by
      the Free Software Foundation, either version 3 of the License, or
      (at your option) any later version.

      This code is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
      GNU General Public License for more details.

      You should have received a copy of the GNU General Public License
      along with this program.  If not, see <https://www.gnu.org/licenses/>.



*******************************************************************************/

#ifndef __UTILITIES_BASIC_H__
#define __UTILITIES_BASIC_H__


#define SUCCESS             0
#define MALLOC_ERROR        1
#define OPEN_FILE_ERROR     2
#define TYPE_ERROR          3
#define FILE_READ_ERROR     4
#define BAD_MATRIX_SIZE     5
#define BAD_ELEMENT_SIZE    6

#define MIN(a,b)           ((a)<(b)?(a):(b))
#define MAX(a,b)           ((a)>(b)?(a):(b))

#define  FLOAT  0
#define  DOUBLE 1
#define  INT    2
#define  CHAR   3
#define  LONG   4
#define  LLONG  5



/******************************************************************************/
/** size_of_block()  returns number of elements in block of array
 *  @comment  This assumes that elements of an array have been distributed
 *            among processes so that the first location is
 *            id*n/p and last is (id+1)*n/p -1.
 *  @param    int id              [IN] rank of calling process
 *  @param    int ntotal_elements [IN] number of elements in array
 *  @param    int p               [IN] number of processes in group
 *  @return   number of elements assigned to process id
 */
int size_of_block(
        int id,
        int ntotal_elements,
        int p );

/******************************************************************************/
/** owner() returns the rank of the process that owns a given element
 *  @comment  This assumes that elements of an array have been distributed
 *            among processes so that the first location is
 *            id*n/p and last is (id+1)*n/p -1.
 *  @param  int row          [IN]  index of element
 *  @param  int num_procs    [IN]  number of processes in group
 *  @param  int num_elements [IN]  number of elements in array
 *  @return int rank of process that own element.
 */
int owner(
        int row,
        int num_procs,
        int num_elements );



/******************************************************************************/
/** alloc_matrix(r,c,e, &Mstorage, &M, &err)
 *  If &err is SUCCESS, on return it allocated storage for two arrays in
 *  the heap. Mstorage is a linear array large enough to hold the elements of
 *  an r by c 2D matrix whose elements are e bytes long. The other, M, is a 2D
 *  matrix such that M[i][j] is the element in row i and column j.
 */
void alloc_matrix(
        int     nrows,          /* number of rows in matrix                   */
        int     ncols,          /* number of columns in matrix                */
        size_t  element_size,   /* number of bytes per matrix element         */
        void  **matrix_storage, /* address of linear storage array for matrix */
        void ***matrix,         /* address of start of matrix                 */
        int    *errvalue        /* return code for error, if any              */
        );




/******************************************************************************/
/** init_communication_arrays() initialize arrays to pass to MPI gather/scatterv
 *  @param  int p            [IN]   Number of processes
 *  @param  int n            [IN]   Total number of elements
 *  @param  int *count       [OUT]  Array of counts
 *  @return int *offset      [OUT]  Array of displacements
 */
void init_communication_arrays (
        int p,
        int n,
        int *count,
        int *offset
        );


/******************************************************************************/
/** finalize()     releases the memory pointed to by its argument
 *  @param  char*  state  [INOUT]  is the address of memory to be freed
 *  @post          The memory is freed and state is an invalid address
 */
void   finalize ( char* state );


/*******************************************************************************
                           Random Number Routines
*******************************************************************************/



/** init_random()  initializes the state for the C random() function
 *  @param  int    state_size  [IN]  Size of state array for random to use
 *  @return char*  a pointer to the state array allocated for random()
 *  @post          After this call, an array of size state_size*sizeof(char) has
 *                 been allocated and initialized by C initstate(). It must be
 *                 freed by calling free()
 */
char*  init_random( int state_size );

/******************************************************************************/

/** uniform_random()  returns a uniformly distributed random number in [0,1]
 *  @return double  a pointer to the state array allocated for random()
 *  @pre           Either init_random() should have been called or srandom()
 */
double uniform_random();


#endif /* __UTILITIES_BASIC_H__ */


