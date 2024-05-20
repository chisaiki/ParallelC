/*******************************************************************************
  Title          : matrix_alloc.c
  Author         : Stewart Weiss,
  Created on     : February 10, 2014
  Description    :
  Purpose        :
  Build with     : gcc -c matrix_alloc.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "alloc_matrix.h"

/******************************************************************************/
void alloc_matrix(
        int     nrows,          /* number of rows in matrix                   */
        int     ncols,          /* number of columns in matrix                */
        size_t  element_size,   /* number of bytes per matrix element         */
        void  **matrix_storage, /* address of linear storage array for matrix */
        void ***matrix,         /* address of start of matrix                 */
        int    *errvalue)       /* return code for error, if any              */
{
    int   i;
    void *ptr_to_row_in_storage; /* pointer to a place in linear storage array
                                    where a row begins                        */
    void **matrix_row_start;     /* address of a 2D matrix row start pointer
                                    e.g., address of (*matrix)[row]           */
    size_t total_bytes;          /* amount of memory to allocate              */

    //printf("alloc_matrix called with r=%d,c=%d,e=%d\n",nrows, ncols, element_size);

    total_bytes = nrows * ncols * element_size;

    /* Step 1: Allocate an array of nrows * ncols * element_size bytes  */
    *matrix_storage = malloc(total_bytes);
    if ( NULL == *matrix_storage ) {
        /* malloc failed, so set error code and quit */
        *errvalue = MALLOC_ERROR;
        return;
    }

    memset(*matrix_storage, 0, total_bytes );

    /* Step 2: To create the 2D matrix, first allocate an array of nrows
       void* pointers */
    *matrix = malloc (nrows * sizeof(void*));
    if ( NULL == *matrix ) {
        /* malloc failed, so set error code and quit */
        *errvalue = MALLOC_ERROR;
        return;
    }


    /* Step 3: (The hard part) We need to put the addresses into the
       pointers of the 2D matrix that correspond to the starts of rows
       in the linear storage array. The offset of each row in linear storage
       is a multiple of (ncols * element_size) bytes.  So we initialize
       ptr_to_row_in_storage to the start of the linear storage array and
       add (ncols * element_size) for each new row start.
       The pointers in the array of pointers to rows are of type void*
       so an increment operation on one of them advances it to the next pointer.
       Therefore, we can initialize matrix_row_start to the start of the
       array of pointers, and auto-increment it to advance it.
    */

    /* Get address of start of array of pointers to linear storage,
       which is the address of first pointer, (*matrix)[0]   */
    matrix_row_start = (void*) &(*matrix[0]);

    /* Get address of start of linear storage array */
    ptr_to_row_in_storage = (void*) *matrix_storage;

    /* For each matrix pointer, *matrix[i], i = 0... nrows-1,
       set it to the start of the ith row in linear storage */
    for ( i = 0; i < nrows; i++ ) {
        /* matrix_row_start is the address of (*matrix)[i] and
           ptr_to_row_in_storage is the address of the start of the
           ith row in linear storage.
           Therefore, the following assignment changes the contents of
           (*matrix)[i]  to store the start of the ith row in linear storage
        */
        *matrix_row_start = (void*) ptr_to_row_in_storage;

        /* advance both pointers */
        matrix_row_start++;     /* next pointer in 2d array */
        ptr_to_row_in_storage +=  ncols * element_size; /* next row */
    }
    *errvalue = SUCCESS;


}

