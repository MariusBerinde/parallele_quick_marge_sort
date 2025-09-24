CC = gcc
CFLAGS = -Wall -g -fopenmp -O3
SRCDIR = src
OUTDIR = out
SOURCES = $(SRCDIR)/main.c $(SRCDIR)/lib.c
TARGET = $(OUTDIR)/main.exe

# Crea la directory out se non esiste e compila
$(TARGET): $(SOURCES) | $(OUTDIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

# Crea la directory out
$(OUTDIR):
	if not exist $(OUTDIR) mkdir $(OUTDIR)

# Pulisce i file generati
clean:
	rm -I $(OUTDIR)/*

# Esegue il programma
run: $(TARGET)
	$(TARGET)
compila_merge: ./src/merge_mp.c
	mpicc -Wall $(SRCDIR)/merge_mp.c -o ./out/merge_mp -lm #-D DEBUG

merge: compila_merge
	mpirun -np 8 ./out/merge_mp 

compila_net:src/net_mp.c
	mpicc -Wall  src/net_mp.c -o out/net_mp #-D DEBUG 

net:compila_net 
	mpirun -np 20 ./out/net_mp
compila_sup: ./src/sup.c 
	gcc -Wall ./src/sup.c -o ./out/sup -D DEBUG -lm

sup:compila_sup 
	./out/sup
compila_test: ./src/test_open_mp.c
	mpicc -Wall  $(SRCDIR)/test_open_mp.c -o ./out/test_open_mp

tests: compila_test
	mpirun -np 4 ./out/test_open_mp

.PHONY: clean run
