#include <math.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef _OPENACC
#include <openacc.h>
#endif

#include "julia.h"

int julia(const double *x, int xres, const double *y, int yres, const double *c,
	int flag, int maxIterations, int *iterations, char * stats)
{
	struct timeval t0, t1;
	srand(time(NULL)); // Seed random value generator
	int maxIterationCount = 0, i, j;
	int count;

	double xgap, ygap, savex, savey;
	xgap = (x[1] - x[0]) / xres;
	ygap = (y[1] - y[0]) / yres;

	int x_array[xres];
	int y_array[yres];
	int randIndex;
	int tmp;

	for(i = 0; i < xres; i++) {
		x_array[i] = i;
	}

	for(i = 0; i < yres; i++) {
		y_array[i] = i;
	}

	for(i = 0; i < xres; i++){
		randIndex = rand()%xres;
		tmp = x_array[i];
		x_array[i] = x_array[randIndex];
		x_array[randIndex] = tmp;
	}

	for(i = 0; i < yres; i++){
		randIndex = rand()%yres;
		tmp = y_array[i];
		y_array[i] = y_array[randIndex];
		y_array[randIndex] = tmp;
	}

	// Start timing parallelized function
	gettimeofday(&t0, NULL);

	#ifdef _OPENACC
	#pragma parallel acc loop reduction(max:maxIterationCount) private(count) copyin(iterations[0:xres*yres]) vector(256)
	#endif
	for (j = 0; j < yres; j++) {
		double yi;
		for (i = 0; i < xres; i++) {
			double xi, radius;
					/* pixel to coordinates */
			xi = x[0] + x_array[i] * xgap;
			yi = y[0] + y_array[j] * ygap;

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

			if(count > maxIterationCount ) {
				maxIterationCount = count;
			}

			int *p = iterations + y_array[j]*xres+x_array[i];

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
