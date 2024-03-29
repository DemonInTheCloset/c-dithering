#include "pngio.h"

bool png_chunck_is_of_type(const struct png_chunk *chunk, const char type[4]) {
  return (chunk->chunk_type[0] == type[0]) & (chunk->chunk_type[1] == type[1])
         & (chunk->chunk_type[2] == type[2]) & (chunk->chunk_type[3] == type[3]);
}

struct png_chunk *make_chunk(uint32_t length) {
  struct png_chunk *chunk = (struct png_chunk *)malloc(sizeof(struct png_chunk) + 4 + length);

  if (!chunk) {
    return NULL;
  }

  chunk->length = length;

  return chunk;
}

uint32_t chunck_crc(struct png_chunk *chunk) {
  uint32_t crc = (uint8_t)chunk->chunk_type[0];

  for (int i = 1; i < 4; ++i) {
    crc = _mm_crc32_u8(crc, chunk->chunk_type[i]);
  }

  for (uint32_t i = 0; i < chunk->length; ++i) {
    crc = _mm_crc32_u8(crc, chunk->data[i]);
  }

  return crc;
}

struct png_chunk_header {
  uint32_t length;
  char chunk_type[4];
};

/* return 0:  read chunck correctly
 *        1:  couldn't read chunk
 *        2:  malloc failed
 */
static int png_read_chunk(FILE *file, struct png_chunk **chunk) {
  struct png_chunk_header chunk_header;

  *chunk = NULL;

  if (fread(&chunk_header, sizeof(chunk_header), 1, file) != 1) {
    return 1;
  }

  chunk_header.length = __builtin_bswap32(chunk_header.length);

  *chunk = make_chunk(chunk_header.length);

  if (!chunk) {
    fprintf(stderr, "Error: Failed to allocate %luB\n", chunk_header.length + 4 + sizeof(struct png_chunk));
    return 2;
  }

  memcpy((*chunk)->chunk_type, &chunk_header.chunk_type, sizeof(chunk_header.chunk_type)); // NOLINT

  if (fread((*chunk)->data, 1, chunk_header.length + 4, file) != chunk_header.length + 4) {
    return 2;
  }

  return 0;
}

static void free_chunks(size_t n, struct png_chunk *chunks[n]) {
  for (size_t i = 0; i < n; ++i) {
    free(chunks[i]);
  }
}

struct png_img png_read(const char *path) {
  struct png_img img;
  FILE *src;
  if ((src = fopen(path, "r")) == NULL) {
    return (struct png_img){.header = {0}, .num_chunks = 0, .chunks = NULL};
  }

  int read;
  if ((read = fread(&img.header, sizeof(img.header), 1, src)) != 1) {
    fprintf(stderr, "Error: expected to read %d, read  %d\n", 1, read);
    fclose(src);
    return (struct png_img){.header = {0}, .num_chunks = 0, .chunks = NULL};
  }

  size_t n = 0, capacity = 2;
  img.chunks = malloc(sizeof(struct png_chunk *) * capacity);

  if (!img.chunks) {
    fclose(src);
    fprintf(stderr, "Error: Failed to allocate %luB\n", sizeof(struct png_chunk *) * capacity);
    return (struct png_img){.header = {0}, .num_chunks = 0, .chunks = NULL};
  }

  int err = 0;
  while (1) {
    // Grow chunk array (Grow Factor 1.5)
    if (n == capacity) {
      capacity += capacity / 2;

      struct png_chunk **new_chunks;
      if ((new_chunks = realloc(img.chunks, sizeof(struct png_chunk *) * capacity)) == NULL) {
        err = 2;
        fprintf(stderr, "Error: Failed to allocate %luB\n", sizeof(struct png_chunk *) * capacity);
        break;
      }

      img.chunks = new_chunks;
    }

    if ((err = png_read_chunk(src, &img.chunks[n++])) != 0) break;

    // Reached EOF
    if (png_chunck_is_of_type(img.chunks[n - 1], "IEND")) break;
  }
  img.num_chunks = n;

  fclose(src);

  if (err) {
    free_chunks(img.num_chunks, img.chunks);
    free(img.chunks);

    return (struct png_img){.header = {0}, .num_chunks = 0, .chunks = NULL};
  }

  return img;
}

void free_png_img(struct png_img img) {
  free_chunks(img.num_chunks, img.chunks);
  free(img.chunks);
  img.chunks = NULL;
}

void print_png_header(struct png_header header) {
  printf("%#02X : %.3s : 0x%02X%02X : %#02X : %#02X\n", header.MARK, header.PNG, header.DOS_CRLF[0],
         header.DOS_CRLF[1], header.DOS_EOF[0], header.UNX_LF[0]);
}

struct png_header_chunk {
  uint32_t length;
  char chunk_type[4];
  uint32_t img_w;
  uint32_t img_h;
  uint8_t bit_depth;
  uint8_t color_type;
  uint8_t compression_method;
  uint8_t filter_method;
  uint8_t interlace_method;
  uint32_t crc32;
} __attribute((packed));

static struct png_header_chunk to_png_header_chunk(struct png_chunk *chunk) {
  struct png_header_chunk header = *((struct png_header_chunk *)chunk);

  header.img_w = __builtin_bswap32(header.img_w);
  header.img_h = __builtin_bswap32(header.img_h);

  return header;
}

void print_png_chunk_info(struct png_chunk *chunk) {
  uint32_t crc = (chunk->data[chunk->length + 0] << 6) | (chunk->data[chunk->length + 1] << 4)
                 | (chunk->data[chunk->length + 2] << 2) | (chunk->data[chunk->length + 3] << 0);
  printf("%#08X : %.4s : %#08X\n", chunk->length, chunk->chunk_type, crc);

  if (png_chunck_is_of_type(chunk, "IHDR") && chunk->length == 13) {
    struct png_header_chunk header = to_png_header_chunk(chunk);
    printf("size %ux%u : depth %u : color type %u : compression %u : filter %u : interlace %u\n",
           header.img_w, header.img_h, header.bit_depth, header.color_type,
           header.compression_method, header.filter_method, header.interlace_method);
  }
}

void print_png_img_headers(struct png_img img) {
  print_png_header(img.header);

  printf("PNG image with %lu chunks:\n", img.num_chunks);

  for (uint32_t i = 0; i < img.num_chunks; ++i) {
    print_png_chunk_info(img.chunks[i]);
  }
}
