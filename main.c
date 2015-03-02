#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>


//TODO: Implement execute
void execute() {
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
