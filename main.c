#include <stdio.h>
#include "gram_parser.h"
#include "analyzer.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        perror("Too few arguments. Please provide your Gram file.\n");
        return 1;
    }

    char* filename = argv[1];
    FILE* file;
    int error = fopen_s(&file, filename, "r");

    if (error != 0) {
        fprintf(stderr, "Failed to open file '%s', error code %d.\n", filename, error);
        return 1;
    }

    printf("Parsing...\n");
    ast_rules_t *ast_root = parse(file);
    printf("parsing success\n\n");

    printf("Analyzing...\n");
    analysis_result_t analysis_result = analyze(ast_root);
    printf("analysis success\n\n");

    free_AST(ast_root);
    free_symbolic_table(&analysis_result.symbolic_table);

    fclose(file);
    return 0;
}
