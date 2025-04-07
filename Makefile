# Makefile för både macOS och Windows (MSYS2)

ifeq ($(OS),Windows_NT)
  # Inställningar för Windows (MSYS2)
  CC      = gcc
  INCLUDE = C:/msys64/mingw64/include/SDL2
  LIBDIR  = C:/msys64/mingw64/lib
  CFLAGS  = -g -I$(INCLUDE) -c
  LDFLAGS = -lmingw32 -lSDL2main -lSDL2_image -lSDL2 -lSDL2_mixer -lSDL2_ttf -lm
  SRCDIR = .\src
else
  # Inställningar för macOS
  SRCDIR = src
  CC      = gcc-14  # Ändra vid behov om du använder en annan version
  INCLUDE = /usr/local/include/SDL2
  LIBDIR  = /usr/local/lib
  CFLAGS  = -g -I$(INCLUDE) -c
  LDFLAGS = -L$(LIBDIR) -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lm
endif

# Plats för källkoden

# Objektfiler som ska byggas
OBJS   = main.o engine.o gameLogic.o input.o render.o

# Målnamn (produkten)
TARGET = projektTest

# Länkningsregel: bygg körbar fil av objektfilerna
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Kompilering av enskilda .c-filer från SRCDIR till .o
%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $< -o $@

# Rensa byggfilerna
clean:
	rm -f *.o $(TARGET)
################
