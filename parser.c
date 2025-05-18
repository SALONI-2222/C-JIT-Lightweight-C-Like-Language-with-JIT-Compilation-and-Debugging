#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "lexer.h"

static Token current_token;

void parser_init(FILE *src) {
    lexer_init(src);
    current_token = lexer_next_token();
}

static void advance() {
    current_token = lexer_next_token();
}

static void expect(TokenType type) {
    if (current_token.type != type) {
        fprintf(stderr, "Syntax error: expected %d but got %d\n", type, current_token.type);
        exit(EXIT_FAILURE);
    }
    advance();
}

ASTNode *new_node(ASTNodeType type) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        perror("new_node");
        exit(EXIT_FAILURE);
    }
    node->type = type;
    return node;
}

// Expression parsing precedence declarations
ASTNode *parse_or();
ASTNode *parse_and();
ASTNode *parse_equality();
ASTNode *parse_relational();
ASTNode *parse_additive();
ASTNode *parse_term();
ASTNode *parse_factor();

ASTList *parse_program(void);
ASTNode *parse_statement();
ASTNode *parse_block();
ASTNode *parse_expression();
ASTNode *parse_variable();
ASTNode *parse_declaration();
ASTNode *parse_assignment();
ASTNode *parse_if_statement();
ASTNode *parse_while_statement();
ASTNode *parse_for_statement();
ASTNode *parse_print_statement();
ASTNode *parse_return_statement();

ASTList *parse_program(void) {
    ASTList *head = NULL, *tail = NULL;
    while (current_token.type != TOKEN_EOF) {
        ASTNode *stmt = parse_statement();
        ASTList *node = malloc(sizeof(ASTList));
        node->stmt = stmt;
        node->next = NULL;
        if (tail) tail->next = node;
        else head = node;
        tail = node;
    }
    return head;
}

ASTNode *parse_statement() {
    if (current_token.type == TOKEN_INT) return parse_declaration();
    if (current_token.type == TOKEN_IDENTIFIER) return parse_assignment();
    if (current_token.type == TOKEN_IF) return parse_if_statement();
    if (current_token.type == TOKEN_WHILE) return parse_while_statement();
    if (current_token.type == TOKEN_FOR) return parse_for_statement();
    if (current_token.type == TOKEN_PRINT) return parse_print_statement();
    if (current_token.type == TOKEN_RETURN) return parse_return_statement();
    if (current_token.type == TOKEN_LBRACE) return parse_block();

    fprintf(stderr, "Unexpected token %d in statement\n", current_token.type);
    exit(EXIT_FAILURE);
}

ASTNode *parse_block() {
    expect(TOKEN_LBRACE);
    ASTList *stmts = NULL, *tail = NULL;
    while (current_token.type != TOKEN_RBRACE) {
        ASTNode *stmt = parse_statement();
        ASTList *node = malloc(sizeof(ASTList));
        node->stmt = stmt;
        node->next = NULL;
        if (tail) tail->next = node;
        else stmts = node;
        tail = node;
    }
    expect(TOKEN_RBRACE);
    ASTNode *block = new_node(AST_BLOCK);
    block->block.stmts = stmts;
    return block;
}

ASTNode *parse_variable() {
    if (current_token.type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Expected identifier\n" );
        exit(EXIT_FAILURE);
    }
    ASTNode *node = new_node(AST_VAR);
    node->var_name = strdup(current_token.text);
    advance();
    return node;
}

ASTNode *parse_expression() {
    return parse_or();
}

ASTNode *parse_or() {
    ASTNode *left = parse_and();
    while (current_token.type == TOKEN_OR) {
        int op = '|';
        advance();
        ASTNode *right = parse_and();
        ASTNode *node = new_node(AST_BINARY_OP);
        node->binop.op = op;
        node->binop.left = left;
        node->binop.right = right;
        left = node;
    }
    return left;
}

ASTNode *parse_and() {
    ASTNode *left = parse_equality();
    while (current_token.type == TOKEN_AND) {
        int op = '&';
        advance();
        ASTNode *right = parse_equality();
        ASTNode *node = new_node(AST_BINARY_OP);
        node->binop.op = op;
        node->binop.left = left;
        node->binop.right = right;
        left = node;
    }
    return left;
}

