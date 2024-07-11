#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <time.h>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define CELL_SIZE 1

#define GRID_WIDTH (SCREEN_WIDTH / CELL_SIZE)
#define GRID_HEIGHT (SCREEN_HEIGHT / CELL_SIZE)

typedef struct
{
    bool current;
    bool next;
} Cell;

Cell grid[GRID_WIDTH][GRID_HEIGHT];
SDL_Renderer *renderer;
bool isPaused = false;

void clear_grid()
{
    for (int x = 0; x < GRID_WIDTH; x++)
    {
        for (int y = 0; y < GRID_HEIGHT; y++)
        {
            grid[x][y].current = false;
            grid[x][y].next = false;
        }
    }
}

void randomize_grid()
{
    srand((unsigned int)time(NULL));
    for (int x = 0; x < GRID_WIDTH; x++)
    {
        for (int y = 0; y < GRID_HEIGHT; y++)
        {
            grid[x][y].current = rand() % 2;
        }
    }
}

void update_grid()
{
    for (int x = 0; x < GRID_WIDTH; x++)
    {
        for (int y = 0; y < GRID_HEIGHT; y++)
        {
            int aliveNeighbors = 0;
            for (int dx = -1; dx <= 1; dx++)
            {
                for (int dy = -1; dy <= 1; dy++)
                {
                    if (dx == 0 && dy == 0)
                        continue;

                    int nx = (x + dx + GRID_WIDTH) % GRID_WIDTH;
                    int ny = (y + dy + GRID_HEIGHT) % GRID_HEIGHT;

                    if (grid[nx][ny].current)
                    {
                        aliveNeighbors++;
                    }
                }
            }

            if (grid[x][y].current)
            {
                if (aliveNeighbors < 2 || aliveNeighbors > 3)
                {
                    grid[x][y].next = false;
                }
                else
                {
                    grid[x][y].next = true;
                }
            }
            else
            {
                if (aliveNeighbors == 3)
                {
                    grid[x][y].next = true;
                }
                else
                {
                    grid[x][y].next = false;
                }
            }
        }
    }

    for (int x = 0; x < GRID_WIDTH; x++)
    {
        for (int y = 0; y < GRID_HEIGHT; y++)
        {
            grid[x][y].current = grid[x][y].next;
        }
    }
}

void draw_grid()
{
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);

    for (int x = 0; x < GRID_WIDTH; x++)
    {
        for (int y = 0; y < GRID_HEIGHT; y++)
        {
            if (grid[x][y].current)
            {
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            }
            else
            {
                SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
            }

            SDL_Rect cellRect = {x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
            SDL_RenderFillRect(renderer, &cellRect);
        }
    }
    SDL_RenderPresent(renderer);
}

void handle_mouse_click(int x, int y)
{
    int gridX = x / CELL_SIZE;
    int gridY = y / CELL_SIZE;

    grid[gridX][gridY].current = !grid[gridX][gridY].current;
}

unsigned int get_time_ms()
{
    return SDL_GetTicks();
}

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not be initialized!\nSDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    SDL_Window *window = SDL_CreateWindow("Conway's Game of Life",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if (!window)
    {
        printf("Window could not be created!\nSDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        printf("Renderer could not be created!\nSDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    clear_grid();

    bool quit = false;
    SDL_Event e;
    unsigned int lastTime = get_time_ms();
    while (!quit)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
            else if (e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_SPACE:
                    isPaused = !isPaused;
                    break;
                case SDLK_r:
                    randomize_grid();
                    break;
                default:
                    break;
                }
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                if (isPaused)
                {
                    int x, y;
                    SDL_GetMouseState(&x, &y);
                    handle_mouse_click(x, y);
                }
            }
        }

        unsigned int start_simulation_time = get_time_ms();
        if (!isPaused && get_time_ms() - lastTime > 50) // Adjusted timing
        {
            update_grid();
            lastTime = get_time_ms();
        }
        unsigned int end_simulation_time = get_time_ms();

        unsigned int start_rendering_time = end_simulation_time;
        draw_grid();
        unsigned int end_rendering_time = get_time_ms();

        printf("Simulation Time: %u ms, Rendering Time: %u ms\n",
               end_simulation_time - start_simulation_time,
               end_rendering_time - start_rendering_time);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}