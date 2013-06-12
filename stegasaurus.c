#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
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
  printf("%s "
	 "--message SECRET "
	 "--input INPUT_IMAGE "
	 "--output OUTPUT_IMAGE\n",
	 argv[0]);
  exit(1);
}


static
void
handle_error(int error)
{
  if (error == GIF_ERROR) {
    char* message = GifErrorString(error);
    printf("ERROR: %s\n", message);
    exit(1);
  }
}


int
main(int argc, char** argv)
{

  // options descriptor
  static struct option options[] = {
    { "message", required_argument, NULL, 'm' },
    { "input",   required_argument, NULL, 'i' },
    { "output",  required_argument, NULL, 'o' },
    { NULL,      0,                 NULL,  0  }
  };

  char* message = NULL;
  int input_fd  = 0;
  int output_fd = 0;

  int getopt_result = 0;
  while ((getopt_result = getopt_long(argc, argv, "mio:", options, NULL)) != -1)
    switch (getopt_result)
      {
      case 'm':
	message = optarg;
	break;
      case 'i':
	input_fd = open(optarg, O_RDONLY);
	if (input_fd == -1) {
	  perror("Failed to open input file");
	  exit(1);
	}
	break;
      case 'o':
	output_fd = open(optarg, O_WRONLY|O_CREAT|O_TRUNC, 0644);
	if (output_fd == -1) {
	  perror("Failed to open output file");
	  exit(1);
	}
	break;
      default:
	usage(argv);
      }

  if (!(message && input_fd && output_fd))
    usage(argv);


  // GET THE PARTY STARTED HERE


  int error = 0;
  GifFileType* image = DGifOpenFileHandle(input_fd, &error);
  if (!image) handle_error(error);


  handle_error(DGifSlurp(image));



  GifFileType* stegasaurus = EGifOpenFileHandle(output_fd, &error);
  if (!stegasaurus) handle_error(error);

  stegasaurus->SWidth  = image->SWidth;
  stegasaurus->SHeight = image->SHeight;
  stegasaurus->SColorResolution = image->SColorResolution;
  stegasaurus->SBackGroundColor = image->SBackGroundColor;
  stegasaurus->SColorMap = GifMakeMapObject(image->SColorMap->ColorCount,
					    image->SColorMap->Colors);

  for (int i = 0; i < image->ImageCount; i++) {
    // split the message up evenly amongst the images
    //
    GifMakeSavedImage(stegasaurus, &image->SavedImages[i]);
    for (int j = 0; j < 100; j++) {
      stegasaurus->SavedImages[i].RasterBits[j] =
	stegasaurus->SavedImages[i].RasterBits[j] | 0xff;
    }
  }

  if (EGifSpew(stegasaurus) == GIF_ERROR)
    printf("fuuuu\n");

  // TODO refactor
  // cleanup
  if (DGifCloseFile(image) == GIF_ERROR)
    printf("error that we don't care about much\n");
  close(input_fd);

  if (EGifCloseFile(stegasaurus) == GIF_ERROR) {
    printf("error that might cause problems later: %s\n",
	   GifErrorString(stegasaurus->Error));
  }
  close(output_fd);

  return 0;
}
