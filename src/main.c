#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include "snake.h"
#include <time.h>

#define WIN_TITLE "Snake"
#define WIN_WIDTH 960
#define WIN_HEIGHT 960

#define BOARD_WIDTH 60
#define BOARD_HEIGHT 60

#define TIMESTEP 50

const struct SDL_Color bg1 = { 0, 0, 0, 255 };
const struct SDL_Color bg2 = { 16, 16, 16, 255 };
const struct SDL_Color sc = { 0, 255, 0, 255 };
const struct SDL_Color tc = { 255, 0, 0, 255 };

const int cell_width = WIN_WIDTH/BOARD_WIDTH;
const int cell_height = WIN_HEIGHT/BOARD_HEIGHT;

SDL_Window* window;
SDL_Renderer* renderer;

struct Snake* snake;

SDL_Point target;

// Cleanup Codes
enum CC {
    CC_All = 1,
    CC_Init,
    CC_Window,
    CC_Renderer,
    CC_Snake,
};

enum State {
    GAME,
    OVER,
    MENU
};

enum State state = GAME;

int cleanup(enum CC code) {
    switch (code) {
        case CC_All:
            snake_destroy(snake);
        case CC_Snake:
            SDL_DestroyRenderer(renderer);
        case CC_Renderer:
            SDL_DestroyWindow(window);
        case CC_Window:
            SDL_Quit();
        case CC_Init:
            break;
    }
    if (code != CC_All) printf("failure: %s\n", SDL_GetError());

    return code;
}

int init() {
    srand(time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) return cleanup(CC_Init);

    window = SDL_CreateWindow(WIN_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIN_WIDTH, WIN_HEIGHT, 0);
    if (window == NULL) return cleanup(CC_Window);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) return cleanup(CC_Renderer);

    snake = snake_init();
    if (snake == NULL) return cleanup(CC_Snake);

    return 0;
}

// chooses a new target from all cells unoccupied by the snake
int newTarget() {
    int snake_size = 0;
    struct Cell* item = snake->cells;
    for (;item != NULL; item = item->next) ++snake_size;

    SDL_Point* valid_points = NULL;
    valid_points = malloc((BOARD_WIDTH*BOARD_HEIGHT - snake_size)*sizeof(SDL_Point));
    if (valid_points == NULL) {
        printf("LMAO NULL\n");
        return 2;
    }

    unsigned int counter = 0;
    for (int b = 0; b < BOARD_WIDTH*BOARD_HEIGHT; ++b) {
        bool in_snake = false;
        int c = 0;
        for (struct Cell* cell = snake->cells; cell != NULL; cell = cell->next) {
            ++c;
            int value = (cell->point.x)*BOARD_WIDTH + cell->point.y;
            if (value == b) {
                in_snake = true;
                break;
            } 
        }

        if (!in_snake) {
            __attribute__((unused))SDL_Point p = { b % BOARD_WIDTH, b / BOARD_HEIGHT };
            valid_points[counter] = p;
            ++counter;
        }
    }

    unsigned int index = rand() % counter;

    SDL_Point p = valid_points[index];
    target.x = p.x;
    target.y = p.y;

    free(valid_points);
    return 0;
}

void reset() {
    if (snake != NULL) {
        snake_destroy(snake);
    }

    snake = snake_init();

    newTarget();
}

int process_event(SDL_Event* eventptr) {
    switch (state) {
        case GAME:
            switch (eventptr->type) {
                case SDL_QUIT:
                    return -1;
                case SDL_KEYDOWN:
                    switch (eventptr->key.keysym.scancode) {
                        case SDL_SCANCODE_UP:
                            snake_set_direction(snake, UP);
                            break;
                        case SDL_SCANCODE_DOWN:
                            snake_set_direction(snake, DOWN);
                            break;
                        case SDL_SCANCODE_LEFT:
                            snake_set_direction(snake, LEFT);
                            break;
                        case SDL_SCANCODE_RIGHT:
                            snake_set_direction(snake, RIGHT);
                            break;
                        default:
                            break;
                    }
            }
            break;
        case OVER:
            switch (eventptr->type) {
                case SDL_QUIT:
                    return -1;
                case SDL_KEYDOWN:
                    if (eventptr->key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                        state = GAME;
                        reset();
                    }
            }
        default: break;
    }
    return 0;
}


