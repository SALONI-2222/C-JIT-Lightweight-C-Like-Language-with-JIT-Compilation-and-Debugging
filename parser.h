#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>

typedef enum {
    AST_NUMBER,
    AST_FOR,
    AST_BINARY_OP,
    AST_VAR,
    AST_ASSIGN,
    AST_EXPR_STMT,
    AST_DECL,
    AST_UNARY_OP,  // ✅ Add this line
    AST_BLOCK,
    AST_IF,
    AST_WHILE,
    AST_PRINT,
    AST_RETURN
} ASTNodeType;

typedef struct ASTNode ASTNode;

typedef struct ASTList {
    ASTNode *stmt;
    struct ASTList *next;
} ASTList;

typedef struct ASTNode {
    ASTNodeType type;
    union {
        int number;
        struct {
            int op;
            struct ASTNode *left;
            struct ASTNode *right;
        } binop;
        char *var_name;
        struct {
            struct ASTNode *lhs;
            struct ASTNode *rhs;
        } assign;
        struct {
            char *var_name;
            struct ASTNode *init;
        } decl;
        struct {
            struct ASTNode *expr;
        } print_stmt;
        struct {
            struct ASTList *stmts;
        } block;
        struct {
            struct ASTNode *condition;
            struct ASTNode *then_stmt;
            struct ASTNode *else_branch;
        } if_stmt;
        struct {
            struct ASTNode *condition;
            struct ASTNode *do_stmt;
        } while_stmt;
        struct {
            struct ASTNode *init;
            struct ASTNode *condition;
            struct ASTNode *update;
            struct ASTNode *body;
        } for_stmt;
        struct {
            int op;
            struct ASTNode *operand;
        } unop;  // ✅ Add this inside the ASTNode union


        struct ASTNode *expr;
        struct ASTNode *return_expr;
    };
} ASTNode;

// Parser functions
void parser_init(FILE *src);
ASTList *parse_program(void);
ASTNode *parse_variable(void);
void ast_free(ASTNode *node);
void ast_list_free(ASTList *list);
ASTNode *new_node(ASTNodeType type);

#endif
