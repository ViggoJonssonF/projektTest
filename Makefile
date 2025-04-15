ifeq ($(OS),Windows_NT)
CC = gcc
INCLUDE = C:/msys64/mingw64/include/SDL2
INCLUDE_NET = C:/msys64/mingw64/include/SDL2_net
LIBDIR = C:/msys64/mingw64/lib
CFLAGS = -g -I$(INCLUDE) -I$(INCLUDE_NET) -c
LDFLAGS = -L$(LIBDIR) -lmingw32 -lSDL2main -lSDL2_image -lSDL2 -lSDL2_mixer -lSDL2_ttf -lSDL2_net -lm
SRCDIR = ./src
else
CC = gcc-14
INCLUDE = /usr/local/include/SDL2
INCLUDE_NET = /usr/local/include/SDL2_net
LIBDIR = /usr/local/lib
CFLAGS = -g -I$(INCLUDE) -I$(INCLUDE_NET) -c
LDFLAGS = -L$(LIBDIR) -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lSDL2_net -lm
SRCDIR = ./src
endif

OBJS = main.o engine.o gameLogic.o input.o render.o paths.o 
TARGET = projektTest

all: $(TARGET) server client

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $< -o $@

server: $(SRCDIR)/server.c
	$(CC) -g -I$(INCLUDE) -I$(INCLUDE_NET) -c $(SRCDIR)/server.c -o server.o
	$(CC) server.o -o server.exe $(LDFLAGS)

client: $(SRCDIR)/client.c
	$(CC) -g -I$(INCLUDE) -I$(INCLUDE_NET) -c $(SRCDIR)/client.c -o client.o
	$(CC) client.o -o client.exe $(LDFLAGS)

clean:
	rm -f *.o $(TARGET) server.exe client.exe
