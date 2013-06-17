#include <unistd.h>
#include <assert.h>
#include "scan_gif.h"

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
      int offset = height * desc.Width * 3; // 3 because R, G, and B
      for (int width = 0; width < (desc.Width*3); width++)
        handler(image.RasterBits[offset + width], ctx);
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


static
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
