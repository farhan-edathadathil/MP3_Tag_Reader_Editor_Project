/*
 NAME : FARHAN E
 ID   : 26018_199
 DATE : 5/10/2026

 
                  MP3 TAG READER AND EDITOR

 DESCRIPTION:

 This project is a command-line based MP3 Tag Reader and Editor developed
 in C. It is designed to read and modify metadata stored in an MP3 file
 using the ID3 v2.3 tagging format.

 The program directly accesses the binary structure of the MP3 file using
 standard C file-handling functions instead of using any external MP3
 metadata library. It validates the input file, checks the ID3 header and
 version, identifies the required ID3 frames, reads their contents and
 allows the user to edit the metadata.


 SUPPORTED ID3 FRAMES:

 The project currently supports the following ID3 v2.3 frames:

      TIT2  -> Title
      TPE1  -> Artist
      TALB  -> Album
      TYER  -> Year
      TCON  -> Genre
      COMM  -> Comment


 FRAME ENCODING SUPPORT:

 The project supports frame data with the following ID3 text encoding
 values:

      0x00 -> ISO-8859-1 / single-byte encoded text
      0x01 -> UTF-16 encoded text

 For 0x00 encoded frames, the text data can be read directly as
 single-byte characters.

 For 0x01 encoded frames, the program handles UTF-16 data containing
 the BOM (Byte Order Mark), including the UTF-16 little-endian BOM
 FF FE. The program processes the character bytes while skipping the
 corresponding zero bytes used in the UTF-16 representation.

 The COMM (Comment) frame is handled separately because it contains
 additional fields such as the three-character language code and
 description/comment data. Both supported encoding formats are handled
 separately for the comment frame.


 FILE VALIDATION:

 Before processing the MP3 file, the program performs multiple checks:

      1. Checks whether the supplied filename has an .mp3 extension.
      2. Opens the file and verifies that it can be accessed.
      3. Checks that the file contains at least the 10-byte ID3 header.
      4. Verifies the "ID3" identifier.
      5. Verifies that the file uses ID3 v2.3.
      6. Reads and validates the ID3 header size.
      7. Ensures that the declared ID3 header does not exceed the
         actual file size.


 TAG READING:

 After validating the file, the program starts reading from byte 10,
 immediately after the ID3 header.

 Each ID3 frame contains:

      Frame ID       -> 4 bytes
      Frame Size     -> 4 bytes
      Frame Flags    -> 2 bytes
      Frame Data     -> Variable length

 The program reads the four-byte frame ID and compares it with the
 supported frame list. When a required frame is found, its frame size
 and file position are stored.

 Unknown or unsupported frames are skipped using their stored frame
 size so that the next frame can be processed correctly.


 MEMORY ALLOCATION:

 Dynamic memory allocation is used when reading frame data whose size
 is determined at runtime.

 malloc() is used to allocate memory according to the frame size instead
 of using a fixed-size character array. This allows the program to handle
 metadata strings of different lengths without assuming a fixed maximum
 size.

 realloc() is used when reconstructing the MP3 file and the amount of
 data that needs to be stored changes.

 free() is used after the dynamically allocated memory is no longer
 required, preventing unnecessary memory usage.

 The program also checks whether malloc() or realloc() succeeds. If
 dynamic memory allocation fails during file reconstruction, the program
 falls back to copying the file data in smaller blocks instead of
 depending completely on a large memory allocation.


 TAG EDITING:

 The program allows the user to modify the supported metadata fields
 through command-line arguments.

 If the new content has the same size as the existing frame, the program
 can directly replace the existing content without reconstructing the
 complete file.

 If the new content has a different size, the existing frame cannot simply
 be overwritten because it would change the position of the following
 frames. In this case, the program creates a temporary file named
 "edit.mp3".

 The temporary file is constructed by:

      1. Copying the data before the selected frame.
      2. Writing the new frame content.
      3. Copying the remaining data after the old frame.
      4. Updating the frame size.
      5. Updating the overall ID3 header size.
      6. Replacing the original MP3 file with the newly constructed file.

 This allows metadata to be increased or decreased in size while
 maintaining the correct structure of the ID3 section.


 INPUT VALIDATION:

 Different metadata fields have different validation rules.

      Title, Artist, Album and Genre
          -> Must not be empty or contain only spaces.

      Year
          -> Must contain exactly four digits and begin with 1 or 2.

      Comment
          -> Must contain a three-character alphabetic language code
             followed by a space and the description/comment.


 COMMAND-LINE USAGE:

      Display help:
          ./a.out -h

      Display MP3 metadata:
          ./a.out -v filename.mp3

      Edit title:
          ./a.out -e filename.mp3 -t "New Title"

      Edit artist:
          ./a.out -e filename.mp3 -a "New Artist"

      Edit album:
          ./a.out -e filename.mp3 -A "New Album"

      Edit year:
          ./a.out -e filename.mp3 -y "2026"

      Edit comment:
          ./a.out -e filename.mp3 -c "ENG New description"

      Edit genre:
          ./a.out -e filename.mp3 -g "Rock"


 MAIN C CONCEPTS USED:

 This project demonstrates practical usage of several C programming
 concepts, including:

      - Command-line arguments (argc and argv)
      - Structures and typedef
      - Pointers and structure pointers
      - Binary file handling
      - fopen(), fclose(), fread(), fwrite()
      - fseek(), ftell() and rewind()
      - Dynamic memory allocation
      - malloc(), realloc() and free()
      - String handling functions
      - Character validation functions
      - Bitwise shift and OR operations
      - File position management
      - Error handling
      - Input validation

 SAMPLE INPUT:

  ./a.out -v sample.mp3

SAMPLE OUTPUT:

sample.mp3 -[8.520077 MB]
-----------------------------------------------------------------------
File Format :- ID3 v2.3                                     Year : 2013
-----------------------------------------------------------------------
Title       : Sunny Sunny - Yo Yo Honey Singh - [SongsPk.CC]
Artist      : Yo Yo Honey Singh - [SongsPk.CC]
Album       : Yariyaan
Genre       : Bollywood Music - [SongsPk.CC]
Comment     :- 
        Language    : Eng
        Description : downloaded_from spotify_india

-----------------------------------------------------------------------

 OVERALL:

 The main purpose of this project is to understand how metadata is
 internally stored inside an MP3 file and how it can be accessed and
 modified directly using C.

 The project provides a practical implementation of ID3 v2.3 frame
 parsing, metadata extraction, encoding-based data processing, dynamic
 memory management and variable-size frame editing without depending
 on an external MP3 metadata library.

 ============================================================================
 */

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"tag.h"
#include"file.h"

