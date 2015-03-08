#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>

#include "julia.h"

#define NUM_PROCS 17

int main(int argc, char *argv[])
{
  clock_t start_t, end_t, total_t;
  start_t = clock();

  int width, height, maxiter, flag;
  int num_procs;

  double x[2], y[2], c[2];
  char *image, *stats;
  getParams(argv, &flag, c, x, y, &width, &height, &maxiter, &image, &stats);

  int *iterations = (int*)malloc( sizeof(int) * width * height );

  assert(iterations);

  /* compute set */
  int maxCount =julia(x, width, y, height, c, flag, maxiter, iterations, NUM_PROCS, stats);
  printf("max iterations hit: %d /%d\n", maxCount, maxiter);

  /* save our picture for the viewer */
  saveBMP(image, iterations, width, height);


  free(iterations);

  end_t = clock();
  double total_time = (double)(end_t - start_t) / (CLOCKS_PER_SEC / 1E6);
  printf("total_time: %.0f", total_time);

  return 0;
}
