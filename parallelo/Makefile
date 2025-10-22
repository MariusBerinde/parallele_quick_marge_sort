CC = gcc
CFLAGS = -Wall   -g -fopenmp -O3
SRC = src
OUT = out
SOURCES = $(SRC)/main.c $(SRC)/lib.c
TARGET = $(OUT)/main.out

# Crea la directory out se non esiste e compila
compila_run: $(SOURCES) | $(OUT)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

# Crea la directory out
$(OUT):
	if not exist $(OUT) mkdir $(OUT)

compila_omp:./src/omp.c
	$(CC) $(CFLAGS) $(SRC)/omp.c $(SRC)/lib.c -o out/omp

omp:compila_omp
	./out/omp


compila_mpi:src/mpi.c 
	mpicc  -g -fopenmp -O3  src/mpi.c src/lib.c -o out/mpi -lm -D DEBUG

mpi:compila_mpi 
	mpirun -np 10 ./out/mpi

# Esegue il programma
run: compila_run
	$(TARGET)
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
