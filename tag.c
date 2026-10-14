#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include"tag.h"
#include"file.h"

void initialise_tag_filter(Tags_info *tags_info)
{
    /*
    ============================================================================
    Function    : initialise_tag_filter
    Description : Initializes the list of ID3 v2.3 frames supported by the
                application and resets their frame information.

    Logic       :
                1. Stores the supported frame IDs in tags_array.
                2. Initializes frame_size of every tag to 0.
                3. Initializes frame_pos of every tag to 0.
                4. Sets tag_count to 0 before starting the tag search.

    Benefit     : Provides a known initial state for all supported tags and
                allows the rest of the program to identify each tag using
                a fixed array index.

    Parameters  :
                tags_info - Stores the supported tag information, frame
                            positions, frame sizes and tag count.

    Return      : void
    ============================================================================
    */
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

void display_tags(File_data *file_data,Tags_info *tags_info,char *file_name)
{
    /*
    ============================================================================
    Function    : display_tags
    Description : Displays the MP3 file information and the contents of all
                supported ID3 v2.3 metadata frames.

    Logic       :
                1. Displays the file name and file size.
                2. Displays the ID3 v2.3 file format.
                3. Displays the Year, Title, Artist, Album, Genre and Comment.
                4. Calls print_tag() to read and display the actual content
                    of each frame.

    Benefit     : Provides a formatted and user-friendly view of the metadata
                without handling the frame decoding logic directly.

    Parameters  :
                file_data - Contains the file pointer and file information.
                tags_info - Contains the positions and sizes of supported tags.
                file_name - Name of the MP3 file being displayed.

    Return      : void
    ============================================================================
    */
    printf("\n%s -[%lf MB]\n",file_name,file_data->file_size/(double)1000000);
    printf("-----------------------------------------------------------------------\n");
    printf("File Format :- ID3 v2.3");
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

    printf("%-11s :- ","Comment");
    print_tag(file_data,(tags_info->tags_array)+5);
    printf("\n");
    printf("-----------------------------------------------------------------------\n");
 
}

void find_tags(File_data *file_data,Tags_info *tag_info)
{
    /*
    ============================================================================
    Function    : find_tags
    Description : Searches the ID3 v2.3 header and identifies all supported
                metadata frames.

    Logic       :
                1. Starts reading after the 10-byte ID3 header.
                2. Reads each 4-byte frame ID.
                3. Compares the frame ID with the supported tag list.
                4. Reads and reconstructs the 4-byte frame size.
                5. Skips the 2-byte frame flags.
                6. Stores the position of the frame data.
                7. Skips the complete frame data using its frame size.
                8. If the frame is unsupported, reads its size and skips
                    the frame to continue searching.
                9. Stops when all supported tags are found or the ID3
                    header area has been completely searched.
                10. Displays the supported tags that were not found.

    Benefit     : Stores the location and size of each supported tag so that
                the metadata can be accessed later without searching the
                complete ID3 header again.

    Parameters  :
                file_data - Contains the file pointer and ID3 header size.
                tag_info  - Stores supported tags, frame sizes, positions
                            and the number of tags found.

    Return      : void
    ============================================================================
    */ 
    fseek(file_data->fptr,10,SEEK_SET);
    char temp[5];
    int flag=0; 
    while((ftell(file_data->fptr) <(file_data->header_size)) && tag_info->tag_count <6)
    {
        if (fread(temp, 1, 4, file_data->fptr) != 4)
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
                if(tag_info->tags_array[i].frame_pos!=0)
                    break;
                fread(temp,1,4,file_data->fptr);
                tag_info->tags_array[i].frame_size=((((unsigned int) temp[0])<<24) | (((unsigned int) temp[1])<<16) | (((unsigned int) temp[2])<<8) | ((unsigned int) temp[3]));
                fseek(file_data->fptr,2,SEEK_CUR);
                tag_info->tags_array[i].frame_pos=ftell(file_data->fptr);
                fseek(file_data->fptr,tag_info->tags_array[i].frame_size,SEEK_CUR);
                tag_info->tag_count++;
                flag=1;
                break;
            }
        }
        if(flag==0)
        {
            unsigned int size;
            fread(temp,1,4,file_data->fptr);
            size=((((unsigned int) temp[0])<<24) | (((unsigned int) temp[1])<<16) | (((unsigned int) temp[2])<<8) | ((unsigned int) temp[3]));
            fseek(file_data->fptr,size+2,SEEK_CUR);
        }
    }
    if(tag_info->tag_count < 6)
    {
        printf("\nBelow listed tags not found in header : \n");
        for(int i=0;i<6;i++)
        {
            if(tag_info->tags_array[i].frame_pos==0)       
                printf("%s\n",tag_info->tags_array[i].tag);
        }
    }

}

