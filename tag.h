#ifndef TAG_H
#define TAG_H

typedef struct 
{
    char tag[5];
    int frame_size;
    char *frame_addr;
}Tag;

typedef struct 
{
    Tag tags_array[7];
    int tag_count;
}Tags_info;

void tag_filter(Tags_info *tags_info);
void display_tags(Tags_info *tags_info);
void find_tag();
void print_tag();

#endif