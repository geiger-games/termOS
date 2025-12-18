#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

void listcpacs() {
	char curlString[512] =
	"curl -s "
	"https://api.github.com/repos/geiger-games/cpacs/contents/";

	FILE *f = popen(curlString, "r");
	if (!f) {
		puts("failed to run curl");
		return;
	}

	int count = 0;

	char line[256];

	while (fgets(line, sizeof(line), f)) {
		// look for: "name": "
		char *name = strstr(line, "\"name\": \"");
		if (!name) continue;

		count++;

		name += 9; // skip

		char *end = strchr(name, '"');
		if (!end) continue;

		*end = '\0';

		puts(name);
	}

	puts("     ▄▄▄");
	puts(" ▄▄▀▀   ▀▀▄▄");
	printf("█▄▄%7d▄▄█\n", count);
	puts("█  ▀▀▄▄▄▀▀  █");
	puts("█  C  █PACS █");
	puts("█     █     █");
	puts("▀▄▄   █   ▄▄▀");
	puts("   ▀▀▄█▄▀▀");

	pclose(f);
}

void getcpac(char *file) {
	char curlString[512] = {0};
    char delPac[512] = {0};

	sprintf(curlString, "curl -s -H \"Accept: application/vnd.github.v3.raw\" -L https://api.github.com/repos/geiger-games/cpacs/contents/%s -o cpacs/%s", file, file);
	sprintf(delPac, "rm %s", file);

	system(curlString);

	// now check if we got a 404 JSON file
	FILE *f = fopen(file, "r");
	if (!f) {
		puts("unusual error: output file not created.");
		return;
	}

	char line[256];
	
	while (fgets(line, sizeof(line), f)) {
		if (!strcmp(line, "  \"status\": \"404\"\n")) {
			printf("404, cpac '%s' doesn't exist.\n", file);
			system(delPac);
			break;
		}
	}
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
      system(execPath);
    } else if (!strcmp(command, "cpacs")) {
      if (!strcmp(arg1, "list")) {
        listcpacs();
      } else if (!strcmp(arg1, "install")) {
        getcpac(arg2);
      } else if (!strcmp(arg1, "run")) {
        char execPath[512] = {0};
        snprintf(execPath, sizeof(execPath) - sizeof(char), "./cpacs/%s", arg2);
        system(execPath);
      }
    }
  }

  exit(42);
}