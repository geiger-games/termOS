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
#include <sys/mount.h>

void putsc(const char *text, int r, int g, int b) {
  printf("\033[38;2;%d;%d;%dm%s\033[0m", r, g, b, text);
}

struct termios orig_term;

void restore_terminal() {
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
  if (len == -1) {
    putsc("readlink failed, computer will reboot soon.\n", 255, 0, 0);
    sleep(5);
    while (1) { reboot(LINUX_REBOOT_CMD_RESTART); }
  }
  exePath[len] = '\0';

  // mount
  if (mount(NULL, "/", NULL, MS_REMOUNT, NULL) < 0) {
    perror("remount / rw");
  }
  mount("proc", "/proc", "proc", 0, NULL);
  mount("sysfs", "/sys", "sysfs", 0, NULL);
  mount("devtmpfs", "/dev", "devtmpfs", 0, NULL);

  // clear screen + cwd
  printf("\033[2J\033[H");
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

  setup_terminal();

  // shell
	pid_t pid = fork();

	if (pid == 0) {
		execl("./sbin/sh", "sh", NULL);
		putsc("execution of shell failed, computer will reboot soon.\n", 255, 0, 0);
		sleep(5);
		while (1) { reboot(LINUX_REBOOT_CMD_RESTART); }
	}

	int status;
	waitpid(pid, &status, 0);

  // final reboot
  while (1) { reboot(LINUX_REBOOT_CMD_RESTART); }
}