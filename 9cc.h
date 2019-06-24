#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
    TK_NUM = 256,
    TK_EQ,
    TK_NE,
    TK_LT,
    TK_LE,
    TK_GT,
    TK_GE,
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

typedef struct {
    void **data;
    int capacity;
    int len;
} Vector;

extern char *user_input;
extern Vector *tokens;
extern int pos;

void error(char *fmt, ...);
void error_at(char *loc, char *msg);

Token *get_token(int idx);
Vector *new_vector();
void vec_push(Vector *vec, void *elem);

Node *new_node(int ty, Node *lhs, Node *rhs);
Node *new_node_num(int val);
int consume(int ty);
Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *term();
Node *num();
void tokenize();
void gen(Node *node);


// Test functions
void expect(int line, int expected, int actual);
void runtest();
