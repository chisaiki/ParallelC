#include <stdio.h>
#include <stdlib.h> /*For exit() and atoi()*/
#include <ctype.h> /*Needed for isdigit*/
#include <stdbool.h>
#include <math.h>
#include "mpi.h"

#define ROOT 0

bool inputvalidation(int argc, char *argv[]);

double approx_log(int num_segments, double input, long long int id, int p);

int main(int argc, char *argv[])
{
   /*All variable declaration*/
   int id; /*rank of executing process*/
   int p; /*number of processes*/
   double estimated_log; /*estimated value of log*/
   double input; /*the number whose log is to be determined*/
   double local_log; /*each process' contribution*/
   char* endptr;
   long long int num_intervals; /*number of rectanges to break into*/
   double elapsed_time; /*total computation time*/


    /*Start MPI Environment*/
    MPI_Init      (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &id);
    MPI_Comm_size (MPI_COMM_WORLD, &p);

    if(ROOT == id)
    {
        /*Check if sufficient arguments are present*/
        if (argc != 3)
        {
            fprintf(stderr, "Usage Error: Invalid number of arguments.\n");
            exit(1);
        }

        if (inputvalidation(argc, argv) == false)
        {
                fprintf(stderr, "Usage Error: Invalid input(s), must be positive whole numbers greater than or equal to 1.");
                exit(1);
        }

         
        input = strtod(argv[1], &endptr);

        printf("Input: %f\n", input);

        num_intervals = strtol(argv[2],&endptr, 10);

        printf("Num Intervals: %lld\n", num_intervals);

    }

    /*Broadcast values to processors*/
    MPI_Bcast(&num_intervals, 1, MPI_UNSIGNED_LONG_LONG, ROOT, MPI_COMM_WORLD);
    MPI_Bcast(&input, 1, MPI_DOUBLE, ROOT, MPI_COMM_WORLD);

    /*Wait for all processors to be ready*/
    MPI_Barrier (MPI_COMM_WORLD);
    elapsed_time = -MPI_Wtime();


    
    local_log = approx_log(num_intervals, input, id, p);

    /* MPI_Reduce collects the local estimate from each process into
           a global value, pi_estimate. The ROOT is the process that receives
           all values in the reduction. The reduce operator is MPI_SUM. */
    MPI_Reduce(&local_log, &estimated_log, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Barrier (MPI_COMM_WORLD);

        /* Stop timer */
        elapsed_time += MPI_Wtime();
        /* ROOT does the printing. The error is caculated by comparing to
           the math library's value for PI, M_PI. */
        if (ROOT == id ) 
        {
            printf("log is approximated to be %.16f. The error is  %.16f\n",
            estimated_log, fabs(log(input) - estimated_log));
            printf("Computation took %8.6f seconds.\n", elapsed_time);
            fflush(stdout);
        }

    MPI_Finalize();

}



/*FUNCTION IMPLEMENTATION*/



bool inputvalidation(int argc, char *argv[]) 
{
    int i = 1; /*Iterator for the arguments*/
    int j = 0; /*Iterator for each char in the arguments*/

    while (i < argc) /*Iterate through all the arguments*/
    {
        /*Checks if argument values are less than 1*/

        while (argv[i][j] != '\0') /*Until the end of the string is reached*/
        {
            /*If current symbol is a +, just iterate to the next symbol*/
            if(argv[i][0] == 43 && argv[i][0] == argv[i][j]) /*43 is ASCII for + */
            {
                printf("In first if statement.\n");
                j++;
            }

            /*If you current symbol is a '.' then start checking if only 0 proceeds it*/
            else if (argv[i][j] == 46) /*46 is ASCII for . */
            {
                int k = j+1; /*Start after '.' */
                while(argv[i][k] != '\0') 
                {
                    /*If at any point past the period, there is a non-zero, then return false*/
                    if(argv[i][k] != '0')
                    {
                        printf("%s is an invalid input. Inputs should be whole numbers.\n", argv[i]);
                        return false;
                    }

                    k++;
                }

                j++; 
            }

            else if(isdigit(argv[i][j]) == 0)
            {
                printf("%s is an invalid input.\n", argv[i]);
                return false;
            }

            else 
            {
                j++;
            }
        }

        if (atoi(argv[i]) <= 0) 
        {
            printf("%s is an invalid input. \n", argv[i]);
            return false;
        }

        i++;
        j = 0; /*Reset index of j*/
    }

    return true;
}

double approx_log(int num_segments, double input, long long int id, int p)
{
    double dt, sum_of_heights, midpoint;
    long long int i;


    /* Set dt to the width of each segments */
    dt = (input - 1) / (double) num_segments; /* (x - 1 ) / segments*/

    /* Initialize sum */
    sum_of_heights = 0.0;
    midpoint = dt/2;

    for (i = id + 1; i <= num_segments; i += p) 
    {
        midpoint += dt; /*is midpoint of segment i */
        sum_of_heights += 1/midpoint;   /* add new area to sum */
        
    }

    return sum_of_heights * dt;
}
