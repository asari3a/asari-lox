#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>

typedef struct Token Token;
typedef struct Node Node;

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

typedef enum {
  ND_ADD,  // +
  ND_MUL,  // *
  ND_NUM   // 数値
} NodeKind;

struct Token {
  TokenType type;
  Token* next;
  char* lexeme;
  size_t length;
};

struct Node {
  NodeKind kind;
  Node* lhs;
  Node* rhs;
  double val;
};

Token head, *tok;

Token* addToken(Token* pos, TokenType type, char* start, size_t len) {
  Token* token = (Token*)calloc(1, sizeof(Token));
  token->type = type;
  token->next = NULL;
  token->length = len;
  token->lexeme = calloc(len + 1, sizeof(char));
  memcpy(token->lexeme, start, len);
  token->lexeme[len] = '\0';
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
    char* start;

    switch (*p) {
      case '(':
        pos = addToken(pos, TK_LEFT_PAREN, p, 1);
        ++p;
        break;

      case ')':
        pos = addToken(pos, TK_RIGHT_PAREN, p, 1);
        ++p;
        break;
      case '{':
        pos = addToken(pos, TK_LEFT_BRACE, p, 1);
        ++p;
        break;
      case '}':
        pos = addToken(pos, TK_RIGHT_BRACE, p, 1);
        ++p;
        break;
      case ',':
        pos = addToken(pos, TK_COMMA, p, 1);
        ++p;
        break;
      case '.':
        pos = addToken(pos, TK_DOT, p, 1);
        ++p;
        break;
      case '-':
        pos = addToken(pos, TK_MINUS, p, 1);
        ++p;
        break;
      case '+':
        pos = addToken(pos, TK_PLUS, p, 1);
        ++p;
        break;
      case ';':
        pos = addToken(pos, TK_SEMICOLON, p, 1);
        ++p;
        break;
      case '*':
        pos = addToken(pos, TK_STAR, p, 1);
        ++p;
        break;
      case '=':
        if (*(p + 1) == '=') {
          pos = addToken(pos, TK_EQUAL_EQUAL, p, 2);
          p += 2;
        } else {
          pos = addToken(pos, TK_EQUAL, p, 1);
          ++p;
        }
        break;
      case '!':
        if (*(p + 1) == '=') {
          pos = addToken(pos, TK_BANG_EQUAL, p, 2);
          p += 2;
        } else {
          pos = addToken(pos, TK_BANG, p, 1);
          ++p;
        }
        break;
      case '<':
        if (*(p + 1) == '=') {
          pos = addToken(pos, TK_LESS_EQUAL, p, 2);
          p += 2;
        } else {
          pos = addToken(pos, TK_LESS, p, 1);
          ++p;
        }
        break;
      case '>':
        if (*(p + 1) == '=') {
          pos = addToken(pos, TK_GREATER_EQUAL, p, 2);
          p += 2;
        } else {
          pos = addToken(pos, TK_GREATER, p, 1);
          ++p;
        }
        break;
      case '/':
        if (*(p + 1) == '/') {
          while (*p != '\n' && *p != '\0') {
            ++p;
          }
        } else {
          pos = addToken(pos, TK_SLASH, p, 1);
          ++p;
        }
        break;
      case '\"':
        start = ++p;
        while (*p != '\"' && *p != '\0') {
          if (*p == '\n') line++;
          ++p;
        }
        if (*p == '\0') {
          error(line, "文字列が終結していません。");
        }
        pos = addToken(pos, TK_STRING, start, (size_t)(p - start));
        ++p;
        break;
      default:
        start = p;
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

          pos = addToken(pos, TK_NUMBER, start, (size_t)(p - start));
          break;
        }
        // 識別子
        else if (isalpha(*p)) {
          while (isalpha(*p)) ++p;

          if ((strlen("and") == p - start) &&
              strncmp(start, "and", p - start) == 0) {
            pos = addToken(pos, TK_AND, start, (size_t)(p - start));
            break;
          }

          else if ((strlen("class") == p - start) &&
                   strncmp(start, "class", p - start) == 0) {
            pos = addToken(pos, TK_CLASS, start, (size_t)(p - start));
            break;
          }

          else if ((strlen("else") == p - start) &&
                   strncmp(start, "else", p - start) == 0) {
            pos = addToken(pos, TK_ELSE, start, (size_t)(p - start));
            break;
          }

          else if ((strlen("false") == p - start) &&
                   strncmp(start, "false", p - start) == 0) {
            pos = addToken(pos, TK_FALSE, start, (size_t)(p - start));
            break;
          }

          else if ((strlen("for") == p - start) &&
                   strncmp(start, "for", p - start) == 0) {
            pos = addToken(pos, TK_FOR, start, (size_t)(p - start));
            break;
          }

          else if ((strlen("fun") == p - start) &&
                   strncmp(start, "fun", p - start) == 0) {
            pos = addToken(pos, TK_FUN, start, (size_t)(p - start));
            break;
          }

          else if ((strlen("if") == p - start) &&
                   strncmp(start, "if", p - start) == 0) {
            pos = addToken(pos, TK_IF, start, (size_t)(p - start));
            break;
          }

          else if ((strlen("nil") == p - start) &&
                   strncmp(start, "nil", p - start) == 0) {
            pos = addToken(pos, TK_NIL, start, (size_t)(p - start));
            break;
          }

          else if ((strlen("or") == p - start) &&
                   strncmp(start, "or", p - start) == 0) {
            pos = addToken(pos, TK_OR, start, (size_t)(p - start));
            break;
          }

          else if ((strlen("print") == p - start) &&
                   strncmp(start, "print", p - start) == 0) {
            pos = addToken(pos, TK_PRINT, start, (size_t)(p - start));
            break;
          }

          else if ((strlen("return") == p - start) &&
                   strncmp(start, "return", p - start) == 0) {
            pos = addToken(pos, TK_RETURN, start, (size_t)(p - start));
            break;
          }

          else if ((strlen("super") == p - start) &&
                   strncmp(start, "super", p - start) == 0) {
            pos = addToken(pos, TK_SUPER, start, (size_t)(p - start));
            break;
          }

          else if ((strlen("this") == p - start) &&
                   strncmp(start, "this", p - start) == 0) {
            pos = addToken(pos, TK_THIS, start, (size_t)(p - start));
            break;
          }

          else if ((strlen("true") == p - start) &&
                   strncmp(start, "true", p - start) == 0) {
            pos = addToken(pos, TK_TRUE, start, (size_t)(p - start));
            break;
          }

          else if ((strlen("var") == p - start) &&
                   strncmp(start, "var", p - start) == 0) {
            pos = addToken(pos, TK_VAR, start, (size_t)(p - start));
            break;
          }

          else if ((strlen("while") == p - start) &&
                   strncmp(start, "while", p - start) == 0) {
            pos = addToken(pos, TK_WHILE, start, (size_t)(p - start));
            break;
          }

          else {
            pos = addToken(pos, TK_IDENTIFIER, start, (size_t)(p - start));
            break;
          }
        }

        error(line, "定義されていないトークンです");
        break;
    }
  }
  addToken(pos, TK_EOF, NULL, 0);
}

