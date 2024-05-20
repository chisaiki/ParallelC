/******************************************************************************
 Title : search.c
 Author : Syeda Rahman
 Created on : May 12, 2024
 Description : Utilizes multi-threading to search through a file, find pattern locations based 
 on user input.
 Usage : ./search <number of threads> <pattern> <input file> 
 Build with : gcc -o psearch psearch.c
 NOTE: DONT USE -Wall flag, it will not compile because of a variable that only is used conditionally
 Modifications: May 20, 2024
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h> /*Needed for exit()*/
#include <sys/stat.h>
#include <fcntl.h> /*Needed to open file*/
#include <unistd.h> /*Needed for read*/
#include <string.h>
#include <math.h>
#include <pthread.h>

/*Structure prototype that gives each thread the necessary data to perform KMP Search*/
    typedef struct _task_data
    {
      int id;
      char *pattern;
      char *text;
      int text_div;
      char *buffer_access;
      int *found_index; /*Array to store all locations pattern was matched in*/
      int found_spot; /*Keeps track of where the last change was made in found_index array*/
    }task_data;

/*Provides the user with instructions on how to use program*/
void usage_message();

/*Used to find the pre-fix of the pattern in order to do KMP's algorithm*/                                              
void computeLPSArray(char* pat, int M, int* lps);

/*Implementation of KMP (Knuth-Morris-Pratt) Algorithm*/
void* KMPSearch(void *thread_data);


int main(int argc, char *argv[])
{
    char *buffer; /*Data array to store file content*/ 	
    struct stat statbuffer; /*Needed to store input file data */
    int thread_count, in_fd; 
    long long int text_div = 0;
    int i = 0;
    
    
    /************Error Handle*************/

    /*Check if number of arguments is valid*/
    if (argc != 4)
    {
        fprintf(stderr, "Error: Invalid number of arguments.");
        usage_message();
        exit(1);
    }

    /*Make sure number of threads is a positive integer*/
    while (argv[1][i] != '\0')
    {
        if(argv[1][0] == '+' && argv[1][0] == argv[1][i])
        {
            i++;
        }
        else if (argv[1][i] >= 48 && argv[1][i] <= 57)
        {
            i++;
        }
        else
        {
            fprintf(stderr, "Number of threads is invalid. '%s' must be positive whole integer.", argv[1]);
            exit(1);
        }
    }

    /*Check input file*/
    /*Check if file is present, or has encountered any issues*/
    if(lstat(argv[3], &statbuffer)== -1)
    {
        fprintf(stderr, "Failed to open file: %s", argv[3]);
        usage_message();
        exit(1);
    }

    /*Make sure file is not empty*/
    if(statbuffer.st_size == 0)
    {
        fprintf(stderr, "File is empty; enter a non-empty file for pattern search.\n");
        exit(1);
    }

    /*Pattern can't equal file size : so manually count each character and compare to file size*/
    long long int character_count = 0;
    for (i = 0; argv[2][i] != '\0'; i++)
    {
        character_count++;
    }

    if (character_count > (statbuffer.st_size - 1)) /*Have to do -1 because the size is recorded as one more than the actual number of chars*/
    {
        fprintf(stderr,"The pattern size is bigger than the actual text," 
        " this isn't necessarily an error, \nbut you will receive no output from this pattern." 
        "\nExiting program ...\n");
        exit(1);
    }

    /*Store total thread count in variable*/
    sscanf(argv[1], "%d", &thread_count);

    /*Check if thread count is larger than file size*/
    if (thread_count > statbuffer.st_size - 1)
    {
    	fprintf(stderr, "Error: Thread count is larger than characters in the file. Please enter a thread count less than %ld\n.",
		statbuffer.st_size - 1);
	    exit(1);
    }

    /***********End of Error Handling***********/

    
    /*Store total thread count in variable*/
    sscanf(argv[1], "%d", &thread_count);
        
    /*Calculate number of characters each thread will be given*/
    text_div = ceil(((statbuffer.st_size - 1) / thread_count));
    
    /*Open file*/
    in_fd = open(argv[3], O_RDONLY);

    /*Create an array big enough to store all file's content*/
    if ((buffer = malloc(statbuffer.st_size * sizeof(char))) == NULL)
    {
    	fprintf(stderr, "Failed to allocate enough space for file buffer");
	    exit(1);
    }

    /*Store file content into a global variable for all threads to access*/
    if(read(in_fd, buffer, statbuffer.st_size - 1) == -1)
    {
    	fprintf(stderr, "Failed to read '%s'\n", argv[3]);
	    exit(1);
    }
   
    /*Declare thread data structs and threads*/
    task_data thread_data[thread_count];
    pthread_t threads[thread_count];

    /*Initialize data*/
    for (i = 0; i < thread_count; i++)
    {   
	thread_data[i].text = malloc((text_div + (character_count - 1)) * sizeof(char));  
	thread_data[i].id = i;
	thread_data[i].pattern = argv[2];
	thread_data[i].text = memcpy(thread_data[i].text, buffer + (i * text_div), text_div + (character_count - 1));
	thread_data[i].text_div = text_div;
	thread_data[i].buffer_access = buffer;
	thread_data[i].found_index = malloc((text_div + (character_count - 1)) * sizeof(int)); 
	thread_data[i].found_spot = 0;
	
    }

    for (i = 0; i < thread_count; i++)
    {
	if (0 != pthread_create(&threads[i], NULL, KMPSearch, (void *) &thread_data[i]))
        {
           fprintf(stderr, "Failed to create thread\n");
           exit(1);
        }
    }

    

    for (i = 0; i < thread_count; i++)
    {
        for (int j = 0; j < thread_data[i].found_spot; j++)
        {      
          printf("%d\n", thread_data[i].found_index[j]);
        }
        pthread_join(threads[i], (void**) NULL);
    }
    
    /*Free all memory allocated to program*/
    free (buffer);
    return 0;
}


