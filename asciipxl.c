#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct asciipxl_art {
  size_t width;
  size_t height;
  size_t length;
  char* body;
};

typedef struct asciipxl_art asciipxl_art;

void asciipxl_free(asciipxl_art* art) {
  if (art) {
    if (art->body) {
      free(art->body);
    }
    free(art);
  }
}

asciipxl_art* asciipxl_render(size_t const in_width, size_t const in_height,
                              size_t const desired_out_width,
                              unsigned char const* const pixels) {
  static char constexpr asciipxl_scale[] = " .,-~:;=!*#$@";
  // static char constexpr asciipxl_scale[] = "@$#*!=;:~-,. ";
  static size_t constexpr asciipxl_scale_len = sizeof(asciipxl_scale) - 1;

  if (desired_out_width == 0) {
    return nullptr;
  }

  asciipxl_art* art = calloc(1, sizeof(asciipxl_art));
  if (!art) {
    return nullptr;
  }

  size_t out_width = desired_out_width;
  size_t w_group_size = in_width / out_width;
  if (in_width % out_width > 0) {
    out_width -= 1;
    w_group_size = in_width / out_width;
    if (in_width % out_width > 0) {
      out_width += 1;
    }
  }

  size_t out_height = desired_out_width * in_height / in_width;
  size_t h_group_size = in_height / out_height;
  if (in_height % out_height > 0) {
    out_height -= 1;
    h_group_size = in_height / out_height;
    if (in_height % out_height > 0) {
      out_height += 1;
    }
  }

  size_t length = out_width * out_height;
  char* body = malloc(length);
  if (!body) {
    asciipxl_free(art);
    return nullptr;
  }

  for (size_t j = 0; j < out_height; ++j) {
    for (size_t i = 0; i < out_width; ++i) {
      size_t p = j * out_width + i;
      size_t s = 0;
      size_t n = 0;

      for (size_t j0 = 0; j0 < h_group_size; ++j0) {
        for (size_t i0 = 0; i0 < w_group_size; ++i0) {
          size_t pixel_j = (j * h_group_size + j0);
          if (pixel_j >= in_height) {
            continue;
          }
          size_t pixel_i = (i * w_group_size + i0);
          if (pixel_i >= in_width) {
            continue;
          }
          size_t pixel_idx = pixel_j * in_width + pixel_i;
          s += (size_t)pixels[pixel_idx];
          ++n;
        }
      }

      size_t avg = (n == 0 ? 0 : (s / n));
      size_t asciipxl_idx = (asciipxl_scale_len - 1) * avg / 255;
      body[p] = asciipxl_scale[asciipxl_idx];
    }
  }

  art->width = out_width;
  art->height = out_height;
  art->length = length;
  art->body = body;

  return art;
}

void asciipxl_draw(asciipxl_art const* art) {
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
    printf("Usage: asciipxl path/to/img.jpg [desired_out_width]\n");
    return EXIT_SUCCESS;
  }

  char* const imgpath = argv[1];
  size_t desired_out_width = 32;

  if (argc > 2) {
    // long int strtol(const char *nptr, char **endptr, int base);
    char* endptr = nullptr;
    long w = strtol(argv[2], &endptr, 10);
    if (*endptr) {
      printf("Error: parsing desired_out_width at %c\n", *endptr);
      return EXIT_FAILURE;
    }
    desired_out_width = (size_t)w;
  }

  int width = 0;
  int height = 0;
  int channels = 0;

  unsigned char* pixels = stbi_load(imgpath, &width, &height, &channels, 1);
  if (!pixels) {
    printf("Error: failed to load image %s\n", imgpath);
    return EXIT_FAILURE;
  }

  if (desired_out_width > width) {
    desired_out_width = width;
  }

  asciipxl_art* art =
      asciipxl_render((size_t)width, (size_t)height, desired_out_width, pixels);
  if (!art) {
    printf("Error: failed to render ascii art\n");
    stbi_image_free(pixels);
    return EXIT_FAILURE;
  }

  printf("%dx%d -> %zux%zu\n", width, height, art->width, art->height);
  asciipxl_draw(art);

  stbi_image_free(pixels);
  asciipxl_free(art);

  return EXIT_SUCCESS;
}
