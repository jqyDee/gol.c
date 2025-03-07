#ifndef GOL_H
#define GOL_H

void clear_array(int **array, const int height, const int width);
void free_array(int **array, const int height);
int **allocate_array(const int width, const int height);
bool initialize_arrays(int **array_bef, const int width, const int height,
                       const char *startpos, const float density);
void swap_arrays(int ***array1, int ***array2);
void simulate_step(int **src_array, int **dst_array, const int width,
                   const int height);

#endif
