#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "ir.h"
#include "optimizer.h" // Include this only if optimizer is available

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <source_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *source = fopen(argv[1], "r");
    if (!source) {
        perror("Failed to open source file");
        return EXIT_FAILURE;
    }

    parser_init(source);

    ASTList *program = parse_program();
    if (!program) {
        fprintf(stderr, "Parsing failed\n");
        fclose(source);
        return EXIT_FAILURE;
    }

    int count = 0;
    for (ASTList *p = program; p; p = p->next) {
        count++;
    }
    printf("Parsed %d statements\n", count);

    // Initialize and generate IR
    IRList ir;
    ir_list_init(&ir);
    ir_generate_program(&ir, program);

    // Optional optimization
    ir_optimize(&ir);  // Comment this line out if you're not using optimizer.c

    // Print IR
    ir_print(&ir);

    // Cleanup
    ir_free(&ir);
    ast_list_free(program);
    fclose(source);
    return EXIT_SUCCESS;
}
