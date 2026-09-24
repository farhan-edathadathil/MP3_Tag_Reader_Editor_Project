#ifndef TAG_H
#define TAG_H

#include"file.h"
typedef struct 
{
    char tag[5];
    int frame_size;
    unsigned int frame_pos;
}Tag;

typedef struct 
{
    Tag tags_array[7];
    int tag_count;
}Tags_info;

void initialise_tag_filter(Tags_info *tags_info);
void display_tags(File_data *file_data,Tags_info *tags_info);
void find_tag(File_data *file_data,Tags_info *tag_info);
void print_tag(File_data *file_data,Tag *tag);
void edit_tag();

#endif