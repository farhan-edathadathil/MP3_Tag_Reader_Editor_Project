#include<stdio.h>
#include<string.h>
#include"tag.h"
#include"file.h"

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
        tags_info->tags_array[i].frame_pos=0;
    }
    tags_info->tag_count=0;
}

void display_tags(File_data *file_data,Tags_info *tags_info)
{
    //display every tags
    printf("\n%s -[%lf MB]\n",FILE_NAME,file_data->file_size/(double)1000000);
    printf("-----------------------------------------------------------------------\n");
    printf("Time : ");
    print_tag(file_data,(tags_info->tags_array)+6);
    printf("%56s","Year : ");
    print_tag(file_data,(tags_info->tags_array)+3);
    printf("\n");
    printf("-----------------------------------------------------------------------\n");
    printf("File Format :- ID3 v2.3\n\n");
    printf("%-11s : ", "Title");
    print_tag(file_data,(tags_info->tags_array)+0);
    printf("\n");

    printf("%-11s : ","Artist");
    print_tag(file_data,(tags_info->tags_array)+1);
    printf("\n");

    printf("%-11s : ","Album");
    print_tag(file_data,(tags_info->tags_array)+2);
    printf("\n");

    printf("%-11s : ","Genre");
    print_tag(file_data,(tags_info->tags_array)+4);
    printf("\n");

    printf("%-11s :- \n","Comment");
    print_tag(file_data,(tags_info->tags_array)+5);
    printf("\n");
    printf("-----------------------------------------------------------------------\n");
 
}

void find_tag(File_data *file_data,Tags_info *tag_info)
{
    //find and save each tag address in struct 
    fseek(file_data->fedit,10,SEEK_SET);
    char temp[5];
    int flag=0; 
    while(ftell(file_data->fedit) <(file_data->header_size + 10))
    {
        if (fread(temp, 1, 4, file_data->fedit) != 4)
        {
            printf("reading fail during tag finding");
            break;
        }
        temp[4]='\0';
        if (temp[0] == 0 && temp[1] == 0 && temp[2] == 0 && temp[3] == 0)
            break;
        flag=0;
        for(int i=0;i<7;i++)
        {
            if(strcmp(temp,tag_info->tags_array[i].tag)==0)
            {
                fread(temp,1,4,file_data->fedit);
                tag_info->tags_array[i].frame_size=((((unsigned int) temp[0])<<24) | (((unsigned int) temp[1])<<16) | (((unsigned int) temp[2])<<8) | ((unsigned int) temp[3]));
                fseek(file_data->fedit,2,SEEK_CUR);
                tag_info->tags_array[i].frame_pos=ftell(file_data->fedit);
                fseek(file_data->fedit,tag_info->tags_array[i].frame_size,SEEK_CUR);
                tag_info->tag_count++;
                flag=1;
                break;
            }
        }
        if(flag==0)
        {
            long unsigned int size;
            fread(temp,1,4,file_data->fedit);
            size=((((unsigned int) temp[0])<<24) | (((unsigned int) temp[1])<<16) | (((unsigned int) temp[2])<<8) | ((unsigned int) temp[3]));
            fseek(file_data->fedit,size+2,SEEK_CUR);
        }
    }
    if(tag_info->tag_count < 7)
    {
        printf("\nBellow listed tags not found in header : \n");
        for(int i=0;i<7;i++)
        {
            if(tag_info->tags_array[i].frame_pos==0)       
                printf("%s\n",tag_info->tags_array[i].tag);
        }
    }

}

void print_tag(File_data *file_data,Tag *tag)
{
    //print each tag charcter by character by reading from file
    if(tag->frame_pos == 0)
    {
        printf("?");
        return;
    }
    char buffer[tag->frame_size];
    fseek(file_data->fedit,(tag->frame_pos)+1,SEEK_SET);
    fread(buffer,1,(tag->frame_size)-1,file_data->fedit);
    buffer[(tag->frame_size)-1]='\0';
    if(strcmp(tag->tag,"COMM")==0)
    {
        
        printf("   \tLanguage    : %.3s\n", buffer);
        printf("   \tDescription : ");
        int i;
        for(i=4;i<tag->frame_size-1;i++)
        {
            if(buffer[i]!='\0')
                printf("%c",buffer[i]);
        }
        printf("\n");
        //printf("Comment : %s",buffer+i+1);
        return;
    }
    printf("%s",buffer);
}

void edit_tag()
{
    
}