#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct ascii_art {
  size_t width;
  size_t height;
  size_t length;
  char* body;
};

typedef struct ascii_art ascii_art;

ascii_art* ascii_render(size_t const in_width, size_t const in_height,
                        size_t const out_width,
                        unsigned char const* const pixels) {
  static char constexpr ascii_scale[] = " .,-~:;=!*#$@";
  static size_t constexpr ascii_scale_len = sizeof(ascii_scale) - 1;

  size_t out_height = out_width * in_height / in_width;
  size_t length = out_width * out_height;
  size_t w_group_size = (in_width + out_width - 1) / out_width;
  size_t h_group_size = (in_height + out_height - 1) / out_height;

  char* body = malloc(length);
  size_t position = 0;

  for (size_t j = 0; j < in_height; j += h_group_size) {
    for (size_t i = 0; i < in_width; i += w_group_size) {
      size_t s = 0;
      size_t n = 0;
      for (size_t j0 = j; j0 < in_height && j0 < j + h_group_size; ++j0) {
        for (size_t i0 = i; i0 < in_width && i0 < i + w_group_size; ++i0) {
          size_t pixel_idx = j0 * in_width + i0;
          s += (size_t)pixels[pixel_idx];
          ++n;
        }
      }
      size_t avg = s / n;
      size_t ascii_idx = (ascii_scale_len - 1) * avg / 255;
      body[position] = ascii_scale[ascii_idx];
      ++position;
    }
  }

  ascii_art* art = malloc(sizeof(ascii_art));
  art->width = out_width;
  art->height = out_height;
  art->length = length;
  art->body = body;

  return art;
}

void ascii_draw(ascii_art const* art) {
  for (size_t j = 0; j < art->height; ++j) {
    for (size_t i = 0; i < art->width; ++i) {
      size_t idx = j * art->width + i;
      char c = art->body[idx];
      printf("%c%c", c, c);
    }
    printf("\n");
  }
}

int main(int argc, char* argv[argc + 1]) {
  if (argc < 2) {
    printf("Usage: asciipxl path/to/img.jpg [out_width]\n");
    return EXIT_SUCCESS;
  }

  char* const imgpath = argv[1];
  size_t out_width = 32;

  if (argc > 2) {
    // long int strtol(const char *nptr, char **endptr, int base);
    char* endptr = nullptr;
    long w = strtol(argv[2], &endptr, 10);
    if (*endptr) {
      printf("Error: parsing out_width at %c\n", *endptr);
      return EXIT_FAILURE;
    }
    out_width = (size_t)w;
  }

  int width = 0;
  int height = 0;
  int channels = 0;

  unsigned char* pixels = stbi_load(imgpath, &width, &height, &channels, 1);
  ascii_art* art =
      ascii_render((size_t)width, (size_t)height, out_width, pixels);

  printf("%dx%d -> %zux%zu\n", width, height, art->width, art->height);
  ascii_draw(art);

  stbi_image_free(pixels);
  free(art);

  return EXIT_SUCCESS;
}
