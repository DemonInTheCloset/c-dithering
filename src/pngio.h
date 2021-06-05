#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

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
