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
  ND_FUNCALL,
} NodeKind;

typedef struct Var Var;

struct Var {
  Var *next;
  char *name;
  int len;
  int offset;
};

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
  Node *args;
  Var *var;
  char *funcname;
  int val;
};

typedef struct VarList VarList;

struct VarList {
  VarList *next;
  Var *var;
};

typedef struct Function Function;

struct Function {
  Function *next;
  char *name;
  VarList *params;
  Node *node;
  VarList *locals;
  int stack_size;
};

Function *program();
Token *tokenize(char *p);
void codegen(Function *prog);

void error_at(char *loc, char *fmt, ...);
void error(char *fmt, ...);
void log_stderror(char *fmt, ...);
