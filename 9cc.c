#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
    TK_NUM = 256,
    TK_EQ,
    TK_NE,
    TK_EOF,
};

typedef struct {
    int ty;
    int val;
    char *input;
} Token;

enum {
    ND_NUM = 256,
};

typedef struct Node {
    int ty;
    struct Node *lhs;
    struct Node *rhs;
    int val;
} Node;

char *user_input;
Token tokens[100];
int pos = 0;

void error(char *fmt, ...);
void error_at(char *loc, char *msg);
Node *new_node(int ty, Node *lhs, Node *rhs);
Node *new_node_num(int val);
int consume(int ty);
Node *expr();
Node *equality();
Node *add();
Node *mul();
Node *unary();
Node *term();
Node *num();
void tokenize();
void gen(Node *node);

void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void error_at(char *loc, char *msg) {
    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, "");
    fprintf(stderr, "^ %s\n", msg);
    exit(1);
}

Node *new_node(int ty, Node *lhs, Node *rhs) {
    Node *node = malloc(sizeof(Node));
    node->ty = ty;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_NUM;
    node->val = val;
    return node;
}

int consume(int ty) {
    if (tokens[pos].ty != ty)
        return 0;
    pos++;
    return 1;
}

Node *expr() {
    return equality();
}

Node *equality() {
    Node *node = add();

    for (;;) {
        if (consume(TK_EQ))
            node = new_node(TK_EQ, node, add());
        else if (consume(TK_NE))
            node = new_node(TK_NE, node, add());
        else
            return node;
    }
}

Node *add() {
    Node *node = mul();

    for (;;) {
        if (consume('+'))
            node = new_node('+', node, mul());
        else if (consume('-'))
            node = new_node('-', node, mul());
        else
            return node;
    }
}

Node *mul() {
    Node *node = unary();

    for (;;) {
        if (consume('*'))
            node = new_node('*', node, unary());
        else if (consume('/'))
            node = new_node('/', node, unary());
        else
            return node;
    }
}

Node *unary() {
    if (consume('+'))
        return term();
    if (consume('-'))
        return new_node('-', new_node_num(0), term());
    return term();
}

Node *term() {
    if (consume('(')) {
        Node *node = expr();
        if (!consume(')'))
            error_at(tokens[pos].input,
                     "開きカッコに対応する閉じカッコがありません");
        return node;
    }

    return num();
}

Node *num(){
    if (tokens[pos].ty == TK_NUM)
        return new_node_num(tokens[pos++].val);

    error_at(tokens[pos].input,
             "数値ではないトークンです");
    return NULL;
}

void gen(Node *node) {
    if (node->ty == ND_NUM) {
        printf("  push %d\n", node->val);
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
    case TK_EQ:
        printf("  cmp rax, rdi\n");
        printf("  sete al\n");
        printf("  movzb rax, al\n");
        break;
    case TK_NE:
        printf("  cmp rax, rdi\n");
        printf("  setne al\n");
        printf("  movzb rax, al\n");
        break;
    }

    printf("  push rax\n");
}


void tokenize() {
    char *p = user_input;

    int i = 0;
    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-' ||
            *p == '*' || *p == '/' ||
            *p == '(' || *p == ')' ) {
            tokens[i].ty = *p;
            tokens[i].input = p;
            i++;
            p++;
            continue;
        }

        if (strncmp(p, "==", 2) == 0) {
            tokens[i].ty = TK_EQ;
            tokens[i].input = p;
            i++;
            p += 2;
            continue;
        }

        if (strncmp(p, "!=", 2) == 0) {
            tokens[i].ty = TK_NE;
            tokens[i].input = p;
            i++;
            p += 2;
            continue;
        }

        if (isdigit(*p)) {
            tokens[i].ty = TK_NUM;
            tokens[i].input = p;
            tokens[i].val = strtol(p, &p, 10);
            i++;
            continue;
        }

        error_at(p, "トークナイズできません");
    }

    tokens[i].ty = TK_EOF;
    tokens[i].input = p;
}



int main(int argc, char *argv[]) {
    if (argc != 2) {
        error("引数の個数が正しくありません");
        return 1;
    }

    // tokenize and parse
    user_input = argv[1];
    tokenize();
    Node *node = expr();

    // print the first part of assembly code
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // gen code
    gen(node);

    printf("  pop rax\n");
    printf("  ret\n");
    return 0;
}
