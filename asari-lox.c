#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>

typedef struct Token Token;

typedef enum {
  TK_LEFT_PAREN,     // (
  TK_RIGHT_PAREN,    // )
  TK_LEFT_BRACE,     // {
  TK_RIGHT_BRACE,    // }
  TK_COMMA,          // ,
  TK_DOT,            // .
  TK_MINUS,          // -
  TK_PLUS,           // +
  TK_SEMICOLON,      // ;
  TK_STAR,           // *
  TK_EQUAL,          // =
  TK_EQUAL_EQUAL,    // ==
  TK_BANG,           // !
  TK_BANG_EQUAL,     // !=
  TK_LESS,           // <
  TK_LESS_EQUAL,     // <=
  TK_GREATER,        // >
  TK_GREATER_EQUAL,  // >=
  TK_SLASH,          // /
  TK_STRING,         // 文字列リテラル
  TK_NUMBER,         // 数値リテラル
  TK_EOF = -1
} TokenType;

struct Token {
  TokenType type;
  Token* next;
  char* lexeme;
};

Token* addToken(Token* pos, TokenType type, char* lexeme) {
  Token* token = (Token*)calloc(1, sizeof(Token));
  token->type = type;
  token->next = NULL;
  pos->next = token;
  return token;
}

static void report(int line, char* where, char* message) {
  fprintf(stderr, "[line %d] Error %s: %s\n", line, where, message);
}

static void error(int line, char* message) {
  report(line, "", message);
  exit(EX_DATAERR);
}

static void run(char* source) {
  unsigned long line = 0;
  char* p = source;
  Token head, *pos = &head;
  head.next = NULL;

  while (*p) {
    if (isspace(*p)) {
      if (*p == '\n') ++line;
      ++p;
      continue;
    }

    switch (*p) {
      case '(':
        pos = addToken(pos, TK_LEFT_PAREN, p);
        ++p;
        break;

      case ')':
        pos = addToken(pos, TK_RIGHT_PAREN, p);
        ++p;
        break;
      case '{':
        pos = addToken(pos, TK_LEFT_BRACE, p);
        ++p;
        break;
      case '}':
        pos = addToken(pos, TK_RIGHT_BRACE, p);
        ++p;
        break;
      case ',':
        pos = addToken(pos, TK_COMMA, p);
        ++p;
        break;
      case '.':
        pos = addToken(pos, TK_DOT, p);
        ++p;
        break;
      case '-':
        pos = addToken(pos, TK_MINUS, p);
        ++p;
        break;
      case '+':
        pos = addToken(pos, TK_PLUS, p);
        ++p;
        break;
      case ';':
        pos = addToken(pos, TK_SEMICOLON, p);
        ++p;
        break;
      case '*':
        pos = addToken(pos, TK_STAR, p);
        ++p;
        break;
      case '=':
        if (*(p + 1) == '=') {
          pos = addToken(pos, TK_EQUAL_EQUAL, p);
          p += 2;
        } else {
          pos = addToken(pos, TK_EQUAL, p);
          ++p;
        }
        break;
      case '!':
        if (*(p + 1) == '=') {
          pos = addToken(pos, TK_BANG_EQUAL, p);
          p += 2;
        } else {
          pos = addToken(pos, TK_BANG, p);
          ++p;
        }
        break;
      case '<':
        if (*(p + 1) == '=') {
          pos = addToken(pos, TK_LESS_EQUAL, p);
          p += 2;
        } else {
          pos = addToken(pos, TK_LESS, p);
          ++p;
        }
        break;
      case '>':
        if (*(p + 1) == '=') {
          pos = addToken(pos, TK_GREATER_EQUAL, p);
          p += 2;
        } else {
          pos = addToken(pos, TK_GREATER, p);
          ++p;
        }
        break;
      case '/':
        if (*(p + 1) == '/') {
          while (*p != '\n' && *p != '\0') {
            ++p;
          }
        } else {
          pos = addToken(pos, TK_SLASH, p);
          ++p;
        }
        break;
      case '\"':
        ++p;
        while (*p != '\"' && *p != '\0') {
          if (*p == '\n') line++;
          ++p;
        }
        if (*p == '\0') {
          error(line, "文字列が終結していません。");
        }
        pos = addToken(pos, TK_STRING, p);
        ++p;
        break;
      default:
        // 数値トークン
        if (isdigit(*p)) {
          while (isdigit(*p)) {
            ++p;
          }

          if (*p == '.' && isdigit(*(p + 1))) {
            ++p;
            while (isdigit(*p)) {
              ++p;
            }
          }

          pos = addToken(pos, TK_NUMBER, p);
          break;
        }
        error(line, "定義されていないトークンです");
        break;
    }
  }
  addToken(pos, TK_EOF, NULL);

  for (Token* p = head.next; p != NULL; p = p->next) {
    if (p != head.next) printf(" ");
    printf("%d", p->type);
  }
  printf("\n");
}

static void runFile(char* path) { run(path); }

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

void scanTokens(char* source) {}

int main(int argc, char** argv) {
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