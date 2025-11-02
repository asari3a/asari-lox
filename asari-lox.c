#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
  TK_IDENTIFIER,     // 識別子
  TK_AND,            // and
  TK_CLASS,          // class
  TK_ELSE,           // else
  TK_FALSE,          // false
  TK_FUN,            // fun
  TK_FOR,            // for
  TK_IF,             // if
  TK_NIL,            // nil
  TK_OR,             // or
  TK_PRINT,          // print
  TK_RETURN,         // return
  TK_SUPER,          // super
  TK_THIS,           // this
  TK_TRUE,           // true
  TK_VAR,            // var
  TK_WHILE,          // while
  TK_EOF = -1
} TokenType;

struct Token {
  TokenType type;
  Token* next;
  char* lexeme;
};

Token head;

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

void scanTokens(char* source) {
  Token* pos = &head;
  head.next = NULL;

  char* p = source;
  unsigned long line = 0;
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
        char* start = p;
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
        // 識別子
        else if (isalpha(*p)) {
          while (isalpha(*p)) ++p;

          if ((strlen("AND") == p - start) &&
              strncmp(start, "AND", p - start) == 0) {
            pos = addToken(pos, TK_AND, p);
            break;
          }

          else if ((strlen("CLASS") == p - start) &&
                   strncmp(start, "CLASS", p - start) == 0) {
            pos = addToken(pos, TK_CLASS, p);
            break;
          }

          else if ((strlen("ELSE") == p - start) &&
                   strncmp(start, "ELSE", p - start) == 0) {
            pos = addToken(pos, TK_ELSE, p);
            break;
          }

          else if ((strlen("FALSE") == p - start) &&
                   strncmp(start, "FALSE", p - start) == 0) {
            pos = addToken(pos, TK_FALSE, p);
            break;
          }

          else if ((strlen("FOR") == p - start) &&
                   strncmp(start, "FOR", p - start) == 0) {
            pos = addToken(pos, TK_FOR, p);
            break;
          }

          else if ((strlen("FUN") == p - start) &&
                   strncmp(start, "FUN", p - start) == 0) {
            pos = addToken(pos, TK_FUN, p);
            break;
          }

          else if ((strlen("IF") == p - start) &&
                   strncmp(start, "IF", p - start) == 0) {
            pos = addToken(pos, TK_IF, p);
            break;
          }

          else if ((strlen("NIL") == p - start) &&
                   strncmp(start, "NIL", p - start) == 0) {
            pos = addToken(pos, TK_NIL, p);
            break;
          }

          else if ((strlen("OR") == p - start) &&
                   strncmp(start, "OR", p - start) == 0) {
            pos = addToken(pos, TK_OR, p);
            break;
          }

          else if ((strlen("PRINT") == p - start) &&
                   strncmp(start, "PRINT", p - start) == 0) {
            pos = addToken(pos, TK_PRINT, p);
            break;
          }

          else if ((strlen("RETURN") == p - start) &&
                   strncmp(start, "RETURN", p - start) == 0) {
            pos = addToken(pos, TK_RETURN, p);
            break;
          }

          else if ((strlen("SUPER") == p - start) &&
                   strncmp(start, "SUPER", p - start) == 0) {
            pos = addToken(pos, TK_SUPER, p);
            break;
          }

          else if ((strlen("THIS") == p - start) &&
                   strncmp(start, "THIS", p - start) == 0) {
            pos = addToken(pos, TK_THIS, p);
            break;
          }

          else if ((strlen("TRUE") == p - start) &&
                   strncmp(start, "TRUE", p - start) == 0) {
            pos = addToken(pos, TK_TRUE, p);
            break;
          }

          else if ((strlen("VAR") == p - start) &&
                   strncmp(start, "VAR", p - start) == 0) {
            pos = addToken(pos, TK_VAR, p);
            break;
          }

          else if ((strlen("WHILE") == p - start) &&
                   strncmp(start, "WHILE", p - start) == 0) {
            pos = addToken(pos, TK_WHILE, p);
            break;
          }

          else {
            pos = addToken(pos, TK_IDENTIFIER, p);
            break;
          }
        }

        error(line, "定義されていないトークンです");
        break;
    }
  }
  addToken(pos, TK_EOF, NULL);
}

static void run(char* source) {
  scanTokens(source);
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