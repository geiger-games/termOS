#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/reboot.h>
#include <linux/reboot.h>
#include <termios.h>
#include <stdio.h>
#include <sys/mount.h>

void putsc(const char *text, int r, int g, int b) {
  printf("\033[38;2;%d;%d;%dm%s\033[0m", r, g, b, text);
}

void sigchld_handler(int sig) {
  (void)sig;
  int status;
  pid_t pid;
  while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
    printf("Ended process %d with status %d\n", pid, status);
  }
}

struct termios orig_term;

void restore_terminal() {
  // restore terminal to original state
  tcsetattr(STDIN_FILENO, TCSANOW, &orig_term);
}

void setup_terminal() {
  struct termios t;
  tcgetattr(STDIN_FILENO, &orig_term); // save original
  atexit(restore_terminal);            // restore on exit

  t = orig_term;
  t.c_lflag |= (ICANON | ECHO);       // enable canonical mode & echo
  tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

int main() {
  char exePath[512] = {0};
  ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
  exePath[len] = '\0';   // null-terminate

  // mount
  if (mount(NULL, "/", NULL, MS_REMOUNT, NULL) < 0) {
    perror("remount / rw");
  }
  mount("proc", "/proc", "proc", MS_REMOUNT, NULL);
  mount("sysfs", "/sys", "sysfs", MS_REMOUNT, NULL);
  mount("devtmpfs", "/dev", "devtmpfs", MS_REMOUNT, NULL);

  // get cwd
  system("clear");
  char *lastSlash = strrchr(exePath, '/');
  if (lastSlash) {
    *lastSlash = '\0';
    chdir(exePath);
    printf("[termOS] Working directory: %s\n", exePath);
  }

  // init messages
  putsc("**red test**\n", 255, 0, 0);
  putsc("**green test**\n", 0, 255, 0);
  putsc("**blue test**\n", 0, 0, 255);

  puts("  /\\--=--/\\");
  puts(" /  \\   /  \\");
  puts("|term|=|inal|");
  puts(" \\ O/   \\S /");
  puts("  \\/--=--\\/");
  puts("\nA minimal Linux Distribution\n");

  // run shell
  pid_t pid = fork();
  if (pid == 0) {
    execl("./sh", "./sh", NULL);
    perror("exec sh failed");
    _exit(1);
  }
  waitpid(pid, NULL, 0);

  setup_terminal();

  // signals
  signal(SIGCHLD, sigchld_handler);
  signal(SIGINT, SIG_IGN);
  signal(SIGTERM, SIG_IGN);
  signal(SIGHUP, SIG_IGN);

  // wait.
  while (1) {
    pid_t pid = fork();

    if (pid == 0) {
      execl("./sh", "sh", NULL);
      perror("exec sh failed");
      _exit(1);
    }

    int status;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status)) {
      int code = WEXITSTATUS(status);
      printf("sh exited with code %d\n", code);

      if (code == 42) {  // your "restart" code
        break;
      }
    }
  }
  
  reboot(LINUX_REBOOT_CMD_RESTART);

  while (1) {}
}