#include<stdio.h>
#include"tag.h"
#include"file.h"


int main()
{
    Tags_info tags_info;
    File_data file_data;
    int choice;
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
    printf("\n***** MP3_TAG_READER & EDITOR *****\n");
    printf("--------------------------------------\n\n");
    display_tags(&tags_info);
    do
    {
        printf("\n***** MAIN MENU *****\n\n");
        printf("1. READ & DISPLAY TAG\n");
        printf("2. EDIT TAG\n");
        printf("3. HELP");
        printf("4. EXIT\n");
        printf("\nENTER THE CHOICE : ");
        scanf("%d",&choice);

        switch(choice)
        {
        case 1:
            display_tags(&tags_info);
            break;
        
        case 2:
            edit_tag(&tags_info);
            break;

        case 3:
            break;

        default:
            printf("\nInvalid input TRY AGAIN !!\n\n");
            break;
        }
    } while (choice!=4);
    

}