void print_tag(File_data *file_data,Tag *tag)
{
    /*
    ============================================================================
    Function    : print_tag
    Description : Reads and displays the content of a selected ID3 frame
                according to its encoding type.

    Logic       :
                1. Checks whether the requested tag was found.
                2. Moves the file pointer to the frame data position.
                3. Reads the encoding byte from the frame.
                4. If the encoding is 0x01, calls print_tag01() to process
                    UTF-16 encoded data.
                5. If the encoding is 0x00, allocates memory according to
                    the frame size and reads the frame data.
                6. Handles the COMM frame separately to display its language
                    and description.
                7. Prints the decoded tag content.
                8. Releases dynamically allocated memory using free().

    Benefit     : Provides a common function for displaying different metadata
                frames while selecting the correct processing method based
                on the encoding stored inside the frame.

    Parameters  :
                file_data - Contains the file pointer used to read the frame.
                tag       - Contains the frame ID, frame size and frame position.

    Return      : void
    ============================================================================
    */
    if(tag->frame_pos == 0)
    {
        printf("?");
        return;
    }

    char ch;
    fseek(file_data->fptr,(tag->frame_pos),SEEK_SET);
    fread(&ch,1,1,file_data->fptr);
    if(ch==0x01)
    {
        print_tag01(file_data,tag);
        return;   
    }
    else if(ch==0x00)
    {
        unsigned char *buffer=malloc(tag->frame_size);
        if(buffer==NULL)
        {
            if(strcmp(tag->tag,"COMM")==0)
            {
                printf("Reading failed due to memory allocation failed !!");
                return;
            }
            unsigned char ch[11];
            ch[10]='\0';
            for(int i=0;i<((tag->frame_size)-1)/10;i++)
            {
                fread(ch,1,10,file_data->fptr);
                printf("%s",ch);
            }
            fread(ch,1,((tag->frame_size)-1)%10,file_data->fptr);
            ch[((tag->frame_size)-1)%10]='\0';
            printf("%s",ch);
            return;
        }
        fread(buffer,1,(tag->frame_size)-1,file_data->fptr);
        buffer[(tag->frame_size)-1]='\0';
        if(strcmp(tag->tag,"COMM")==0)
        {
            printf("\n");
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
            return;
        }
        printf("%s",buffer);
        free(buffer);
    }
    else
        printf("Unsupported encoding bit !!");
}

