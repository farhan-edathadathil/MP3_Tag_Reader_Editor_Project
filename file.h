#ifndef FILE_H
#define FILE_H

#include <stdio.h>

typedef struct
{
    FILE *fptr;
    long unsigned int file_size;
}File_data;
#endif