#include<stdio.h>
#include"tag.h"
#include"file.h"


int main()
{
    Tags_info tags_info;
    File_data file_data;
    char choice,ch;
    if(pre_open_check(FILE_NAME)==0)
    {
        printf("\nInvalid file extension !!\n\n");
        return 0;
    }
    if(post_open_check(&file_data)==0)
    {
        printf("\nInvalid file header or Currepted file !!\n\n");
        return 0;
    }
    initialise_tag_filter(&tags_info);
    //find_tag(&file_data, &tags_info);
    printf("\n***** MP3_TAG_READER & EDITOR *****\n");
    printf("--------------------------------------\n\n");
    //display_tags(&file_data,&tags_info);
    do
    {
        printf("\n***** MAIN MENU *****\n\n");
        printf("1. (v) VIEW TAG\n");
        printf("2. (e) EDIT TAG\n");
        printf("3. (h) HELP\n");
        printf("4. (c) EXIT\n");
        printf("\nENTER THE CHOICE : ");
        scanf("%c",&choice);
        ch = getchar();
        if (ch != '\n' && ch != EOF)
        {
            printf("\nInvalid input !!\n\n");

            while ((ch = getchar()) != '\n' && ch != EOF);
            continue;
        }

        switch(choice)
        {
        case 'v':
            find_tag(&file_data, &tags_info);
            display_tags(&file_data,&tags_info);
            break;
        
        case 'e':
            //edit_tag(&tags_info);
            break;

        case 'h':
            print_help();
            break;

        case 'c':
            break;

        default:
            printf("\nInvalid input TRY AGAIN !!\n\n");
            break;
        }
    } while (choice!='c');
    

}