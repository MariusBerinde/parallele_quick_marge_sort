CC = gcc
CFLAGS = -Wall   -g -fopenmp -O3 -D DEBUG
SRC = src
OUT = out
SOURCES = $(SRC)/main.c $(SRC)/lib.c
TARGET = $(OUT)/main.out

# Crea la directory out se non esiste e compila
compila_run: $(SOURCES) | $(OUT)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)
compila_run_int:./src/main_int.c
	$(CC) $(CFLAGS) $(SRC)/main_int.c $(SRC)/lib_int.c -o out/i_main

# Crea la directory out
$(OUT):
	if not exist $(OUT) mkdir $(OUT)

compila_omp:./src/omp.c
	$(CC) $(CFLAGS) $(SRC)/omp.c $(SRC)/lib.c -o out/omp

omp:compila_omp
	./out/omp

compila_omp_int:./src/omp_int.c
	$(CC) $(CFLAGS) $(SRC)/omp_int.c $(SRC)/lib_int.c -o out/i_omp

iomp:compila_omp_int
	./out/i_omp

compila_mpi:src/mpi.c 
	mpicc  -g -fopenmp -O3  src/mpi.c src/lib.c -o out/mpi -lm #-D DEBUG

compila_mpi_int:src/mpi_int.c 
	mpicc  -g -fopenmp -O3  src/mpi_int.c src/lib_int.c -o out/i_mpi -lm #-D DEBUG

mpi:compila_mpi 
	mpirun -np 16 ./out/mpi

i_mpi:compila_mpi_int 
	mpirun -np 16 ./out/i_mpi
	
# Esegue il programma
run: compila_run
	$(TARGET)

irun: compila_run_int
	./out/i_main

compila_merge: ./src/merge_mp.c
	mpicc -Wall -O3 -fopenmp  $(SRC)/merge_mp.c $(SRC)/lib.c -o ./out/merge_mp -lm #-D DEBUG

merge: compila_merge
	mpirun -np 16 ./out/merge_mp 

compila_net:src/net_mp.c
	mpicc -Wall  src/net_mp.c -o out/net_mp #-D DEBUG 

net:compila_net 
	mpirun -np 20 ./out/net_mp
compila_sup: ./src/sup.c 
	gcc -Wall ./src/sup.c -o ./out/sup -D DEBUG -lm

sup:compila_sup 
	./out/sup

# Pulisce i file generati
clean:
	rm -I $(OUT)/*
.PHONY: clean run
