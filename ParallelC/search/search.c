#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <strings>
#include <fcntl.h> /*Needed to open file*/
#include <math.h>


/*ADD FUNCTION DESCRIPTION*/
void usagemessage();


/*----------------------------------------------*/

// C++ program for implementation of KMP pattern searching
// algorithm

void computeLPSArray(char* pat, int M, int* lps);

// Prints occurrences of pat[] in txt[]
void KMPSearch(char* pat, char* txt)
{
	int M = strlen(pat);
	int N = strlen(txt);

	// create lps[] that will hold the longest prefix suffix
	// values for pattern
	int lps[M];

	// Preprocess the pattern (calculate lps[] array)
	computeLPSArray(pat, M, lps);

	int i = 0; // index for txt[]
	int j = 0; // index for pat[]
	while ((N - i) >= (M - j)) {
		if (pat[j] == txt[i]) {
			j++;
			i++;
		}

		if (j == M) {
			printf("Found pattern at index %d ", i - j);
			j = lps[j - 1];
		}

		// mismatch after j matches
		else if (i < N && pat[j] != txt[i]) {
			// Do not match lps[0..lps[j-1]] characters,
			// they will match anyway
			if (j != 0)
				j = lps[j - 1];
			else
				i = i + 1;
		}
	}
}

// Fills lps[] for given pattern pat[0..M-1]
void computeLPSArray(char* pat, int M, int* lps)
{
	// length of the previous longest prefix suffix
	int len = 0;

	lps[0] = 0; // lps[0] is always 0

	// the loop calculates lps[i] for i = 1 to M-1
	int i = 1;
	while (i < M) {
		if (pat[i] == pat[len]) {
			len++;
			lps[i] = len;
			i++;
		}
		else // (pat[i] != pat[len])
		{
			// This is tricky. Consider the example.
			// AAACAAAA and i = 7. The idea is similar
			// to search step.
			if (len != 0) {
				len = lps[len - 1];

				// Also, note that we do not increment
				// i here
			}
			else // if (len == 0)
			{
				lps[i] = 0;
				i++;
			}
		}
	}
}


int main(int argc, char *argv[])
{
    struct stat statbuffer; /*Needed to store input file data to check for its size*/
	int in_fd;


    /*--------Error Handling--------*/

    /*Check for valid number of command line arguments*/
    if(argc != 3)
    {
        fprintf(stderr, "Usage Error: Incorrect number of arguments.\n");
        usagemessage();
        exit(1);
    }

    /*Validate pattern input : cannot be control characters or white space character*/
    for (int i = 0; argv[1][i] != '\0'; i++)
    {
            /*ASCII 0-31 have all control characters and white space character*/
            if(argv[1][i] >= 0 && argv[1][i] <= 32) 
            {
                fprintf(stderr, "Invalid pattern, contains control character: '%d'.", argv[1][i]);
                usagemessage();
                exit(1);
            }
    } //Not sure how to test this though, can't actually put in a control character

    /*Check if file is present, or has encountered any issues*/
    if(lstat(argv[2], &statbuffer)== -1)
    {
        fprintf(stderr, "Failed to stat file: %s\n", argv[2]);
        usagemessage();
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
    for (int i = 0; argv[1][i] != '\0'; i++)
    {
            character_count++;
    }

    printf("%lld Characters \n", character_count);
    if(character_count > (statbuffer.st_size - 1)) //Have to do -1 because the size is recorded as one more than the actual number of chars
    {
        fprintf(stderr,"The pattern size is bigger than the actual text," 
        "this isn't necessarily an error, \nbut you will receive no output from this pattern." 
        "\nExiting program ...\n");
        exit(1);
    }

    /*--------End of Error Handling--------*/

	/*Open file*/
	if((in_fd = open(argv[2], O_RDONLY)) == -1)
	{
		fprintf(stderr, "Could not open %S for reading.\n", argv[2]);
		exit(1);
	}

	char pat[] = "ABABCABAB";
    char txt[] = "ABABDABACDABABCABAB";
	KMPSearch(pat, txt);
	return 0;

}

void usagemessage()
{
    printf("Program Usage: search <pattern> <filename>\n"
            "1. Pattern must be a string with no control characters or white space character.\n"
            "2. Program will assume file is in current directory, otherwise please" 
            " specify filepath leading to the file.\n");
}