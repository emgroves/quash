#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>

#include <readline/readline.h>
#include <readline/history.h>

struct backgroundTask {
  int pid;
  char * task;
  int id;
};

static int *execTaskCount = 0;
static struct backgroundTask tasks[100];

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
			
			strncpy(copyOfInput, &currentInput[indexOfCharacter+2], lengthOfInput);
			
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
			if(execlp(tokenizedInput, tokenizedInput, currentInput, NULL) < 0) { //see if this works, if not you can edit out the (char *)
				fprintf(stderr, "\nError: incorrect input (input remaining)\n"); //might need to correct these error messages, i think this is what they handle
				exit(0);
			}
		}else{
			if(execlp(tokenizedInput, tokenizedInput, NULL) < 0) {
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

void cd(const char* path) {
  //make sure to check for invalid pathing
  if(path == NULL) {
    if (chdir(getenv("HOME")) == -1) 
      printf("<%s> is an invalid path \n", strerror(errno));
    } else {
      if (chdir(path) == -1)
        printf("<%s> is an invalid path.\n", strerror(errno));
    }
}

void pwd() {
  char currDir[1024];
  //get current working directory and print it
  getcwd(currDir, sizeof(currDir));
  printf("%s\n", currDir);
}

//we have to remove spaces in input to parse it
char *remove_spaces(char *s) {
  char *temp;

  while (isspace(*s)) {
    s++;
  }

  if (*s == 0) {
    return s;
  }

  temp = s + strlen(s) - 1;

  while (temp > s && isspace(*temp)) {
    temp--;
  }

  *(temp+1) = 0;

  return s;
}

void get_current_tasks() {
  int x;
  for (x = 0; x < *execTaskCount; x++) {
    if (kill(tasks[x].pid, 0) == 0) {
      printf("[%d] %d %s\n", tasks[x].id, tasks[x].pid, tasks[x].task);
    }
  }
}

void parse(char *input) {
  int pipefd[2];
  char * ptrA;
  char * ptrB;


  char * in = strdup(input);
  
  
  char * parsedin = strtok(input, " \n=");
  char * bgtask = strchr(in, '&');
  char * pipetask = strchr(in, '|');
  char * cmd = strdup(parsedin);

  int setcheck = strcmp("set", parsedin);

  int bgloc = bgtask - in;
  int pipeloc = pipetask - in;
  
  //remove newlines
  for (ptrA=ptrB=in;*ptrB=*ptrA;ptrB+=(*ptrA++!='\n'));


  if (setcheck == 0) {
    parsedin = strtok(NULL," \n=");
    char * assign = strtok(NULL, "\n =");
    //remove quotes
    for (ptrA=ptrB=assign;*ptrB=*ptrA;ptrB+=(*ptrA++!='\''));
    //set environment
    setenv(parsedin, assign, 1);
  }


  if (pipetask != NULL) {
    if (pipe(pipefd) == -1) {
      printf("error");
      exit(1);
    }

    char * fst = strdup(in);
    char * snd = strdup(in);

    strncpy(fst, &in[0], pipeloc);
    fst[pipeloc] = '\0';
    strncpy(snd, &in[pipeloc+2], strlen(in));

    //new process for pipe
    pid_t p1;
    pid_t p2;
    p1 = fork();
    if (p1 == 0) {
      dup2(pipefd[1], STDOUT_FILENO);
      parse(remove_spaces(fst));
      exit(0);
    }

    p2 = fork();
    if (p2 == 0) {
      dup2(pipefd[0], STDIN_FILENO);
      parse(remove_spaces(snd));
      exit(0);
    }
  } else if (setcheck != 0) {
    
    if (bgtask == NULL) {
      if (strcmp("cd", parsedin) == 0) {
        parsedin = strtok(NULL, " \n");
        cd(parsedin);
      } else if (strcmp("jobs", parsedin) == 0) {
        get_current_tasks();
      } else {
        execute(in);
      }
    } else if (bgloc + 1 == strlen(in)) {
      for (ptrA=ptrB=in;*ptrB=*ptrA;ptrB+=(*ptrA++!='&'));
      pid_t p1;
      p1 = fork();
      pid_t sessionId;
      if (p1 == 0) {
        sessionId = setsid();
        if (sessionId < 0) {
          printf("Failed to start session");
          exit(0);
        }
        close(pipefd[0]);
        close(pipefd[1]);
        close(STDERR_FILENO);

        printf("PID='%d' executing as a background task\n", getpid());
        parse(in);
        printf("PID='%d' has finished executing\n", getpid());
        kill(getpid(),-9);
        
        exit(0);
      } else {
        //struct a background task
        struct backgroundTask thisTask = {.id = p1, .pid = *execTaskCount, .task = cmd};
        tasks[*execTaskCount] = thisTask;

        *execTaskCount = *execTaskCount + 1;
        
        //wait for the task..
        while (waitid(p1, NULL, WEXITED,WNOHANG) > 0) {}

      }
    } else {
      printf("Invalid background task entered. \n");
    }
  }

}

int main(int argc, char **argv, char **envpath) {
  execTaskCount = mmap(NULL, sizeof *execTaskCount, PROT_READ | PROT_WRITE,  
              MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  *execTaskCount = 0;

  char* input;
  char* prompt[100];

  char cdir[128];
  
  int x = 0;
  do { 
    snprintf(prompt, sizeof(prompt), "%s:%s ~> ", getenv("USER"), getcwd(NULL, 1024));
    input = readline(prompt);
    
    if (!input) {
      break;
    }

    while(waitpid(-1, NULL, WNOHANG) > 0) {}


    if(strcmp("quit", input) != 0 && strcmp("exit",input) != 0){
      if(strlen(input) > 1)  {
        char * ptrA;
        char * ptrB;
        for (ptrA=ptrB=input;*ptrB=*ptrA;ptrB+=(*ptrA++!='\n'));
        
        parse(remove_spaces(input));
      }
    } else {
      break;
    }
    free(input);
  } while (1);

  return 0;
}
