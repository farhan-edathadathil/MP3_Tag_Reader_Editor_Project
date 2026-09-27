#include<stdio.h>
#include<string.h>
#include<stdlib.h>
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

    for(int i=0;i<6;i++)
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
    printf("File Format :- ID3 v2.3");
    // printf("Time : ");
    // print_tag(file_data,(tags_info->tags_array)+6);
    printf("%44s","Year : ");
    print_tag(file_data,(tags_info->tags_array)+3);
    printf("\n");
    printf("-----------------------------------------------------------------------\n");
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
    while(ftell(file_data->fedit) <(file_data->header_size))
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
        for(int i=0;i<6;i++)
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
    if(tag_info->tag_count < 6)
    {
        printf("\nBellow listed tags not found in header : \n");
        for(int i=0;i<6;i++)
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
    char *buffer=malloc(tag->frame_size);
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
        free(buffer);
        //printf("Comment : %s",buffer+i+1);
        return;
    }
    printf("%s",buffer);
    free(buffer);
}

int edit_option(File_data *file_data,Tags_info *tags_info)
{

    find_tag(file_data,tags_info);
    char ch,choice;
    printf("\n");
    printf("Usage: mp3tag -[tTaAycg] \"value\" file1\n");
    printf("       mp3tag -v\n");
    printf("\n");

    printf("-t    Modifies a Title tag\n");
    printf("-a    Modifies an Artist tag\n");
    printf("-A    Modifies an Album tag\n");
    printf("-y    Modifies a Year tag\n");
    printf("-c    Modifies a Comment tag\n");
    printf("-g    Modifies a Genre tag\n");
    // printf("-h    Displays this help info\n");
    // printf("-v    Prints version info\n");
    printf("\nEnter the choice :");
    scanf("%c",&choice);
    ch = getchar();
    if (ch != '\n' && ch != EOF)
    {
        printf("\nInvalid input !!\n\n");

        while ((ch = getchar()) != '\n' && ch != EOF);
        return 0;
    }

    switch(choice)
    {
        case 't':
            edit_tag(file_data,(tags_info->tags_array)+0);
            break;

        case 'a':
            edit_tag(file_data,(tags_info->tags_array)+1);
            break;

        case 'A':
            edit_tag(file_data,(tags_info->tags_array)+2);
            break;

        case 'y':
            edit_tag(file_data,(tags_info->tags_array)+3);
            break;

        case 'c':
            edit_tag(file_data,(tags_info->tags_array)+5);
            break;

        case 'g':
            edit_tag(file_data,(tags_info->tags_array)+4);
            break;

        default :
            printf("\nInvalid input !!\n\n");
            break;
    }
    if(open_file(file_data) == 0)
        return 0;
    if(create_edit_file(file_data) == 0)
        return 0;
    find_tag(file_data,tags_info);
    return 1;
}

void edit_tag(File_data *file_data,Tag *tag)
{
    char ch,confirm;
    unsigned int count=2;
    tryagain:
    printf("\n%c%c%c%c -tag : ",tag->tag[0],tag->tag[1],tag->tag[2],tag->tag[3]);
    print_tag(file_data,tag);
    printf("\n\nConfirm this content wants to edit (y/n):");
    scanf("%c",&confirm);
    ch = getchar();
    if (ch != '\n' && ch != EOF)
    {
        printf("\nInvalid input, try again!!\n\n");

        while ((ch = getchar()) != '\n' && ch != EOF);
        goto tryagain;
    }
    char *buffer=malloc(count);
    if(confirm=='y')
    {
        char *buffer=malloc(count);
        printf("Enter the new tag content :");
        while((ch = getchar()) != '\n' && ch != EOF)
        {
            buffer[count-2]=ch;
            count++;
            buffer=realloc(buffer,count);
        }
        buffer[count-1]='\0';
        count=count-2;
        fseek(file_data->fedit,(tag->frame_pos)+1,SEEK_SET);
        fwrite(buffer,1,count,file_data->fedit);
        count++;
        if(tag->frame_size != count)
        {
            file_data->fptr=fopen(FILE_NAME,"r");
            long unsigned int size=((file_data->file_size)-((long)(tag->frame_pos)+(long)(tag->frame_size)));
            buffer=realloc(buffer,size);
            fseek(file_data->fptr,((tag->frame_pos)+(tag->frame_size)),SEEK_SET);
            fread(buffer,1,size,file_data->fptr);
            fseek(file_data->fedit,((tag->frame_pos)+(count)),SEEK_SET);
            fwrite(buffer,1,size,file_data->fedit);
            long difference = (long)count - (long)tag->frame_size;
            (file_data->file_size)=(file_data->file_size)+difference;

            unsigned char size_bytes[4];
            size_bytes[0] = (count >> 24) & 0xFF;
            size_bytes[1] = (count >> 16) & 0xFF;
            size_bytes[2] = (count >> 8)  & 0xFF;
            size_bytes[3] = count & 0xFF;

            fseek(file_data->fedit,(tag->frame_pos)-6,SEEK_SET);
            fwrite(size_bytes,1,4,file_data->fedit);

            (file_data->header_size)+=(int)difference;

            size_bytes[0] = (file_data->header_size >> 21) & 0x7F;
            size_bytes[1] = (file_data->header_size >> 14) & 0x7F;
            size_bytes[2] = (file_data->header_size >> 7)  & 0x7F;
            size_bytes[3] = file_data->header_size & 0x7F;

            fseek(file_data->fedit,6,SEEK_SET);
            fwrite(size_bytes,1,4,file_data->fedit);
        }
    }
    else if(confirm=='n')
    {
        printf("\nTag content not edited, Editing cancelled !!\n\n");
        return;
    }
    else
    {
        printf("\nInvalid input, try again!!\n\n");
        goto tryagain;
    }
    fclose(file_data->fptr);
    buffer=realloc(buffer,file_data->file_size);
    fseek(file_data->fedit,0,SEEK_SET);
    fread(buffer,1,file_data->file_size,file_data->fedit);
    fclose(file_data->fedit);
    file_data->fptr=fopen(FILE_NAME,"w");
    fwrite(buffer,1,file_data->file_size,file_data->fptr);
    fclose(file_data->fptr);
    free(buffer);
    remove("edit.mp3");
    printf("\nTag content successfully edited !!\n\n");
  
}

void print_help(void)
{
    printf("\n");
    printf("Usage: mp3tag -[tTaAycg] \"value\" file1\n");
    printf("       mp3tag -v\n");
    printf("\n");

    printf("-t    Modifies a Title tag\n");
    printf("-a    Modifies an Artist tag\n");
    printf("-A    Modifies an Album tag\n");
    printf("-y    Modifies a Year tag\n");
    printf("-c    Modifies a Comment tag\n");
    printf("-g    Modifies a Genre tag\n");
    printf("-h    Displays this help info\n");
    printf("-v    Prints version & tag info\n");
}