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
    file_data->fptr=fopen(FILE_NAME,"r");
    if(file_data->fptr==NULL)
    {
        perror("File could not open : \n");
        return 0;
    }
    fseek(file_data->fptr,0,SEEK_END);
    file_data->file_size=ftell(file_data->fptr);
    rewind(file_data->fptr);
    char buffer[5];
    if((file_data->file_size)<10)
    {
        printf("\nFile is currupted!!\n\n");
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
    if(create_edit_file(file_data) == 0)
        return 0;
    // char *buffer1 = malloc(file_data->file_size);
    // if (buffer1 == NULL)
    // {
    //     printf("Memory allocation of buffer failed\n");
    //     return 0;
    // }
    // file_data->fedit=fopen("edit.mp3","w+");
    // rewind(file_data->fptr);
    // if(fread(buffer1,1,file_data->file_size,file_data->fptr)!=file_data->file_size)
    // {
    //     printf("reading failed");
    //     return 0;
    // }
    // if(fwrite(buffer1,1,file_data->file_size,file_data->fedit)!=file_data->file_size)
    // {
    //     printf("writing failed");
    //     return 0;
    // }
    // rewind(file_data->fedit);
    // free(buffer1);
    fclose(file_data->fptr);
    return 1;
}

int open_file(File_data *file_data)
{
    file_data->fptr=fopen(FILE_NAME,"r");
    if(file_data->fptr==NULL)
    {
        perror("File could not open : \n");
        return 0;
    }
    return 1;
}

int create_edit_file(File_data *file_data)
{
    char *buffer = malloc(file_data->file_size);
    if (buffer == NULL)
    {
        printf("Memory allocation of buffer failed\n");
        return 0;
    }
    file_data->fedit=fopen("edit.mp3","w+");
    rewind(file_data->fptr);
    if(fread(buffer,1,file_data->file_size,file_data->fptr)!=file_data->file_size)
    {
        printf("reading failed");
        return 0;
    }
    if(fwrite(buffer,1,file_data->file_size,file_data->fedit)!=file_data->file_size)
    {
        printf("writing failed");
        return 0;
    }
    rewind(file_data->fedit);
    free(buffer);
    return 1;
}

void file_close(File_data *file_data)
{


}