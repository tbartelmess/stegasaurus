#include <stdlib.h>
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


int
encode_message(const char* message, GifFileType* image)
{
  /**
   * The easy way of doing this would be to store the message into a GIF
   * extension using GifAddExtensionBlock, but that would be too esay to
   * get out again later... ;)
   */

//  size_t message_length = strlen(message);

  return GIF_OK;
}


/**
 * Prints the raw bit values of the image to stderr.
 */
void
print_images(GifFileType* gif)
{
  for (int i = 0; i < gif->ImageCount; i++) {
    fprintf(stderr, "\nImage %d:\n", i);
    SavedImage image  = gif->SavedImages[i];
    GifImageDesc desc = image.ImageDesc;

    fprintf(stderr, "Top: %d; Left: %d; Height: %d; Width: %d\n",
	    desc.Top, desc.Left, desc.Height, desc.Width);

    for (int height = 0; height < desc.Height; height++) {
      for (int width = 0; width < desc.Width; width++) {
	fprintf(stderr, "%x", image.RasterBits[width]);
      }
      fprintf(stderr, "\n");
    }
  }
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
  print_images(input);

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
