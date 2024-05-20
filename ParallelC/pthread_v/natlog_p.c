#include <stdio.h>
#include <stdlib.h> /*For exit()*/
#include <ctype.h> /*Needed for isdigit*/
#include <stdbool.h>
#include <math.h>
#include <pthread.h>

 typedef struct _task_data
    {
      int num_segments, input, start, end, id;
      pthread_t thread_id; /* id returned by pthread_create()   */
      double sum;
    }task_data;


/*Will check if input is a positive whole number <= 1*/
bool inputvalidation(int argc, char *argv[]);

void usage_message();

void* approx_log(void* _data);

int main(int argc, char *argv[])
{

    /*Check if sufficient arguments are present*/
    if (argc != 4)
    {
        fprintf(stderr, "Usage Error: Invalid number of arguments.\n");
        usage_message();
        exit(1);
    }

   if (inputvalidation(argc, argv) == false)
   {
        fprintf(stderr, "Usage Error: Invalid input(s), must be positive whole numbers greater than or equal to 1.");
        exit(1);
   }

   /*All variable declaration*/
   int i = 0;
   int thread_count = 0;
   double input; /*the number whose log is to be determined*/
   long long int num_intervals; /*number of rectanges to break into*/
   double estimated_log = 0; /*estimated value of log*/


    sscanf(argv[1], "%lf", &input);
    sscanf(argv[2], "%lld", &num_intervals);
    sscanf(argv[3], "%d", &thread_count);

    if(thread_count > num_intervals)
    {
        fprintf(stderr, "Thread count cannot be larger than interval amount.\n");
        exit(1);
    }

    printf("\nInput is: %lf Number of Segments is: %lld  Thread: %d\n", input, num_intervals, thread_count); 
    printf("\nStarting the estimate.\n");

    /*Declare thread data structs and threads*/
    task_data thread_data[thread_count];
    pthread_t threads[thread_count];
    pthread_attr_t attr;

    /* Make all threads joinable */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

   /*Initialize data*/
    for (i = 0; i < thread_count; i++)
    {   
        
        thread_data[i].start = (i*num_intervals) / thread_count;
        thread_data[i].end = ((i+1) * num_intervals) / thread_count - 1;
        thread_data[i].input = input;
        thread_data[i].num_segments = num_intervals;
        thread_data[i].sum = 0;
        thread_data[i].id = i;

        if (0 != pthread_create(&(thread_data[i].thread_id), NULL, approx_log, (void *) &thread_data[i]))
        {
           fprintf(stderr, "Failed to create thread\n");
           exit(1);
        }
    }
  
      for (i = 0 ; i < thread_count; i++) 
    {	
   	    pthread_join(thread_data[i].thread_id, (void**) NULL);
    }
   
    /* Collect partial sums into a final total */
    for ( int t = 0 ; t < thread_count; t++) 
    {
        estimated_log += thread_data[t].sum;
    }

    printf("Estimated log: %f", estimated_log);

    return 0;
}


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

void usage_message()
{
    printf("\nUsage: <number whose logarithm is to be determined> <number of segments> <number of threads>\n");
}

void* approx_log(void*_data)
{
    task_data *t_data = (task_data*) _data;
    double dt, sum_of_heights, midpoint;
    long long int i;

    /* Set dt to the width of each segments */
    dt = (t_data->input - 1) / (double) t_data->num_segments; /* (x - 1 ) / segments*/

    /* Initialize sum */
    sum_of_heights = 0.0;
    //midpoint = dt/2;

    for (i = t_data->start; i < t_data->end; i++)
    { 
        midpoint = ((double)i - 0.5) * dt; /*is midpoint of segment i */
        printf("Mid: %f\n", midpoint);
        sum_of_heights += 1/midpoint;   /* add new area to sum */
        printf("Sum: %f\n", sum_of_heights);
    } 

    t_data->sum = sum_of_heights * dt;
    printf("Sum of thread %d is : %f\n", t_data->id, t_data->sum);

    pthread_exit(NULL);
}
