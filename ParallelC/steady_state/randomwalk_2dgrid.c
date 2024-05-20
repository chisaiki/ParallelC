/******************************************************************************
  Description    : A random walk on a 2D grid
  Purpose        : To demonstrate how to generate a single random walk
  Usage          : randomwalk
  Build with     : gcc -o randomwalk randomwalk.c utilities_basic.c -lm 
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include "utilities_basic.h"

#define NORTH   1
#define EAST    2
#define SOUTH   3
#define WEST    4

/* A 2D Point */
typedef struct {
    int x;
    int y;
} point2d;

/* The four possible directions to go */
const point2d East  = {1, 0};
const point2d West  = {-1,0};
const point2d North = {0, 1};
const point2d South = {0,-1};

/* Randomly generate a new direction to walk */
point2d next_dir()
{
    double u = uniform_random();
    if ( u < 0.25 )
        return North;
    else if ( u < 0.50 )
        return East;
    else if ( u < 0.75 )
        return South;
    else
        return West;
}

/* Generate next point from current point and direction */
point2d next_point(point2d oldpoint, point2d direction)
{
    point2d temp;

    temp.x = oldpoint.x + direction.x;
    temp.y = oldpoint.y + direction.y;
    return temp;
}

/* Test if given point is on a boundary */
int  on_boundary(point2d point, int width, int height) 
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


int main ( int argc, char * argv[] ) 
{
    point2d current, next;  
    int     width         = 50;   /* Width of grid  */
    int     height        = 50;   /* Height of grid */
    char   *gaptable;             /* Gap table for random() */
    int     gaptable_size = 1000; /* Size of gap table      */

    /* Start walk from center of grid */
    current.x = width/2;      
    current.y = height/2;

    /* Print first point on standard output */
    printf("%d\t%d\n", current.x, current.y);

    /* Generate a larger gap table for random() function */
    gaptable = init_random(gaptable_size);

    /* Iterate until the walk reaches a boundary */
    while ( 1 ) {
        next = next_point(current, next_dir());
        printf("%d\t%d\n", next.x, next.y);
        if ( on_boundary(next, width, height) ) 
            break;
        else
            current = next;
    }
    printf("%d\t%d\n", next.x, next.y);
    free(gaptable); /* Release memory allocated to gap table */
    return 0;
}
