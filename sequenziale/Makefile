CC = gcc
CFLAGS = -Wall -g
#SRCDIR = src
#OUTDIR = out
#SOURCES = $(SRCDIR)/main.c $(SRCDIR)/lib.c
#TARGET = $(OUTDIR)/main.exe

# Crea la directory out se non esiste e compila
#$(TARGET): $(SOURCES) | $(OUTDIR)
#	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

# Crea la directory out
#$(OUTDIR):
#	if not exist $(OUTDIR) mkdir $(OUTDIR)

# Pulisce i file generati
#clean:
	#rm -I $(OUTDIR)/*

# Esegue il programma
#run: $(TARGET)
	#$(TARGET)

OUT = out
SRC = src
compila_main: $(SRC)/main.c | $(OUT) 
	gcc $(CFLAGS)  $(SRC)/main.c $(SRC)/lib.c -o $(OUT)/main
run:compila_main
	./$(OUT)/main
$(OUT):
	mkdir -p $(OUT)

.PHONY: clean run
