/*
 * convert_to_labeled_directories.c
 *
 * Usage: convert_to_labeled_directories <labels_file> <images_file>
 *
 * Read the data from the raw MNIST format (either training or test)
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef __GNUC__
#include <string.h>
#include <arpa/inet.h>
#endif

// Illusion of type safety
typedef unsigned char byte;
typedef int bool;

// Function Declarations
int read_int(FILE *file);
byte *read_labels(char *filename);
byte *read_images(char *filename);
bool isDirectory(char *path);
void write_pgm_image(char *directory, char *filename, byte *image);

// Global State
int num_labels = 0;
int num_images = 0;
int width = 0;
int height = 0;

// Useful Constants
#define LABEL_MAGIC 2049
#define IMAGE_MAGIC 2051
#define PGM_MAGIC "P5"
#define MAX_GRAY 255
#define MAX_PATH 255
#define TRUE 1
#define FALSE 0

int main(int argc, char **argv) {
  if (argc != 4) {
    printf("Usage: convert_to_labeled_directories <labels_file> <images_file> <output_dir>\n");
    exit(1);
  }
  char *labels_file = argv[1];
  char *images_file = argv[2];
  char *output_dir = argv[3];

  byte *labels = read_labels(labels_file);
  printf("Read %d labels from: %s\n", num_labels, labels_file);

  byte *images = read_images(images_file);
  printf("Read %d images from: %s\n", num_images, images_file);

  if (num_images != num_labels) {
    fprintf(stderr, "Number of labels and number of images do not match, stopping.\n");
    exit(1);
  }

  if (!isDirectory(output_dir)) {
    fprintf(stderr, "Directory %s does not exist, please create it and try again.\n", output_dir);
    exit(1);
  }

  char directory[MAX_PATH];
  char filename[MAX_PATH];

  for (int i = 0; i < num_images; i++) {
    byte *current_image = &images[i * width * height];
    byte label = labels[i];

    bzero(directory, MAX_PATH);
    if (snprintf(directory, MAX_PATH, "%s/%d/", output_dir, label) >= MAX_PATH) {
      fprintf(stderr, "Directory path too long: %s, stopping.\n", directory);
      exit(1);
    }

    bzero(filename, MAX_PATH);
    if (snprintf(filename, MAX_PATH, "image%d.pgm", i) >= MAX_PATH) {
      fprintf(stderr, "Filename too long: %s, stopping.\n", filename);
      exit(1);
    }
    write_pgm_image(directory, filename, current_image);
  }
  
  free(labels);
  free(images);
}

/*
 * Open the file, allocate a buffer, and read the images data
 * The caller should free the memory when done.
 */
byte *read_images(char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    perror("Unable to read images file");
    exit(1);
  }

  int magic = read_int(file);
  if (magic != IMAGE_MAGIC) {
    fprintf(stderr, "%s is not an image file -- magic number does not match\n", filename);
    exit(1);
  }

  num_images = read_int(file);
  height = read_int(file);
  width = read_int(file);
  if (num_images <= 0 || height <= 0 || width <= 0) {
    fprintf(stderr, "Error reading image data: num_images=%d, height=%d, width=%d\n", num_images, height, width);
    exit(1);
  }

  int num_bytes = num_images * height * width;
  byte *images = calloc(num_bytes, sizeof(byte));
  
  int bytes_read = fread(images, sizeof(byte), num_bytes, file);
  if (bytes_read != num_bytes) {
    fprintf(stderr, "Error read %d bytes from images file, expected %d\n", bytes_read, num_bytes);
    exit(1);
  }
  fclose(file);

  return images;
}

/*
 * Open the file, allocate a buffer, and read the label data
 * The caller should free the memory when done.
 */
byte *read_labels(char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    perror("Unable to read labels file");
    exit(1);
  }

  int magic = read_int(file);

  if (magic != LABEL_MAGIC) {
    fprintf(stderr, "%s is not a label file -- magic number does not match\n", filename);
    exit(1);
  }

  num_labels = read_int(file);

  byte *labels = calloc(num_labels, sizeof(byte));
  int items_read = fread(labels, sizeof(byte), num_labels, file);
  if (items_read != num_labels) {
    fprintf(stderr, "Error reading from label file\n");
    exit(1);
  }
  fclose(file);

  return labels;
}

/*
 * Read an unsigned 32-bit integer value in big-endian format
 */
int read_int(FILE *file) {
  int item = 0;
  int items_read = fread(&item, sizeof(int), 1, file);
  if (items_read != 1) {
    fprintf(stderr, "Error reading int from file\n");
    exit(1);
  }
  return ntohl(item);
}

/*
 * Return a non-zero value if and only if the path is an accessible directory
 */
bool isDirectory(char *path) {
  struct stat info;

  if (stat(path, &info) != 0) {
    return FALSE;
  }
  return (info.st_mode & S_IFDIR);
}

/*
 * Save the image in PGM format with the path specified by the directory and filename provided
 */
void write_pgm_image(char *directory, char *filename, byte *image) {
  if (!isDirectory(directory)) {
    if (mkdir(directory, 0755) < 0) {
      perror("Unable to create directory");
      exit(1);
    }
  }

  char path[MAX_PATH];
  bzero(&path, MAX_PATH);
  if (strlcpy(path, directory, MAX_PATH) >= MAX_PATH) {
    fprintf(stderr, "Directory path too long: %s, stopping.\n", path);
    exit(1);
  }
  if (strlcat(path, filename, MAX_PATH) >= MAX_PATH) {
    fprintf(stderr, "File path too long: %s, stopping.\n", path);
    exit(1);
  }

  FILE *outfile = fopen(path, "w");
  if (outfile == NULL) {
    perror("Unable to open image file for writing");
    exit(1);
  }

  fprintf(outfile, "%s %d %d %d\n", PGM_MAGIC, width, height, MAX_GRAY);
  int num_pixels = width * height;
  if (fwrite(image, sizeof(byte), num_pixels, outfile) != num_pixels) {
    perror("Unable to write to image file");
    exit(1);
  }

  fclose(outfile);
}
