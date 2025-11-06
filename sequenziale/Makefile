CC = gcc
CFLAGS = -Wall -g -O3 #-fopenmp

OUT = out
SRC = src

compila_main:$(SRC)/main.c | $(OUT) 
	gcc $(CFLAGS)  $(SRC)/main.c $(SRC)/sort.c -o $(OUT)/main

compila_main_int:$(SRC)/main_int.c | $(OUT) 
	gcc $(CFLAGS)  $(SRC)/main_int.c $(SRC)/sort_int.c -o $(OUT)/i_main

irun: compila_main_int
	./$(OUT)/i_main
run: compila_main
	./$(OUT)/main

compila_test:$(SRC)/test.c | $(OUT) 
	gcc $(CFLAGS)  $(SRC)/test.c -o $(OUT)/test

test:compila_test
	./$(OUT)/test

$(OUT):
	mkdir -p $(OUT)

.PHONY: clean run
