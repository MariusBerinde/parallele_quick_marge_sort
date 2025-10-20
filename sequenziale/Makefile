CC = gcc
CFLAGS = -Wall -g #-fopenmp

OUT = out
SRC = src

compila_main:$(SRC)/main.c | $(OUT) 
	gcc $(CFLAGS)  $(SRC)/main.c $(SRC)/sort.c -o $(OUT)/main

run: compila_main
	./$(OUT)/main

compila_test:$(SRC)/test.c | $(OUT) 
	gcc $(CFLAGS)  $(SRC)/test.c -o $(OUT)/test

test:compila_test
	./$(OUT)/test

$(OUT):
	mkdir -p $(OUT)

.PHONY: clean run
