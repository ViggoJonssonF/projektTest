# Makefile for Windows

SRCDIR = C:\Users\PC\programsSDL\projektTest\projektTest\src
CC = gcc
INCLUDE = C:\msys64\mingw64\include\SDL2

CFLAGS = -g -I$(INCLUDE) -c
LDFLAGS = -lmingw32 -lSDL2main -lSDL2_image -lSDL2 -mwindows -lm -lSDL2_mixer -lSDL2_ttf

OBJS = main.o engine.o gameLogic.o input.o render.o

projektTest: $(OBJS)
	$(CC) $(OBJS) -o projektTest $(LDFLAGS)

main.o: $(SRCDIR)\main.c
	$(CC) $(CFLAGS) $(SRCDIR)\main.c

engine.o: $(SRCDIR)\engine.c
	$(CC) $(CFLAGS) $(SRCDIR)\engine.c

gameLogic.o: $(SRCDIR)\gameLogic.c
	$(CC) $(CFLAGS) $(SRCDIR)\gameLogic.c

input.o: $(SRCDIR)\input.c
	$(CC) $(CFLAGS) $(SRCDIR)\input.c

render.o: $(SRCDIR)\render.c
	$(CC) $(CFLAGS) $(SRCDIR)\render.c

clean:
	del projektTest.exe
	del *.o