int main(int argc,char **argv)
{
    Tags_info tags_info;
    File_data file_data;
    if(argc==1)
    {
        printf("No enough cammand line arguments are given !!\n\n");
        print_help();
        exit(0);
    }
    if(strcmp("-h",argv[1])==0)
    {
        print_help();
    }
    else if(strcmp("-v",argv[1])==0)
    {
        if(argc!=3)
        {
            printf("\nFilename argument is not given or more arguments are given !!\n\n");
            exit(0);
        }
        if(pre_open_check(argv[2])==0)
        {
            printf("\nUnsupported file extension or invalid file name !!\n\n");
            exit(0);
        }
        if(open_file(&file_data,argv[2])==0)
            exit(0);
        if(post_open_check(&file_data)==0)
        {
            exit(0);
        }
        initialise_tag_filter(&tags_info);
        find_tags(&file_data, &tags_info);
        display_tags(&file_data,&tags_info,argv[2]);
        fclose(file_data.fptr);
    }
    else if(strcmp("-e",argv[1])==0)
    {
        if(argc<5)
        {
            printf("\nEnough cammand line argument is not given !!\n\n");
            exit(0);
        }
        if(argc>5)
        {
            printf("\nWarnning :- if the new content that given for edit contain space then give inside \"new content\" !!\n\n");
        }
        if(pre_open_check(argv[2])==0)
        {
            printf("\nUnsupported file extension or invalid file name !!\n\n");
            exit(0);
        }
        if(open_file(&file_data,argv[2])==0)
            exit(0);
        if(post_open_check(&file_data)==0)
        {
            exit(0);
        }
        initialise_tag_filter(&tags_info);
        if(edit_option(&file_data,&tags_info,argv)==0)
        {
            printf("\nediting failed !!\n\n");                               
        }
        fclose(file_data.fptr);
    }
    else
    {
        printf("\nInvalid input !!\n");
        print_help();
        exit(0);
    }

}

