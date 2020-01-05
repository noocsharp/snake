#include "snake.h"


// determines where the cell should be added
static struct SDL_Point snake_new_cell(struct Snake* snake) {
    switch (snake->dir) {
        case UP:
            {
            SDL_Point value = {snake->cells->point.x, snake->cells->point.y-1};
            return value;
            }
        case DOWN:
            {
            SDL_Point value = {snake->cells->point.x, snake->cells->point.y+1};
            return value;
            }
        case LEFT:
            {
            SDL_Point value = {snake->cells->point.x-1, snake->cells->point.y};
            return value;
            }
        case RIGHT:
            {
            SDL_Point value = {snake->cells->point.x+1, snake->cells->point.y};
            return value;
            }
    }

    SDL_Point value = {0, 0};
    return value;
}

// adds cell to cells list in snake struct
int snake_add_cell(struct Snake* snake) {
    struct Cell* second_cell = snake->cells;
    struct Cell* first_cell = calloc(1, sizeof(struct Cell));

    if (first_cell == NULL) return 1;
    
    struct SDL_Point first_cell_point = snake_new_cell(snake);
    *first_cell = (struct Cell){ first_cell_point, second_cell };

    snake->cells = first_cell;

    return 0;
}

static int opposite_direction(enum Direction dir) {
    switch (dir) {
        case UP:
            return DOWN;
        case DOWN:
            return UP;
        case LEFT:
            return RIGHT;
        case RIGHT:
            return LEFT;
    }

    return -1;
}

void snake_set_direction(struct Snake* snake, enum Direction dir) {
    if (dir != snake->dir && dir != opposite_direction(snake->dir)) snake->dir = dir;
}

int snake_move(struct Snake* snake) {
    struct Cell* last = snake->last;
    struct Cell* second_last = snake->cells;

    if (last == second_last) {
        struct SDL_Point point = snake_new_cell(snake);
        last->point = point;
        return 0;
    }

    while (second_last->next != last)
        second_last = second_last->next;

    second_last->next = NULL;
    snake->last = second_last;

    struct SDL_Point point = snake_new_cell(snake);
    last->point = point;
    last->next = snake->cells;

    snake->cells = last;

    return 0;
}

struct Snake* snake_init() {
    struct Snake* snake = calloc(1, sizeof(struct Snake));
    if (snake == NULL) return NULL;

    struct Cell* first_cell = calloc(1, sizeof(struct Cell));
    if (first_cell == NULL) {
        free(snake);
        return NULL;
    }

    *first_cell = (struct Cell) { {1, 1}, NULL };
    *snake = (struct Snake) { DOWN, first_cell, first_cell };

    return snake;
}

int snake_destroy(struct Snake* snake) {
    while (snake->cells != NULL) {
        struct Cell* to_free = snake->cells;
        snake->cells = snake->cells->next;
        free(to_free);
    }

    free(snake);
    return 0;
}
