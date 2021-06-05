#pragma once
#include <stddef.h>
#include <stdint.h>

// Pixels
struct pix_rgb {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

struct pix_rgba {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
};

struct pix_gs {
  uint8_t g;
};

struct pix_gsa {
  uint8_t g;
  uint8_t a;
};

// Image
struct img_rgb {
  uint32_t w;
  uint32_t h;
  struct pix_rgb data[];
};

struct img_rgba {
  uint32_t w;
  uint32_t h;
  struct pix_rgba data[];
};

struct img_gs {
  uint32_t w;
  uint32_t h;
  struct pix_gs data[];
};

struct img_gsa {
  uint32_t w;
  uint32_t h;
  struct pix_gsa data[];
};
