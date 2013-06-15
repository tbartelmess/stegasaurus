#include <gif_lib.h>

void
scan_gif(GifFileType* gif, void (*handler)(int, void*), void* ctx);

int
analyze_images(GifFileType* gif, int colour);

int
find_highest_colour(GifFileType* gif);