void print_tag01(File_data *file_data,Tag *tag)
{

    /*
    ============================================================================
    Function    : print_tag01
    Description : Reads and displays ID3 frame data encoded using UTF-16.

    Logic       :
                1. Allocates memory according to the frame size.
                2. Checks whether memory allocation was successful.
                3. Handles the COMM frame separately by extracting its
                    language and UTF-16 description/comment data.
                4. For normal UTF-16 frames, skips the UTF-16 BOM.
                5. Reads one character byte and skips the corresponding
                    second byte of the UTF-16 character.
                6. Builds a null-terminated C string.
                7. Displays the converted text.
                8. Releases the allocated memory using free().

    Benefit     : Allows the application to read metadata stored using the
                UTF-16 encoding supported by ID3 v2.3.

    Parameters  :
                file_data - Contains the file pointer used to read the frame.
                tag       - Contains the frame ID, size and position.

    Return      : void
    ============================================================================
    */
    unsigned char *buffer=malloc(tag->frame_size);
    if(buffer==NULL)
    {
        if(strcmp(tag->tag,"COMM")==0)
        {
            printf("Reading failed due to memory allocation failed !!");
            return;
        }
        fseek(file_data->fptr,2,SEEK_CUR); //fffe :-BOM skiping
        unsigned char ch;  
        for(int i=0;i<((tag->frame_size)-3)/2;i++)
        {
            fread(&ch,1,1,file_data->fptr);
            fseek(file_data->fptr,1,SEEK_CUR);
            printf("%c",ch);
        }

    }
    if(strcmp(tag->tag,"COMM")==0)
    {
        fread(buffer,1,(tag->frame_size)-1,file_data->fptr);
        printf("\n");
        printf("   \tLanguage    : %.3s\n", buffer);
        printf("   \tDescription : ");
        int i;
        for(i=5;i<tag->frame_size-1;i++)
        {

            if(buffer[i]==0xfe && buffer[i-1]==0xff)
            {
                printf("\n\t%14s"," ");
            }
            else if(buffer[i]!='\0' && buffer[i]!=0xff)
                printf("%c",buffer[i]);
        }
        printf("\n");
        free(buffer);
        return;
    }
    fseek(file_data->fptr,2,SEEK_CUR);  //fffe :-BOM skiping
    for(int i=0;i<((tag->frame_size)-3)/2;i++)
    {
        fread(&buffer[i],1,1,file_data->fptr);
        fseek(file_data->fptr,1,SEEK_CUR);
    }
    buffer[(((tag->frame_size)-3)/2)]='\0';
    printf("%s",buffer);
    free(buffer);

}

