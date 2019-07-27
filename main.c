#include "9cc.h"

char *user_input;
Vector *tokens;
int pos = 0;
Vector *code;
int label_cnt = 0;
int func_cnt = 0;
Vector *locals;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        error("引数の個数が正しくありません");
        return 1;
    }

    if (strncmp(argv[1], "-test", 5) == 0) {
        runtest();
        return 0;
    }
   
    // tokenize and parse
    user_input = argv[1];
    tokenize();
    program();

    // print the first part of assembly code
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");

    // gen code
    for (int i = 0; i < code->len; i++) {
        gen(code->data[i]);
    }

    return 0;
}
