#include "pngio.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

static const char usage[] = "usage: %s [-h] [-o OUTPUT] FILE\n"
                            "\nREQUIRED:\n"
                            "\tFILE      \tPath to the image to load\n"
                            "\nOPTIONAL:\n"
                            "\t-h        \tPrint this help message\n"
                            "\t-o OUTPUT \tPath to the output file (default: out.png)\n";

struct args {
  char *src;
  char *dst;
};

static struct args get_args(int argc, char **argv) {
  struct args args = {.src = NULL, .dst = "out.png"};
  int opt;

  while ((opt = getopt(argc, argv, "ho:")) != -1) {
    switch (opt) {
      case 'o':
        args.dst = optarg;
        break;
      case 'h':
        printf(usage, *argv);
        exit(EXIT_SUCCESS);
      case '?':
        fprintf(stderr, "Unknown option: %c\n", optopt);
        printf(usage, *argv);
        exit(EXIT_FAILURE);
      default:
        fprintf(stderr, "Unhandled option: %c\n", optopt);
        printf(usage, *argv);
        exit(EXIT_FAILURE);
    }
  }

  if (optind != argc - 1) {
    fprintf(stderr, "Expected FILE but none/too many given\n");
    printf(usage, *argv);
    exit(EXIT_FAILURE);
  }

  args.src = argv[optind];

  return args;
}

int main(int argc, char **argv) {
  struct args args = get_args(argc, argv);

  struct png_img img = png_read(args.src);

  print_png_img_headers(img);

  return EXIT_SUCCESS;
}
