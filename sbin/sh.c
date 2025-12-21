
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

void putsc(const char *text, int r, int g, int b) {
  printf("\033[38;2;%d;%d;%dm%s\033[0m", r, g, b, text);
}

int main() {
  char buf[256] = {0};

  while (1) {
    printf("termOS> ");
    fflush(stdout);
    
    if (!fgets(buf, sizeof(buf), stdin)) {
      puts("\nEOF, rebooting");
      break;
    }

    buf[strcspn(buf, "\n")] = 0;

    char *command = strtok(buf, " ");
    char *arg1 = strtok(NULL, " ");
    char *arg2 = strtok(NULL, " ");

    if (!command) {puts("Empty"); continue;}

    if (!strcmp(command, "exit")) {
      break;
    } else if (!strcmp(command, "q")) {
      break;
    } else if (!strcmp(command, "quit")) {
      break;
    } else if (!strcmp(command, "exsb")) {
      char execPath[512] = {0};
      snprintf(execPath, sizeof(execPath) - sizeof(char), "./sbin/%s", arg1);

      pid_t pid = fork();

			if (pid == 0) {
				execl("./sbin/sh", "sh", NULL);
				putsc("execution failed.\n", 255, 255, 0);
				continue;
			}

			int status;
			waitpid(pid, &status, 0);
    } else if (!strcmp(command, "rand")) {
      
    }
  }
}