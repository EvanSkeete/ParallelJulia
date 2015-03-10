CC=pgcc
#CFLAGS= -Wall -O3


C_OMP = main.c julia_omp.c savebmp.c color.c getparams.c
C_ACC = main.c julia_acc_d.c main_s.c julia_acc_s.c savebmp.c color.c getparams.c
OBJS_OMP = main.o julia_omp.o savebmp.o color.o getparams.o

OBJS_ACC = main.o julia_acc_d.o main_s.o julia_acc_s.o savebmp.o color.o getparams.o

OBJS_ACC_S =  main_s.o julia_acc_s.o savebmp.o color.o getparams.o
OBJS_ACC_D =  main.o julia_acc_d.o savebmp.o color.o getparams.o

all: julia_acc_s julia_acc_d julia_omp


julia_acc_s: CFLAGS=-fast -acc -Minfo -ta=nvidia,cc13 -O2
julia_acc_s: LDFLAGS=-acc -ta=nvidia,cc13

julia_acc_d: CFLAGS=-fast -acc -Minfo -ta=nvidia,cc13 -O2
julia_acc_d: LDFLAGS=-acc -ta=nvidia,cc13

julia_omp: CFLAGS=-O2 -mp
julia_omp: LDFLAGS=-mp

julia_acc_s: $(OBJS_ACC_S)
	$(CC) $(LDFLAGS) -o $@ $?

julia_acc_d: $(OBJS_ACC_D)
	$(CC) $(LDFLAGS) -o $@ $?

julia_omp: $(OBJS1)
	$(CC) $(LDFLAGS) -o $@ $?

#julia_acc_s: $(OBJS_ACC_S)
#	pgcc -acc -Minfo=all -ta=nvidia,cc13 -o julia_acc_s $(OBJS_ACC_S)

# julia_acc_d: $(OBJS_ACC_D)
# 	pgcc -acc -Minfo=all -ta=nvidia,cc13 -o julia_acc_d $(OBJS_ACC_D)

# julia_omp: $(C_OMP)
# 	gcc -o julia_omp $(C_OMP) -fopenmp -O3

julia_omp: $(C_OMP)
	gcc -o julia_omp $(C_OMP) -fopenmp -O3

run_acc_s:
	./julia_acc_s 0 -0.4 0.6 -0.181862 -0.181772 1.019085 1.019175 4000 4000 4000 image.bmp stats-acc-s;

run_acc_d:
	./julia_acc_d 0 -0.4 0.6 -0.181862 -0.181772 1.019085 1.019175 4000 4000 4000 image.bmp stats-acc-d;

run_omp_gpu: run_omp_gpu_test_1 run_omp_gpu_test_2 run_omp_gpu_test_4 run_omp_gpu_test_8 run_omp_gpu_test_16 run_omp_gpu_test_32

run_omp_gpu_test_1:
	export OMP_NUM_THREADS=1; ./julia_omp 0 -0.4 0.6 -0.181862 -0.181772 1.019085 1.019175 4000 4000 4000 image.bmp stats-omp-gpu-1;

run_omp_gpu_test_2:
	export OMP_NUM_THREADS=2; ./julia_omp 0 -0.4 0.6 -0.181862 -0.181772 1.019085 1.019175 4000 4000 4000 image.bmp stats-omp-gpu-2;

run_omp_gpu_test_4:
	export OMP_NUM_THREADS=4; ./julia_omp 0 -0.4 0.6 -0.181862 -0.181772 1.019085 1.019175 4000 4000 4000 image.bmp stats-omp-gpu-4;

run_omp_gpu_test_8:
	export OMP_NUM_THREADS=8; ./julia_omp 0 -0.4 0.6 -0.181862 -0.181772 1.019085 1.019175 4000 4000 4000 image.bmp stats-omp-gpu-8;

run_omp_gpu_test_16:
	export OMP_NUM_THREADS=16; ./julia_omp 0 -0.4 0.6 -0.181862 -0.181772 1.019085 1.019175 4000 4000 4000 image.bmp stats-omp-gpu-16;

run_omp_gpu_test_32:
	export OMP_NUM_THREADS=32; ./julia_omp 0 -0.4 0.6 -0.181862 -0.181772 1.019085 1.019175 4000 4000 4000 image.bmp stats-omp-gpu-32;

run_omp_wobbie: run_omp_test_1 run_omp_test_2 run_omp_test_4 run_omp_test_8 run_omp_test_16 run_omp_test_32

run_omp_test_1:
	sqsub -t -r 1h --mpp=1.0G  -o parallel.log -f threaded -n 1 ./julia_omp 0 -0.4 0.6 -0.181862 -0.181772 1.019085 1.019175 4000 4000 4000 image.bmp stats-omp-1;

run_omp_test_2:
	sqsub -t -r 1h --mpp=1.0G  -o parallel.log -f threaded -n 2 ./julia_omp 0 -0.4 0.6 -0.181862 -0.181772 1.019085 1.019175 4000 4000 4000 image.bmp stats-omp-2;

run_omp_test_4:
	sqsub -t -r 1h --mpp=1.0G  -o parallel.log -f threaded -n 4 ./julia_omp 0 -0.4 0.6 -0.181862 -0.181772 1.019085 1.019175 4000 4000 4000 image.bmp stats-omp-4;

run_omp_test_8:
	sqsub -t -r 1h --mpp=1.0G  -o parallel.log -f threaded -n 8 ./julia_omp 0 -0.4 0.6 -0.181862 -0.181772 1.019085 1.019175 4000 4000 4000 image.bmp stats-omp-8;

run_omp_test_16:
	sqsub -t -r 1h --mpp=1.0G  -o parallel.log -f threaded -n 16 ./julia_omp 0 -0.4 0.6 -0.181862 -0.181772 1.019085 1.019175 4000 4000 4000 image.bmp stats-omp-16;

run_omp_test_32:
	sqsub -t -r 1h --mpp=1.0G  -o parallel.log -f threaded -n 32 ./julia_omp 0 -0.4 0.6 -0.181862 -0.181772 1.019085 1.019175 4000 4000 4000 image.bmp stats-omp-32;

clean:
	@rm -rf $(OBJS) $(OBJS_ACC_S) $(OBJS_ACC_D) julia_omp julia_acc_s julia_acc_d *~ *.bak *.bmp
