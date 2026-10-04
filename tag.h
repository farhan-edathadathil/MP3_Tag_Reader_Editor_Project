#ifndef TAG_H
#define TAG_H

#include"file.h"
typedef struct 
{
    char tag[5];
    unsigned int frame_size;
    unsigned int frame_pos;
}Tag;

typedef struct 
{
    Tag tags_array[6];
    int tag_count;
}Tags_info;

void initialise_tag_filter(Tags_info *tags_info);
void display_tags(File_data *file_data,Tags_info *tags_info,char *file_name);
void find_tags(File_data *file_data,Tags_info *tag_info);
void print_tag(File_data *file_data,Tag *tag);
void print_tag01(File_data *file_data,Tag *tag);
int edit_option(File_data *file_data,Tags_info *tags_info,char **argv);
int find_a_tag(File_data *file_data,Tag *tag);
void edit_tag(File_data *file_data,Tag *tag,char *new_content,char *file_name);
void print_help();
int valid_year(char *year);
int valid_COMM(char *comm);
int valid_input(char *input);

#endif