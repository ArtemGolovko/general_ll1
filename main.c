#include <stdio.h>
#include "gram_parser.h"

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
    printf("parsing success");

    free_AST(ast_root);

    fclose(file);
    return 0;
}
