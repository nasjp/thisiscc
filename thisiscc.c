#include "thisiscc.h"

int main(int argc, char **argv) {
  if (argc != 2) {
    error("引数の個数が正しくありません");
    return 1;
  }

  user_input = argv[1];
  token = tokenize(user_input);
  Function *prog = program();

  for (Function *func = prog; func; func = func->next) {
    int offset = 0;
    for (LVar *var = prog->locals; var; var = var->next) {
      offset += 8;
      var->offset = offset;
    }
    func->stack_size = offset;
  }

  codegen(prog);

  return 0;
}
