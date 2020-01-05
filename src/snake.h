#include <SDL2/SDL.h>

enum Direction {
    UP, DOWN, LEFT, RIGHT
};

struct Cell {
    SDL_Point point;
    struct Cell* next;
};

struct Snake {
    enum Direction dir;
    struct Cell* cells;
    struct Cell* last;
};

int snake_add_cell(struct Snake* snake);
struct Snake* snake_init();
int snake_destroy(struct Snake* snake);
int snake_move(struct Snake* snake);
void snake_set_direction(struct Snake* snake, enum Direction dir);
