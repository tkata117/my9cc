#include "9cc.h"

Token *get_token(int idx) {
    if (idx > tokens->len)
        error("[get_token]: 引数idx(%d) の値が トークン数(%d) より大きいです。",
              idx, tokens->len);
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

Node *new_node_lvar(Token *tok) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_LVAR;
    
    LVar *func_local = locals->data[func_cnt];
    LVar *lvar = find_lvar(func_local, tok);
    if (lvar) {
        node->offset = lvar->offset;
    } else {
        lvar = malloc(sizeof(LVar));
        lvar->next = func_local;
        lvar->name = tok->input;
        lvar->len = tok->len;
        if (func_local) {
            lvar->offset = func_local->offset + 8;
        } else {
            lvar->offset = 8;
        }
        node->offset = lvar->offset;
        locals->data[func_cnt] = lvar;
    }

    return node;
}

Node *new_node_if(Node *cond, Node *then_stmt) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_IF;
    node->cond = cond;
    node->then_stmt = then_stmt;

    return node;
}

Node *new_node_ifelse(Node *cond, Node *then_stmt, Node *else_stmt) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_IFELSE;
    node->cond = cond;
    node->then_stmt = then_stmt;
    node->else_stmt = else_stmt;

    return node;
}

Node *new_node_while(Node *cond, Node *then_stmt) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_WHILE;
    node->cond = cond;
    node->then_stmt = then_stmt;

    return node;
}

Node *new_node_for(Node *init, Node *cond, Node *incr, Node *then_stmt) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_FOR;
    node->init = init;
    node->cond = cond;
    node->incr = incr;
    node->then_stmt = then_stmt;

    return node;
}

Node *new_node_func_call(Token *tok, Vector *args) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_FUNC_CALL;
    node->name = tok->input;
    node->len = tok->len;
    node->args = args;
    
    return node;
}

Node *new_node_func_declare(Token *tok, Vector *block_stmts, Vector *params) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_FUNC_DECLARE;
    node->name = tok->input;
    node->len = tok->len;
    node->block_stmts = block_stmts;
    node->func_num = func_cnt++;
    node->params = params;
    
    return node;
}

int consume(int ty) {
    Token *token = get_token(pos);
    if ( token->ty != ty)
        return 0;
    pos++;
    return 1;
}


LVar *find_lvar(LVar *lvars, Token *tok) {
    for (LVar *var = lvars; var; var = var->next) {
        if ( (var->len == tok->len) &&
             (memcmp(tok->input, var->name, var->len) == 0) ) {
            return var;
        }
    }
    return NULL;
}


void program() {
    Token *token = get_token(pos);

    locals = new_vector();
    code = new_vector();
    while (token->ty != TK_EOF) {
        vec_push(code, (void *)func());
        token = get_token(pos);
    }
}

Node *func() {
    Token *token = get_token(pos);
    Node *node;

    if (consume(TK_IDENT)) {
        Vector *params = NULL;

        vec_push(locals, NULL);

        if (!consume('(')) {
            token = get_token(pos);
            error_at(token->input, "'('ではないトークンです");
        }

        if (!consume(')')) {
            params = param();
            if (!consume(')')) {
                token = get_token(pos);
                error_at(token->input, "')'ではないトークンです");
            }
        }

        if (!consume('{')) {
            token = get_token(pos);
            error_at(token->input, "'{'ではないトークンです");
        }

        Vector *block_stmts = new_vector();
        while (!consume('}')) {
            vec_push(block_stmts, (void *)stmt());
        }

        return new_node_func_declare(token, block_stmts, params);
    } else {
        error_at(token->input, "関数名ではないトークンです");
    }
}

