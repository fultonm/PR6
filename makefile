SRC  := .
OBJ  := obj
LIBS := -lncurses -lmenu -lm


SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

a.out: $(OBJECTS)
	$(CC) -g $^ -o $@ $(LIBS)

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) -g -I$(SRC) -c $< -o $@ $(LIBS)