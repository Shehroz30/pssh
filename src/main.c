#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "shellfunctions.h"
#include "shellcommands.h"


//run the shell process
void lsh_loop(void) {
      char *line;
      char **args;
      int status;
      FILE *ufp;
      char username[2047] = "user";
      //i fialed here lol, gotta get working on actually making this usuable on other people's computers
  /*    char *homedir = getenv("HOME");
      char pathtofile[2047] = strcat(homedir, ".pssh_rc"); */
      if (fopen("/home/maan/.pssh_rc", "r+") != NULL) {
         ufp = fopen("/home/maan/.pssh_rc", "r+"); 
         fscanf(ufp, "user=%s", username);
      }

      do {
        printf("%s> ", username);
        line = lsh_read_line();
        args = lsh_split_line(line);
        status = lsh_execute(args);

        free(line);
        free(args);
      } while (status);

}

int main(int argc, char **argv) {
    lsh_loop();
    
    return EXIT_SUCCESS;
}
