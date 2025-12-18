#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

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
      system(execPath);
    } else if (!strcmp(command, "rand")) {
      
    }
  }

  exit(42);
}