#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
    TK_NUM = 256,
    TK_IDENT,
    TK_RETURN,
    TK_EQ,
    TK_NE,
    TK_LT,
    TK_LE,
    TK_GT,
    TK_GE,
    TK_IF,
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
    ND_RETURN,
    ND_EQ,
    ND_NE,
    ND_LT,
    ND_LE,
    ND_GT,
    ND_IF,
    ND_GE,
};

typedef struct Node {
    int ty;
    struct Node *lhs;
    struct Node *rhs;
    struct Node *cond;
    struct Node *then_stmt;
    
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

// global variables
extern char *user_input;
extern Vector *tokens;
extern int pos;
extern Node *code[100];
extern LVar *locals;
extern int label_cnt;


/*** main.c ***/
void error(char *fmt, ...);
void error_at(char *loc, char *msg);


/*** parse.c ***/
void tokenize();
Token *get_token(int idx);
Node *new_node(int ty, Node *lhs, Node *rhs);
Node *new_node_num(int val);
Node *new_node_lvar(Token *tok);
Node *new_node_if(Node *cond, Node *then_stmt);
int consume(int ty);
int is_alnum(char c);
LVar *find_lvar(Token *tok);
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

/*** codegen.c ***/
void gen_lval(Node *node);
void gen(Node *node);

/*** container.c ***/
Vector *new_vector();
void vec_push(Vector *vec, void *elem);
// Test functions
void expect(int line, int expected, int actual);
void runtest();
