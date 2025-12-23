#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "types.h"
#include "encode.h"

//validate encoding input arguments
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    //validate source image file
    if(strstr(argv[2], ".bmp") != NULL)
    {
        printf("beautiful.bmp is present\n");
        encInfo -> src_image_fname = argv[2]; 
    }
    else
    {
       printf("beautiful.bmp is not present\n");
       return e_failure; 
    }

    //validate secret file
    if(strstr(argv[3], ".txt") != NULL)
    {
        printf("secret.txt is present\n");
        encInfo -> secret_fname = argv[3];
    }
    else
    {
       printf("secret.txt is not present\n");
       return e_failure; 
    }

    //validate or create default output file
    if(argv[4] != NULL)
    {
        if(strstr(argv[4], ".bmp") != NULL)
        {
            printf("stego.bmp is present\n");
            encInfo -> stego_image_fname = argv[4];
        }
        else
        {
            printf("Error: Output file must be .bmp\n");
            return e_failure; 
        }
    }
    else
    {
        printf("No output file provided. creating default stego.bmp\n");
        encInfo -> stego_image_fname = "stego.bmp";
    }
    return e_success;
}

//perfom encoding steps
Status do_encoding(EncodeInfo *encInfo)
{
    //open all required files
    if(open_files(encInfo) == e_success)
    {
        printf("All files are opened successfully\n");
    }
    else
    {
        printf("All files are not opened successfully\n");
        return e_failure;
    }

    //Get image size
    encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);
    
    //check if image has enough space
    if(check_capacity(encInfo) == e_success)
    {
        printf("Image has enough capacity to hold secret file\n");
    }
    else
    {
        printf("Image does not have enough capacity to hold secret file\n");
        return e_failure;
    }
    
    //copy BMP header to stego image
    if(copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
    {
        printf("BMP header copied successfully\n");
    }
    else
    {
        printf("BMP header copy failed\n");
        return e_failure;
    }
    
    //Ask the user for magic string
    char user_magic_string[20];
    printf("Enter the magic string: ");
    scanf("%19s", user_magic_string);
    
    if(encode_magic_string(user_magic_string, encInfo) == e_success)
    {
        printf("Magic string encoded successfully\n");
    }
    else
    {
        printf("Magic string encoding failed\n");
        return e_failure;
    }

    if(encode_secret_file_extn(strstr(encInfo -> secret_fname, ".txt"), encInfo) == e_success)
    {
        printf("Secret file extension encoded successfully\n");
    }
    else
    {
        printf("Secret file extension encoding failed\n");
        return e_failure;
    }

    encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret);
    if(encode_secret_file_size(encInfo -> size_secret_file, encInfo) == e_success)
    {
        printf("Secret file size encoded successfully\n");
    }
    else
    {
        printf("Secret file size encoding failed\n");
        return e_failure;
    }

    if(encode_secret_file_data(encInfo) == e_success)
    {
        printf("Secret file data encoded successfully\n");
    }
    else
    {
        printf("Secret file data encoding failed\n");
        return e_failure;
    }

    if(copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
    {
        printf("Remaining image data copied successfully\n");
    }
    else
    {
        printf("Remaining image data copy failed\n");
        return e_failure;
    }

    //close all files
    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_stego_image);

    return e_success;
}

Status open_files(EncodeInfo *encInfo)
{
    encInfo -> fptr_src_image = fopen(encInfo -> src_image_fname, "rb");

    if(encInfo -> fptr_src_image == NULL)
    {
        printf("Source file is not present\n");
        return e_failure;
    }
    else
    {
        printf("Source file is opened\n");
    }

    encInfo -> fptr_secret = fopen(encInfo -> secret_fname, "rb");
    if(encInfo -> fptr_secret == NULL)
    {
        printf("Secret file is not present\n");
        return e_failure;
    }
    else
    {
        printf("Secret file is opened\n");
    }

    encInfo -> fptr_stego_image = fopen(encInfo -> stego_image_fname, "wb");

    printf("Stego file is opened\n");

    return e_success;
}

//Function to get the image size for bmp
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;

    //Seek to width (offset 18 in BMP header)
    fseek(fptr_image, 18, SEEK_SET);
    fread(&width, sizeof(int), 1, fptr_image);

    //Seek to height (offset 22 in BMP header)
    fread(&height, sizeof(int), 1, fptr_image);

    //Reset file pointer to beginning
    fseek(fptr_image, 0, SEEK_SET);

    return width * height * 3;   //Each pixel has 3 bytes (R, G, B)
}

//Function to get the size of a file
uint get_file_size(FILE *fptr)
{
    uint current_pos = ftell(fptr);       //Get the current position of the file pointer
    fseek(fptr, 0, SEEK_END);             //Move file pointer to the end of the file
    uint size = ftell(fptr);              //Get the size of the file
    fseek(fptr, current_pos, SEEK_SET);   //Move th file pointer back to original position
    return size;
}

