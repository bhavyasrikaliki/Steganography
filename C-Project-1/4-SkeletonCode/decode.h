#ifndef DECODE_H
#define DECODE_H

#include "types.h"           //Contains user defined types 

#define MAX_EXTN 20 

typedef struct _DecodeIfo
{
    char *stego_image_fname;
    FILE *fptr_stego_image;

    char output_fname[50];
    FILE *fptr_output;
} DecodeInfo;

/*Decoding function prototype*/

/*Read and validate decode args from argv*/
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/*Perform the decoding*/
Status do_decoding(DecodeInfo *decInfo);

/*Get file pointers for i/p and o/p files*/
Status open_decode_files(DecodeInfo *decInfo);

/*Store magic string*/
Status decode_magic_string(DecodeInfo *decInfo, const char *user_magic, char *decoded_magic);

/*Decode secret file extension*/
Status decode_secret_file_extn(DecodeInfo *decInfo, char *file_extn);

/*decode a byte from lsb of image data*/
Status decode_byte_from_lsb(char *image_buffer, char *data);

/*decode size from lsb of image data*/
Status decode_size_from_lsb(int *size, char *image_buffer);

#endif