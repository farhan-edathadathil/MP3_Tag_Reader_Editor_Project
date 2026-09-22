#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#define FILE_NAME "sample.mp3"

typedef struct
{
    FILE *fptr;
    long unsigned int file_size;
    FILE *fedit;
}File_data;

int pre_open_check(char *file_name);
int post_open_check(File_data *file_data);
void file_close(File_data *file_data);

#endif