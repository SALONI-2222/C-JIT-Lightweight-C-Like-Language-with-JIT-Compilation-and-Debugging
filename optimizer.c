#include <stdio.h>
#include "optimizer.h"

void ir_optimize(IRList *list) {
    IRInst *curr = list->head;

    printf("[Optimizer] Starting optimization pass...\n");

    while (curr != NULL) {
        // Skip control flow instructions
        if (curr->op == IR_LABEL || curr->op == IR_JUMP || curr->op == IR_JUMP_IF_FALSE || curr->op == IR_RETURN) {
            curr = curr->next;
            continue;
        }

        if (curr->op == IR_ADD || curr->op == IR_SUB || curr->op == IR_MUL || curr->op == IR_DIV ||
            curr->op == IR_EQ  || curr->op == IR_NEQ || curr->op == IR_LT  || curr->op == IR_GT ||
            curr->op == IR_LE  || curr->op == IR_GE  || curr->op == IR_AND || curr->op == IR_OR) {

            IRInst *src1 = NULL, *src2 = NULL;
            IRInst *scan = list->head;

            while (scan != NULL) {
                if (scan->dest == curr->src1) src1 = scan;
                if (scan->dest == curr->src2) src2 = scan;
                scan = scan->next;
            }

            if (src1 && src2 && src1->op == IR_LOAD_CONST && src2->op == IR_LOAD_CONST) {
                int result = 0;
                switch (curr->op) {
                    case IR_ADD: result = src1->value + src2->value; break;
                    case IR_SUB: result = src1->value - src2->value; break;
                    case IR_MUL: result = src1->value * src2->value; break;
                    case IR_DIV: result = src1->value / src2->value; break;
                    case IR_EQ:  result = (src1->value == src2->value); break;
                    case IR_NEQ: result = (src1->value != src2->value); break;
                    case IR_LT:  result = (src1->value < src2->value); break;
                    case IR_GT:  result = (src1->value > src2->value); break;
                    case IR_LE:  result = (src1->value <= src2->value); break;
                    case IR_GE:  result = (src1->value >= src2->value); break;
                    case IR_MOD: result = src1->value % src2->value; break;
                    case IR_AND: result = (src1->value && src2->value); break;
                    case IR_OR:  result = (src1->value || src2->value); break;
                    default: break;
                }

                if (src1 && src1->op == IR_LOAD_CONST) {
                    int result;
                    switch (curr->op) {
                        case IR_NEG: result = -src1->value; break;
                        case IR_LOG_NOT: result = !src1->value; break;
                        case IR_BIT_NOT: result = ~src1->value; break;
                        default: continue;
                    }

                    curr->op = IR_LOAD_CONST;
                    curr->value = result;
                    curr->src1 = curr->src2 = -1;
                }

                printf("[Optimizer] Folding: t%d = %d (was t%d op t%d)\n", curr->dest, result, curr->src1, curr->src2);

                curr->op = IR_LOAD_CONST;
                curr->value = result;
                curr->src1 = curr->src2 = -1;
            }
        }
        curr = curr->next;
    }

    printf("[Optimizer] Optimization complete.\n");
}
