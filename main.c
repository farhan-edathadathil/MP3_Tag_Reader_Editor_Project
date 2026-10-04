#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"tag.h"
#include"file.h"

int main(int argc,char **argv)
{
    Tags_info tags_info;
    File_data file_data;
    if(argc==1)
    {
        printf("No enough cammand line arguments are given !!\n\n");
        printf("Usage :-\n");
        printf("./a.out -h  ->for showing help menu\n");
        printf("./a.out -v filename.mp3  ->for showing file tags data\n");
        printf("./a.out -e filename.mp3 -(alphabet reprecenting each tag from help menu (eg:- t,A...)) \"new content\"  ->for editing tag content\n");
        exit(0);
    }
    if(strcmp("-h",argv[1])==0)
    {
        print_help();
    }
    else if(strcmp("-v",argv[1])==0)
    {
        if(argc<3)
        {
            printf("\nFilename argument is not given !!\n\n");
            exit(0);
        }
        if(pre_open_check(argv[2])==0)
        {
            printf("\nUnsupported file extension or invalid file name !!\n\n");
            exit(0);
        }
        if(open_file(&file_data,argv[2])==0)
            exit(0);
        if(post_open_check(&file_data)==0)
        {
            exit(0);
        }
        initialise_tag_filter(&tags_info);
        find_tags(&file_data, &tags_info);
        display_tags(&file_data,&tags_info,argv[2]);
        fclose(file_data.fptr);
    }
    else if(strcmp("-e",argv[1])==0)
    {
        if(argc<5)
        {
            printf("\nEnough cammand line argument is not given !!\n\n");
            exit(0);
        }
        if(argc>5)
        {
            printf("\nWarnning :- if the new content that given for edit contain space then give inside \"new content\" !!\n\n");
        }
        if(pre_open_check(argv[2])==0)
        {
            printf("\nUnsupported file extension or invalid file name !!\n\n");
            exit(0);
        }
        if(open_file(&file_data,argv[2])==0)
            exit(0);
        if(post_open_check(&file_data)==0)
        {
            exit(0);
        }
        initialise_tag_filter(&tags_info);
        if(edit_option(&file_data,&tags_info,argv)==0)
        {
            printf("\nediting failed !!\n\n");                               
        }
        fclose(file_data.fptr);
    }
    else
    {
        printf("\nInvalid input !!\n");
        printf("Usage :-\n");
        printf("./a.out -h  ->for showing help menu\n");
        printf("./a.out -v filename.mp3  ->for showing file tags data\n");
        printf("./a.out -e filename.mp3 -(alphabet reprecenting each tag from help menu (eg:- t,A...)) \"new content\"  ->for editing tag content\n");
        exit(0);
    }

}

