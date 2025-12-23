#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "types.h"
#include "decode.h"

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if(argv[2] == NULL)     //check if stego file is pressent
    {
        printf("Stego.bmp is not present\n");
        return e_failure;
    }

    if(strstr(argv[2], ".bmp") == NULL)     //check if stego file is .bmp
    {
        printf("Stego file must be .bmp\n");
        return e_failure;
    }

    decInfo -> stego_image_fname = argv[2];       //check stego file name
    printf("Stego file is present\n");

    if(argv[3] == NULL)       //check if output file is present
    {
        strcpy(decInfo -> output_fname, "output.txt");     //default output file name
        printf("No output file provided. Creating default output.txt\n");
    }
    else
    {
        strcpy(decInfo -> output_fname, argv[3]);      
        printf("Output file is present\n");
    }

    return e_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
    if(open_decode_files(decInfo) == e_success)    
    {
        printf("All files are opened successfully\n");
    }
    else
    {
        printf("All files are not opened successfully\n");
        return e_failure;
    }

    char user_magic[50];
    printf("Enter the same magic string used during encoding: ");
    scanf("%s", user_magic);

    char decoded_magic[50];        //to store decoded magic string
    if(decode_magic_string(decInfo, user_magic, decoded_magic) == e_success) 
    {
        printf("Magic string decoded successfully\n");
    }
    else
    {
        printf("Magic string decoding failed\n");
        return e_failure;
    }
    printf("Decoded magic string: %s\n", decoded_magic);

    char file_extn[MAX_EXTN];   
    if(decode_secret_file_extn(decInfo, file_extn) == e_success)
    {
        printf("Secret file extension decoded successfully\n");
    }
    else
    {
        printf("Secret file extension decoding failed\n");
        return e_failure;
    }
    return e_success;
}

Status open_decode_files(DecodeInfo *decInfo)
{
    decInfo -> fptr_stego_image = fopen(decInfo -> stego_image_fname, "rb");      //open stego file
    if(decInfo -> fptr_stego_image == NULL)
    {
        printf("Stego file is not present\n");
        return e_failure;
    }
    else
    {
        printf("Stego file is opened\n");
    }

    decInfo -> fptr_output = fopen(decInfo -> output_fname, "wb");     //open output file
    if(decInfo -> fptr_output == NULL)
    {
        printf("Output file is not opened\\n");
        return e_failure;
    }
    else
    {
        printf("Output file is opened\n");
    }

    return e_success;
}

Status decode_magic_string(DecodeInfo *decInfo, const char *user_magic, char *decoded_magic)
{
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);  // Skip BMP header

    //Decode size of magic string
    char buffer[32];
    if (fread(buffer, 1, 32, decInfo->fptr_stego_image) != 32)
    {
        printf("Error reading magic string size\n");
        return e_failure;
    }

    int magic_len = 0;
    decode_size_from_lsb(&magic_len, buffer);

    if (magic_len <= 0 || magic_len >= 50)
    {
        printf("Invalid magic string size decoded: %d\n", magic_len);
        return e_failure;
    }

    //Decode actual magic string
    for (int i = 0; i < magic_len; i++)
    {
        char img_buffer[8];
        if (fread(img_buffer, 1, 8, decInfo->fptr_stego_image) != 8)
        {
            printf("Error reading image buffer for magic string\n");
            return e_failure;
        }
        decode_byte_from_lsb(img_buffer, &decoded_magic[i]);
    }

    decoded_magic[magic_len] = '\0';

    //Compare user input
    if (strcmp(user_magic, decoded_magic) != 0)
    {
        printf("Magic string does not match\n");
        return e_failure;
    }
    printf("Magic string is matched\n");
    return e_success;
}

Status decode_secret_file_extn(DecodeInfo *decInfo, char *file_extn)
{
    char buffer[32]; // To store 32 image bytes for 32 bits
    int extn_size = 0;
    
    if (fread(buffer, 1, 32, decInfo->fptr_stego_image) != 32)
    {
        printf("Error reading extension size\n");
        return e_failure;
    }

    decode_size_from_lsb(&extn_size, buffer);

    // Validate the extension size
    if (extn_size <= 0 || extn_size >= MAX_EXTN)
    {
        printf("Invalid extension size: %d\n", extn_size);
        return e_failure;
    }

    // Decode each character of extension
    for (int i = 0; i < extn_size; i++)
    {
        char img_buffer[8];
        if (fread(img_buffer, 1, 8, decInfo->fptr_stego_image) != 8)
        {
            printf("Error reading extension data\n");
            return e_failure;
        }
        decode_byte_from_lsb(img_buffer, &file_extn[i]);
    }

    file_extn[extn_size] = '\0'; // Null terminate
    return e_success;
}

Status decode_byte_from_lsb(char *image_buffer, char *data)
{
    unsigned char ch = 0;
    for (int i = 0; i < 8; i++)
    {
        ch = (ch << 1) | (image_buffer[i] & 1); 
    }
    *data = ch;    
    return e_success;
}

Status decode_size_from_lsb(int *size, char *image_buffer)
{
    *size = 0;
    for(int i = 0; i < 32; i++)
    {
        *size = (*size << 1) | (image_buffer[i] & 1);     
    }
    return e_success;
}