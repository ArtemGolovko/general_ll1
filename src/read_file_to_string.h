#pragma once


typedef struct {
    char *buffer;
    size_t length;
    int error;
} ReadInfo;


ReadInfo read_file_to_string(const char *filename);