ASTNode *parse_equality() {
    ASTNode *left = parse_relational();
    while (current_token.type == TOKEN_EQ || current_token.type == TOKEN_NEQ) {
        int op = (current_token.type == TOKEN_EQ) ? '=' : '!';
        advance();
        ASTNode *right = parse_relational();
        ASTNode *node = new_node(AST_BINARY_OP);
        node->binop.op = op;
        node->binop.left = left;
        node->binop.right = right;
        left = node;
    }
    return left;
}

ASTNode *parse_relational() {
    ASTNode *left = parse_additive();
    while (current_token.type == TOKEN_LT || current_token.type == TOKEN_GT ||
           current_token.type == TOKEN_LE || current_token.type == TOKEN_GE) {
        int op;
        switch (current_token.type) {
            case TOKEN_LT: op = '<'; break;
            case TOKEN_GT: op = '>'; break;
            case TOKEN_LE: op = 'l'; break;
            case TOKEN_GE: op = 'g'; break;
            default:
                fprintf(stderr, "Unexpected token %d in relational operator\n", current_token.type);
                exit(EXIT_FAILURE);
        }
        advance();
        ASTNode *right = parse_additive();
        ASTNode *node = new_node(AST_BINARY_OP);
        node->binop.op = op;
        node->binop.left = left;
        node->binop.right = right;
        left = node;
    }
    return left;
}

ASTNode *parse_additive() {
    ASTNode *left = parse_term();
    while (current_token.type == TOKEN_PLUS || current_token.type == TOKEN_MINUS) {
        int op = (current_token.type == TOKEN_PLUS) ? '+' : '-';
        advance();
        ASTNode *right = parse_term();
        ASTNode *node = new_node(AST_BINARY_OP);
        node->binop.op = op;
        node->binop.left = left;
        node->binop.right = right;
        left = node;
    }
    return left;
}

ASTNode *parse_term() {
    ASTNode *left = parse_factor();
    while (current_token.type == TOKEN_STAR || current_token.type == TOKEN_SLASH || current_token.type == TOKEN_PERCENT) {
        int op;
        switch (current_token.type) {
            case TOKEN_STAR: op = '*'; break;
            case TOKEN_SLASH: op = '/'; break;
            case TOKEN_PERCENT: op = '%'; break;
            default:
                fprintf(stderr, "Unexpected token %d in term operator\n", current_token.type);
                exit(EXIT_FAILURE);
        }
        advance();
        ASTNode *right = parse_factor();
        ASTNode *node = new_node(AST_BINARY_OP);
        node->binop.op = op;
        node->binop.left = left;
        node->binop.right = right;
        left = node;
    }
    return left;
}



ASTNode *parse_factor() {
    if (current_token.type == TOKEN_MINUS || current_token.type == TOKEN_LOG_NOT || current_token.type == TOKEN_BIT_NOT) {
        int op;
        switch (current_token.type) {
            case TOKEN_MINUS: op = '-'; break;
            case TOKEN_LOG_NOT: op = '!'; break;
            case TOKEN_BIT_NOT: op = '~'; break;
            default:
                fprintf(stderr, "Unexpected token %d in unary operator\n", current_token.type);
                exit(EXIT_FAILURE);
        }
        advance();
        ASTNode *operand = parse_factor();
        ASTNode *node = new_node(AST_UNARY_OP);
        node->unop.op = op;
        node->unop.operand = operand;
        return node;
    } else if (current_token.type == TOKEN_NUMBER) {
        ASTNode *node = new_node(AST_NUMBER);
        node->number = current_token.value;
        advance();
        return node;
    } else if (current_token.type == TOKEN_IDENTIFIER) {
        return parse_variable();
    } else if (current_token.type == TOKEN_LPAREN) {
        advance();
        ASTNode *node = parse_expression();
        expect(TOKEN_RPAREN);
        return node;
    } else {
        fprintf(stderr, "Unexpected token in factor: %d\n", current_token.type);
        exit(EXIT_FAILURE);
    }
}



ASTNode *parse_assignment() {
    ASTNode *lhs = parse_variable();
    expect(TOKEN_ASSIGN);
    ASTNode *rhs = parse_expression();
    expect(TOKEN_SEMICOLON);
    ASTNode *node = new_node(AST_ASSIGN);
    node->assign.lhs = lhs;
    node->assign.rhs = rhs;
    return node;
}

ASTNode *parse_declaration() {
    expect(TOKEN_INT);
    if (current_token.type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Expected identifier in declaration\n" );
        exit(EXIT_FAILURE);
    }
    char *name = strdup(current_token.text);
    advance();
    ASTNode *init = NULL;
    if (current_token.type == TOKEN_ASSIGN) {
        advance();
        init = parse_expression();
    }
    expect(TOKEN_SEMICOLON);
    ASTNode *decl = new_node(AST_DECL);
    decl->decl.var_name = name;
    decl->decl.init = init;
    return decl;
}

