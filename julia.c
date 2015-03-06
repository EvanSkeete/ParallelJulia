#include <assert.h>

#include "julia.h"

int julia(const double *x, int xres, const double *y, int yres, const double *c, 
	  int flag, int maxIterations, int *iterations)
{
  int maxIterationCount = 0, i,j;
  
  double xi, yi, xgap, ygap, savex, savey, radius;
  int count;
  xgap = (x[1] - x[0]) / xres;
  ygap = (y[1] - y[0]) / yres;

  for (j = 0; j < yres; j++)
    {
      for (i = 0; i < xres; i++)    
	{
	  /* pixel to coordinates */
	  xi = x[0] + i * xgap;
	  yi = y[0] + j * ygap;
	  
	  /* initial value for the iteration */
	  savex = flag*c[0] + (1-flag)*xi;
	  savey = flag*c[1] + (1-flag)*yi;
	  
	  radius = 0.0;
	  count = 0;
	  while ( radius <= 4.0 && count < maxIterations )
	    {
	     double savex2 = xi;
	      xi = xi * xi - yi * yi + savex;
	      yi = 2.0f * savex2 * yi + savey;
	      radius = xi * xi + yi * yi;
	      count++;

	    }
	  
	  if(count > maxIterationCount )
	    maxIterationCount = count;
	  
	  int *p = iterations + j*xres+i;

	  /* If radius <= 4.0, we have hit maxIterations. The point is
	     likely in the set. */
	  if (radius <= 4.0)
	    {
	      assert(count==maxIterations);
	      *p = 0;
	    }
	  else
	    /* radius > 4.0. The point tends to infinity. We recognize
	       this at iteration number count */
	    {
	      *p = count;
	    }
	}
    }
  
  return maxIterationCount;
}
