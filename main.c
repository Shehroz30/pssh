#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "shellfunctions.h"
#include "shellcommands.h"


int main(int argc, char **argv) {
    lsh_loop();
    
    return EXIT_SUCCESS;
}

//run the shell process
void lsh_loop(void) {
      char *line;
      char **args;
      int status;

      do {
        printf("> ");
        line = lsh_read_line();
        args = lsh_split_line(line);
        status = lsh_execute(args);

        free(line);
        free(args);
      } while (status);

}
