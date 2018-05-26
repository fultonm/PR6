CC     := gcc
SRC    := .
OBJ    := .
LIBS   := -lm -lmenu -lncurses
CFLAGS := -g -Wall


SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

a.out: $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)