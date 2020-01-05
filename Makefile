SRC = main.c snake.c
OBJS = $(addprefix src/, $(SRC))

LIBS = sdl2
FLAGS = -pedantic -Wall -Werror $(shell pkg-config --libs --cflags $(LIBS))

OUT = game

all:
	gcc $(OBJS) -o $(OUT) $(FLAGS)

clean:
	rm game
