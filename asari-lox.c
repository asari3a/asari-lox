#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>

typedef struct Token Token;

typedef enum {
  TK_LEFT_PAREN,
  TK_RIGHT_PAREN,
  TK_LEFT_BRACE,
  TK_RIGHT_BRACE,
  TK_COMMA,
  TK_DOT,
  TK_MINUS,
  TK_PLUS,
  TK_SEMICOLON,
  TK_STAR,
  TK_EOF
} TokenType;

struct Token {
  TokenType type;
  Token *next;
};

Token *addToken(Token *pos, TokenType type) {
  Token *token = (Token *)calloc(1, sizeof(Token));
  token->type = type;
  token->next = NULL;
  pos->next = token;
  return token;
}

static void report(int line, char *where, char *message) {
  fprintf(stderr, "[line %d] Error %s: %s\n", line, where, message);
}

static void error(int line, char *message) {
  report(line, "", message);
  exit(EX_DATAERR);
}

static void run(char *source) {
  char *p = source;
  Token head, *pos = &head;
  head.next = NULL;

  while (*p) {
    if (isspace(*p)) {
      ++p;
      continue;
    }

    switch (*p) {
      case '(':
        pos = addToken(pos, TK_LEFT_PAREN);
        ++p;
        break;

      case ')':
        pos = addToken(pos, TK_RIGHT_PAREN);
        ++p;
        break;
      case '{':
        pos = addToken(pos, TK_LEFT_BRACE);
        ++p;
        break;
      case '}':
        pos = addToken(pos, TK_RIGHT_BRACE);
        ++p;
        break;
      case ',':
        pos = addToken(pos, TK_COMMA);
        ++p;
        break;
      case '.':
        pos = addToken(pos, TK_DOT);
        ++p;
        break;
      case '-':
        pos = addToken(pos, TK_MINUS);
        ++p;
        break;
      case '+':
        pos = addToken(pos, TK_PLUS);
        ++p;
        break;
      case ';':
        pos = addToken(pos, TK_SEMICOLON);
        ++p;
        break;
      case '*':
        pos = addToken(pos, TK_STAR);
        ++p;
        break;
      default:
        error(1, "定義されていないトークンです");
        break;
    }
  }
  addToken(pos, TK_EOF);

  for (Token *p = head.next; p != NULL; p = p->next) {
    printf("%d\n", p->type);
  }
  printf("\n");
}

static void runFile(char *path) { run(path); }

static void runPrompt() {
  // ToDO: 空白文字があるとバグる
  char buf[4096] = "";
  for (;;) {
    printf("> ");
    int n_inputs = scanf("%s", buf);
    if (n_inputs != 1) break;
    run(buf);
  }
}

void scanTokens(char *source) {}

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