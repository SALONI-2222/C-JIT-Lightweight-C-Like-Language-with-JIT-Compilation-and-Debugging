
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

static FILE *input;

void lexer_init(FILE *source) {
    input = source;
}

static void skip_whitespace() {
    int c;
    while ((c = fgetc(input)) != EOF && isspace(c)) {}
    if (c != EOF) ungetc(c, input);
}

Token lexer_next_token() {
    skip_whitespace();
    Token tok = {TOKEN_UNKNOWN, 0, NULL};
    int c = fgetc(input);
    if (c == EOF) {
        tok.type = TOKEN_EOF;
        return tok;
    }

    if (isdigit(c)) {
        int val = c - '0';
        while (isdigit(c = fgetc(input))) val = val * 10 + (c - '0');
        if (c != EOF) ungetc(c, input);
        tok.type = TOKEN_NUMBER;
        tok.value = val;
        return tok;
    }

    if (isalpha(c) || c == '_') {
        char buf[256]; int len = 0;
        buf[len++] = c;
        while (isalnum(c = fgetc(input)) || c == '_') buf[len++] = c;
        if (c != EOF) ungetc(c, input);
        buf[len] = '\0';
        if (strcmp(buf, "int") == 0) tok.type = TOKEN_INT;
        else if (strcmp(buf, "if") == 0) tok.type = TOKEN_IF;
        else if (strcmp(buf, "else") == 0) tok.type = TOKEN_ELSE;
        else if (strcmp(buf, "while") == 0) tok.type = TOKEN_WHILE;
        else if (strcmp(buf, "for") == 0) tok.type = TOKEN_FOR;
        else if (strcmp(buf, "print") == 0) tok.type = TOKEN_PRINT;
        else if (strcmp(buf, "return") == 0) tok.type = TOKEN_RETURN;
        else { tok.type = TOKEN_IDENTIFIER; tok.text = strdup(buf); }
        return tok;
    }

    int next;
    switch (c) {
        case '+': tok.type = TOKEN_PLUS; break;
        case '-': tok.type = TOKEN_MINUS; break;
        case '*': tok.type = TOKEN_STAR; break;
        case '/': tok.type = TOKEN_SLASH; break;
        case '~': tok.type = TOKEN_BIT_NOT; break;
        case '%': tok.type = TOKEN_PERCENT; break;
        case '=': next = fgetc(input);
                  tok.type = (next == '=') ? TOKEN_EQ : TOKEN_ASSIGN;
                  if (next != '=') {
                      ungetc(next, input);
                  }
                  break;
        case '!':
            next = fgetc(input);
            if (next == '=') {
                tok.type = TOKEN_NEQ;
            } else {
                ungetc(next, input);
                tok.type = TOKEN_LOG_NOT;
            }
            break;
        case '<': next = fgetc(input);
                  tok.type = (next == '=') ? TOKEN_LE : TOKEN_LT;
                  if (next != '=') {
                      ungetc(next, input);
                  }
                  break;
        case '>': next = fgetc(input);
                  tok.type = (next == '=') ? TOKEN_GE : TOKEN_GT;
                  if (next != '=') {
                      ungetc(next, input);
                  }
                  break;
        case '(': tok.type = TOKEN_LPAREN; break;
        case ')': tok.type = TOKEN_RPAREN; break;
        case '{': tok.type = TOKEN_LBRACE; break;
        case '}': tok.type = TOKEN_RBRACE; break;
        case ';': tok.type = TOKEN_SEMICOLON; break;
        case '&':
            next = fgetc(input);
            tok.type = (next == '&') ? TOKEN_AND : TOKEN_UNKNOWN;
            if (next != '&') ungetc(next, input);
            break;

        case '|':
            next = fgetc(input);
            tok.type = (next == '|') ? TOKEN_OR : TOKEN_UNKNOWN;
            if (next != '|') ungetc(next, input);
            break;

        default:
    fprintf(stderr, "Unknown character: '%c' (%d)\n", c, c);
    tok.type = TOKEN_UNKNOWN;
    break;
    }

    return tok;
}
