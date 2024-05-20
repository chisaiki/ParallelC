/******************************************************************************
  Description    : Uses a random walk to find the steady-state heat distribution
  Purpose        : To demonstrate how to performa random walk using a Dirichlet
                   problem as input.
  Usage          : randomwalk_dirichlet <inputfile> <outputfile>
                   where <inputfile> is a plain text file containing:
                   the dimension of a square matrix, followed by 4 floats,
                   representing the temperatures on the N, E, S, and W edges
                   of the plate
  Build with     : gcc -g -Wall -o randomwalk_dirichlet \
                     randomwalk_dirichlet.c -I../include -L../lib  -lm -lutils_basic

  Note:
  This will run for a very long time. For a 100 by 100 grid on a relatively
  fast processor, be prepared to wait 30 minutes or more. This is why it is
  better to use a parallel program to solve it.
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <errno.h>
#include "utilities_basic.h"

/*
Input file consists of
integer N  size of square array
four doubles: temperatures at N, E, S, and W in that order
*/

#define CONVERGENCE_THRESHOLD  0.05

#define NORTH   1
#define EAST    2
#define SOUTH   3
#define WEST    4

typedef struct point2d_tag {
    int x;
    int y;
} point2d;

const point2d East  = {1, 0};
const point2d West  = {-1,0};
const point2d North = {0, 1};
const point2d South = {0,-1};

point2d next_dir() /*Determines which direction the point will head to*/
{
    double u;

    u = uniform_random(); //Change the rand function
    if ( u < 0.25 )
        return North;
    else if ( u < 0.50 )
        return East;
    else if ( u < 0.75 )
        return South;
    else
        return West;
}

int  on_boundary(point2d point, int width, int height) /*Checks if the current point has reached a boundary*/
{
    if ( 0 == point.x )
        return WEST;
    else if ( width -1 == point.x )
        return EAST;
    else if ( 0 == point.y )
        return NORTH;
    else if ( height - 1 == point.y )
        return SOUTH;
    else
        return 0;
}

point2d next_point(point2d oldpoint, point2d direction) /*Sets next point*/
{
    point2d temp;

    temp.x = oldpoint.x + direction.x;
    temp.y = oldpoint.y + direction.y;
    return temp;
}

int main ( int argc, char * argv[] )
{
    point2d current, next;
    int     count = 0;
    int     i, j,  error;
    int     width, height;
    int     wtime;
    double  oldvalue, diff;
    double  maxdiff;
    double  tolerance;
    char    *randtable;
    int     tablesize = 1000;
    FILE    *inputfile;
    FILE    *outfile;
    int     location;
    double  boundary_temperature[4]; /*Need to send the boundary temperatures*/
    double  **plate;
    double  *plate_storage;
    double  convergence_data[5000];

    if ( argc < 3 ) {
        printf("usage %s arrayfile outfile \n", argv[0]);
        exit(1);
    }

    inputfile = fopen( argv[1],  "r");
    if (NULL == inputfile ) {
         fprintf(stderr, "Failed to open %s\n", argv[1] );
         exit(1);
    }

    outfile = fopen( argv[2],  "w");
    if (NULL == outfile ) {
         fprintf(stderr, "Failed to open %s for writing\n", argv[2] );
         exit(1);
    }

    fscanf(inputfile, "%d ", &width);
    height = width;
/*
    printf("height = %d\n", height);
*/
    for ( i = 0; i < 4; i++ )
        fscanf(inputfile, " %lf ", &(boundary_temperature[i]) );
/*
    for ( i = 0; i < 4; i++ )
       printf( "temperature = %lf ", boundary_temperature[i] );
    printf("\n");
*/
    alloc_matrix( width, height, sizeof(double), //need this function
                 (void**) &plate_storage, (void***) &plate, &error);
    if ( error != 0 )
        exit(1);

    wtime = - time(NULL);

    randtable = init_random(tablesize);
    /* Initialize temperatures at the four corners to be the average of the
       temperatures of the adjacent edges
    */
    plate[0][0]              = (boundary_temperature[0] + boundary_temperature[3])/2;
    plate[0][width-1]        = (boundary_temperature[0] + boundary_temperature[1])/2;
    plate[height-1][0]       = (boundary_temperature[3] + boundary_temperature[2])/2;
    plate[height-1][width-1] = (boundary_temperature[2] + boundary_temperature[1])/2;

    /* Initialize the termperatures along the edges of the plate */
    for ( j = 1; j < width -1; j++ ) {
        plate[0][j]        = boundary_temperature[0];
        plate[height-1][j] = boundary_temperature[2];
    }
    for ( i = 1; i < height -1; i++ ) {
        plate[i][0]        = boundary_temperature[3];
        plate[i][width-1]  = boundary_temperature[1];
    }

    /* Initialize the interior temperatures to be 0 */
    for ( i = 1; i < height -1 ; i++ )
        for ( j = 1; j < width -1 ; j++ )
            plate[i][j] = 0.0;






/*Actual Temperature Processing*/
    count = 0;
    tolerance = CONVERGENCE_THRESHOLD;
    while ( count < 10000 ) { //Turn to while true
        maxdiff = 0;
        for ( i = 1; i < height -1 ; i++ ) { //just need to change the boundary it starts in
            for ( j = 1; j < width -1 ; j++ ) {
                current.x = j;
                current.y = i;
                while ( 0 == (location = on_boundary(current, width, height)) ) {
                    next = next_point(current, next_dir());
                    //printf("%d\t%d\n", next.x, next.y);
                    current = next;
                }
                oldvalue = plate[i][j];
                plate[i][j] = (oldvalue*count + boundary_temperature[location-1]) /
                              (count + 1);
                diff = fabs(plate[i][j] - oldvalue);
                if ( diff > maxdiff )
                    maxdiff = diff;
                /* maxdiff is largest difference in current iteration */
            }
        }

        if ( maxdiff < tolerance )
            break;
        else {
            convergence_data[count] = maxdiff;
            count++;
        }
    }







    wtime += time(NULL);

/*Printing Results*/
    for ( i = 0; i < height  ; i++ )  {
        for ( j = 0; j < width-1  ; j++ )
            fprintf(outfile, "%f,", plate[i][j] );
        fprintf(outfile,"%f;\n", plate[i][width-1]);
    }

    for ( i = 0; i < count  ; i++ )  {
        fprintf(stdout, "%f\n", convergence_data[i]);
    }

    printf("Random walk used %d seconds of elapsed time.\n", wtime);
    printf("and needed  %d iterations.\n", count);
    free( plate);
    free( plate_storage );
    free(randtable);
    fclose(outfile);
    fclose(inputfile);
    return 0;
}
