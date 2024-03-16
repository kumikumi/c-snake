/*
 * december 2023? 
 * a quick performance experiment based on the snake demo to get an idea of simulation vs rendering bottlenecks with a large number of game objects
 */

#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

// Define MAX and MIN macros
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

#define GRID_WIDTH 160
#define GRID_HEIGHT 120

#define GRID_CELL_SIZE_PX 10
#define FOOD_MARGIN_PX 6
#define FOOD_WIDTH_PX GRID_CELL_SIZE_PX - 2 * FOOD_MARGIN_PX

#define SNAKE_MARGIN_PX 4
#define SNAKE_WIDTH_PX GRID_CELL_SIZE_PX - 2 * SNAKE_MARGIN_PX

#define SCREEN_WIDTH_PX GRID_WIDTH *GRID_CELL_SIZE_PX
#define SCREEN_HEIGHT_PX GRID_HEIGHT *GRID_CELL_SIZE_PX

#define GRID_CELL_COUNT GRID_WIDTH *GRID_HEIGHT

#define NUM_CARS 3000000
#define NUM_RENDERED_CARS 300

#define MS_PER_TICK 100

enum Direction
{
    DIRECTION_UP = 0,
    DIRECTION_DOWN = 1,
    DIRECTION_LEFT = 2,
    DIRECTION_RIGHT = 3
};

struct Coords
{
    int16_t x, y;
};

typedef struct Coords Coords;

struct Car
{
    Coords coords;
};
typedef struct Car Car;

// int snake_delay_ms = 20;

struct GameState
{
    Car cars[NUM_CARS];
};

typedef struct GameState GameState;

SDL_Renderer *renderer;
SDL_Rect squareRect;
GameState game;

void new_game()
{
}

void draw()
{
    // Initialize renderer color white for the background
    SDL_SetRenderDrawColor(renderer, 0x42, 0x42, 0x62, 0xFF);
    // Clear screen
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0x80, 0x80, 0x80, 0xFF);

    squareRect.w = GRID_CELL_SIZE_PX;
    squareRect.h = GRID_CELL_SIZE_PX;

    // grid
    for (int i = 0; i < GRID_WIDTH; i++)
    {
        for (int j = 0; j < GRID_HEIGHT; j++)
        {
            squareRect.x = i * GRID_CELL_SIZE_PX;
            squareRect.y = j * GRID_CELL_SIZE_PX;
            SDL_RenderDrawRect(renderer, &squareRect);
        }
    }

    // cars
    SDL_SetRenderDrawColor(renderer, 0x80, 0x00, 0x00, 0xFF);

    for (int i = 0; i < NUM_RENDERED_CARS; i++)
    {
        Car car = game.cars[i];
        // printf("(%d, %d)", car.coords.x, car.coords.y);
        squareRect.x = car.coords.x * GRID_CELL_SIZE_PX;
        squareRect.y = car.coords.y * GRID_CELL_SIZE_PX;
        SDL_RenderFillRect(renderer, &squareRect);
    }
}

void tick()
{
    for (int i = 0; i < NUM_CARS; i++)
    {
        Car car = game.cars[i];
        int dir = 0; //rand() % 5;
        switch (dir)
        {
        case DIRECTION_UP:
            car.coords.y--;
            break;
        case DIRECTION_DOWN:
            car.coords.y++;
            break;
        case DIRECTION_LEFT:
            car.coords.x--;
            break;
        case DIRECTION_RIGHT:
            car.coords.x++;
            break;
        default:
            break;
        }
        car.coords.x = (car.coords.x + GRID_WIDTH) % GRID_WIDTH;
        car.coords.y = (car.coords.y + GRID_HEIGHT) % GRID_HEIGHT;

        game.cars[i] = car;
    }
}

int main(int argc, char *argv[])
{
    printf("Hello world!\n");

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not be initialized!\n"
               "SDL_Error: %s\n",
               SDL_GetError());
        return 0;
    }

    SDL_Window *window = SDL_CreateWindow("Basic C SDL project",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SCREEN_WIDTH_PX, SCREEN_HEIGHT_PX,
                                          SDL_WINDOW_SHOWN);
    if (!window)
    {
        printf("Window could not be created!\n"
               "SDL_Error: %s\n",
               SDL_GetError());
    }
    else
    {
        // Create renderer
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer)
        {
            printf("Renderer could not be created!\n"
                   "SDL_Error: %s\n",
                   SDL_GetError());
        }
        else
        {
            // Event loop exit flag
            bool quit = false;

            new_game();

            Uint32 timeBuffer = 0;
            Uint32 lastTimeMs = SDL_GetTicks();

            // Event loop
            while (!quit)
            {
                SDL_Event e;

                // Wait for the next available event
                SDL_WaitEventTimeout(&e, 10);
                Uint32 currentTime = SDL_GetTicks();
                timeBuffer += currentTime - lastTimeMs;
                lastTimeMs = currentTime;
                int ms_per_tick = MS_PER_TICK;
                while (timeBuffer > ms_per_tick)
                {
                    tick();
                    timeBuffer -= ms_per_tick;
                }

                // User requests quit
                if (e.type == SDL_QUIT)
                {
                    quit = true;
                }

                if (e.type == SDL_KEYDOWN)
                {
                    switch (e.key.keysym.sym)
                    {
                    case SDLK_F1:
                    case SDLK_RETURN:
                        new_game();
                        break;
                    default:
                        break;
                    }
                }

                draw();

                // Update screen
                SDL_RenderPresent(renderer);

                // SDL_Delay(600);
                // tick();
            }

            // Destroy renderer
            SDL_DestroyRenderer(renderer);
        }

        // Destroy window
        SDL_DestroyWindow(window);
    }

    // Quit SDL
    SDL_Quit();

    return 0;
}
