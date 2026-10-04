#ifndef FILE_H
#define FILE_H

#include <stdio.h>


typedef struct
{
    FILE *fptr;
    long unsigned int file_size;
    unsigned int header_size;
}File_data;

int pre_open_check(char *file_name);
int post_open_check(File_data *file_data);
int open_file(File_data *file_data,char *file_name);

#endif