CFLAGS= -Wall -O3

C_OMP = main.c julia_omp.c savebmp.c color.c getparams.c
C_ACC = main.c julia_acc_d.c main_s.c julia_acc_s.c savebmp.c color.c getparams.c

OBJS_ACC_S =  main_s.o julia_acc_s.o savebmp.o color.o getparams.o
OBJS_ACC_D =  main.o julia_acc_d.o savebmp.o color.o getparams.o

all: julia_acc_s julia_acc_d julia_omp

julia_acc_d: $(OBJS_ACC_D)
	pgcc -acc -Minfo -ta=nvidia,cc13 -o julia_acc_d $(OBJS_ACC_D)

julia_acc_s: $(OBJS_ACC_S)
	pgcc -acc -Minfo -ta=nvidia,cc13 -o julia_acc_s $(OBJS_ACC_S)

julia_omp: $(C_OMP)
	gcc -o julia_omp $(C_OMP) -fopenmp -O3

# this runs are on Mac. On Linux, e.g. penguin, replace open by gthumb
run0:
	./julia 0 -0.4 0.6  -0.2 -0.1 1  1.1 1000 1000 1000  image.bmp ; open image.bmp

run1: julia
	./julia 0 -0.4 0.6  -0.2 -0.1 1  1.1 1000 1000 3000  image.bmp ; open image.bmp

run2: julia
	./julia 0 -0.4 0.6  -0.181862 -0.181772 1.019085 1.019175 1000 1000 100  image.bmp ; open image.bmp

run3: julia
	./julia 0 -0.4 0.6  -0.181862 -0.181772 1.019085 1.019175 2000 2000 10000  image.bmp ; open image.bmp

run4: julia
	./julia 1 -0.8  0.156  -2 2 -2 2 1000 1000 100  image.bmp ; open image.bmp

run5: julia
	./julia 1 -0.8  0.156  -2 2 -2 2 1000 1000 1000  image.bmp ; open image.bmp

run6: julia
	./julia 1 -0.8  0.156  -.2 .2 -.2 .2 4000 4000 10000  image.bmp ; open image.bmp

run7: julia
	./julia 1 0.285 0.01  -2 2 -2 2 1000 1000 1000  image.bmp ; open image.bmp

run8: julia
	./julia 1 0.285 0.01  -.2 .2 -.2 .2 1000 1000 1000  image.bmp ; open image.bmp

run9:
	./julia 0 -0.4 0.6  -2 2 -2 2  1000 1000 1000  image.bmp ; open image.bmp

	# this runs are on Mac. On Linux, e.g. penguin, replace open by gthumb
run_acc_s:
	time ./julia_acc_s 0 -0.4 0.6  -0.2 -0.1 1  1.1 1000 1000 1000  image.bmp ;

run_omp:
	sqsub -t -r 1h --mpp=1.0G  -o parallel.log -f threaded -n 17 ./julia_omp 0 -0.4 0.6  -0.2 -0.1 1  1.1 1000 1000 1000  image.bmp results.txt;

run_omp_test_8:
	sqsub -t -r 1h --mpp=1.0G  -o parallel.log -f threaded -n 8 ./julia_omp 0 -0.4 0.6 -0.181862 -0.181772 1.019085 1.019175 4000 4000 4000  image.bmp results.txt;


clean:
	@rm -rf $(OBJS) $(OBJS_ACC_S) $(OBJS_ACC_D) julia_omp julia_acc_s julia_acc_d *~ *.bak *.bmp
