#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SUCCESS 0
#define MALLOC_ERROR 1

#define OPEN_FILE_ERROR 2
#define TYPE_ERROR 3
#define FILE_READ_ERROR 4
#define BAD_MATRIX_SIZE 5
#define BAD_ELEMENT_SIZE 6

/*Used to provide instructions on program usage*/
void usage_message();

/*Creates a 2D Matrix based on inputs: Taken from Professor Weiss's Repo*/
void alloc_matrix(
    int nrows,             /* number of rows in matrix                   */
    int ncols,             /* number of columns in matrix                */
    size_t element_size,   /* number of bytes per matrix element         */
    void **matrix_storage, /* address of linear storage array for matrix */
    void ***matrix,        /* address of start of matrix                 */
    int *errvalue);        /* return code for error, if any              */

int main(int argc, char *argv[])
{
    FILE *inputfile;
    int dimensions[2];
    int user_input_row, user_input_column, error_handle, i, j, row_amount_to_give_out;
    double boundary_temperature[4];
    double **plate;
    double *plate_storage;
    char *randtable;
    int tablesize = 1000;

    /*Start of Error Handling*/
    if (argc != 4)
    {
        fprintf(stderr, "Invalid Usage: Not enough arguments.");
        usage_message();
        exit(1);
    }

    inputfile = fopen(argv[1], "r");
    if (NULL == inputfile)
    {
        fprintf(stderr, "Failed to open file named: '%s'\n", argv[1]);
        exit(1);
    }

    /*Check if coordinates are only integer*/
    int tracker = 0; /*Just need this as a do nothing part of the if-else statement*/
    for(int i = 2; i <= 3; i++)
    {
        for (int j = 0; argv[i][j] != '\0'; j++)
        {
            if(argv[i][j] >= 48 && argv[i][j] <= 57)
            {
                tracker = 0;
            }
            else
            {
                fprintf(stderr, "%s is an invalid coordinate. Must be an integar.", argv[i]);
                usage_message();
                exit(1);
            }   
        }
    }

    /*Scan data from file*/
    for (int i = 0; i < 2; i++)
    {
        fscanf(inputfile, " %d ", &dimensions[i]);
    }

    for (int i = 0; i < 2; i++)
    {
        printf("dimension = %d ", dimensions[i]);
        printf("\n");
    }

    for (int i = 0; i < 4; i++)
        fscanf(inputfile, " %lf ", &(boundary_temperature[i]));

    for (int i = 0; i < 4; i++)
        printf("temperature = %lf ", boundary_temperature[i]);
    printf("\n");

    /*Store user input coordinates into variables*/    
    sscanf(argv[2], "%d", &user_input_row);
    sscanf(argv[3], "%d", &user_input_column);

    /*Check if command line coordinates are valid*/
    if (user_input_row > dimensions[0] || user_input_column > dimensions[1])
    {
        fprintf(stderr, "Error: User inputted coordinates are out of range.");
        usage_message();
        exit(1);
    }

    /*Broadcast bordervalues*/
    /*Allocate Matrix and Check if its dimensions is valid*/
    alloc_matrix(dimensions[0], dimensions[1], sizeof(double),
                 (void **)&plate_storage, (void ***)&plate, &error_handle);
    if (error_handle != 0)
        exit(1);

    /*Initialize Matrix*/

    /* Initialize temperatures at the four corners to be the average of the
       temperatures of the adjacent edges

       NOTE: dimensions[0] = row      dimensions[1] = column
    */
    plate[0][0]                 = (boundary_temperature[0] + boundary_temperature[3]) / 2;
    plate[0][dimensions[0] - 1] = (boundary_temperature[0] + boundary_temperature[1]) / 2;
    plate[dimensions[1] - 1][0] = (boundary_temperature[3] + boundary_temperature[2]) / 2;
    plate[dimensions[1] - 1][dimensions[0] - 1] = (boundary_temperature[2] + boundary_temperature[1]) / 2;

    /* Initialize the termperatures along the edges of the plate */
    for (j = 1; j < dimensions[0] - 1; j++)
    {
        plate[0][j] = boundary_temperature[0];
        plate[dimensions[1] - 1][j] = boundary_temperature[2];
    }

    for (i = 1; i < dimensions[1] - 1; i++)
    {
        plate[i][0] = boundary_temperature[3];
        plate[i][dimensions[0] - 1] = boundary_temperature[1];
    }

    /* Initialize the interior temperatures to be 0 */
    for (i = 1; i < dimensions[1] - 1; i++)
        for (j = 1; j < dimensions[0] - 1; j++)
            plate[i][j] = 0.0; //DONT NEED TO DO THIS IF I INITIALIZE DURING array

    /*Print Matrix*/
    for (int i = 0; i < dimensions[0]; i++)
    {
        for (int j = 0; j < dimensions[1]; j++)
        {
            printf(" %f ", plate[i][j]); // I was getting the wrong output because the %type was wrong
        }
        printf(" \n ");
    }

    // randtable = init_random(tablesize);

    /*Calculate which points to give processes*/
    /*Divide total number of rows by processes amounts and give that amount, need to preserve the coordinates
    ONLY NEED TO FIGURE OUT ROW VALUE SINCE COLUMN IS THE SAME*/

    // dimension[0] / # of processes = # of rows each process gets
        //if dimension[0] % # of process != 0 then p - 1 gets one extra row
            // malloc # of rows each process gets * size of column * data type
             //intialize all array values to 0 
             //row sent cant include first and last number because thats the boundary, not the inner
             // 
    /*Give each process its own designated coordinates (start to stop)*/

    /*Malloc a vector big enough to store all points values; initialize all vector values to 0*/
    /*Send border data*/

    /*Record which process has the coordinate given from user*/

    /*Make each process run the code until its reached steady state*/

    /*Send Process 0 the temperature from the user input coordinate*/

    /*Output the temperature of user coordinate*/
    // THIS WORKS printf("\n%f", plate[user_input_row][user_input_column]);
}

