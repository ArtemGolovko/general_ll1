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

    bool accepted = parse(filename, file);

    if (accepted) {
        printf("parsing success\n");
    } else {
        printf("parsing failed\n");
    }


    fclose(file);

    return 0;
}
