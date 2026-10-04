#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
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

void display_tags(File_data *file_data,Tags_info *tags_info,char *file_name)
{
    //display every tags
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
    //find and save each tag address in struct 
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

    unsigned char *buffer=malloc(tag->frame_size);
    if(strcmp(tag->tag,"COMM")==0)
    {
        fread(buffer,1,(tag->frame_size)-1,file_data->fptr);
        printf("\n");
        printf("   \tLanguage    : %.3s\n", buffer);
        printf("   \tDescription : ");
        int i;
        for(i=5;i<tag->frame_size-1;i++)
        {
            if(buffer[i]==0xff && buffer[i+1]==0xfe)
            {
                printf("\n\t%14s"," ");
                i++;
            }
            else if(buffer[i]!='\0')
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
    buffer[(tag->frame_size)-3]='\0';
    printf("%s",buffer);
    free(buffer);

}

int edit_option(File_data *file_data,Tags_info *tags_info,char **argv)
{
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
    printf("\n Given tag frame not found in this file !!\n\n");
    return 0;
}

void edit_tag(File_data *file_data,Tag *tag,char *new_content,char *file_name)
{
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
        if(tag->frame_size == new_tag_size+1)
        {
            fclose(file_data->fptr);
            file_data->fptr=fopen(file_name,"r+");
            fseek(file_data->fptr,(tag->frame_pos)+1,SEEK_SET);
            fwrite(new_content,1,new_tag_size,file_data->fptr);
            printf("\nTag content successfully edited !!\n\n");
            return;
        }
        FILE *fedit=fopen("edit.mp3","w");
        char *buffer=malloc((tag->frame_pos)+1);
        fseek(file_data->fptr,0,SEEK_SET);
        fread(buffer,1,(tag->frame_pos)+1,file_data->fptr);
        fwrite(buffer,1,(tag->frame_pos)+1,fedit);
        fwrite(new_content,1,new_tag_size,fedit);
        new_tag_size++;  //to include the encoding byte;

        long unsigned int size=((file_data->file_size)-((long)(tag->frame_pos)+(long)(tag->frame_size)));
        buffer=realloc(buffer,size);
        fseek(file_data->fptr,((tag->frame_pos)+(tag->frame_size)),SEEK_SET);
        fread(buffer,1,size,file_data->fptr);
        //fseek(fedit,((tag->frame_pos)+(new_tag_size)),SEEK_SET);
        fwrite(buffer,1,size,fedit);
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

        size_bytes[0] = (file_data->header_size >> 21) & 0x7F;
        size_bytes[1] = (file_data->header_size >> 14) & 0x7F;
        size_bytes[2] = (file_data->header_size >> 7)  & 0x7F;
        size_bytes[3] = file_data->header_size & 0x7F;

        fseek(fedit,6,SEEK_SET);
        fwrite(size_bytes,1,4,fedit);
        free(buffer);
        fclose(file_data->fptr);
        fclose(fedit);
        remove(file_name);
        rename("edit.mp3",file_name);
        printf("\nTag content successfully edited !!\n\n");
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

void print_help()
{
    printf("\n");
    printf("Usage :-\n");
    printf("./a.out -h  ->for showing help menu\n");
    printf("./a.out -v filename.mp3  ->for showing file tags data\n");
    printf("./a.out -e filename.mp3 -(alphabet reprecenting each tag from help menu (eg:- t,A...)) \"new content\"  ->for editing tag content\n");
    printf("\n");

    printf("-h    Displays this help info\n");
    printf("-v    Prints version & tag info\n");

    printf("-e    Edit the content of tag\n");
    printf("You can select the tag that want to edit from bellow and give new content\n");
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