#include "9cc.h"

void tokenize() {
    char *p = user_input;
    tokens = new_vector();

    Token *new_token;

    int i = 0;
    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (strncmp(p, "==", 2) == 0) {
            new_token = malloc(sizeof(Token));
            new_token->ty = TK_EQ;
            new_token->input = p;
            vec_push(tokens, (void *)new_token);
            i++;
            p += 2;
            continue;
        }

        if (strncmp(p, "!=", 2) == 0) {
            new_token = malloc(sizeof(Token));
            new_token->ty = TK_NE;
            new_token->input = p;
            vec_push(tokens, (void *)new_token);
            i++;
            p += 2;
            continue;
        }

        if (strncmp(p, ">=", 2) == 0) {
            new_token = malloc(sizeof(Token));
            new_token->ty = TK_GE;
            new_token->input = p;
            vec_push(tokens, (void *)new_token);
            i++;
            p += 2;
            continue;
        }

        if (strncmp(p, "<=", 2) == 0) {
            new_token = malloc(sizeof(Token));
            new_token->ty = TK_LE;
            new_token->input = p;
            vec_push(tokens, (void *)new_token);
            i++;
            p += 2;
            continue;
        }

        if (*p == '+' || *p == '-' ||
            *p == '*' || *p == '/' ||
            *p == '(' || *p == ')' ||
            *p == '=' || *p == ';' ) {
            new_token = malloc(sizeof(Token));
            new_token->ty = *p;
            new_token->input = p;
            vec_push(tokens, (void *)new_token);
            i++;
            p++;
            continue;
        }

        if (*p == '>') {
            new_token = malloc(sizeof(Token));
            new_token->ty = TK_GT;
            new_token->input = p;
            vec_push(tokens, (void *)new_token);
            i++;
            p++;
            continue;
        }


        if (*p == '<') {
            new_token = malloc(sizeof(Token));
            new_token->ty = TK_LT;
            new_token->input = p;
            vec_push(tokens, (void *)new_token);
            i++;
            p++;
            continue;
        }

        if (isdigit(*p)) {
            new_token = malloc(sizeof(Token));
            new_token->ty = TK_NUM;
            new_token->input = p;
            new_token->val = strtol(p, &p, 10);
            vec_push(tokens, (void *)new_token);
            i++;
            continue;
        }

        if ('a' <= *p && *p <= 'z') {
            new_token = malloc(sizeof(Token));
            new_token->ty = TK_IDENT;
            new_token->input = p;
            vec_push(tokens, (void *)new_token);
            i++;
            p++;
            continue;
        }

        error_at(p, "トークナイズできません");
    }

    new_token = malloc(sizeof(Token));
    new_token->ty = TK_EOF;
    new_token->input = p;
    vec_push(tokens, (void *)new_token);
}


Token *get_token(int idx) {
    if (idx > pos)
        error("[get_token]: 引数idx(%d) の値が pos変数(%d) より大きいです。",
              idx, pos);
    return (Token *)tokens->data[idx];
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

Node *new_node_lvar(char varname) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_LVAR;
    node->offset = (varname - 'a' + 1) * 8;
    return node;
}

int consume(int ty) {
    Token *token = get_token(pos);
    if ( token->ty != ty)
        return 0;
    pos++;
    return 1;
}

void program() {
    int i = 0;
    Token *token = get_token(pos);
    while (token->ty != TK_EOF) {
        code[i++] = stmt();
        token = get_token(pos);
    }
    code[i] = NULL;
}

Node *stmt() {
    Node *node = expr();
    if (!consume(';')) {
        Token *token = get_token(pos);
        error_at(token->input, "';'ではないトークンです");
    }
    return node;
}

Node *expr() {
    return assign();
}

Node *assign() {
    Node *node = equality();

    if (consume('=')) {
        node = new_node('=', node, assign());
    }
    return node;
}

Node *equality() {
    Node *node = relational();

    for (;;) {
        if (consume(TK_EQ))
            node = new_node(TK_EQ, node, relational());
        else if (consume(TK_NE))
            node = new_node(TK_NE, node, relational());
        else
            return node;
    }
}

Node *relational() {
    Node *node = add();

    for (;;) {
        if (consume(TK_GT))
            node = new_node(TK_LT, add(), node);
        else if (consume(TK_GE))
            node = new_node(TK_LE, add(), node);
        else if (consume(TK_LT))
            node = new_node(TK_LT, node, add());
        else if (consume(TK_LE))
            node = new_node(TK_LE, node, add());
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
    Token *token = get_token(pos);

    if (consume('(')) {
        Node *node = expr();
        if (!consume(')'))
            error_at( token->input,
                     "開きカッコに対応する閉じカッコがありません");
        return node;
    }

    if (token->ty == TK_NUM) {
        pos++;
        return new_node_num(token->val);
    }

    if (token->ty == TK_IDENT) {
        pos++;
        return new_node_lvar(token->input[0]);
    }


    error_at(token->input,
             "数値 or 変数 ではないトークンです");
    return NULL;
}
