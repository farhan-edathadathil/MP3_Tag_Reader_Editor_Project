#include<stdio.h>
#include<string.h>
#include"file.h"

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
    char *str=&temp;
    file_data->fptr=fopen(FILE_NAME,"r");
    if(file_data->fptr==NULL)
    {
        perror("File could not open : \n");
        return 0;
    }
    fseek(file_data->fptr,0,SEEK_END);
    file_data->file_size=ftell(file_data->fptr);
    rewind(file_data->fptr);
    if((file_data->file_size)<10)
    {
        printf("\nFile is currupted!!\n\n");
        return 0;
    }
    for(int i=0;i<3;i++)
        fread(str+i,1,1,file_data->fptr);
    str[3]='\0';
    if(strcmp(str,"ID3"))
    {
        printf("\nUnsupported file header id!!!\n\n");
        return 0;
    }
    for(int i=0;i<2;i++)
        fread(str+i,1,1,file_data->fptr);
    if(str[0]!=3 || str[1]!=0)
    {
        printf("\nFile version is not supported !!\n\n");
        return 0;
    }
    fseek(file_data->fptr, 1, SEEK_CUR);
    fread(&temp,4,1,file_data->fptr);
    if(temp+10>(file_data->file_size))
    {
        printf("\nFile is currupted !!\n\n");
        return 0;
    }
    return 1;
}

void file_close(File_data *file_data)
{


}