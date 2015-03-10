#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sys/time.h>
#include <time.h>

#include "julia.h"

#define NUM_PROCS 8

int main(int argc, char *argv[])
{
  struct timeval t0, t1;
  gettimeofday(&t0, 0);

  int width, height, maxiter, flag;

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

  gettimeofday(&t1, 0);
  long elapsed = (t1.tv_sec-t0.tv_sec)*1000000 + t1.tv_usec-t0.tv_usec;

  printf("total_time: %ld\n", elapsed);

  return 0;
}
