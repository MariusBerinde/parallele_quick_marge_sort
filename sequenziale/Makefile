OUT = out
SRC = src

compila:
	gcc $(SRC)/main.c $(SRC)/lib.c  -o $(OUT)/main

esegui:compila
	./out/main


compila_lib:
	gcc src/lib.c -o out/lib

lib:compila_lib
	./out/lib

clean:
	rv -v -c out/*
