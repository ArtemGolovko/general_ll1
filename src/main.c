#include <stdio.h>
#include <stdlib.h>
#include <excpt.h>
#include "gram_parser.h"
#include "gram_parser/parser.h"
#include "read_file_to_string.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        perror("Too few arguments. Please provide your Gram file.\n");
        return 1;
    }

    char* filename = argv[1];

    ReadInfo info = read_file_to_string(filename); 
    
    if (info.error != 0) {
        free(info.buffer);
        fprintf(stderr, "Failed to open file '%s', error code %d.\n", filename, info.error);

        return 1;
    }

    printf("Size: %d\nContent:\n%.*s\n", info.length, info.length, info.buffer);

    __try { 
        printf("Parsing...\n");

        test_lexer(filename, info.buffer, info.length);

        // bool accepted = parse(filename, info.buffer, info.length);
        //
        // if (accepted) {
        //     printf("parsing success\n");
        // } else {
        //     printf("parsing failed\n");
        // }
    }  __except(-1) {
        perror("Segmentaion fault\n");
    }

    free(info.buffer);

    return 0;
}
