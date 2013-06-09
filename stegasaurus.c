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

  int error = 0;
  GifFileType* image = DGifOpenFileHandle(input_fd, &error);
  if (!image) handle_error(error);

  handle_error(DGifSlurp(image));

  // TODO refactor
  // cleanup
  handle_error(DGifCloseFile(image));
  close(input_fd);
  close(output_fd);

  return 0;
}
