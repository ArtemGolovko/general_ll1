#include "error.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

void SyntaxError_display(SyntaxError *error) {

    if (error->token_length == 0) {
        printf("%s:%zd:%zd\n    ", error->loc.filename, error->loc.row, error->loc.col);

        if (error->origin == LexerError) {
            printf("Lexer");
        } else {
            printf("Parser");
        }

        printf(" Error: %s\n", error->message);
        return;
    }

    // Get the line from source code where error happend
    int line_start = error->loc.pos - (error->loc.col - 1);
    const char *line = error->source + line_start;

    char *line_end = strchr(line, '\n');
    int line_length = line_end - line;

    // Calculte padding size to displaying row number

    int padding_size = floor(log10(error->loc.row)) + 1;

    // Normalize token length
    
    int token_length = error->token_length;
    int max_token_length = line_length - (error->loc.col - 1);

    if (max_token_length < token_length) {
        token_length = max_token_length; 
    }

    // Display everything
    //
    // Format:
    // [filename]:[row]:[col]
    //       |
    // [row] | [Line of code ...]
    //       |          ^^^^ [Lexer|Parser] Error:[error message ...]

    printf("%s:%zd:%zd\n", error->loc.filename, error->loc.row, error->loc.col);
    printf("%*s |\n", padding_size, "");
    printf("%zd | %.*s\n", error->loc.row, line_length, line);
    printf("%*s | %*s", padding_size, "", error->loc.col - 1, "");

    for (int i = 0; i < token_length; i += 1) {
        printf("^");
    }

    if (error->origin == LexerError) {
        printf(" Lexer");
    } else {
        printf(" Parser");
    }

    printf(" Error: %s\n", error->message);
}


void free_SyntaxError(SyntaxError *error) {
    free(error->message);
}
