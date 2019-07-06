#include "9cc.h"

static char *argregs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

void gen_lval(Node *node) {
    if (node->ty != ND_LVAR)
        error("代入の左辺値が変数ではありません");

    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->offset);
    printf("  push rax\n");
}

void gen(Node *node) {

    int cur_label_cnt;
    Vector *block_stmts;
    Vector *args;

    switch (node->ty) {
    case ND_NUM:
        printf("  push %d\n", node->val);
        return;
    case ND_LVAR:
        gen_lval(node);
        printf("  pop rax\n");
        printf("  mov rax, [rax]\n");
        printf("  push rax\n");
        return;
    case '=':
        gen_lval(node->lhs);
        gen(node->rhs);

        printf("  pop rdi\n");
        printf("  pop rax\n");
        printf("  mov [rax], rdi\n");
        printf("  push rdi\n");
        return;
    case ND_RETURN:
        gen(node->lhs);
        printf("  pop rax\n");
        printf("  mov rsp, rbp\n");
        printf("  pop rbp\n");
        printf("  ret\n");
        return;
    case ND_IF:
        cur_label_cnt = label_cnt++;
        gen(node->cond);
        printf("  mov rax, [rsp]\n"); // if条件が成立しないときは、
                                      // stack pointer を戻してはいけない
                                      // (mainのgen()呼び出し毎に
                                      // stackをpopするため、評価結果をstackに
                                      // 残しておく必要がある) ので、
                                      // ここではpopしてはいけない
        printf("  cmp rax, 0\n");
        printf("  je .Lend%d\n", cur_label_cnt);
        printf("  pop rax\n"); // 条件が成立したら、then_stmt を処理する前にpop
        gen(node->then_stmt);
        printf(".Lend%d:\n", cur_label_cnt);
        return;
    case ND_IFELSE:
        cur_label_cnt = label_cnt++;
        gen(node->cond);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .Lelse%d\n", cur_label_cnt);
        gen(node->then_stmt);
        printf("  jmp .Lend%d\n", cur_label_cnt);
        printf(".Lelse%d:\n", cur_label_cnt);
        gen(node->else_stmt);
        printf(".Lend%d:\n", cur_label_cnt);
        return;
    case ND_WHILE:
        cur_label_cnt = label_cnt++;
        printf(".Lbegin%d:\n", cur_label_cnt);
        gen(node->cond);
        printf("  mov rax, [rsp]\n"); // ND_IF と同様に、ここでpopしてはいけない
        printf("  cmp rax, 0\n");
        printf("  je .Lend%d\n", cur_label_cnt);
        printf("  pop rax\n"); // 条件が成立したら、then_stmt を処理する前にpop
        gen(node->then_stmt);
        printf("  jmp .Lbegin%d\n", cur_label_cnt);
        printf(".Lend%d:\n", cur_label_cnt);
        return;
    case ND_FOR:
        cur_label_cnt = label_cnt++;
        if (node->init) {
            gen(node->init);
            printf("  pop rax\n"); // init処理の評価結果は使用しないのでpop
        }
        printf(".Lbegin%d:\n", cur_label_cnt);
        if (node->cond) {
            gen(node->cond);
            printf("  mov rax, [rsp]\n"); // ND_IF と同様に、
                                          // ここでpopしてはいけない
            printf("  cmp rax, 0\n");
            printf("  je .Lend%d\n", cur_label_cnt);
            printf("  pop rax\n"); // 条件が成立したら、then_stmt を処理する前にpop
        }
        gen(node->then_stmt);
        if (node->incr) {
            printf("  pop rax\n"); // then_stmt処理の評価結果は使用しないのでpop
            gen(node->incr);
        }
        printf("  jmp .Lbegin%d\n", cur_label_cnt);
        printf(".Lend%d:\n", cur_label_cnt);
        return;
    case ND_BLOCK:
        block_stmts = node->block_stmts;
        for (int i = 0; i < block_stmts->len; i++) {          
            gen((Node *)block_stmts->data[i]);

            // 各ステートメントは1つの値をスタックに残すので、毎回popする
            // ただし、Blockの最後の式の評価結果はstackに残す
            if (i != (block_stmts->len - 1)) {
                printf("  pop rax\n");
            }
        }
        return;
    case ND_FUNC_CALL:
        args = node->args;
        for (int i = 0; i < args->len; i++) {
            gen(args->data[i]);
        }
        for (int i = args->len; i > 0; i--) {
            printf("  pop %s\n", argregs[i-1]);
        }
        printf("  call %.*s\n", node->len, node->name);
        printf("  push rax\n"); // 関数の返り値を stack に push
                                // (Statementの処理結果はstackに入れる方針)
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->ty) {
    case '+':
        printf("  add rax, rdi\n");
        break;
    case '-':
        printf("  sub rax, rdi\n");
        break;
    case '*':
        printf("  imul rdi\n");
        break;
    case '/':
        printf("  cqo\n");
        printf("  idiv rdi\n");
        break;
    case ND_EQ:
        printf("  cmp rax, rdi\n");
        printf("  sete al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_NE:
        printf("  cmp rax, rdi\n");
        printf("  setne al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_LE:
        printf("  cmp rax, rdi\n");
        printf("  setle al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_LT:
        printf("  cmp rax, rdi\n");
        printf("  setl al\n");
        printf("  movzb rax, al\n");
        break;

    }

    printf("  push rax\n");
}