Node* new_node(NodeKind kind, Node* lhs, Node* rhs) {
  Node* node = (Node*)calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node* new_node_num(double val) {
  Node* node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

static void run(char* source) {
  // --- トークナイズ ---
  scanTokens(source);

  for (Token* p = head.next; p != NULL && p->type != TK_EOF; p = p->next) {
    if (p != head.next) printf(" ");
    printf("%s", p->lexeme);
  }
  printf("\n");
}

static void runFile(char* path) {
  FILE* fp = fopen(path, "r");
  if (fp == NULL) {
    fprintf(stderr, "ファイルを開けませんでした: %s\n", path);
    exit(EX_IOERR);
  }

  fseek(fp, 0L, SEEK_END);
  size_t file_size = ftell(fp);
  rewind(fp);

  char* buf = (char*)calloc(file_size + 1, sizeof(char));
  if (buf == NULL) {
    fprintf(stderr, "バッファの確保に失敗しました: %s\n", path);
    exit(EX_IOERR);
  }

  size_t n_read = fread(buf, sizeof(char), file_size, fp);
  if (n_read < file_size) {
    fprintf(stderr, "ファイルの読み取りに失敗しました: %s\n", path);
    exit(EX_IOERR);
  }

  buf[n_read] = '\0';
  fclose(fp);

  run(buf);
}

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