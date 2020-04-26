#include "thisiscc.h"

Token *token;

bool consume(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
    return false;
  token = token->next;
  return true;
}

int expect_number() {
  if (token->kind != TK_NUM)
    error_at(token->str, "数ではありません");

  int val = token->val;
  token = token->next;
  return val;
}

void expect(char *op) {
  if (token->kind != TK_RESERVED || strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
    error_at(token->str, "'%c'ではありません。\n", op);
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

Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();
Node *expr() { return equality(); }

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
  return new_node_num(expect_number());
}
