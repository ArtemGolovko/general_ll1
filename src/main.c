#include <stdio.h>
#include <stdlib.h>
#include <excpt.h>
#include <stddef.h>

#include "gram_parser/ast.h"
#include "gram_parser/error.h"
#include "gram_parser/parser.h"

#include "read_file_to_string.h"
#include "datastructs/vector.h"

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

    printf("Size: %zd\nContent:\n%.*s\n", info.length, info.length, info.buffer);

    __try { 
        printf("Parsing...\n");
        
        ParsingResult result = parse(filename, info.buffer, info.length);
        size_t number_of_errors = vector_len(result.errors);

        if (number_of_errors == 0) {
            printf("parsing success\n");
        } else {
            printf("parsing failed\nNumber of errors: %zd\n", number_of_errors);
            
            for (size_t i = 0; i < number_of_errors; i += 1) {
                SyntaxError_display(&result.errors[i]);
                free_SyntaxError(&result.errors[i]);
            }
        }
        
        free_ast((ASTNode *)result.ast);
        free_vector(result.errors);
    }  __except(-1) {
        perror("Segmentaion fault\n");
    }

    free(info.buffer);

    return 0;
}

// (*((ASTNode **)((char *)rules + 8)))->type
