
// C program for implementation of KMP pattern searching
// algorithm
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>
#include <omp.h>

#define MAXITER 1

void computeLPSArray(char* pat, int M, int* lps);
  
// Prints occurrences of txt[] in pat[]
void KMPSearch(char* pat, char* txt)
{
    int M = strlen(pat);
    int N = strlen(txt);
  
    // create lps[] that will hold the longest prefix suffix
    // values for pattern
    int lps[M];
  
    // Preprocess the pattern (calculate lps[] array)
    computeLPSArray(pat, M, lps);
    
    #pragma omp parallel
    {
        // int i = 0; // index for txt[]
        int j = 0; // index for pat[]
        #pragma omp for
        for (int i = 0; i < N; i++) {
            // printf("%d, %d before ", i, j);
            if (pat[j] == txt[i]) {
                j++;
            }
            // printf("%d, %d before ", i, j);
            if (j == M) {
                printf("Found pattern at index %d \n", i - j + 1);
                j = lps[j - 1];
            }
    
            // mismatch after j matches
            else if (i < N && pat[j] != txt[i+1]) {
                // Do not match lps[0..lps[j-1]] characters,
                // they will match anyway
                if (j != 0) {
                    j = lps[j - 1];
                }
            }
        }
    }
}
  
// Fills lps[] for given patttern pat[0..M-1]
void computeLPSArray(char* pat, int M, int* lps)
{
    #pragma omp parallel
    {
        // length of the previous longest prefix suffix
        int len = 0;
    
        lps[0] = 0; // lps[0] is always 0
    
        // the loop calculates lps[i] for i = 1 to M-1
        #pragma omp for
        for (int i = 1; i < M; i++) {
            if (pat[i] == pat[len]) {
                len++;
                lps[i] = len;
            }
            else // (pat[i] != pat[len])
            {
                // This is tricky. Consider the example.
                // AAACAAAA and i = 7. The idea is similar
                // to search step.
                if (len != 0) {
                    len = lps[len - 1];
                    i--;
                    // Also, note that we do not increment
                    // i here
                }
                else // if (len == 0)
                {
                    lps[i] = 0;
                }
            }
        }
    }
}

#define MAXCHAR 10000
  
// Driver program to test above function
int main()
{
    // for time calculation
	double dtime = 0;
	int ssec, esec, susec, eusec;
	struct timeval tv;
        
    // file reading
    FILE *file;
    file = fopen("Text.txt", "r");
    
    if(file == NULL)
        printf("File does not exist!");
    else{
        fseek(file, 0, SEEK_END); // seek to end of file
        int size = ftell(file); // get current file pointer
        fseek(file, 0, SEEK_SET); // seek back to beginning of file
        
        // proceeding with allocating memory and reading the file
        char *text = (char*) malloc((size * sizeof(char)) + 1);
        fread(text, 1, size, file);
        fclose(file);
        
        // pattern reading
        FILE *file = fopen("Pattern.txt", "r");
        fseek(file, 0, SEEK_END); // seek to end of file
        int size_ = ftell(file); // get current file pointer
        fseek(file, 0, SEEK_SET); // seek back to beginning of file
        
        // proceeding with allocating memory and reading the file
        char *pat = (char*) malloc((size_ * sizeof(char)) + 1);
        fread(pat, 1, size_, file);
        fclose(file);

        for(int i = 0; i<MAXITER; i++){
            gettimeofday(&tv, NULL);
            ssec = tv.tv_sec;
            susec = tv.tv_usec;
            
            KMPSearch(pat, text);

            gettimeofday(&tv, NULL);
            esec = tv.tv_sec;
            eusec = tv.tv_usec;

            dtime += ((esec * 1.0) + ((eusec * 1.0)/1000000.0)) - ((ssec * 1.0) + ((susec * 1.0)/1000000.0));
        }

        printf("PARALLEL TIME %.3f\n", dtime/MAXITER);
        
        // free memory
        free(text);
        free(pat);
    }

    return 0;
}