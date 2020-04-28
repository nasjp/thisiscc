#include "thisiscc.h"

Token *token;

bool consume(char *op) {
  if (!(token->kind == TK_RESERVED) || strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
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

char *expect_ident(void) {
  if (token->kind != TK_IDENT)
    error_at(token->str, "識別子ではありません");
  char *s = strndup(token->str, token->len);
  token = token->next;
  return s;
}

void expect(char *op) {
  if (!(token->kind == TK_RESERVED) || strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
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

VarList *locals;

Var *new_var(char *name) {
  Var *var = calloc(1, sizeof(Var));
  var->name = name;
  VarList *vl = calloc(1, sizeof(VarList));
  vl->var = var;
  vl->next = locals;
  locals = vl;
  return var;
}

Var *find_var(Token *tok) {
  for (VarList *vl = locals; vl; vl = vl->next) {
    Var *var = vl->var;
    if (strlen(var->name) == tok->len &&
        !strncmp(tok->str, var->name, tok->len))
      return var;
  }
  return NULL;
}

VarList *read_func_params() {
  if (consume(")"))
    return NULL;

  VarList *head = calloc(1, sizeof(VarList));
  head->var = new_var(expect_ident());
  VarList *cur = head;

  while (!consume(")")) {
    expect(",");
    cur->next = calloc(1, sizeof(VarList));
    cur->next->var = new_var(expect_ident());
    cur = cur->next;
  }

  return head;
}

Function *program();
Function *function();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

Function *program(void) {
  Function *head = calloc(1, sizeof(Function));
  Function *cur = head;
  while (!at_eof()) {
    cur->next = function();
    cur = cur->next;
  }
  return head->next;
}

Function *function() {
  locals = NULL;
  Function *func = calloc(1, sizeof(Function));
  func->name = expect_ident();
  expect("(");
  func->params = read_func_params();
  expect("{");
  Node *head = new_node(0, NULL, NULL);
  Node *cur = head;
  while (!consume(("}"))) {
    cur->next = stmt();
    cur = cur->next;
  }
  func->node = head->next;
  func->locals = locals;
  return func;
};

Node *stmt() {
  Node *node;
  if (consume("if")) {
    Node *node = new_node(ND_IF, NULL, NULL);
    expect("(");
    node->cond = expr();
    expect(")");
    node->then = stmt();
    if (consume("else"))
      node->els = stmt();
    return node;
  }

  if (consume("while")) {
    Node *node = new_node(ND_WHILE, NULL, NULL);
    expect("(");
    node->cond = expr();
    expect(")");
    node->then = stmt();
    return node;
  }

  if (consume("for")) {
    Node *node = new_node(ND_FOR, NULL, NULL);
    expect("(");
    if (!consume(";")) {
      node->init = expr();
      expect(";");
    }
    if (!consume(";")) {
      node->cond = expr();
      expect(";");
    }
    if (!consume(")")) {
      node->inc = expr();
      expect(")");
    }
    node->then = stmt();
    return node;
  }

  if (consume("{")) {
    Node *head = new_node(0, NULL, NULL);
    Node *cur = head;
    while (!consume("}")) {
      cur->next = stmt();
      cur = cur->next;
    }

    Node *node = new_node(ND_BLOCK, NULL, NULL);
    node->body = head->next;
    return node;
  }

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
    return unary();
  if (consume("-"))
    return new_node(ND_SUB, new_node_num(0), unary());
  return primary();
}

Node *func_args() {
  if (consume(")"))
    return NULL;

  Node *head = assign();
  Node *cur = head;
  while (consume(",")) {
    cur->next = assign();
    cur = cur->next;
  }
  expect(")");
  return head;
}

Node *primary() {
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  Token *tok = consume_ident();
  if (tok) {
    if (consume("(")) {
      Node *node = new_node(ND_FUNCALL, NULL, NULL);
      node->funcname = strndup(tok->str, tok->len);
      node->args = func_args();
      return node;
    }
    Node *node = new_node(ND_LVAR, NULL, NULL);

    Var *var = find_var(tok);
    if (!var)
      var = new_var(strndup(tok->str, tok->len));

    node->var = var;
    return node;
  }
  return new_node_num(expect_number());
}
