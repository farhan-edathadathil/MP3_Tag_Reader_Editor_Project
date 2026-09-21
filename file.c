#include<stdio.h>
#include<string.h>
#include"file.h"

int pre_open_check(char *file_name)
{
    //check the file name and .mp3 extension

}

int post_open_check(File_data *file_data)
{
    //check file size >=10byte
    //check for the ID3 and v2.3 
    //read full header frame size and check it is less than (file size - 10)
}
