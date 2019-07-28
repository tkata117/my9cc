#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    TK_ELSE,
    TK_WHILE,
    TK_FOR,
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
    ND_ADDR,
    ND_DEREF,
    ND_RETURN,
    ND_EQ,
    ND_NE,
    ND_LT,
    ND_LE,
    ND_GT,
    ND_GE,
    ND_IF,
    ND_IFELSE,
    ND_WHILE,
    ND_FOR,
    ND_BLOCK,
    ND_FUNC_CALL,
    ND_FUNC_DECLARE
};

typedef struct Node {
    int ty;
    struct Node *lhs;
    struct Node *rhs;
    struct Node *init;
    struct Node *cond;
    struct Node *incr;
    struct Node *then_stmt;
    struct Node *else_stmt;
    Vector *block_stmts;

    int val;

    // for variable
    int offset;

    // for function declare
    int func_num;
    Vector *params;

    // for function call
    char *name;
    int len;
    Vector *args;
} Node;

// global variables
extern char *user_input;
extern Vector *tokens;
extern int pos;
extern Vector *code;
extern int label_cnt;
extern int func_cnt;
extern Vector *locals;

/*** tokenize.c ***/
int is_alnum(char c);
int is_alnumus(char c);
Token *new_token(int ty, int val, char *input, int len);
void tokenize();

/*** parse.c ***/
Token *get_token(int idx);
Node *new_node(int ty, Node *lhs, Node *rhs);
Node *new_node_num(int val);
Node *new_node_lvar(Token *tok);
Node *new_node_if(Node *cond, Node *then_stmt);
Node *new_node_ifelse(Node *cond, Node *then_stmt, Node *else_stmt);
Node *new_node_while(Node *cond, Node *then_stmt);
Node *new_node_for(Node *init, Node *cond, Node *incr, Node *then_stmt);
Node *new_node_func_call(Token *tok, Vector *args);
Node *new_node_func_declare(Token *tok, Vector *block_stmts, Vector *params);
int consume(int ty);
LVar *find_lvar(LVar *lvars, Token *tok);
void program();
Node *func();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *term();
Vector *argument();
Vector *parameter();

/*** codegen.c ***/
void gen_lval(Node *node);
void gen(Node *node);

/*** container.c ***/
Vector *new_vector();
void vec_push(Vector *vec, void *elem);

/*** util.c ***/
void error(char *fmt, ...);
void error_at(char *loc, char *msg);


/*** test.c ***/
void expect(int line, int expected, int actual);
void runtest();
