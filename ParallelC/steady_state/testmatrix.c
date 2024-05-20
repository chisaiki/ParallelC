#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SUCCESS             0
#define MALLOC_ERROR        1
#define ROW                 5
#define COLUMN              2


/*Matrix Allocation Function*/

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

    printf("alloc_matrix called with r=%d,c=%d,e=%d\n",nrows, ncols, element_size);

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

    /* Get address of start of array of pointers to linear storage,
       which is the address of first pointer, (*matrix)[0]   */
    matrix_row_start = (void*) &(*matrix[0]);

    /* Get address of start of linear storage array */
    ptr_to_row_in_storage = (void*) *matrix_storage;

    /* For each matrix pointer, *matrix[i], i = 0... nrows-1,
       set it to the start of the ith row in linear storage */
    for ( i = 0; i < nrows; i++ ) {
        *matrix_row_start = (void*) ptr_to_row_in_storage;
        matrix_row_start++;     /* next pointer in 2d array */
        ptr_to_row_in_storage +=  ncols * element_size; /* next row */
    }
    *errvalue = SUCCESS;
}


int main(int argc, char *argv[])
{
    double  **plate;
    double  *plate_storage;
    int     error;


    alloc_matrix( ROW , COLUMN , sizeof(double), 
                 (void**) &plate_storage, (void***) &plate, &error);
    if ( error != 0 )
        exit(1);
    
    for ( int i = 0; i < ROW ; i++ )
    {
        for ( int j = 0; COLUMN < 3 ; j++ )
        {
            plate[i][j] = 2.5;
        }
    }
        
            
    for ( int i = 0; i < ROW ; i++ )
    {
        for ( int j = 0; j < COLUMN ; j++ )
        {
            printf(" %f ", plate[i][j]);
        }
        printf("\n");
    }       
}