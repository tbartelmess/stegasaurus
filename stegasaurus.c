#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
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
  char* input   = NULL;
  char* output  = NULL;

  int getopt_result = 0;
  while ((getopt_result = getopt_long(argc, argv, "mio:", options, NULL)) != -1)
    switch (getopt_result)
      {
      case 'm':
	message = optarg;
	break;
      case 'i':
	input = optarg;
	break;
      case 'o':
	output = optarg;
	break;
      default:
	usage(argv);
      }

  if (!(message && input && output))
    usage(argv);

  printf("Message:         %s\n", message);
  printf("Input GIF:       %s\n", input);
  printf("Stegasaurus GIF: %s\n", output);

  return 0;
}
