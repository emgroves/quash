#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

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
	
		//I/O redirection (to-file and from-file)
		
		if(strchr(currentInput, '>') != NULL ){
			
			char *ptrToCharacter = strchr(currentInput, '>');
			char *copyOfInput = strdup(executeInput);
			int indexOfCharacter = (int)(ptrToCharacter - currentInput);
			int lengthOfInput = strlen(currentInput);
			
			strncopy(copyOfInput, &currentInput[indexOfCharacter+2], lengthOfInput);
			
			int newOutput = open(copyOfInput, O_TRUNC | O_WRONLY | O_CREAT, S_IRUSR | S_IWGRP | S_IRGRP | S_IWUSR);
			
			dup2(newOutput, 1);
			close(newOutput);
			
			
			currentInput[indexOfCharacter-1] = '\0';
			
		}else if(strchr(currentInput, '<' ) != NULL) {
			
			char *ptrToCharacter0 = strchr(currentInput, '<');
			char *copyOfInput0 = strdup(currentInput);
			int indexOfCharacter0 = (int)(ptrToCharacter0 - currentInput);
			int lengthOfInput0 = strlen(currentInput);
			
			strncpy(copyOfInput0, &currentInput[indexOfCharacter0+2], lengthOfInput0);
			
			int newInput = open(copyOfInput0, O_RDONLY);
			
			dup2(newInput, 0);
			close(newInput);
			
			
			currentInput[0] = '\0';
		}
		
		
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
void cd(const char* path) {
  if(path == NULL) {
    if (chdir(getenv("HOME")) == -1) 
      printf("<%s> is an invalid path \n", strerror(errno));
    } else {
      if (chdir(path) == -1)
        printf("<%s> is an invalid path.\n", strerror(errno));
    }
}

//TODO: Implement pwd
void pwd() {
  char currDir[1024];
  getcwd(currDir, sizeof(currDir));
  printf("%s\n", currDir);
}


void parse(char *input) {
  int count;
  char * word;

  word = strtok(input, " \n");

  if (strcmp("cd", word) == 0) {
    word = strtok(NULL, " \n");
    printf("%d", strlen(word));
    if (word != NULL) {
      cd(word);
    }
  } else if (strcmp("pwd", word) == 0) {
    pwd();
  }

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