//Function to check whether image has enough capacity to hold secret file
Status check_capacity(EncodeInfo *encInfo)
{
    //Get the size of secret file
    uint secret_file_size = get_file_size(encInfo -> fptr_secret);

    //Calculate the required capacity
    uint required_capacity = 54 + (2 * 4 * 4 * 4 * secret_file_size * 8);

    //Get the size of image file
    uint image_size = get_file_size(encInfo -> fptr_src_image);

    //Compare image size with required capacity
    if(image_size >= required_capacity)
    {
        return e_success;
    }
    else
    {
        printf("Image capacity is not sufficient to hold the secret file\n");
        return e_failure;
    }
}

//Function to copy first 54 bytes of bmp header from source image to stego image
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char header[54];     

    //Move both file pointers to the starting position
    rewind(fptr_src_image);
    rewind(fptr_dest_image);

    //Read first 54 bytes from source image
    if(fread(header, 1, 54, fptr_src_image) != 54)
    {
        printf("Error in reading bmp header from source image\n");
        return e_failure;
    }

    //Write 54 bytes to destination image
    if(fwrite(header, 1, 54, fptr_dest_image) != 54)
    {
        printf("Error in writing bmp header to destination image\n");
        return e_failure;
    }

    return e_success;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    int magic_len = strlen(magic_string);

    // Encode the size of magic string first (32 bits)
    char buffer[32];
    fread(buffer, 1, 32, encInfo->fptr_src_image);
    encode_size_to_lsb(magic_len, buffer);
    fwrite(buffer, 1, 32, encInfo->fptr_stego_image);

    // Encode the actual magic string characters
    if (encode_data_to_image((char *)magic_string, magic_len, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char arr[8];

    for(int i = 0; i < size; i++)
    {
        if(fread(arr, 1, 8, fptr_src_image) != 8)
        {
            printf("Error in reading from the source image\n");
            return e_failure;
        }

        encode_byte_to_lsb(data[i], arr);

        if(fwrite(arr, 1, 8, fptr_stego_image) != 8)
        {
            printf("Error in writing to the stego image\n");
            return e_failure;
        }
    }

    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i= 0; i < 8; i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((data >> (7 - i)) & 1);
    }

    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    int size = strlen(file_extn);

    if(size > MAX_FILE_SUFFIX)
    {
        size = MAX_FILE_SUFFIX;
    }

    char arr[32] = {0};

    fread(arr, 1, 32, encInfo->fptr_src_image);

    encode_size_to_lsb(size, arr);

    fwrite(arr, 1, 32, encInfo->fptr_stego_image);

    encode_data_to_image((char *)file_extn, size, encInfo->fptr_src_image, encInfo->fptr_stego_image);

    return e_success;
}

Status encode_size_to_lsb(int size, char *image_buffer)
{
    for(int i = 0; i < 32; i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((size >> (31 - i)) & 1);
    }

    return e_success;
}

Status encode_secret_file_size(long size, EncodeInfo *encInfo)
{
    char arr[32] = {0};

    // Read next 32 bytes from source image
    for(int i = 0; i < 32; i++)
    {
        if(fread(&arr[i], 1, 1, encInfo->fptr_src_image) != 1)
        {
            printf("Error reading source image for secret file size\n");
            return e_failure;
        }
    }

    // Encode the size into LSBs
    encode_size_to_lsb(size, arr);

    // Write 32 bytes to stego image
    if(fwrite(arr, 1, 32, encInfo->fptr_stego_image) != 32)
    {
        printf("Error writing secret file size to stego image\n");
        return e_failure;
    }

    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    long size = encInfo -> size_secret_file;     //size of the secret file

    //Allocate buffer to hold the entire secret file
    char *buffer = (char *)malloc(size);

    if(buffer == NULL)
    {
        printf("Memory allocation failed\n");
        return e_failure;
    }

    //Read the entire secret file into buffer
    rewind(encInfo -> fptr_secret);         //Move file pointer to the beginning
    
    if(fread(buffer, 1, size, encInfo -> fptr_secret) != size)
    {
        printf("Failed to read secret file\n");
        free(buffer);
        return e_failure;
    }

    if(encode_data_to_image(buffer, size, encInfo -> fptr_src_image, encInfo -> fptr_stego_image) != e_success)
    {
        printf("Error in encoding secret file data\n");
        free(buffer);
        return e_failure;
    }

    free(buffer);
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char buffer[1024];  // Temporary buffer for copying
    size_t bytes_read;

    // Copy remaining bytes until end of file
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), fptr_src)) > 0)
    {
        if (fwrite(buffer, 1, bytes_read, fptr_dest) != bytes_read)
        {
            printf("Failed to write remaining image data to stego image\n");
            return e_failure;
        }
    }
    return e_success;
}
