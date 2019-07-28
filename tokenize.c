#include "9cc.h"

int is_alnum(char c) {
    return ( isalpha(c) || ('0' <= c && c <= '9') );
}

int is_alnumus(char c) {
    return ( is_alnum(c) || (c == '_') );
}

Token *new_token(int ty, int val, char *input, int len) {
    Token *token = malloc(sizeof(Token));
    token->ty = ty;
    token->val = val;
    token->input = input;
    token->len = len;
    return token;
}

void tokenize() {
    char *p = user_input;
    tokens = new_vector();

    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6])) {
            vec_push(tokens, (void *)new_token(TK_RETURN, 0, p, 6));
            p += 6;
            continue;
        }

        if (strncmp(p, "if", 2) == 0 && !is_alnum(p[2])) {
            vec_push(tokens, (void *)new_token(TK_IF, 0, p, 2));
            p += 2;
            continue;
        }

        if (strncmp(p, "else", 4) == 0 && !is_alnum(p[4])) {
            vec_push(tokens, (void *)new_token(TK_ELSE, 0, p, 4));
            p += 4;
            continue;
        }

        if (strncmp(p, "while", 5) == 0 && !is_alnum(p[5])) {
            vec_push(tokens, (void *)new_token(TK_WHILE, 0, p, 5));
            p += 5;
            continue;
        }

        if (strncmp(p, "for", 3) == 0 && !is_alnum(p[3])) {
            vec_push(tokens, (void *)new_token(TK_FOR, 0, p, 3));
            p += 3;
            continue;
        }

        if (strncmp(p, "==", 2) == 0) {
            vec_push(tokens, (void *)new_token(TK_EQ, 0, p, 2));
            p += 2;
            continue;
        }

        if (strncmp(p, "!=", 2) == 0) {
            vec_push(tokens, (void *)new_token(TK_NE, 0, p, 2));
            p += 2;
            continue;
        }

        if (strncmp(p, ">=", 2) == 0) {
            vec_push(tokens, (void *)new_token(TK_GE, 0, p, 2));
            p += 2;
            continue;
        }

        if (strncmp(p, "<=", 2) == 0) {
            vec_push(tokens, (void *)new_token(TK_LE, 0, p, 2));
            p += 2;
            continue;
        }

        if (strncmp(p, "int", 3) == 0) {
            vec_push(tokens, (void *)new_token(TK_INT, 0, p, 3));
            p += 3;
            continue;
        }

        if (*p == '+' || *p == '-' ||
            *p == '*' || *p == '/' ||
            *p == '(' || *p == ')' ||
            *p == '{' || *p == '}' ||
            *p == '=' || *p == ';' ||
            *p == ',' || *p == '&' ) {
            vec_push(tokens, (void *)new_token(*p, 0, p, 1));
            p++;
            continue;
        }

        if (*p == '>') {
            vec_push(tokens, (void *)new_token(TK_GT, 0, p, 1));
            p++;
            continue;
        }


        if (*p == '<') {
            vec_push(tokens, (void *)new_token(TK_LT, 0, p, 1));
            p++;
            continue;
        }

        if (isdigit(*p)) {
            char *str = p;
            int val = strtol(p, &p, 10);
            int len = p - str;
            vec_push(tokens, (void *)new_token(TK_NUM, val, str, len));
            continue;
        }

        if (isalpha(*p)) {
            char *str = p;
            int len = 0;

            while (is_alnumus(*p)) {
                p++;
                len++;
            }

            vec_push(tokens, (void *)new_token(TK_IDENT, 0, str, len));
            continue;
        }

        error_at(p, "トークナイズできません");
    }

    vec_push(tokens, (void *)new_token(TK_EOF, 0, p, strlen(p)));
}
