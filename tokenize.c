#include "9cc.h"

int is_alnum(char c) {
    return ( isalpha(c) || ('0' <= c && c <= '9') );
}

int is_alnumus(char c) {
    return ( is_alnum(c) || (c == '_') );
}

void tokenize() {
    char *p = user_input;
    tokens = new_vector();

    Token *new_token;

    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6])) {
            new_token = malloc(sizeof(Token));
            new_token->ty = TK_RETURN;
            new_token->input = p;
            vec_push(tokens, (void *)new_token);
            p += 6;
            continue;
        }

        if (strncmp(p, "if", 2) == 0 && !is_alnum(p[2])) {
            new_token = malloc(sizeof(Token));
            new_token->ty = TK_IF;
            new_token->input = p;
            vec_push(tokens, (void *)new_token);
            p += 2;
            continue;
        }

        if (strncmp(p, "else", 4) == 0 && !is_alnum(p[4])) {
            new_token = malloc(sizeof(Token));
            new_token->ty = TK_ELSE;
            new_token->input = p;
            vec_push(tokens, (void *)new_token);
            p += 4;
            continue;
        }

        if (strncmp(p, "while", 5) == 0 && !is_alnum(p[5])) {
            new_token = malloc(sizeof(Token));
            new_token->ty = TK_WHILE;
            new_token->input = p;
            vec_push(tokens, (void *)new_token);
            p += 5;
            continue;
        }

        if (strncmp(p, "for", 3) == 0 && !is_alnum(p[3])) {
            new_token = malloc(sizeof(Token));
            new_token->ty = TK_FOR;
            new_token->input = p;
            vec_push(tokens, (void *)new_token);
            p += 3;
            continue;
        }

        if (strncmp(p, "==", 2) == 0) {
            new_token = malloc(sizeof(Token));
            new_token->ty = TK_EQ;
            new_token->input = p;
            vec_push(tokens, (void *)new_token);
            p += 2;
            continue;
        }

        if (strncmp(p, "!=", 2) == 0) {
            new_token = malloc(sizeof(Token));
            new_token->ty = TK_NE;
            new_token->input = p;
            vec_push(tokens, (void *)new_token);
            p += 2;
            continue;
        }

        if (strncmp(p, ">=", 2) == 0) {
            new_token = malloc(sizeof(Token));
            new_token->ty = TK_GE;
            new_token->input = p;
            vec_push(tokens, (void *)new_token);
            p += 2;
            continue;
        }

        if (strncmp(p, "<=", 2) == 0) {
            new_token = malloc(sizeof(Token));
            new_token->ty = TK_LE;
            new_token->input = p;
            vec_push(tokens, (void *)new_token);
            p += 2;
            continue;
        }

        if (*p == '+' || *p == '-' ||
            *p == '*' || *p == '/' ||
            *p == '(' || *p == ')' ||
            *p == '{' || *p == '}' ||
            *p == '=' || *p == ';' ||
            *p == ',') {
            new_token = malloc(sizeof(Token));
            new_token->ty = *p;
            new_token->input = p;
            vec_push(tokens, (void *)new_token);
            p++;
            continue;
        }

        if (*p == '>') {
            new_token = malloc(sizeof(Token));
            new_token->ty = TK_GT;
            new_token->input = p;
            vec_push(tokens, (void *)new_token);
            p++;
            continue;
        }


        if (*p == '<') {
            new_token = malloc(sizeof(Token));
            new_token->ty = TK_LT;
            new_token->input = p;
            vec_push(tokens, (void *)new_token);
            p++;
            continue;
        }

        if (isdigit(*p)) {
            new_token = malloc(sizeof(Token));
            new_token->ty = TK_NUM;
            new_token->input = p;
            new_token->val = strtol(p, &p, 10);
            vec_push(tokens, (void *)new_token);
            continue;
        }

        if (isalpha(*p)) {
            int len = 0;

            new_token = malloc(sizeof(Token));
            new_token->ty = TK_IDENT;
            new_token->input = p;

            while (is_alnumus(*p)) {
                p++;
                len++;
            }
            new_token->len = len;

            vec_push(tokens, (void *)new_token);
            continue;
        }

        error_at(p, "トークナイズできません");
    }

    new_token = malloc(sizeof(Token));
    new_token->ty = TK_EOF;
    new_token->input = p;
    vec_push(tokens, (void *)new_token);
}