void usage_message()
{
    printf("\nUsage: <number of threads> <pattern string> <input file>\n");
}


void* KMPSearch(void *thread_data)
{
	/*Need to do this for some reason ASK PROFESSOR*/
	task_data *t_data = (task_data*) thread_data;

        int M = strlen(t_data->pattern);
        int N = strlen(t_data->text);

        /*create lps[] that will hold the longest prefix suffix*/
        int lps[M];
	 
        /* Preprocess the pattern (calculate lps[] array) */
        computeLPSArray(t_data->pattern, M, lps);

        int i = 0; /* index for text[]: also controlls where search starts*/
        int j = 0; /* index for pat[]*/	
	
    while (i < N) 
	{
        if (t_data->pattern[j] == t_data->text[i]) 
		{
            j++;
            i++;
        }

        if (j == M) 
	    {   /*If an index is found store it in the found_index array*/
            int real_index = (i - j) + (t_data->id * t_data->text_div); /*FInds the actual index reference to file location*/
	        t_data->found_index[t_data->found_spot] = real_index;
	        t_data->found_spot++; /*The next available spot to fill in an index value*/
            j = lps[j - 1];
       	}
 
        /*Mismatch after j matches*/
        else if (i < N && t_data->pattern[j] != t_data->text[i]) 
	    {
            if (j != 0)
                j = lps[j - 1];
            else
                i = i + 1;
        }
    }
	
    pthread_exit(NULL);
	
/*warning: control reaches end of non-void function [-Wreturn-type]: I got this error because I forgot pthread_exit(NULL)*/
}	


/* Fills lps[] for given pattern pat[0..M-1]*/
void computeLPSArray(char* pat, int M, int* lps)
{
        /*length of the previous longest prefix suffix*/
        int len = 0;

        lps[0] = 0;

        /*the loop calculates lps[i] for i = 1 to M-1*/
        int i = 1;
        while (i < M) {
                if (pat[i] == pat[len])
                {
                        len++;
                        lps[i] = len;
                        i++;
                }
                else /*(pat[i] != pat[len])*/
                {
                        if (len != 0)
                        {
                                len = lps[len - 1];

                        }
                        else /* if (len == 0) */
                        {
                                lps[i] = 0;
                                i++;
                        }
                }
        }
}



