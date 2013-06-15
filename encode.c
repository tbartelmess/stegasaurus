#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <getopt.h>
#include <gif_lib.h>

// dirty global variables that getopt uses
extern char *optarg;
extern int optind;
extern int optopt;
extern int opterr;
extern int optreset;


static
void
usage(char** argv)
{
  fprintf(stderr,
	  "%s --message SECRET_MESSAGE < INPUT > OUTPUT",
	  argv[0]);
  exit(1);
}

char*
parse_encode_arguments(int argc, char** argv)
{
  static struct option options[] = {
    { "message", required_argument, NULL, 'm' },
    { NULL,      0,                 NULL, 0   }
  };

  int result  = 0;
  while ((result = getopt_long(argc, argv, "m", options, NULL)) != -1)
    switch (result)
      {
      case 'm':
	return optarg;
	break;
      default:
	usage(argv);
      }

  usage(argv);
  return NULL;
}


void
copy_gif(GifFileType* from, GifFileType* to)
{
  to->SWidth  = from->SWidth;
  to->SHeight = from->SHeight;
  to->SColorResolution = from->SColorResolution;
  to->SBackGroundColor = from->SBackGroundColor;
  to->SColorMap = GifMakeMapObject(from->SColorMap->ColorCount,
				   from->SColorMap->Colors);

  for (int i = 0; i < from->ImageCount; i++)
    GifMakeSavedImage(to, &from->SavedImages[i]);
}


/**
 * Scan through each raster byte in a gif image and yield the value
 * to the function pointer. Think of this as a stream of bytes, with
 * no notion of image, coloumn, or row boundaries passed to the the
 * function pointer.
 *
 * Optionally pass a context that will be given to the handler.
 */
void
scan_gif(GifFileType* gif, void (*handler)(int, void*), void* ctx)
{
  assert(handler);

  for (int i = 0; i < gif->ImageCount; i++) {
    SavedImage image  = gif->SavedImages[i];
    GifImageDesc desc = image.ImageDesc;

    for (int height = 0; height < desc.Height; height++) {
      int offset = height * desc.Width;
      for (int width = 0; width < desc.Width; width++)
	handler(image.RasterBits[offset + width], ctx);
    }
  }

}


int
encode_message(const char* message, GifFileType* image)
{
  /**
   * The easy way of doing this would be to store the message into a GIF
   * extension using GifAddExtensionBlock, but that would be too easy to
   * get out again later... ;)
   */

  // Just put the information as an extension block
  //extern int GifAddExtensionBlock(int *ExtensionBlock_Count,
  //				ExtensionBlock **ExtensionBlocks,
  //				int Function,
  //				unsigned int Len, unsigned char ExtData[]);

  return GIF_OK;
}


/**
 * Prints the raw bit values of the image to stderr.
 */
void
print_images(GifFileType* gif)
{
  if (gif->SColorMap) {
    fprintf(stderr, "Bits per pixel: %d\n",
	    gif->SColorMap->BitsPerPixel);
  }

  fprintf(stderr, "Total Images: %d\n", gif->ImageCount);

  for (int i = 0; i < gif->ImageCount; i++) {
    fprintf(stderr, "\nImage %d:\n", i);
    SavedImage image  = gif->SavedImages[i];
    GifImageDesc desc = image.ImageDesc;

    fprintf(stderr, "Top: %d; Left: %d; Height: %d; Width: %d\n",
	    desc.Top, desc.Left, desc.Height, desc.Width);

    if (desc.ColorMap) {
      fprintf(stderr, "Bits per pixel: %d\n",
	      desc.ColorMap->BitsPerPixel);
    }

    for (int height = 0; height < desc.Height; height++) {
      int offset = height * desc.Width;
      for (int width = 0; width < desc.Width; width++) {
	fprintf(stderr, "%x", image.RasterBits[offset + width]);
      }
      fprintf(stderr, "\n");
    }
  }
}


static
void
pixel_counter(int pixel, void* ctx)
{
  int* context = (int*)ctx;
  if (pixel == context[0]) {
    context[1]++;
  }
  else if (context[1]) {
    context[1] = 0;
    context[2]++;
  }
}

int
analyze_images(GifFileType* gif, int colour)
{
  int ctx[3] = { colour, 0, 0 };
  scan_gif(gif, pixel_counter, ctx);
  return ctx[2];
}


void
highest_colour(int pixel, void* ctx)
{
  int* context = (int*)ctx;
  if (pixel > *context)
    (*context)++;
}


int
find_highest_colour(GifFileType* gif)
{
  int colour = 0;
  scan_gif(gif, highest_colour, &colour);
  return colour;
}


int
stegasaurus_main(int argc, char** argv)
{
  int error = 0;

  // parse the secrect message that we want to encode
  char* message = parse_encode_arguments(argc, argv);

  // prepare to read in the input image
  GifFileType* input = DGifOpenFileHandle(STDIN_FILENO, &error);
  if (!input) {
    fprintf(stderr, "Input ERROR: %s\n", GifErrorString(error));
    return 1;
  }

  // read in the data for the input image
  if ((error = DGifSlurp(input)) == GIF_ERROR) {
    fprintf(stderr, "Input ERROR: %s\n", GifErrorString(error));
    return 1;
  }

  // prepare some memory for the output
  GifFileType* output = EGifOpenFileHandle(STDOUT_FILENO, &error);
  if (!output) {
    fprintf(stderr, "Output ERROR: %s\n", GifErrorString(error));
    return 1;
  }

  // make the initial copy of the input image into the output structure
  copy_gif(input, output);

  // use some tricks to encode the given message into the output structure
  //encode_message(message, output);
  //print_images(input);
  int colour = find_highest_colour(input);
  fprintf(stderr, "The target colour is 0x%x\n", colour);

  int count = analyze_images(input, colour);
  fprintf(stderr, "There are %zd bytes available for encoding\n", count);

  // write the output structure to file (standard out)
  if ((error = EGifSpew(output)) == GIF_ERROR) {
    fprintf(stderr, "Output ERROR: %s\n", GifErrorString(error));
    return 1;
  }

  // "cleanup"
  DGifCloseFile(input); // don't care much about errors in this case
  EGifCloseFile(output); // or in this case

  return 0;
}
