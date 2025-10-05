#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>

typedef struct Token Token;

struct Token {
  Token *next;
};

static Token *head;

static void run(char *source) {
  for (Token *p = head; p != NULL; p = p->next) {
  }
  printf("\n");
}

static void report(int line, char *where, char *message) {
  fprintf(stderr, "[line %d] Error %s: %s\n", line, where, message);
}

static void error(int line, char *message) {
  report(line, "", message);
  exit(EX_DATAERR);
}

static void runFile(char *path) { run(path); }

static void runPrompt() {
  char buf[4096];
  for (;;) {
    printf("> ");
    int n_inputs = scanf("%s", buf);
    if (n_inputs != 1) break;
  }
}

int main(int argc, char **argv) {
  if (argc > 2) {
    printf("Usage: asari-lox [script]\n");
    exit(EX_USAGE);
  }

  if (argc == 2) {
    runFile(argv[1]);
  } else {
    runPrompt();
  }
  return 0;
}