#include <stdio.h>
#include <excpt.h>
#include "gram_parser.h"
#include "read_file_to_string.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        perror("Too few arguments. Please provide your Gram file.\n");
        return 1;
    }

    char* filename = argv[1];

    ReadInfo info = read_file_to_string(filename); 

    printf("Size: %d\nContent:\n%.*s\n", info.length, info.length, info.buffer);

    FILE* file;
    int error = fopen_s(&file, filename, "r");

    if (error != 0) {
        fprintf(stderr, "Failed to open file '%s', error code %d.\n", filename, error);
        return 1;
    }
    __try { 
        printf("Parsing...\n");

        bool accepted = parse(filename, file);

        if (accepted) {
            printf("parsing success\n");
        } else {
            printf("parsing failed\n");
        }
    }  __except(-1) {
        perror("Segmentaion fault\n");
    }

    fclose(file);

    return 0;
}
