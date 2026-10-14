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
    /*
    ============================================================================
    Function Name : post_open_check()

    Description :
    This function performs validation checks on an already opened MP3 file.
    It verifies the minimum file size, ID3 header identifier, ID3 version,
    and the complete ID3 header size before allowing further processing.

    The function specifically supports ID3 version 2.3 files.

    Logic :
    1. Moves the file pointer to the end of the file and obtains the complete
        file size using ftell().
    
    2. Rewinds the file pointer back to the beginning of the file.
    
    3. Checks whether the file size is at least 10 bytes, which is the minimum
        size required for an ID3 header.
    
    4. Reads the first 3 bytes and checks whether they contain "ID3".
        If the identifier is different, the file header is considered unsupported.
    
    5. Reads the next 2 bytes and checks whether the ID3 version is 2.3
        (03 00).
    
    6. Skips the flags byte present in the ID3 header.
    
    7. Reads the next 4 bytes containing the ID3 tag/header size.
        These bytes are stored in synchsafe format.
    
    8. Reconstructs the complete header size using bit shifting:
    
        byte0 << 21
        byte1 << 14
        byte2 << 7
        byte3
    
        The four values are combined using the bitwise OR operator.
    
    9. Checks whether the calculated ID3 header size, including the
        10-byte ID3 header, exceeds the actual file size.
        If it does, the file is considered corrupted.
    
    10. Stores the complete ID3 header size in file_data->header_size.
    
    Benefit :
    This function prevents invalid or unsupported MP3 files from being
    processed further. It ensures that the file contains a valid ID3 header,
    uses the supported ID3 v2.3 format, and has a valid header size within
    the actual file boundary.

    Parameters :
    file_data : Pointer to File_data structure containing the opened file
                pointer. The function also stores the file size and complete
                ID3 header size in this structure.

    Return :
    1 : File passed all validation checks successfully.
    0 : File is corrupted or does not satisfy the supported ID3 v2.3 format.
    ============================================================================
    */

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