// 0 = no collision, 1 = target collision, 2 = border collision, 3 = hits itself
uint8_t checkCollision() {
    SDL_Point spos = snake->cells->point;
    if (spos.x == target.x && spos.y == target.y) return 1;

    if (spos.x < 0 || spos.x >= BOARD_WIDTH || spos.y < 0 || spos.y >= BOARD_HEIGHT)
        return 2;

    for (struct Cell* cell = snake->cells->next; cell != NULL; cell = cell->next) {
        if (cell->point.x == spos.x && cell->point.y == spos.y) return 3;
    }

    return 0;
}

int drawBackground() {
    for (int i = 0; i < BOARD_WIDTH; ++i) {
        for (int j = 0; j < BOARD_WIDTH; ++j) {
            if ((i + j) % 2 == 0) SDL_SetRenderDrawColor(renderer, bg1.r, bg1.g, bg1.b, bg1.a);
            else SDL_SetRenderDrawColor(renderer, bg2.r, bg2.g, bg2.b, bg2.a);

            SDL_Rect rect = { cell_width*i, cell_height*j, cell_width, cell_height };
            SDL_RenderFillRect(renderer, &rect);
        }
    }
    return 0;
}

int drawSnake() {
    SDL_SetRenderDrawColor(renderer, sc.r, sc.g, sc.b, sc.a);
    struct Cell* current_cell = snake->cells;
    for (; current_cell != NULL; current_cell = current_cell->next) {
        int x = cell_width*current_cell->point.x;
        int y = cell_width*current_cell->point.y;
        SDL_Rect rect = { x, y, cell_width, cell_height };
        SDL_RenderFillRect(renderer, &rect);
    }
    return 0;
}

int drawTarget() {
    SDL_SetRenderDrawColor(renderer, tc.r, tc.g, tc.b, tc.a);
    SDL_Rect rect = { target.x*cell_width, target.y*cell_height, cell_width, cell_height };
    SDL_RenderFillRect(renderer, &rect);

    return 0;
}

int drawOver() {
    SDL_SetRenderDrawColor(renderer, tc.r, tc.g, tc.b, tc.a);
    SDL_Rect rect = { 0, 0, WIN_WIDTH, WIN_HEIGHT };
    SDL_RenderFillRect(renderer, &rect);

    return 0;
}

int render() {
    switch (state) {
        case GAME:
            drawBackground();
            drawSnake();
            drawTarget();
            break;

        case OVER:
            drawOver();

        default: break;
    }

    SDL_RenderPresent(renderer);
    return 0;
}

int logic(uint32_t* last_time) {
    uint32_t current_time = SDL_GetTicks();
    switch (state) {
        case GAME:
            if (current_time > *last_time + TIMESTEP) {
                snake_move(snake);
                switch (checkCollision()) {
                    case 1:
                        newTarget();
                        snake_add_cell(snake);
                        break;
                    case 2:
                    case 3:
                        state = OVER;
                    default:
                        break;

                }
                *last_time = current_time;
            }
            break;
        case OVER:
            break;

        case MENU:
            break;
    }

    return 0;
}


int main() {
    if (init() != 0) return -1;

    reset();

    SDL_Event event;
    uint32_t last = 0, current;
    while (true) {
        while (SDL_PollEvent(&event)) {
            if (process_event(&event) == -1) goto end;
        }

	    current = SDL_GetTicks();
	    switch (state) {
	        case GAME:
	            if (current > last + TIMESTEP) {
	                snake_move(snake);
	                switch (checkCollision()) {
	                    case 1:
	                        newTarget();
	                        snake_add_cell(snake);
	                        break;
	                    case 2:
	                    case 3:
	                        state = OVER;
	                    default:
	                        break;

	                }
	                *last_time = current_time;
	            }
	            break;
	        case OVER:
	            break;

	        case MENU:
	            break;
	    }

        render();
    }


    
end:
    cleanup(CC_All);
}
