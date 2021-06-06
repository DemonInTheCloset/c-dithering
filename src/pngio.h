#include <nmmintrin.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct png_header {
  uint8_t MARK;
  char PNG[3];
  char DOS_CRLF[2];
  char DOS_EOF[1];
  char UNX_LF[1];
};

static const struct png_header PNG_HEADER = {
.MARK = 0x89,
.PNG = {'P', 'N', 'G'},
.DOS_CRLF = {0x0D, 0x0A},
.DOS_EOF = {0x1A},
.UNX_LF = {0x0A},
};

struct png_chunk {
  uint32_t length;
  char chunk_type[4];
  uint8_t data[];
};

struct png_img {
  struct png_header header;
  size_t num_chunks;
  struct png_chunk **chunks;
};

static struct png_chunk *make_chunk(uint32_t length);

static uint32_t chunck_crc(struct png_chunk *chunk);

static struct png_img png_read(const char *path);

static void free_png_img(struct png_img img);

static void print_png_header(struct png_header header);

static void print_png_chunk_info(struct png_chunk *chunk);

static void print_png_img_headers(struct png_img img);
