#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <sys/stat.h>

#define true 1
#define false 0

int byteCount(char*);
int lineCount(char*);
int wordCount(char*);
int createProcess(int, char**, char*);

int main(int argc, char **argv) { 

	int k, i = 0, c = 2;
	char *charPtr, *strPtr;
	pid_t pid;
  	int pipefd[2];	
	char *fileArr[argc];
	char charArr[256];

	FILE *filePtr;

	if(argc == 1) { 
		printf("\nNo input files were found\n\n"); 
		return 1;	
	}
	
	else { // Parse input	
	
		for(k = 1; k < argc; k++) { 

			charPtr = argv[k];

			if(*charPtr == '-') {

				if(*++charPtr == 'a') { sprintf(charArr, " %d %d %d %s PID: %d", lineCount(argv[c++]), 
							wordCount(argv[c]), byteCount(argv[c]), argv[c], getpid()); }

				else if (*charPtr == 'c') { sprintf(charArr, " %d %s PID: %d", byteCount(argv[c++]), argv[c], getpid()); }
				else if (*charPtr == 'l') { sprintf(charArr, " %d %s PID: %d", lineCount(argv[c++]), argv[c], getpid()); }
				else if (*charPtr == 'w') { sprintf(charArr, " %d %s PID: %d", wordCount(argv[c++]), argv[c], getpid()); }
			 	else { 
	
					if(filePtr != stdin) {
						fprintf(stderr, "%s: too many arguments \n", argv[c++]);
						exit(1);
					}

					filePtr = fopen(charPtr, "r");

					if(filePtr == NULL) {				
						fprintf(stderr, "%s: unable to read %s\n", argv[c++], charPtr);
						exit(1);
					}
				}
			
			} else { fileArr[i++] = argv[k]; }
		}

		createProcess(argc, fileArr, charArr);
	}		

	return 0;
} 

int byteCount(char *filePtr) {

	int bytes = 0, c = 0;

	FILE *inputFile = fopen(filePtr, "r");

	if(!inputFile) { 
		printf("Error: Input file not found\n");
		return 1;

	} else { while((c = fgetc(inputFile)) != EOF ) { bytes++; } } 

	fclose(inputFile);
	 	
	return bytes; 
}

int wordCount(char *filePtr) {
	
	int words = 0, c = 0, flag = 0;	

	FILE *inputFile = fopen(filePtr, "r");

	if(!inputFile) { 
		printf("Error: Input file not found\n");
		return 1;

	} else { 

		while((c = fgetc(inputFile)) != EOF) {
			if (!isspace(c)) {

				if (!flag) {
					flag = 1;
					words++;
				}

			} else { flag = 0; }
		} 
	} 

	fclose(inputFile);	

	return words;
}

int lineCount(char *filePtr) {	

	int lines = 0, c = 0;	

	FILE *inputFile = fopen(filePtr, "r");

	if(!inputFile) { 
		printf("Error: Input file not found\n");
		return 1;
	
	} else { while((c = fgetc(inputFile)) != EOF ) { if(c == '\n') { lines++; } } }

	fclose(inputFile); 

	return lines;
} 

int createProcess(int argc, char** fileArr, char* charArr) {
	
	pid_t pid;
	char readBuffer[100], *str;
	int k, pipefd[2];
	FILE* file;
	const int fileCount = argc - 2; // Flag and a.out

	if (pipe(pipefd) == -1) {
        	fprintf(stderr, "Pipe failed");
        	return 1;
    	}

	for(k = 0; k < argc; k++) {
		
		if((pid = fork()) == -1) {
      			fprintf(stderr, "Error while forking.\n");
      			exit(1);
    		}

		else if (pid == 0) { // Child process
			close(pipefd[0]);
              		file = fopen(fileArr[k], "r");
      			if(write(pipefd[1], charArr,  strlen(charArr)+1) == -1) { 
				fprintf(stderr, "Error writing to pipe.\n"); 
				return 1;
			}
			close(pipefd[1]);
			exit(0);	
		}	
	}

	for(k = 0; k < fileCount; k++) {

		if (pid > 0) {
			close(pipefd[1]);
                	if(read(pipefd[0], &readBuffer, 100) == -1) { fprintf(stderr, "Error reading the pipe.\n"); }	
			printf("%s \n", readBuffer);
			close(pipefd[0]);			
		}
	}
}

