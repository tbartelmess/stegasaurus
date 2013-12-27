#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <gif_lib.h>
#include "scan_gif.h"

char* decode_message(GifFileType *gif, int colour) {
    fprintf(stderr, "Decoding message using colour:%d\n", colour);

    char * message = malloc(2);
    
    int message_offset = 0;
    for (int i = 0; i < gif->ImageCount; i++) {
        SavedImage image = gif->SavedImages[i];
        GifImageDesc desc = image.ImageDesc;
        int f = 0;

        if (desc.ColorMap && desc.ColorMap->BitsPerPixel < 8) {
            fprintf(stderr, "WARN: image %d uses less than "
	              "8 bit colour depth. Skipping...\n", i);
            continue;
        }

        for (int height = 0; height < desc.Height; height++) {
            int offset = height * desc.Width;

            for (int width = 0; width < desc.Width; width++) {
                int pixel = image.RasterBits[offset + width];
                if (pixel == colour) {
                    f++;
                } else if (f) {
                    f=0;
                    message[message_offset] = image.RasterBits[offset + width];
                    message_offset++;
                    message = realloc(message, sizeof(char));
                    if (message == NULL) {
                        fprintf(stderr, "Failed to realloc memory\n");
                    }
                    message[message_offset] = 0x0;
                }
            }
        }
    }
    return message;
}

int unstegasaurus_main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: unstegasaurus [file]");
        exit(1);
    }

    char * in_file = argv[1];
    fprintf(stderr, "Using %s\n", in_file);

    int error = GIF_OK;
    GifFileType* input = DGifOpenFileName(in_file, &error);

    if (!input) {
        fprintf(stderr, "Error loading file %s. ERROR: %s\n",
                in_file,
                GifErrorString(error));
        return 1;
    }

    if (input->SColorMap) {
        fprintf(stderr, "Bits per pixel: %d\n", input->SColorMap->BitsPerPixel);
    }

    if ((error = DGifSlurp(input)) == GIF_ERROR) {
        fprintf(stderr, "Input ERROR: %s\n", GifErrorString(error));
        return 1;
    }
    int colour = find_highest_colour(input);

    if (colour < 128) {
        fprintf(stderr, "ERROR: No dark colours in the image. Aborting...\n");
        return 1;

    } else {
        fprintf(stderr, "The target colour is 0x%x \n", colour);
    }

    char * message = decode_message(input, colour);
    fprintf(stdout, "%s", message);
    return 0;

}
