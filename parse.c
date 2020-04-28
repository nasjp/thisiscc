#include "thisiscc.h"

Token *token;

bool consume(char *op) {
  if (!(token->kind == TK_RESERVED || token->kind == TK_RETURN) ||
      strlen(op) != token->len || memcmp(token->str, op, token->len))
    return false;
  token = token->next;
  return true;
}

Token *consume_ident() {
  if (token->kind != TK_IDENT)
    return NULL;
  Token *tmp = token;
  token = token->next;
  return tmp;
}

int expect_number() {
  if (token->kind != TK_NUM)
    error_at(token->str, "数ではありません");
  int val = token->val;
  token = token->next;
  return val;
}

void expect(char *op) {
  if (!(token->kind == TK_RESERVED || token->kind == TK_RETURN) ||
      strlen(op) != token->len || memcmp(token->str, op, token->len))
    error_at(token->str, "'%s'ではありません。\n", op);
  token = token->next;
}

bool at_eof() { return token->kind == TK_EOF; }

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

typedef struct LVar LVar;

struct LVar {
  LVar *next;
  char *name;
  int len;
  int offset;
};

LVar *locals;

int offset = 0;

LVar *new_lvar(char *name) {
  LVar *var = calloc(1, sizeof(LVar));
  var->offset = offset;
  offset += 8;
  var->name = name;
  var->len = strlen(name);
  var->next = locals;
  locals = var;
  return var;
}

LVar *find_lvar(Token *tok) {
  for (LVar *var = locals; var; var = var->next)
    if (strlen(var->name) == tok->len &&
        !strncmp(tok->str, var->name, tok->len))
      return var;
  return NULL;
}

void program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

Node *code[100];

void program() {
  int i = 0;
  while (!at_eof())
    code[i++] = stmt();
  code[i] = NULL;
}

Node *stmt() {
  Node *node;
  if (consume("return")) {
    node = new_node(ND_RETURN, expr(), NULL);
  } else {
    node = expr();
  }
  expect(";");
  return node;
}

Node *expr() { return assign(); }

Node *assign() {
  Node *node = equality();
  if (consume("=")) {
    node = new_node(ND_ASSIGN, node, assign());
  }
  return node;
}

Node *equality() {
  Node *node = relational();
  for (;;) {
    if (consume("==")) {
      node = new_node(ND_EQ, node, relational());
      continue;
    }
    if (consume("!=")) {
      node = new_node(ND_NE, node, relational());
      continue;
    }
    return node;
  }
}

Node *relational() {
  Node *node = add();
  for (;;) {
    if (consume("<=")) {
      node = new_node(ND_LE, node, add());
      continue;
    }
    if (consume(">=")) {
      node = new_node(ND_LE, add(), node);
      continue;
    }
    if (consume("<")) {
      node = new_node(ND_LT, node, add());
      continue;
    }
    if (consume(">")) {
      node = new_node(ND_LT, add(), node);
      continue;
    }
    return node;
  }
}

Node *add() {
  Node *node = mul();
  for (;;) {
    if (consume("+")) {
      node = new_node(ND_ADD, node, mul());
      continue;
    }
    if (consume("-")) {
      node = new_node(ND_SUB, node, mul());
      continue;
    }
    return node;
  }
}

Node *mul() {
  Node *node = unary();
  for (;;) {
    if (consume("*")) {
      node = new_node(ND_MUL, node, unary());
      continue;
    }
    if (consume("/")) {
      node = new_node(ND_DIV, node, unary());
      continue;
    }
    return node;
  }
}

Node *unary() {
  if (consume("+"))
    return primary();
  if (consume("-"))
    return new_node(ND_SUB, new_node_num(0), primary());
  return primary();
}

Node *primary() {
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  Token *tok = consume_ident();
  if (tok) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;

    LVar *lvar = find_lvar(tok);
    if (!lvar) {
      lvar = new_lvar(strndup(tok->str, tok->len));
      node->offset = lvar->offset;
      locals = lvar;
    }
    node->offset = lvar->offset;
    return node;
  }
  return new_node_num(expect_number());
}
