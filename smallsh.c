//Author: Laura Jones
// Class: 344 Fall 2021
// Date: 10/24/2021
// Assignment 3: smallsh

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h> 


//struct for commandLine 
//contains input array, if it's a background process, an input file and output file if needed
typedef struct commandLine {
	char* args[513];
	int background;
	char inputFile[300];
	char outputFile[300];

} commandLine;

struct commandLine *parseCommand() {
	char* commandText = (char*)malloc(2049);
	printf(": ");
	fflush(stdout);
	fgets(commandText, 2048, stdin);
	commandText[strcspn(commandText, "\n")] = 0;
	

	char* saveptr;
	char* token = strtok_r(commandText, " ", &saveptr);
	int i = 0;
	struct commandLine* currCommand = malloc(sizeof(struct commandLine));

	currCommand->background = 0;


	while (token != NULL) {
		if (strcmp(token, "&") == 0) {
			currCommand->background = 1;
		}
		else if (strcmp(token, "<") == 0) {
			token = strtok_r(NULL, " ", &saveptr);
			strcpy(currCommand->inputFile, token);

		}
		else if((strcmp(token, ">") == 0)) {
			token = strtok_r(NULL, " ", &saveptr);
			strcpy(currCommand->outputFile, token);
		}
		else {
			currCommand->args[i] = calloc(strlen(token) + 1, sizeof(char));
			strcpy(currCommand->args[i], token);
			i++; 
			//implement $$ expansion 
		}
		token = strtok_r(NULL, " ", &saveptr);
	}
	return currCommand;
}

//for testing parsing data correctly
void printCommand(struct commandLine *aCommand) {
	int i;
	for (i = 0; aCommand->args[i] != NULL; i++) {
		printf("%s\n", aCommand->args[i]);
	}
}


void executeCommand(struct commandLine *currentCommand) {
	int childStatus;

	// Fork a new process
	int size = 1;
	char *args[size + 1];
	int i;
	for (i = 0; currentCommand->args[i] != NULL; i++) {
		size = size + 1;
	}
	for (i = 0; i < size; i++) {
		args[i] = NULL;
	}
	for (i = 0; i < size - 1; i++) {
		args[i] = currentCommand->args[i];
	}
	pid_t spawnPid = fork();
	switch(spawnPid){
	case -1:
		perror("fork()\n");
		exit(1);
		break;
	case 0:
		// In the child process
		execvp(args[0], args);
		perror("execv");   /* execve() returns only on error */
		exit(EXIT_FAILURE);
		break;
	default:
		// In the parent process
		// Wait for child's termination
		spawnPid = waitpid(spawnPid, &childStatus, 0);
		break;
	} 
}

int main() {

	int pid = getpid();
	int exitStatus = 0;
	int keepGoing = 1;

	while (keepGoing) {
		//user Input

		struct commandLine *currentCommand = parseCommand();
		//printCommand(currentCommand);
		
		
		//comments
		if (strcmp(currentCommand->args[0], "#") == 0 || strcmp(currentCommand->args[0], "") == 0) {
			continue;
		}
		//exit stops while loop and exits program
		else if (strcmp(currentCommand->args[0], "exit") == 0) {
			keepGoing = 0;

		}//status
		else if (strcmp(currentCommand->args[0], "status") == 0) {
			//TODO print exit status
			printf("Exit status is %d\n", exitStatus);

		} //cd
		else if (strcmp(currentCommand->args[0], "cd") == 0) {
			if (currentCommand->args[1] != NULL) {
				if (chdir(currentCommand->args[1]) == -1) {
					printf("no directory with that name\n");
				}
			}
			else {
				chdir(getenv("HOME"));
			}
		}
		else {
			//executeCommand(currentCommand);

			executeCommand(currentCommand);
		}
	}

	return 0;
}