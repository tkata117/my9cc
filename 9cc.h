#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
    TK_NUM = 256,
    TK_IDENT,
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
    int len;
} Token;

enum {
    ND_NUM = 256,
    ND_LVAR,
};

typedef struct Node {
    int ty;
    struct Node *lhs;
    struct Node *rhs;
    int val;
    int offset;
} Node;

typedef struct {
    void **data;
    int capacity;
    int len;
} Vector;


typedef struct LVar {
    struct LVar *next;
    char *name;
    int len;
    int offset;
} LVar;

extern char *user_input;
extern Vector *tokens;
extern int pos;
extern Node *code[100];
extern LVar *locals;

void error(char *fmt, ...);
void error_at(char *loc, char *msg);

Token *get_token(int idx);
Vector *new_vector();
void vec_push(Vector *vec, void *elem);

LVar *find_lvar(Token *tok);

Node *new_node(int ty, Node *lhs, Node *rhs);
Node *new_node_num(int val);
Node *new_node_lvar(Token *tok);
int consume(int ty);

void program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *term();

void tokenize();

void gen_lval(Node *node);
void gen(Node *node);


// Test functions
void expect(int line, int expected, int actual);
void runtest();