int edit_option(File_data *file_data,Tags_info *tags_info,char **argv)
{
    /*
    ============================================================================
    Function    : edit_option
    Description : Processes the command-line option used to select the ID3
                frame that has to be edited.

    Logic       :
                1. Checks whether the tag selection argument has the
                    expected "-x" format.
                2. Extracts the tag selection character.
                3. Uses a switch statement to identify the required frame.
                4. Validates the new content according to the selected tag.
                5. Searches for the selected frame using find_a_tag().
                6. Calls edit_tag() to perform the editing operation.
                7. Returns failure if the option, input or requested tag
                    is invalid or not found.

    Tag Options :
                -t -> Title
                -a -> Artist
                -A -> Album
                -y -> Year
                -c -> Comment
                -g -> Genre

    Benefit     : Separates command-line option processing from the actual
                frame searching and editing operations.

    Parameters  :
                file_data - Contains file and ID3 information.
                tags_info - Contains the supported tag definitions.
                argv      - Contains the command-line arguments.

    Return      : 1 if the editing option is processed successfully,
                0 if an invalid option or input is detected.
    ============================================================================
    */

    if(strlen(argv[3]) != 2 || argv[3][0] != '-')
    {
        printf("\nInvalid input or unsupported tag selection !!\n\n");
        return 0;
    }
    char ch=argv[3][1];

    switch(ch)
    {
        case 't':
            if(valid_input(argv[4])==0)
            {
                printf("\nGiven input is empthy or contain only spaces !!\n\n");
                return 0;
            }
            if(find_a_tag(file_data,(tags_info->tags_array)+0)==0)
                return 0;
            edit_tag(file_data,(tags_info->tags_array)+0,argv[4],argv[2]);
            break;

        case 'a':
            if(valid_input(argv[4])==0)
            {
                printf("\nGiven input is empthy or contain only spaces !!\n\n");
                return 0;
            }
            if(find_a_tag(file_data,(tags_info->tags_array)+1)==0)
                return 0;
            edit_tag(file_data,(tags_info->tags_array)+1,argv[4],argv[2]);
            break;

        case 'A':
            if(valid_input(argv[4])==0)
            {
                printf("\nGiven input is empthy or contain only spaces !!\n\n");
                return 0;
            }
            if(find_a_tag(file_data,(tags_info->tags_array)+2)==0)
                return 0;
            edit_tag(file_data,(tags_info->tags_array)+2,argv[4],argv[2]);
            break;

        case 'y':
            if(valid_year(argv[4])==0)
            {
                printf("\nGiven argument is not a valid year !!\n\n");
                return 0;
            }
            if(find_a_tag(file_data,(tags_info->tags_array)+3)==0)
                return 0;
            edit_tag(file_data,(tags_info->tags_array)+3,argv[4],argv[2]);
            break;

        case 'c':
            if(valid_COMM(argv[4])==0)
            {
                printf("\nGiven argument is not a valid comment for ID3 v2.3 !!\n");
                printf("give like \"LAN discription\" :- LAN is language");
                return 0;
            }
            if(find_a_tag(file_data,(tags_info->tags_array)+5)==0)
                return 0;
            edit_tag(file_data,(tags_info->tags_array)+5,argv[4],argv[2]);
            break;

        case 'g':
            if(valid_input(argv[4])==0)
            {
                printf("\nGiven input is empthy or contain only spaces !!\n\n");
                return 0;
            }
            if(find_a_tag(file_data,(tags_info->tags_array)+4)==0)
                return 0;
            edit_tag(file_data,(tags_info->tags_array)+4,argv[4],argv[2]);
            break;

        default :
            printf("\nInvalid input or unsupported tag selection !!\n\n");
            return 0;
    }
    return 1;
}

int find_a_tag(File_data *file_data,Tag *tag)
{
    /*
    ============================================================================
    Function    : find_a_tag
    Description : Searches the ID3 v2.3 header for one specific metadata frame.

    Logic       :
                1. Starts reading after the 10-byte ID3 header.
                2. Reads each 4-byte frame ID.
                3. Checks whether the frame ID matches the requested tag.
                4. If matched, reads and reconstructs the frame size.
                5. Skips the 2-byte frame flags.
                6. Stores the position of the frame data.
                7. If the frame does not match, reads its size and skips
                    the complete frame.
                8. Continues until the requested frame is found or the
                    ID3 header ends.

    Benefit     : Locates the exact frame that needs to be edited and stores
                its position and size so the editing function can directly
                access the required frame.

    Parameters  :
                file_data - Contains the file pointer and ID3 header size.
                tag       - Contains the frame ID and stores its size and
                            frame position after the search.

    Return      : 1 if the requested tag is found,
                0 if the tag is not found.
    ============================================================================
    */

    fseek(file_data->fptr,10,SEEK_SET);
    char temp[5];
    int flag=0; 
    while((ftell(file_data->fptr) <(file_data->header_size)))
    {
        if (fread(temp,1,4,file_data->fptr) != 4)
        {
            printf("reading fail during tag finding");
            break;
        }
        temp[4]='\0';
        if (temp[0] == 0 && temp[1] == 0 && temp[2] == 0 && temp[3] == 0)
            break;
        if(strcmp(temp,tag->tag)==0)
        {
            fread(temp,1,4,file_data->fptr);
            tag->frame_size=((((unsigned int) temp[0])<<24) | (((unsigned int) temp[1])<<16) | (((unsigned int) temp[2])<<8) | ((unsigned int) temp[3]));
            fseek(file_data->fptr,2,SEEK_CUR);
            tag->frame_pos=ftell(file_data->fptr);
            fseek(file_data->fptr,tag->frame_size,SEEK_CUR);
            flag=1;
            return 1;
        }
        unsigned int size;
        fread(temp,1,4,file_data->fptr);
        size=((((unsigned int) temp[0])<<24) | (((unsigned int) temp[1])<<16) | (((unsigned int) temp[2])<<8) | ((unsigned int) temp[3]));
        fseek(file_data->fptr,size+2,SEEK_CUR);
    }
    printf("\nGiven tag frame not found in this file !!\n");
    return 0;
}

