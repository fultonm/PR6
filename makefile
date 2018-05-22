CC   := gcc
SRC  := .
OBJ  := .
LIBS := -lcurses -lmenu -lm


SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

a.out: $(OBJECTS)
	$(CC) -g $^ -o $@ $(LIBS)