#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>


//TODO: Implement I/O redirection & clean-up 
void execute(char * executeInput) {

	char * currentInput = strdup(executeInput);
	char * tokenizedInput = strtok(executeInput, " \n");
	
	//find a better way to do this... removes newlines
	char * ptrA; 
	char * ptrB;
  	for (ptrA=ptrB=currentInput;*ptrB=*ptrA;ptrB+=(*ptrA++!='\n'));
	//END

	int stringIndex = strlen(tokenizedInput);
	
	//PROTOTYPE:: char *strncpy(char *dest, const char *src, size_t n)
	strncpy(currentInput, &currentInput[stringIndex + 1], strlen(currentInput));
	
	
	
	//start process
	pid_t pidProcess;
	pidProcess = fork();
	
	if(pidProcess == 0) {
	
		//TODO: I/O redirection (to-file and from-file)
		
		
		if(strlen(currentInput) > 0 ) {
			if(execlp(tokenizedInput, tokenizedInput, currentInput, (char *)NULL) < 0) { //see if this works, if not you can edit out the (char *)
				fprintf(stderr, "\nError: incorrect input (input remaining)\n"); //might need to correct these error messages, i think this is what they handle
				exit(0);
			}
		}else{
			if(execlp(tokenizedInput, tokenizedInput, (char *)NULL) < 0) {
				fprintf(stderr, "\nError: incorrect input (no input remaining)\n");
				exit(0);
			}
		}


	}else{
	
		int waitPidStatusPtr;
		waitpid(pidProcess, &waitPidStatusPtr, 0);
		
		if(waitPidStatusPtr == 1) {
			fprintf(stderr, "\n%s\n", "Error: waitpid in execute()");
		}
		
	}
}

//TODO: Implement cd
void cd() {
}

//TODO: Implement pwd
void pwd() {
}


void parse(char *input) {
  int count;
  char * word;
  word = strtok(input, " ");

  while (word != NULL) {
    count = count + 1;
    printf("%s\n", word);
    word = strtok(NULL, " ");
  }

}

int main(int argc, char **argv, char **envpath) {
  char input[128];
  bzero(input, sizeof(input));

  int input_flag = 1;

  do {
    printf("~> ");
    fgets(input, sizeof(input), stdin);
    parse(input);
    if (strcmp(input,"exit")) {
      input_flag = 0;
    }
  } while (input_flag);

  return 0;
}
