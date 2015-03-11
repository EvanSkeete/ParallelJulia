/*
 *	Names:	Nicholas Cianflone - Joshua Segeren  - Evan Skeete      - Michael Jansen
 *	ID:		1065021 (cianflnp)   1048766 (segerej) 1063790 (skeeteel) 1046843 (jansenmp)
 *	Date:		11 March 2015
 *
 *	Description:
 *	-	julia_acc_s.c is a single-precision OpenACC implementation of both Mandelbrot and Julia sets
 */

#include <math.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <openacc.h>

#include "julia_acc_s.h"

int julia(const float *x, int xres, const float *y, int yres, const float *c,
	int flag, int maxIterations, int *iterations, char * stats)
{
	struct timeval t0, t1;
	srand(time(NULL)); // Seed random value generator
	int maxIterationCount = 0, i, j;

	float xgap, ygap, savex, savey, xisq, yisq;
	xgap = (x[1] - x[0]) / xres;
	ygap = (y[1] - y[0]) / yres;

	// Start timing parallelized function
	gettimeofday(&t0, NULL);

	#pragma acc parallel copy(iterations[0:xres*yres])
	{
		#pragma acc loop reduction(max:maxIterationCount)
		for (j = 0; j < yres; j++) {
			float yi;
			for (i = 0; i < xres; i++) {
				int count;
				float xi, radius;
					/* pixel to coordinates */
				xi = x[0] + i * xgap;
				yi = y[0] + j * ygap;

					/* initial value for the iteration */
				savex = flag*c[0] + (1-flag)*xi;
				savey = flag*c[1] + (1-flag)*yi;

				radius = 0.0;
				count = 0;

				xisq = xi * xi;
				yisq = yi * yi;
				while ( radius <= 4.0 && count < maxIterations )
				{
					float savex2 = xi;
					xi = xisq - yisq + savex;
					yi = 2.0f * savex2 * yi + savey;
					xisq = xi * xi;
					yisq = yi * yi;
					radius = xisq + yisq;
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
	}

	// Stop timing parallelized julia
	gettimeofday(&t1, 0);
	long elapsed = (t1.tv_sec-t0.tv_sec)*1000000 + t1.tv_usec-t0.tv_usec;

	// Output to file
	FILE *statsFile;
	statsFile = fopen(stats, "w");

	fprintf(statsFile, "%d %ld us %d\n", 1, elapsed, maxIterationCount);
	fclose(statsFile);

	return maxIterationCount;

}
