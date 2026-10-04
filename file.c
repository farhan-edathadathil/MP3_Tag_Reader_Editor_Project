#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"file.h"
#include"tag.h"

int pre_open_check(char *file_name)
{
    //check the file name and .mp3 extension
    char *extension = strrchr(file_name,'.');
    if(extension != NULL && (strcasecmp(extension,".mp3") == 0))
        return 1;
    else
        return 0;

}

int post_open_check(File_data *file_data)
{
    //check file size >=10byte
    //check for the ID3 and v2.3 
    //read full header frame size and check it is less than (file size - 10)

    int temp;
    char buffer[5];
    fseek(file_data->fptr,0,SEEK_END);
    file_data->file_size=ftell(file_data->fptr);
    rewind(file_data->fptr);
    if((file_data->file_size)<10)
    {
        printf("\nFile is currupted !!\n\n");
        return 0;
    }
    fread(buffer,1,3,file_data->fptr);
    buffer[3]='\0';
    if(strcmp(buffer,"ID3"))
    {
        printf("\nUnsupported file header id !!\n\n");
        return 0;
    }
    fread(buffer,1,2,file_data->fptr);
    if(buffer[0]!=3 || buffer[1]!=0)
    {
        printf("\nFile version is not supported !!\n\n");
        return 0;
    }
    fseek(file_data->fptr, 1, SEEK_CUR);
    fread(buffer,1,4,file_data->fptr);
    temp=((buffer[0]<<21) | (buffer[1]<<14) | (buffer[2]<<7) | buffer[3]);
    if(temp+10>(file_data->file_size))
    {
        printf("\nFile is currupted !!\n\n");
        return 0;
    }
    file_data->header_size=temp+10;
    return 1;
}

int open_file(File_data *file_data,char *file_name)
{
    file_data->fptr=fopen(file_name,"r");
    if(file_data->fptr==NULL)
    {
        perror("\nFile could not open ");
        printf("\n");
        return 0;
    }
    return 1;
}