void edit_tag(File_data *file_data,Tag *tag,char *new_content,char *file_name)
{
    /*
    ============================================================================
    Function    : edit_tag
    Description : Edits the content of a selected ID3 frame when the frame
                uses the 0x00 encoding or transfers UTF-16 editing to
                edit_tag01() when required.

    Logic       :
                1. Calculates the size of the new content.
                2. Displays the current tag content.
                3. Asks the user to confirm the editing operation.
                4. Validates the confirmation input.
                5. Handles the COMM frame language field separately.
                6. Reads the encoding byte of the selected frame.
                7. For 0x00 encoding, checks whether the new content has
                    the same size as the existing frame.
                8. If the size is unchanged, directly replaces the content.
                9. If the size changes, creates a temporary edit.mp3 file.
                10. Copies the data before the selected frame.
                11. Writes the new frame content.
                12. Copies the remaining data after the old frame.
                13. Calculates the difference between old and new frame sizes.
                14. Updates the frame size stored in the frame header.
                15. Updates the overall ID3 header size.
                16. Replaces the original file with the temporary file.
                17. For 0x01 encoding, calls edit_tag01().

    Benefit     : Allows metadata to be changed even when the new content is
                larger or smaller than the original content while keeping
                the surrounding MP3 data and ID3 structure consistent.

    Parameters  :
                file_data   - Contains file size, header size and file pointer.
                tag         - Contains the selected frame information.
                new_content - New metadata supplied by the user.
                file_name   - Name of the original MP3 file.

    Return      : void
    ============================================================================
    */


    char ch,confirm;
    unsigned int new_tag_size=strlen(new_content);
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

    if(strcmp("COMM",tag->tag)==0)
    {
        new_content[3]='\0';
    }
    
    if(confirm=='y')
    {
        fseek(file_data->fptr,(tag->frame_pos),SEEK_SET);
        fread(&ch,1,1,file_data->fptr);
        if(ch==0x00)
        {
            if(tag->frame_size == new_tag_size+1)
            {
                fclose(file_data->fptr);
                file_data->fptr=fopen(file_name,"r+");
                fseek(file_data->fptr,(tag->frame_pos)+1,SEEK_SET);
                fwrite(new_content,1,new_tag_size,file_data->fptr);
                printf("\nTag content successfully edited !!\n\n");
                return;
            }
            FILE *fedit=fopen("edit.mp3","wb");
            if(fedit==NULL)
            {
                perror("\nTemporary file could'nt make :");
                printf("\n");
                exit(0);
            }

            fseek(file_data->fptr,0,SEEK_SET);
            char *buffer=malloc((tag->frame_pos)+1);
            if(buffer==NULL)
            {
                unsigned char ch[100];
                for(int i=0;i<((tag->frame_pos)+1)/100;i++)
                {
                    fread(ch,1,100,file_data->fptr);
                    fwrite(ch,1,100,fedit);
                }
                fread(ch,1,((tag->frame_pos)+1)%100,file_data->fptr);
                fwrite(ch,1,((tag->frame_pos)+1)%100,fedit);  
                goto skip; 
            }
            fread(buffer,1,(tag->frame_pos)+1,file_data->fptr);
            fwrite(buffer,1,(tag->frame_pos)+1,fedit);
            skip :
            fwrite(new_content,1,new_tag_size,fedit);
            new_tag_size++;  //to include the encoding byte;

            long unsigned int size=((file_data->file_size)-((long)(tag->frame_pos)+(long)(tag->frame_size)));
            fseek(file_data->fptr,((tag->frame_pos)+(tag->frame_size)),SEEK_SET);
            if(buffer==NULL)
            {
                buffer=malloc(size);
                goto skip3;
            }
            buffer=realloc(buffer,size);
            skip3 :
            if(buffer==NULL)
            {
                unsigned char ch[100];
                for(int i=0;i<(size)/100;i++)
                {
                    fread(ch,1,100,file_data->fptr);
                    fwrite(ch,1,100,fedit);
                }
                fread(ch,1,(size)%100,file_data->fptr);
                fwrite(ch,1,(size)%100,fedit);  
            }
            else 
            {
                fread(buffer,1,size,file_data->fptr);
                fwrite(buffer,1,size,fedit);
            }
            long difference = (long)new_tag_size - (long)tag->frame_size;
            (file_data->file_size)=(file_data->file_size)+difference;

            unsigned char size_bytes[4];
            size_bytes[0] = (new_tag_size >> 24) & 0xFF;
            size_bytes[1] = (new_tag_size >> 16) & 0xFF;
            size_bytes[2] = (new_tag_size >> 8)  & 0xFF;
            size_bytes[3] = new_tag_size & 0xFF;

            fseek(fedit,(tag->frame_pos)-6,SEEK_SET);
            fwrite(size_bytes,1,4,fedit);

            (file_data->header_size)+=(int)difference;

            size_bytes[0] = (((file_data->header_size)-10) >> 21) & 0x7F;
            size_bytes[1] = (((file_data->header_size)-10) >> 14) & 0x7F;
            size_bytes[2] = (((file_data->header_size)-10) >> 7)  & 0x7F;
            size_bytes[3] = ((file_data->header_size)-10) & 0x7F;

            fseek(fedit,6,SEEK_SET);
            fwrite(size_bytes,1,4,fedit);
            free(buffer);
            fclose(file_data->fptr);
            fclose(fedit);
            remove(file_name);
            rename("edit.mp3",file_name);
            printf("\nTag content successfully edited !!\n\n");
        }
        else if(ch==0x01)
        {
            edit_tag01(file_data,tag,new_content,file_name);
        }
        else
        {
            printf("Unsupported encoding bit !!");
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
  
}

void edit_tag01(File_data *file_data,Tag *tag,char *new_content,char *file_name)
{
    /*
    ============================================================================
    Function    : edit_tag01
    Description : Edits the content of an ID3 frame encoded using UTF-16.

    Logic       :
                1. Calculates the required UTF-16 frame size.
                2. Checks whether the new content has the same size as the
                    existing frame for non-COMM tags.
                3. If the size is unchanged, directly replaces the UTF-16
                    character bytes in the original file.
                4. If the size changes, creates a temporary edit.mp3 file.
                5. Copies the data before the selected frame.
                6. For COMM, writes the language code, UTF-16 BOM,
                    description and comment data according to the format.
                7. For normal tags, writes each character followed by its
                    corresponding UTF-16 byte.
                8. Copies the data after the old frame.
                9. Calculates the difference between the new and old
                    frame sizes.
                10. Updates the frame size in the frame header.
                11. Updates the overall ID3 header size.
                12. Frees allocated memory and closes the files.
                13. Replaces the original MP3 file with edit.mp3.

    Benefit     : Allows UTF-16 encoded metadata to be edited while correctly
                handling the encoding bytes, BOM and variable frame size.

    Parameters  :
                file_data   - Contains file size, header size and file pointer.
                tag         - Contains the selected frame information.
                new_content - New metadata supplied by the user.
                file_name   - Name of the original MP3 file.

    Return      : void
    ============================================================================
    */


    unsigned int new_tag_size=strlen(new_content)+1;
    int n=0;
    if((tag->frame_size == (new_tag_size*2)+3) && strcmp(tag->tag,"COMM")!=0)
    {
        fclose(file_data->fptr);
        file_data->fptr=fopen(file_name,"rb+");
        fseek(file_data->fptr,(tag->frame_pos)+3,SEEK_SET);
        for(int i=0;i<new_tag_size;i++)
        {
            fwrite(&new_content[i],1,1,file_data->fptr);
            fseek(file_data->fptr,1,SEEK_CUR);
        }

        printf("\nTag content successfully edited !!\n\n");
        return;
    }


    FILE *fedit=fopen("edit.mp3","wb");
    if(fedit==NULL)
    {
        perror("\nTemporary file could'nt make :");
        printf("\n");
        exit(0);
    }

    fseek(file_data->fptr,0,SEEK_SET);
    char *buffer=malloc((tag->frame_pos)+3);
    if(buffer==NULL)
    {
        unsigned char ch[100];
        for(int i=0;i<((tag->frame_pos)+3)/100;i++)
        {
            fread(ch,1,100,file_data->fptr);
            fwrite(ch,1,100,fedit);
        }
        fread(ch,1,((tag->frame_pos)+3)%100,file_data->fptr);
        fwrite(ch,1,((tag->frame_pos)+3)%100,fedit);  
        goto skip; 
    }
    fread(buffer,1,(tag->frame_pos)+3,file_data->fptr);
    fwrite(buffer,1,(tag->frame_pos)+3,fedit);
    skip :
    if(strcmp(tag->tag,"COMM")==0)
    {
        new_content[3]=' ';
        new_tag_size=strlen(new_content)+1;
        unsigned char bom[] = {0xFF, 0xFE};
        fseek(fedit,(tag->frame_pos)+1,SEEK_SET);
        fwrite(new_content,1,3,fedit);
        fwrite(bom,1,2,fedit);
        int count=0,flag=1;
        for(int i=4;i<new_tag_size;i++)
        {
            if(new_content[i]==' ')
            {
                if(flag==0)
                {
                    new_content[i]='\0';
                    fwrite(&new_content[i],1,1,fedit);
                    fwrite(&n,1,1,fedit);
                    break;
                }
                fwrite(bom,1,2,fedit);
                flag=0;
                continue;
            }
            fwrite(&new_content[i],1,1,fedit);
            fwrite(&n,1,1,fedit);
        }
        if(flag==1)
        {
            fwrite(bom,1,2,fedit);
            fwrite(&n,1,1,fedit);
            fwrite(&n,1,1,fedit);
        }
        new_tag_size=strlen(new_content)+1;
        new_tag_size=((new_tag_size-4)*2)+8+flag*2-(!flag)*2;
        goto comm;
    }
    for(int i=0;i<new_tag_size;i++)
    {
        fwrite(&new_content[i],1,1,fedit);
        fwrite(&n,1,1,fedit);
    }
    new_tag_size=(new_tag_size*2)+3;  //to include the encoding byte and fffe BOM bytes ;
    comm:
    fseek(file_data->fptr,((tag->frame_pos)+(tag->frame_size)),SEEK_SET);
    long unsigned int size=((file_data->file_size)-((long)(tag->frame_pos)+(long)(tag->frame_size)));
    if(buffer==NULL)
    {
        buffer=malloc(size);
        goto skip3;
    }
    buffer=realloc(buffer,size);
    skip3 :
    if(buffer==NULL)
    {
        unsigned char ch[100];
        for(int i=0;i<(size)/100;i++)
        {
            fread(ch,1,100,file_data->fptr);
            fwrite(ch,1,100,fedit);
        }
        fread(ch,1,(size)%100,file_data->fptr);
        fwrite(ch,1,(size)%100,fedit);  
    }
    else 
    {
        fread(buffer,1,size,file_data->fptr);
        fwrite(buffer,1,size,fedit);
    }
    long difference = (long)new_tag_size - (long)tag->frame_size;
    (file_data->file_size)=(file_data->file_size)+difference;

    unsigned char size_bytes[4];
    size_bytes[0] = (new_tag_size >> 24) & 0xFF;
    size_bytes[1] = (new_tag_size >> 16) & 0xFF;
    size_bytes[2] = (new_tag_size >> 8)  & 0xFF;
    size_bytes[3] = new_tag_size & 0xFF;

    fseek(fedit,(tag->frame_pos)-6,SEEK_SET);
    fwrite(size_bytes,1,4,fedit);

    (file_data->header_size)+=(int)difference;

    size_bytes[0] = (((file_data->header_size)-10) >> 21) & 0x7F;
    size_bytes[1] = (((file_data->header_size)-10) >> 14) & 0x7F;
    size_bytes[2] = (((file_data->header_size)-10) >> 7)  & 0x7F;
    size_bytes[3] = ((file_data->header_size)-10) & 0x7F;

    fseek(fedit,6,SEEK_SET);
    fwrite(size_bytes,1,4,fedit);
    free(buffer);
    fclose(file_data->fptr);
    fclose(fedit);
    remove(file_name);
    rename("edit.mp3",file_name);
    printf("\nTag content successfully edited !!\n\n");

}

void print_help()
{
    printf("\n");
    printf("Usage :-\n");
    printf("./a.out -h  ->for showing help menu\n");
    printf("./a.out -v filename.mp3  ->for showing file tags data\n");
    printf("./a.out -e filename.mp3 -[t/a/A/y/c/g](alphabet reprecenting each tag from help menu) \"new content\"  ->for editing tag content\n");
    printf("Example :- \n    ./a.out -e sample.mp3 -A \"new content for edit\"\n");
    printf("\n");

    printf("-h    Displays this help info\n");
    printf("-v    Prints version & tag info\n");

    printf("-e    Edit the content of tag\n");
    printf("You can select the tag that want to edit like shown bellow and give new content\n");
    printf("   -t    Modifies a Title tag\n");
    printf("   -a    Modifies an Artist tag\n");
    printf("   -A    Modifies an Album tag\n");
    printf("   -y    Modifies a Year tag\n");
    printf("   -c    Modifies a Comment tag (comment new content first 3 bytes should be language the leave a space eg:-\"ENG discription\" )\n");
    printf("   -g    Modifies a Genre tag\n");
    
}

int valid_year(char *year)
{
    if(strlen(year) != 4)
        return 0;
    if(year[0] != '1' && year[0] != '2')
        return 0;

    for(int i = 1; i < 4; i++)
    {
        if(year[i]<'0' || year[i]>'9')
            return 0;
    }
    return 1;
}

int valid_COMM(char *comm)
{
    /*
    ============================================================================
    Function    : valid_COMM
    Description : Validates the input format used for editing an ID3 v2.3
                COMM (Comment) frame.

    Logic       :
                1. Checks that the input has at least three characters.
                2. Checks that the first three characters are alphabetic.
                3. Checks that the fourth character is a space.
                4. Rejects the input if any of these conditions fail.

    Expected Format :
                "ENG description"

                The first three characters represent the language code and
                the remaining part represents the description/comment.

    Benefit     : Ensures that the comment input follows the format expected
                by the COMM editing logic.

    Parameters  :
                comm - Comment string supplied by the user.

    Return      : 1 if the comment format is valid,
                0 if the comment format is invalid.
    ============================================================================
    */
    if(strlen(comm)<3)
        return 0;
    if(comm[3]!=' ')
        return 0;
    for(int i=0;i<3;i++)
    {
        if(isalpha(comm[i])==0)
            return 0;
    }
    return 1;

}

int valid_input(char *input)
{
    if(input[0] == '\0')
        return 0;
    for(int i=0;input[i] !='\0';i++)
    {
        if(input[i] != ' ')
            return 1;
    }

    return 0;
}