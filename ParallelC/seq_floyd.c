/*******************************************************************************
  Title          : seq_floyd.c
  Author         : Stewart Weiss
  Created on     : January 20, 2014
  Description    : Computes all-pairs shortest-paths matrix sequentially
  Purpose        : To compare to the parallel version
  Usage          : seq_floyd <input-matrix>
                       where <input-matrix> is a binary file containing first
                       number of rows, then number of columns, then the data in
                       row major order.
  Build with     : gcc -Wall -g -o seq_floyd seq_floyd.c
  Modifications  :
  Notes:
       The file defaults to integer matrices. To change to a different type
       compile using -DElement_type=typename

*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#define  MIN(a,b)        ((a)<(b)?(a):(b))
#define  Element_type    int


int main ( int argc, char* argv[] )
{
    FILE *         infile;
    int            numrows;
    int            numcols;
    int            i,j,k;
    Element_type **c;
    Element_type  *cstorage;
    size_t         sizeof_element = sizeof(Element_type);

    if ( argc < 2 ) {
        printf( "usage: %s inputfile\n", argv[0]);
        exit(1);
    }

    infile = fopen(argv[1], "rb");
    if ( NULL == infile )
        exit(1);

    fread(&numrows,sizeof(int), 1, infile);
    fread(&numcols,sizeof(int), 1, infile);

    if ( numcols != numrows )
        exit(1);
/*
    alloc_matrix( nrows, ncols, sizeof_element, &cstorage, &c, &err);
    if ( SUCCESS != err ) {
        printf("Error allocating matrix\n");
        exit(1);
    }
*/

    cstorage = malloc(numrows * numcols * sizeof_element);
    if ( NULL == cstorage ) {
        fprintf(stderr, "Failed to allocate storage for cstorage array.\n");
        exit(EXIT_FAILURE);
    }

    /* Write computed shortest path array to standard output */
    for ( i = 0; i < numrows; i++ ) {
        for ( j = 0; j < numcols; j++ ) {
            printf(" %d ", c[i][j]);
        }
        printf("\n");
    }

    return 0;
}


    c = malloc (numrows * sizeof(Element_type*));
    if ( NULL == c ) {
        fprintf(stderr, "Failed to allocate storage for c pointer array.\n");
        exit(EXIT_FAILURE);
    }


    for ( i = 0; i < numrows; i++ ) {
        c[i] = &(cstorage[i * numcols]);
    }

    /* Read the data from the file into the matrix */
    for ( i = 0; i < numrows; i++ ) {
        for ( j = 0; j < numrows; j++ ) {
            fread(&c[i][j],sizeof_element, 1, infile);
        }
    }

    for ( k = 0; k < numrows; k++)
        for ( i = 0; i < numrows; i++ )
            for ( j = 0; j < numrows; j++ )
                c[i][j] = MIN(c[i][j], c[i][k]+c[k][j]);

 /* Write computed shortest path array to standard output */
    for ( i = 0; i < numrows; i++ ) {
        for ( j = 0; j < numcols; j++ ) {
            printf(" %d ", c[i][j]);
        }
        printf("\n");
    }

    return 0;
}