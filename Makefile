# Makefile for Windows

SRCDIR=C:\Users\PC\programsSDL\projektTest\projektTest\src
CC=gcc
INCLUDE = C:\msys64\mingw64\include\SDL2

CFLAGS = -g -I$(INCLUDE) -c 
LDFLAGS = -lmingw32 -lSDL2main -lSDL2_image -lSDL2 -mwindows -lm

# Målet länkar ihop både main.o och engine.o
simpleSDLexample2: main.o engine.o
	$(CC) main.o engine.o -o simpleSDLexample2 $(LDFLAGS)

main.o: $(SRCDIR)\main.c
	$(CC) $(CFLAGS) $(SRCDIR)\main.c

engine.o: $(SRCDIR)\engine.c
	$(CC) $(CFLAGS) $(SRCDIR)\engine.c

clean:
	del simpleSDLexample2.exe
	del *.o
