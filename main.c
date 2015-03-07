#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "julia.h"

int main(int argc, char *argv[])
{
  int width, height, maxiter, flag;
  double x[2], y[2], c[2];
  char *image, *stats;
  getParams(argv, &flag, c, x, y, &width, &height, &maxiter, &image, &stats);

  int *iterations = (int*)malloc( sizeof(int) * width * height );
  assert(iterations);

  /* compute set */
  int maxCount = julia(x, width, y, height, c, flag, maxiter, iterations);
  printf("max iterations hit: %d /%d\n", maxCount, maxiter);

  /* save our picture for the viewer */
  saveBMP(image, iterations, width, height);

  free(iterations);
  return 0;
}
