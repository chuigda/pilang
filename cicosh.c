#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define eprintf0(fmt) fprintf(stderr, fmt)
#define eprintf1(fmt, e1) fprintf(stderr, fmt, e1)
#define eprintf2(fmt, e1, e2) fprintf(stderr, fmt, e2)

static char shbuf[256];

void parse_and_exec(const char *command);
void exec_internal(const char *command);
void exec_external(const char *command);
void exec_scp(const char *command);

int main() {
  char *buffer = (char*)malloc(4096);
  while (1) {
    eprintf0("cicosh$ ");
    fgets(buffer, 4095, stdin);
    char *cur_head = buffer;
    while (*cur_head != '\0' && 
           (*cur_head == ' ' || *cur_head == '\t')) {
      ++cur_head;
    }
    parse_and_exec(cur_head);
  }
  free(buffer);
}

void parse_and_exec(const char *command) {
  if (command[0] == '\0' || command[0] == '\n') {
  }
  else if (command[0] == '-' && !isalpha(command[1])) {
    eprintf0("welecome to cicosh.\n");
  }
  else if (command[0] == '-') {
    exec_internal(command+1);
  }
  else {
    exec_external(command);
  }
}

void exec_external(const char *command) {
  pid_t pid = fork();
  if (pid < 0) {
    eprintf0("failed to create process.\n");
    return;
  }
  if (pid == 0) {
    int i = 0;
    while (*command != ' ' && *command != '\0') {
      shbuf[i] = *command;
      ++command;
    }
    shbuf[i] = '\0';
    if ( execlp(shbuf, command) < 0 ) {
      eprintf1("cicosh: %s: not found\n", command);
    }
  }
  else {
    waitpid(pid, NULL, 0);
  }
}

void exec_internal(const char *command) {
  eprintf1("internal: received command: %s\n", command);
  if (!strncmp(command, "quit", 3)) {
    exit(0);
  }
  else if (!strncmp(command, "scp", 3)) {
    eprintf0("WARNING: ARE YOU SURE THAT YOu KNOW "\
               "WHAT ARE YOU DOING?\n");
    eprintf0("input the string \"I AM SURE\" to confirm.\n");
    eprintf0("cicosh-scp-auth: ");
    fgets(shbuf, 255, stdin);
    if (!strncmp(shbuf, "I AM SURE", 7)) {
      exec_scp(strchr(command, '-'));
    }
  }
}

void exec_scp(const char *command) {
  if (command == NULL) {
    eprintf0("no argument specified for scp, returning\n");
    return;
  }
  eprintf1("scp request: %s\n", command);
}

