CC = gcc
CFLAGS = -Wall -g

OUT = out
SRC = src

compila_main:$(SRC)/main.c | $(OUT) 
	gcc $(CFLAGS)  $(SRC)/main.c $(SRC)/sort.c -o $(OUT)/main

run: compila_main
	./$(OUT)/main

$(OUT):
	mkdir -p $(OUT)

.PHONY: clean run
