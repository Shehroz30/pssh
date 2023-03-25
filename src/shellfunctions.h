#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <setjmp.h>

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

void sigint_handler(int);

static sigjmp_buf env;
static volatile sig_atomic_t jump_active = 0;


char *lsh_read_line(void) {
  char *line = NULL;
  ssize_t bufsize = 0; // have getline allocate a buffer for us
  if (getline(&line, &bufsize, stdin) == -1){
    if (feof(stdin)) {
      exit(EXIT_SUCCESS);  // We recieved an EOF
    } else  {
      perror("readline");
      exit(EXIT_FAILURE);
    }
  }
  return line;
}
//split each line into tokens, put in into an array called tokens to feed into lsh_execute 
char **lsh_split_line(char *line)
{
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;
  //opne up a file, and then write the line inputted by the user to it
  FILE *psshHist = fopen("/home/maan/.pssh_history", "r+");
  if (psshHist != NULL) {
      fseek(psshHist, 0, SEEK_END);
      fprintf(psshHist, "%s", line);
      fclose(psshHist);
  } 
  if (!tokens) {
    fprintf(stderr, "psh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "pssh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}


//actually do what shells do
int lsh_launch(char **args)
{
    pid_t pid, wpid;
    int status;

    struct sigaction s;
    s.sa_handler = sigint_handler;
    sigemptyset(&s.sa_mask);
    s.sa_flags = SA_RESTART;
    sigaction(SIGINT, &s, NULL);

    pid = fork();
    if (pid == 0) {
        
        struct sigaction s_child;
        s_child.sa_handler = sigint_handler;
        sigemptyset(&s_child.sa_mask);
        s_child.sa_flags = SA_RESTART;
        sigaction(SIGINT, &s_child, NULL);

        if (execvp(args[0], args) == -1) {
            perror("pssh");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("pssh");
    } else {
    // Parent process
    do {
          if (sigsetjmp(env, 1) == 42) {
            printf("\n");
            continue;
        }

        jump_active = 1;
        wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}
void sigint_handler(int signo) {
    if (!jump_active) {
        return;
    }
    siglongjmp(env, 42);
}