void usage_message()
{
    printf("\nProgram usage: <filename> <X coordinate point> <Y coordinate point>\n");
    printf("Note: If file contains non-numeric values, it will be turned into 0.\n");
}

void alloc_matrix(
    int nrows,             /* number of rows in matrix                   */
    int ncols,             /* number of columns in matrix                */
    size_t element_size,   /* number of bytes per matrix element         */
    void **matrix_storage, /* address of linear storage array for matrix */
    void ***matrix,        /* address of start of matrix                 */
    int *errvalue)         /* return code for error, if any              */
{
    int i;
    void *ptr_to_row_in_storage; /* pointer to a place in linear storage array
                                    where a row begins                        */
    void **matrix_row_start;     /* address of a 2D matrix row start pointer
                                    e.g., address of (*matrix)[row]           */
    size_t total_bytes;          /* amount of memory to allocate              */

    printf("alloc_matrix called with r=%d,c=%d,e=%d\n", nrows, ncols, element_size);

    if (ncols == 0 || nrows == 0)
    {
        fprintf(stderr, "Error: Matrix Dimensions are invalid.");
        exit(1);
    }

    total_bytes = nrows * ncols * element_size;

    /* Step 1: Allocate an array of nrows * ncols * element_size bytes  */
    *matrix_storage = malloc(total_bytes);
    if (NULL == *matrix_storage)
    {
        /* malloc failed, so set error code and quit */
        *errvalue = MALLOC_ERROR;
        return;
    }

    memset(*matrix_storage, 0, total_bytes);

    /* Step 2: To create the 2D matrix, first allocate an array of nrows
       void* pointers */
    *matrix = malloc(nrows * sizeof(void *));
    if (NULL == *matrix)
    {
        /* malloc failed, so set error code and quit */
        *errvalue = MALLOC_ERROR;
        return;
    }

    /* Get address of start of array of pointers to linear storage,
       which is the address of first pointer, (*matrix)[0]   */
    matrix_row_start = (void *)&(*matrix[0]);

    /* Get address of start of linear storage array */
    ptr_to_row_in_storage = (void *)*matrix_storage;

    /* For each matrix pointer, *matrix[i], i = 0... nrows-1,
       set it to the start of the ith row in linear storage */
    for (i = 0; i < nrows; i++)
    {
        *matrix_row_start = (void *)ptr_to_row_in_storage;
        matrix_row_start++;                            /* next pointer in 2d array */
        ptr_to_row_in_storage += ncols * element_size; /* next row */
    }
    *errvalue = SUCCESS;
}