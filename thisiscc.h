#define _GNU_SOURCE
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char *user_input;

typedef enum {
  TK_RESERVED,
  TK_IDENT,
  TK_NUM,
  TK_EOF,
} TokenKind;

typedef struct Token Token;

struct Token {
  TokenKind kind;
  Token *next;
  int val;
  char *str;
  int len;
};

extern Token *token;

typedef enum {
  ND_ADD,
  ND_SUB,
  ND_MUL,
  ND_DIV,
  ND_EQ,
  ND_NE,
  ND_LT,
  ND_LE,
  ND_NUM,
  ND_ASSIGN,
  ND_LVAR,
  ND_IF,
  ND_WHILE,
  ND_FOR,
  ND_RETURN,
  ND_BLOCK,
} NodeKind;

typedef struct Node Node;

struct Node {
  NodeKind kind;
  Node *next;
  Node *lhs;
  Node *rhs;
  Node *cond;
  Node *then;
  Node *els;
  Node *init;
  Node *inc;
  Node *body;
  int val;
  int offset;
};

typedef struct LVar LVar;

struct LVar {
  LVar *next;
  char *name;
  int len;
  int offset;
};

typedef struct Function Function;

struct Function {
  Node *node;
  LVar *locals;
  int stack_size;
};

Function *program();
Token *tokenize(char *p);
void codegen(Function *prog);

void error_at(char *loc, char *fmt, ...);
void error(char *fmt, ...);
void log_stderror(char *fmt, ...);
