#include <assert.h>
#include <math.h>
#ifdef _OPENACC
#include <openacc.h>
#endif

#include "julia.h"

int julia(const double *x, int xres, const double *y, int yres, const double *c,
		int flag, int maxIterations, int *iterations)
{
	int maxIterationCount = 0, i,j;
	int count;

double xgap, ygap, savex, savey;
	xgap = (x[1] - x[0]) / xres;
	ygap = (y[1] - y[0]) / yres;

	#ifdef _OPENACC
	#pragma parallel acc loop reduction(max:maxIterationCount) private(count) copyin(iterations[0:xres*yres]) vector(256)
	#endif
	for (j = 0; j < yres; j++) {
			double yi;
			for (i = 0; i < xres; i++) {
					double xi, radius;
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
							//assert(count==maxIterations);
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
