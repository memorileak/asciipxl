#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

char const ascii_scale[] =
    "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";
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

  for (size_t j = 0; j < height; ++j) {
    for (size_t i = 0; i < width; ++i) {
      size_t pixel_idx = j * width + i;
      size_t ascii_idx =
          (ascii_scale_len - 1) * (size_t)pixels[pixel_idx] / 255;
      printf("%c%c", ascii_scale[ascii_idx], ascii_scale[ascii_idx]);
    }
    printf("\n");
  }

  stbi_image_free(pixels);

  return EXIT_SUCCESS;
}
