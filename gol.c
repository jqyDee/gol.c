#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "dir.h"

// 1 is black; 0 is white
#define ALIVE_CHAR_VALUE '0'
#define DEAD_CHAR_VALUE '1'

static void print_usage(const char *programName) {
  printf("usage: %s <width> <height> <density> <steps> <startingpos>\n",
         programName);
}

void clear_array(int **array, const int height, const int width) {
  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      array[row][col] = false;
    }
  }
}

void free_array(int **array, const int height) {
  if (!array)
    return;

  for (int row = 0; row < height; row++) {
    if (array[row])
      free(array[row]);
  }
  free(array);
}

int **allocate_array(const int width, const int height) {
  int **array = malloc(sizeof(int *) * height);
  if (!array)
    return NULL;

  for (int row = 0; row < height; row++) {
    array[row] = malloc(sizeof(int) * width);
    if (!array[row])
      free_array(array, height);
  }

  return array;
}

static void initialize_array_glider(int **array, const int width,
                                    const int height) {
  int middleX = width / 2;
  int middleY = height / 2;

  // OOX
  // XOX
  // OXX

  // first line
  array[middleY - 1][middleX + 1] = true;

  // second line
  array[middleY][middleX - 1] = true;
  array[middleY][middleX + 1] = true;

  // third line
  array[middleY + 1][middleX] = true;
  array[middleY + 1][middleX + 1] = true;
}

static void initialize_array_random(int **array, const int width,
                                    const int height, const float density) {
  float curr_probability;
  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      curr_probability = (float)rand() / RAND_MAX;
      if (curr_probability < density) {
        array[row][col] = true;
      }
    }
  }
}

bool initialize_arrays(int **array_bef, const int width, const int height,
                       const char *startpos, const float density) {
  if (strcmp(startpos, "random") == 0) {
    initialize_array_random(array_bef, width, height, density);
  } else if (strcmp(startpos, "glider") == 0) {
    initialize_array_glider(array_bef, width, height);
  } else {
    return false;
  }
  return true;
}

void swap_arrays(int ***array1, int ***array2) {
  int **array_buffer = *array1;
  *array1 = *array2;
  *array2 = array_buffer;
}

static size_t count_surrounding(int **array, const int row, const int col,
                                const int width, const int height) {
  size_t surrounding = 0;

  // THIS IS NOT A TOROIDIAL ARRAY SO NO WRAP AROUND
  // EDGES INTERRUPT ALIVE CELLS AND MOST OF THE TIME CREATE STATIC PATTERNS

  // check if at the edge of the field
  int y_offset_top = -1;
  if (row == 0) {
    y_offset_top = 0;
  }
  int x_offset_left = -1;
  if (col == 0) {
    x_offset_left = 0;
  }
  int y_offset_bottom = 1;
  if (row == height - 1) {
    y_offset_bottom = 0;
  }
  int x_offset_right = 1;
  if (col == width - 1) {
    x_offset_right = 0;
  }

  for (int i = y_offset_top; i <= y_offset_bottom; i++) {
    for (int j = x_offset_left; j <= x_offset_right; j++) {
      if (array[row + i][col + j] && !(i == 0 && j == 0)) {
        surrounding++;
      }
    }
  }
  return surrounding;
}

static bool check_alive(const bool alive, const size_t surrounding) {
  // ALIVE
  if (alive) {
    // UNDERPOPULATION
    if (surrounding < 2) {
      return false;
      // OVERPOPULATION
    } else if (surrounding > 3) {
      return false;
      // CORRECT POPULATION
    } else {
      return true;
    }
    // REPRODUCTION
  } else if (surrounding == 3) {
    return true;
    // DEAD AND NO REPRODUCTION
  } else {
    return false;
  }
}

void simulate_step(int **src_array, int **dst_array, const int width,
                   const int height) {
  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      size_t surrounding =
          count_surrounding(src_array, row, col, width, height);
      dst_array[row][col] = check_alive(src_array[row][col], surrounding);
    }
  }
}

bool create_pbm(int **array, const int width, const int height,
                const int step) {
  // create filename
  char filename[32];
  sprintf(filename, "./pbm/gol_%05d.pbm", step);

  FILE *fd = fopen(filename, "w");
  if (!fd) {
    fprintf(stderr, "error: %s\n", strerror(errno));
    return false;
  }

  fprintf(fd, "P1\n");
  fprintf(fd, "%d %d\n", width, height);
  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      if (array[row][col]) {
        if (fputc(ALIVE_CHAR_VALUE, fd) == EOF) {
          fprintf(stderr, "error: %s\n", strerror(errno));
          return false;
        }
      } else {
        if (fputc(DEAD_CHAR_VALUE, fd) == EOF) {
          fprintf(stderr, "error: %s\n", strerror(errno));
          return false;
        }
      }
    }

    // no newline at end of file
    if (row != height - 1)
      fputc('\n', fd);
  }

  fclose(fd);
  return true;
}

int main(int argc, char *argv[]) {
  if (argc != 6) {
    print_usage(argv[0]);
    return EXIT_FAILURE;
  }

  const int width = atoi(argv[1]);
  const int height = atoi(argv[2]);
  const double density = atof(argv[3]);
  const int steps = atoi(argv[4]);
  const char *startpos = argv[5];

  printf("width:    %4d\n", width);
  printf("height:   %4d\n", height);
  printf("density:  %4.0f%%\n", density * 100);
  printf("steps:    %4d\n", steps);
  printf("startpos:  %s\n", startpos);

  // Seeding the random number generator so we get a different starting array
  // every time.
  srand(time(NULL));

  if (width < 1 || height < 1) {
    fprintf(stderr, "error: width and height have to be greater or "
                    "equal to 1\n");
    return 1;
  }

  create_dir_if_not_exists("./pbm");

  // create arrays
  int **array_bef = allocate_array(width, height);
  if (!array_bef) {
    fprintf(stderr, "error: couldn't allocate array\n");
    return 1;
  }
  int **array_aft = allocate_array(width, height);
  if (!array_aft) {
    fprintf(stderr, "error: couldn't allocate array\n");
    return 1;
  }

  // clear array
  clear_array(array_bef, height, width);
  clear_array(array_aft, height, width);

  // initialize array(s)
  if (!initialize_arrays(array_bef, width, height, startpos, density)) {
    print_usage(argv[0]);
    return 1;
  }

  // simulate
  for (int i = 0; i < steps; i++) {
    if (!create_pbm(array_bef, width, height, i)) {
      return 1;
    }
    simulate_step(array_bef, array_aft, width, height);
    swap_arrays(&array_bef, &array_aft);
  }

  // free arrays
  free_array(array_bef, height);
  free_array(array_aft, height);

  return EXIT_SUCCESS;
}
