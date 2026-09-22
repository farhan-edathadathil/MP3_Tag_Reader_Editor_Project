#include<stdio.h>
#include<string.h>
#include"tag.h"

void initialise_tag_filter(Tags_info *tags_info)
{
    strcpy(tags_info->tags_array[0].tag, "TIT2");
    strcpy(tags_info->tags_array[1].tag, "TPE1");
    strcpy(tags_info->tags_array[2].tag, "TALB");
    strcpy(tags_info->tags_array[3].tag, "TYER");
    strcpy(tags_info->tags_array[4].tag, "TCON");
    strcpy(tags_info->tags_array[5].tag, "COMM");
    strcpy(tags_info->tags_array[6].tag, "TLEN");

    for(int i=0;i<7;i++)
    {
        tags_info->tags_array[i].frame_size=0;
        tags_info->tags_array[i].frame_addr=NULL;
    }
    tags_info->tag_count=7;
}

void display_tags(Tags_info *tags_info)
{
    //display every tags
    
}

void find_tag()
{
    //find and save each tag address in struct 

}

void print_tag()
{
    //print each tag charcter by character by reading from file
}

void edit_tag()
{
    
}