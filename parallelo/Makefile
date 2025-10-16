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

# Pulisce i file generati
clean:
	rm -I $(OUT)/*

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
compila_test: ./src/test_open_mp.c
	mpicc -Wall  $(SRC)/test_open_mp.c -o ./out/test_open_mp

tests: compila_test
	mpirun -np 4 ./out/test_open_mp

.PHONY: clean run