ASTNode *parse_if_statement() {
    expect(TOKEN_IF);
    expect(TOKEN_LPAREN);
    ASTNode *cond = parse_expression();
    expect(TOKEN_RPAREN);
    ASTNode *then_stmt = parse_statement();
    ASTNode *stmt = new_node(AST_IF);
    stmt->if_stmt.condition = cond;
    stmt->if_stmt.then_stmt = then_stmt;
    stmt->if_stmt.else_branch = NULL;
    if (current_token.type == TOKEN_ELSE) {
        advance();
        stmt->if_stmt.else_branch = parse_statement();
    }
    return stmt;
}

ASTNode *parse_while_statement() {
    expect(TOKEN_WHILE);
    expect(TOKEN_LPAREN);
    ASTNode *cond = parse_expression();
    expect(TOKEN_RPAREN);
    ASTNode *body = parse_statement();
    ASTNode *stmt = new_node(AST_WHILE);
    stmt->while_stmt.condition = cond;
    stmt->while_stmt.do_stmt = body;
    return stmt;
}

ASTNode *parse_for_statement() {
    expect(TOKEN_FOR);
    expect(TOKEN_LPAREN);
    ASTNode *init = NULL;
    ASTNode *cond = NULL;
    ASTNode *update = NULL;

    if (current_token.type == TOKEN_INT) {
        init = parse_declaration();
    } else if (current_token.type != TOKEN_SEMICOLON) {
        init = parse_assignment();
    } else {
        advance();
    }

    if (current_token.type != TOKEN_SEMICOLON) {
        cond = parse_expression();
    }
    expect(TOKEN_SEMICOLON);

    if (current_token.type != TOKEN_RPAREN) {
        update = parse_assignment();
    }
    expect(TOKEN_RPAREN);

    ASTNode *body = parse_statement();
    ASTNode *stmt = new_node(AST_FOR);
    stmt->for_stmt.init = init;
    stmt->for_stmt.condition = cond;
    stmt->for_stmt.update = update;
    stmt->for_stmt.body = body;
    return stmt;
}

ASTNode *parse_print_statement() {
    expect(TOKEN_PRINT);
    ASTNode *expr = parse_expression();
    expect(TOKEN_SEMICOLON);
    ASTNode *stmt = new_node(AST_PRINT);
    stmt->print_stmt.expr = expr;
    return stmt;
}

ASTNode *parse_return_statement() {
    expect(TOKEN_RETURN);
    ASTNode *expr = parse_expression();
    expect(TOKEN_SEMICOLON);
    ASTNode *stmt = new_node(AST_RETURN);
    stmt->expr = expr;
    return stmt;
}

void ast_list_free(ASTList *list) {
    while (list) {
        ASTList *next = list->next;
        ast_free(list->stmt);
        free(list);
        list = next;
    }
}

void ast_free(ASTNode *node) {
    if (!node) return;
    switch (node->type) {
        case AST_VAR: free(node->var_name); break;
        case AST_NUMBER: break;
        case AST_ASSIGN:
            ast_free(node->assign.lhs);
            ast_free(node->assign.rhs);
            break;
        case AST_DECL:
            free(node->decl.var_name);
            ast_free(node->decl.init);
            break;
        case AST_BINARY_OP:
            ast_free(node->binop.left);
            ast_free(node->binop.right);
            break;
        case AST_UNARY_OP:
            ast_free(node->unop.operand);
            break;
        case AST_PRINT:
            ast_free(node->print_stmt.expr);
            break;
        case AST_RETURN:
            ast_free(node->expr);
            break;
        case AST_IF:
            ast_free(node->if_stmt.condition);
            ast_free(node->if_stmt.then_stmt);
            ast_free(node->if_stmt.else_branch);
            break;
        case AST_WHILE:
            ast_free(node->while_stmt.condition);
            ast_free(node->while_stmt.do_stmt);
            break;
        case AST_FOR:
            ast_free(node->for_stmt.init);
            ast_free(node->for_stmt.condition);
            ast_free(node->for_stmt.update);
            ast_free(node->for_stmt.body);
            break;
        case AST_BLOCK:
            ast_list_free(node->block.stmts);
            break;
        default:
            break;
    }
    free(node);
}