Node *stmt() {
    Node *node;

    if (consume(TK_RETURN)) {
        node = new_node(ND_RETURN, expr(), NULL);
    } else if (consume(TK_IF)) {
        if (!consume('(')) {
            Token *token = get_token(pos);
            error_at(token->input, "'('ではないトークンです");
        }

        Node *cond = expr();

        if (!consume(')')) {
            Token *token = get_token(pos);
            error_at(token->input, "')'ではないトークンです");
        }

        Node *then_stmt = stmt();

        if (consume(TK_ELSE)) {
            node = new_node_ifelse(cond, then_stmt, stmt());
        } else {
            node = new_node_if(cond, then_stmt);
        }

        return node;
    } else if (consume(TK_WHILE)) {
        if (!consume('(')) {
            Token *token = get_token(pos);
            error_at(token->input, "'('ではないトークンです");
        }

        Node *cond = expr();

        if (!consume(')')) {
            Token *token = get_token(pos);
            error_at(token->input, "')'ではないトークンです");
        }

        return new_node_while(cond, stmt());
    } else if (consume(TK_FOR)) {
        if (!consume('(')) {
            Token *token = get_token(pos);
            error_at(token->input, "'('ではないトークンです");
        }

        Node *init = NULL;
        if (!consume(';')) {
            init = expr();
            if (!consume(';')) {
                Token *token = get_token(pos);
                error_at(token->input, "';'ではないトークンです");
            }
        }

        Node *cond = NULL;
        if (!consume(';')) {
            cond = expr();
            if (!consume(';')) {
                Token *token = get_token(pos);
                error_at(token->input, "';'ではないトークンです");
            }
        }

        Node *incr = NULL;
        if (!consume(')')) {
            incr = expr();
            if (!consume(')')) {
                Token *token = get_token(pos);
                error_at(token->input, "')'ではないトークンです");
            }
        }

        return new_node_for(init, cond, incr, stmt());

    } else if (consume('{')) {
        node = malloc(sizeof(Node));
        node->ty = ND_BLOCK;
        node->block_stmts = new_vector();

        while (!consume('}')) {
            vec_push(node->block_stmts, (void *)stmt());
        }

        return node;
    } else {
        node = expr();
    }

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
            node = new_node(ND_EQ, node, relational());
        else if (consume(TK_NE))
            node = new_node(ND_NE, node, relational());
        else
            return node;
    }
}

Node *relational() {
    Node *node = add();

    for (;;) {
        if (consume(TK_GT))
            node = new_node(ND_LT, add(), node);
        else if (consume(TK_GE))
            node = new_node(ND_LE, add(), node);
        else if (consume(TK_LT))
            node = new_node(ND_LT, node, add());
        else if (consume(TK_LE))
            node = new_node(ND_LE, node, add());
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
    if (consume('&'))
        return new_node(ND_ADDR, unary(), NULL);
    if (consume('*'))
        return new_node(ND_DEREF, unary(), NULL);
    return term();
}

Node *term() {
    Token *token = get_token(pos);

    if (consume('(')) {
        Node *node = expr();
        if (!consume(')')) {
            token = get_token(pos);
            error_at( token->input,
                     "開きカッコに対応する閉じカッコがありません");
        }
        return node;
    }

    if (consume(TK_NUM)) {
        return new_node_num(token->val);
    }

    if (consume(TK_IDENT)) {
        if (consume('(')) { // 関数呼び出し

            Vector *args = NULL;
            if (!consume(')')) {
                args = arg();
                if (!consume(')')) {
                    token = get_token(pos);
                    error_at( token->input, "')' がありません");
                }
            }

            return new_node_func_call(token, args);

        } else { // 変数
            return new_node_lvar(token);
        }
    }

    error_at(token->input,
             "数値 or 変数 ではないトークンです");
    return NULL;
}

Vector *arg() {
    Vector *args = new_vector();
    for (;;) {
        if (args->len > 0) {
            if (!consume(',')) {
                return args;
            }
        }
        vec_push(args, (void *)expr());
    }
}

Vector *param() {
    Vector *params = new_vector();

    for (;;) {
        Token *token;
        if (params->len > 0) {
            if (!consume(',')) {
                return params;
            }
        }

        token = get_token(pos);
        if (consume(TK_IDENT)) {
            vec_push(params, new_node_lvar(token));
        } else {
            error_at(token->input, "引数名ではないトークンです");
        }
    }
}
