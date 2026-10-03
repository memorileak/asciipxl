#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

char const ascii_scale[] = " .,-~:;=!*#$@";
size_t const ascii_scale_len = sizeof(ascii_scale) - 1;

int main(void) {
  int int_width = 0;
  int int_height = 0;
  int int_channels = 0;

  unsigned char* pixels =
      stbi_load("doge.jpg", &int_width, &int_height, &int_channels, 1);

  printf("%d x %d, %d channels\n", int_width, int_height, int_channels);

  size_t width = (size_t)int_width;
  size_t height = (size_t)int_height;

  size_t max_out_width = 64;
  size_t max_out_height = max_out_width * height / width;
  size_t w_group_size = (width + max_out_width - 1) / max_out_width;
  size_t h_group_size = (height + max_out_height - 1) / max_out_height;

  for (size_t j = 0; j < height; j += h_group_size) {
    for (size_t i = 0; i < width; i += w_group_size) {
      size_t s = 0;
      size_t n = 0;
      for (size_t j0 = j; j0 < height && j0 < j + h_group_size; ++j0) {
        for (size_t i0 = i; i0 < width && i0 < i + w_group_size; ++i0) {
          size_t pixel_idx = j0 * width + i0;
          s += (size_t)pixels[pixel_idx];
          ++n;
        }
      }
      size_t avg = s / n;
      size_t ascii_idx = (ascii_scale_len - 1) * avg / 255;
      printf("%c%c", ascii_scale[ascii_idx], ascii_scale[ascii_idx]);
    }
    printf("\n");
  }

  stbi_image_free(pixels);

  return EXIT_SUCCESS;
}
