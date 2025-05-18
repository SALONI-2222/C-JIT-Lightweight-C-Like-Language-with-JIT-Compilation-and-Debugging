#ifndef IR_H
#define IR_H

struct ASTNode;
struct ASTList;

typedef enum {
    IR_LOAD_CONST,
    IR_LOAD_VAR,
    IR_STORE_VAR,
    IR_ADD,
    IR_SUB,
    IR_MUL,
    IR_DIV,
    IR_MOD, 
    IR_NEG,       // -a
    IR_LOG_NOT,   // !a
    IR_BIT_NOT,   // ~a
    IR_EQ,
    IR_NEQ,
    IR_LT,
    IR_GT,
    IR_LE,
    IR_GE,
    IR_AND,
    IR_OR,
    IR_LABEL,
    IR_JUMP,
    IR_JUMP_IF_FALSE,
    IR_RETURN
} IROp;

typedef struct IRInst {
    IROp op;
    int dest;
    int src1;
    int src2;
    int value;
    char *var_name;
    char *label;
    struct IRInst *next;
} IRInst;

typedef struct {
    IRInst *head;
    IRInst *tail;
    int temp_count;
    int label_count;
} IRList;

void ir_list_init(IRList *list);
int ir_emit_const(IRList *list, int value);
int ir_emit_binop(IRList *list, IROp op, int left, int right);
int ir_emit_assign(IRList *list, const char *var_name, int src);
int ir_emit_load_var(IRList *list, const char *var_name);
void ir_emit_label(IRList *list, const char *label);
void ir_emit_jump(IRList *list, const char *label);
void ir_emit_jump_if_false(IRList *list, int cond, const char *label);
void ir_emit_return(IRList *list, int value);

int ir_generate_expr(IRList *list, struct ASTNode *node);
void ir_generate(IRList *list, struct ASTNode *node);
void ir_generate_program(IRList *list, struct ASTList *program);

void ir_print(IRList *list);
void ir_free(IRList *list);

#endif
