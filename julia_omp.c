/*
 *	Names:	Nicholas Cianflone - Joshua Segeren  - Evan Skeete      - Michael Jansen
 *	ID:		1065021 (cianflnp)   1048766 (segerej) 1063790 (skeeteel) 1046843 (jansenmp)
 *	Date:		11 March 2015
 *
 *	Description:
 *	-	julia_omp.c is a OpenMP implementation of both Mandelbrot and Julia sets
 */

#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include <stdio.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#include "julia.h"

#define MAX_NUMBER_THREADS 32

int julia(const double *x, int xres, const double *y, int yres, const double *c,
	int flag, int maxIterations, int *iterations, char * stats)
{
	double xgap, ygap;
	xgap = (x[1] - x[0]) / xres;
	ygap = (y[1] - y[0]) / yres;

	int global_max_iteration_count = 0;
	int global_max_iterations[MAX_NUMBER_THREADS];
	double global_times[MAX_NUMBER_THREADS];

	int i;
	for (i = 0; i < MAX_NUMBER_THREADS; i++) {
		global_max_iterations[i] = 0;
		global_times[i] = 0.;
	}

	int x_array[xres];
	int y_array[yres];

	for(i = 0; i < xres; i++) {
		x_array[i] = i;
	}

	for(i = 0; i < yres; i++) {
		y_array[i] = i;
	}

	for(i = 0; i < xres; i++){
		int randIndex = rand()%xres;
		int tmp = x_array[i];
		x_array[i] = x_array[randIndex];
		x_array[randIndex] = tmp;
	}

	for(i = 0; i < yres; i++){
		int randIndex = rand()%yres;
		int tmp = y_array[i];
		y_array[i] = y_array[randIndex];
		y_array[randIndex] = tmp;
	}

	#ifdef _OPENMP
	#pragma omp parallel
	#endif

	{
		int my_rank = 0;
		int thread_count = 1;

	#ifdef _OPENMP
		my_rank = omp_get_thread_num();
		thread_count = omp_get_num_threads();
	#endif

		double savex, savey, yi, xi, radius, savex2, xisq, yisq, startt;

		int maxIterationCount = 0;
		int count, i, j;

		startt = omp_get_wtime();
	#pragma omp for schedule(guided) collapse(2)
		for (j = 0; j <= yres; j++) {
			for (i = 0; i < xres; i++) {
					/* pixel to coordinates */
				xi = x[0] + x_array[i] * xgap;
				yi = y[0] + y_array[j] * ygap;

					/* initial value for the iteration */
				savex = flag*c[0] + (1-flag)*xi;
				savey = flag*c[1] + (1-flag)*yi;

				radius = 0.0;
				count = 0;

				xisq = xi*xi;
				yisq = yi*yi;

				while ( radius <= 4.0 && count < maxIterations )
				{
					savex2 = xi;
					xi = xisq - yisq + savex;
					yi = 2.0f * savex2 * yi + savey;
					xisq = xi * xi;
					yisq = yi * yi;
					radius = xisq + yisq;
					count++;
				}

				if(count > maxIterationCount ){
					maxIterationCount = count;
				}

				int *p = iterations + y_array[j]*xres+x_array[i];

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

		global_max_iterations[my_rank] = maxIterationCount;

		global_times[my_rank] =  omp_get_wtime() - startt;
	}

	FILE *f;
	f = fopen(stats, "w");

	for (i=0; i< MAX_NUMBER_THREADS; i++){
		if (global_max_iterations[i] == 0 || global_times[i] == 0.) {
			break;
		}
		if(global_max_iterations[i] > global_max_iteration_count ) {
			global_max_iteration_count = global_max_iterations[i];
		}
		fprintf(f, "%d %fs %d\n", i, global_times[i], global_max_iterations[i]);
	}

	fclose(f);

	return global_max_iteration_count;

}
