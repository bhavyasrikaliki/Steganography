/*
Documentation:
Name : K. Bhavya
Description : This program implements steganography - the process of hiding a secret text file inside a BMP image file 
              without visible changes to the image. It supports two operations:
              1. Encoding (-e) : Hide a secret file into a BMP image
              2. Decoding (-d) : Extract hidden secret file from the stego image
Date : 27-10-2025
sample I/O : 
$ ./a.out -e beautiful.bmp secret.txt stego.bmp
You have choosed encoding
beautiful.bmp is present
secret.txt is present
No output file provided. creating default stego.bmp
Read and validate is successful
Source file is opened
Secret file is opened
Stego file is opened
All files are opened successfully
Image has enough capacity to hold secret file
BMP header copied successfully
Enter the magic string: secret123
Magic string encoded successfully
Secret file extension encoded successfully
Secret file size encoded successfully
Secret file data encoded successfully
Remaining image data copied successfully
Encoding is successful

$ ./a.out -d stego.bmp
You have choosed decoding
Stego file is present
No output file provided. Creating default output.txt
Read and validate successful
Stego file is opened
Output file is opened
All files are opened successfully
Enter the same magic string used during encoding: secret123
Magic string is matched
Magic string decoded successfully
Decoded magic string: secret123
Secret file extension decoded successfully
Decoding is successful
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        printf("Invalid!\n");
        printf("For encoding : ./a.out -e <beautiful.bmp> <secret.txt> [stego.bmp]\n");
        printf("For decoding : ./a.out -d <stego.bmp> <output.txt>\n");
        return e_failure;
    }

    OperationType op_type = check_operation_type(argv);
    if(op_type == e_encode)
    {
        if(argc < 4)
        {
            printf("Error: Insufficient arguments for encoding.\n");
            printf("Usage: ./a.out -e <input.bmp> <secret.txt> [output.bmp]\n");
            return e_failure;
        }
        printf("You have choosed encoding\n");
        EncodeInfo encInfo;
        if(read_and_validate_encode_args(argv, &encInfo) == e_success)
        {
            printf("Read and validate is successful\n");
            if(do_encoding(&encInfo) == e_success)
            {
                printf("Encoding is successful\n");
            }
            else
            {
                printf("Encoding is unsuccessful\n");
                return e_failure;
            }
        }
        else
        {
            printf("Read and validate is unsuccessful\n");
            return e_failure;
        }
    }
    else if(op_type == e_decode)
    {
         if (argc < 3)
        {
            printf("Error: Insufficient arguments for decoding.\n");
            printf("Usage: ./a.out -d <stego.bmp> [output.txt]\n");
            return e_failure;
        }
        printf("You have choosed decoding\n");

        DecodeInfo decInfo;
        if(read_and_validate_decode_args(argv, &decInfo) == e_success)
        {
            printf("Read and validate successful\n");
            if(do_decoding(&decInfo) == e_success)
            {
                printf("Decoding is successful\n");
            }
            else
            {
                printf("Decoding is unsuccessful\n");
                return e_failure;
            }
        }
        else
        {
            printf("Read and validate unsuccessful\n");
            return e_failure;
        }
    }
    else
    {
        printf("It is not supporting\n");
        printf("Use -e for encoding and -d for decoding\n");
        return e_failure;
    }
    return e_success;
}

OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1], "-e") == 0)
    {
        return e_encode;
    }
    else if(strcmp(argv[1], "-d") == 0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}