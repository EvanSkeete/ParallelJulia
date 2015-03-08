#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include <stdio.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#include "julia.h"

int julia(const double *x, int xres, const double *y, int yres, const double *c,
		int flag, int maxIterations, int *iterations, int num_procs, char * stats)
{

double xgap, ygap;
	xgap = (x[1] - x[0]) / xres;
	ygap = (y[1] - y[0]) / yres;

	int global_max_iteration_count = 0;
	int *global_max_iterations = (int*)malloc(sizeof(int) * num_procs);

	double global_times[num_procs];
	int x_array[xres];
	int y_array[yres];

	int i;

	for(i = 0; i < xres; i++) x_array[i] = i;

	for(i = 0; i < yres; i++) y_array[i] = i;

	for(i = 0; i < xres/2; i++){
		int randIndex = rand()%xres;
		int tmp = x_array[i];
		x_array[i] = x_array[randIndex];
		x_array[randIndex] = tmp;
	}

	for(i = 0; i < yres/2; i++){
		int randIndex = rand()%yres;
		int tmp = y_array[i];
		y_array[i] = y_array[randIndex];
		y_array[randIndex] = tmp;
	}

	#ifdef _OPENMP
	#pragma omp parallel num_threads (num_procs)
	#endif

	{

	#ifdef _OPENMP
		int my_rank = omp_get_thread_num();
		int thread_count = omp_get_num_threads();
	#else
		int my_rank = 0;
		int thread_count = 1;
	#endif

	clock_t start_t, end_t, total_t;
	start_t = clock();


	double savex, savey;

	int maxIterationCount = 0;
	int count, i, j, local_start, local_end;
	int local_yres = yres / thread_count;

	int remaining = yres%thread_count;

	if (my_rank < remaining){
		++local_yres;
		local_start = my_rank * local_yres;
		local_end = local_start + (local_yres - 1);
	} else {
		local_start = my_rank * local_yres + remaining;
		local_end = local_start + (local_yres - 1);
	}

	for (j = local_start; j <= local_end; j++) {
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

					if(count > maxIterationCount )
						maxIterationCount = count;

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

		end_t = clock();
		global_times[my_rank] = (double)(end_t - start_t) / (CLOCKS_PER_SEC / 1E6);

		//printf("my rank: %d, time: %d, iterations:%d\n", my_rank, total_t, maxIterationCount);

	}

	FILE *f;
	f = fopen(stats,"w");

	for (i=0; i<num_procs; i++){
		if(global_max_iterations[i] > global_max_iteration_count )
			global_max_iteration_count = global_max_iterations[i];

			fprintf(f, "%d %.0fus %d\n", i, global_times[i], global_max_iterations[i]);
	}

	fclose(f);
	free(global_max_iterations);

	return global_max_iteration_count;

}
