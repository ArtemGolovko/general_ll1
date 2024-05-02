#include <stdio.h>
#include "gram_parser.c"

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
    printf("Parsing...");
    parse(file);
    fclose(file);
    return 0;
}
