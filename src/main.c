#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

// Define MAX and MIN macros
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

#define GRID_WIDTH 20
#define GRID_HEIGHT 15

#define GRID_CELL_SIZE_PX 40
#define FOOD_MARGIN_PX 6
#define FOOD_WIDTH_PX GRID_CELL_SIZE_PX - 2 * FOOD_MARGIN_PX

#define SNAKE_MARGIN_PX 6
#define SNAKE_WIDTH_PX GRID_CELL_SIZE_PX - 2 * SNAKE_MARGIN_PX

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define GRID_CELL_COUNT GRID_WIDTH *GRID_HEIGHT
#define SNAKE_MAX_LENGTH GRID_CELL_COUNT

#define SNAKE_DEFAULT_SPEED 20

#define ALLOW_WRAP

enum Direction
{
    DIRECTION_UP,
    DIRECTION_DOWN,
    DIRECTION_LEFT,
    DIRECTION_RIGHT
};

struct Cell
{
    int8_t x, y;
};

int snake_len = 0;
enum Direction snake_dir = DIRECTION_RIGHT;
struct Cell snake[SNAKE_MAX_LENGTH];
bool snake_collision_lookup[GRID_CELL_COUNT];

struct Cell food;
// int snake_delay_ms = 20;

SDL_Renderer *renderer;
SDL_Rect squareRect;

bool inputReceived = false;
int growCount = 0;
bool game_over = false;
bool game_over_invisible = false;

int snake_speed = SNAKE_DEFAULT_SPEED;
// int ms_per_tick = 1000/snake_speed;

void make_random_food()
{
    int available_space = GRID_CELL_COUNT - snake_len;
    if (!available_space)
    {
        food = (struct Cell){.x = 255, .y = 255};
        return;
    }
    int new_food_pos = rand() % available_space;
    int pos = 0;
    for (int i = 0; i < GRID_WIDTH; i++)
    {
        for (int j = 0; j < GRID_HEIGHT; j++)
        {
            if (snake_collision_lookup[j * GRID_WIDTH + i])
            {
                continue;
            }
            pos++;
            if (pos == new_food_pos)
            {
                food = (struct Cell){.x = i, .y = j};
            }
        }
    }
}

void new_game()
{
    inputReceived = false;
    game_over = false;
    snake_dir = DIRECTION_RIGHT;
    snake_speed = SNAKE_DEFAULT_SPEED;
    make_random_food();
    // ms_per_tick = 100;
    //  memset(&snake, SNAKE_BODY_NULL, sizeof(struct Cell) * SNAKE_MAX_LENGTH);
    snake[0] = (struct Cell){.x = 16, .y = 10};
    snake[1] = (struct Cell){.x = 15, .y = 10};
    snake[2] = (struct Cell){.x = 14, .y = 10};
    snake[3] = (struct Cell){.x = 13, .y = 10};
    snake[4] = (struct Cell){.x = 12, .y = 10};
    snake[5] = (struct Cell){.x = 11, .y = 10};
    snake[6] = (struct Cell){.x = 10, .y = 10};
    snake_len = 7;
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



    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
    // snek
    if (!game_over || !game_over_invisible)
    {
        squareRect.w = GRID_CELL_SIZE_PX;
        squareRect.h = GRID_CELL_SIZE_PX;
        for (int i = snake_len -1; i >= 0; i--)
        {
            struct Cell cell = snake[i];
            //squareRect.x = cell.x * GRID_CELL_SIZE_PX;
            //squareRect.y = cell.y * GRID_CELL_SIZE_PX;

            squareRect.x = cell.x * GRID_CELL_SIZE_PX + SNAKE_MARGIN_PX;
            squareRect.y = cell.y * GRID_CELL_SIZE_PX + SNAKE_MARGIN_PX;
            squareRect.w = SNAKE_WIDTH_PX;
            squareRect.h = SNAKE_WIDTH_PX;

            SDL_RenderFillRect(renderer, &squareRect);

            bool join_up = false;
            bool join_down = false;
            bool join_left = false;
            bool join_right = false;
            if (i > 0) {
                struct Cell prev = snake[i-1];
                if (prev.x < cell.x) {
                    join_left = true;
                }
                if (prev.x > cell.x) {
                    join_right = true;
                }
                if (prev.y < cell.y) {
                    join_up = true;
                }
                if (prev.y > cell.y) {
                    join_down = true;
                }
            }

            if (i < snake_len - 1) {
                struct Cell next = snake[i+1];
                if (next.x < cell.x) {
                    join_left = true;
                }
                if (next.x > cell.x) {
                    join_right = true;
                }
                if (next.y < cell.y) {
                    join_up = true;
                }
                if (next.y > cell.y) {
                    join_down = true;
                }
            }

            if (join_up) {
                squareRect.x = cell.x * GRID_CELL_SIZE_PX + SNAKE_MARGIN_PX;
                squareRect.y = cell.y * GRID_CELL_SIZE_PX;
                squareRect.w = SNAKE_WIDTH_PX;
                squareRect.h = SNAKE_MARGIN_PX;
                SDL_RenderFillRect(renderer, &squareRect);
            }
            if (join_down) {
                squareRect.x = cell.x * GRID_CELL_SIZE_PX + SNAKE_MARGIN_PX;
                squareRect.y = cell.y * GRID_CELL_SIZE_PX + SNAKE_MARGIN_PX + SNAKE_WIDTH_PX;
                squareRect.w = SNAKE_WIDTH_PX;
                squareRect.h = SNAKE_MARGIN_PX;
                SDL_RenderFillRect(renderer, &squareRect);
            }
            if (join_left) {
                squareRect.x = cell.x * GRID_CELL_SIZE_PX;
                squareRect.y = cell.y * GRID_CELL_SIZE_PX + SNAKE_MARGIN_PX;
                squareRect.w = SNAKE_MARGIN_PX;
                squareRect.h = SNAKE_WIDTH_PX;
                SDL_RenderFillRect(renderer, &squareRect);
            }
            if (join_right) {
                squareRect.x = cell.x * GRID_CELL_SIZE_PX + SNAKE_MARGIN_PX + SNAKE_WIDTH_PX;
                squareRect.y = cell.y * GRID_CELL_SIZE_PX + SNAKE_MARGIN_PX;
                squareRect.w = SNAKE_MARGIN_PX;
                squareRect.h = SNAKE_WIDTH_PX;
                SDL_RenderFillRect(renderer, &squareRect);
            }
        }
    }

    // food
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xAA, 0x00, 0xFF);
    squareRect.x = food.x * GRID_CELL_SIZE_PX + FOOD_MARGIN_PX;
    squareRect.y = food.y * GRID_CELL_SIZE_PX + FOOD_MARGIN_PX;
    squareRect.w = FOOD_WIDTH_PX;
    squareRect.h = FOOD_WIDTH_PX;
    SDL_RenderFillRect(renderer, &squareRect);
}

void update_snake_collision_lookup()
{
    for (int i = 0; i < GRID_CELL_COUNT; i++)
    {
        snake_collision_lookup[i] = false;
    }
    for (int i = 0; i < snake_len; i++)
    {
        struct Cell cell = snake[i];
        snake_collision_lookup[cell.y * GRID_WIDTH + cell.x] = true;
    }
}

void tick()
{
    if (game_over)
    {
        game_over_invisible = !game_over_invisible;
        return;
    }
    update_snake_collision_lookup();
    struct Cell head = snake[0];
    struct Cell tmp = head;
    struct Cell tmp2;

    switch (snake_dir)
    {
    case DIRECTION_UP:
        head.y--;
        break;
    case DIRECTION_DOWN:
        head.y++;
        break;
    case DIRECTION_RIGHT:
        head.x++;
        break;
    case DIRECTION_LEFT:
        head.x--;
        break;
    }

    #ifdef ALLOW_WRAP
    head.x = (head.x + GRID_WIDTH) % GRID_WIDTH;
    head.y = (head.y + GRID_HEIGHT) % GRID_HEIGHT;
    #endif

    if (
        head.x < 0 ||
        head.x >= GRID_WIDTH ||
        head.y < 0 ||
        head.y >= GRID_HEIGHT ||
        snake_collision_lookup[head.y * GRID_WIDTH + head.x])
    {
        game_over = true;
        // set slow snake speed to slow down flickering of snake
        snake_speed = SNAKE_DEFAULT_SPEED;
        return;
    }

    if (head.x == food.x && head.y == food.y)
    {
        growCount++;
        snake_speed++;
        make_random_food();
    }

    snake[0] = head;
    for (int i = 1; i < snake_len; i++)
    {
        tmp2 = snake[i];
        snake[i] = tmp;
        tmp = tmp2;
    }
    if (growCount)
    {
        snake[snake_len] = (struct Cell){.x = 255, .y = 255};
        snake_len++;
        growCount--;
    }
    inputReceived = false;
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
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
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
                int ms_per_tick = 4000 / snake_speed;
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
                    if (!inputReceived)
                    {
                        inputReceived = true;
                        switch (e.key.keysym.sym)
                        {
                        case SDLK_LEFT:
                            snake_dir = DIRECTION_LEFT;
                            break;
                        case SDLK_RIGHT:
                            snake_dir = DIRECTION_RIGHT;
                            break;
                        case SDLK_UP:
                            snake_dir = DIRECTION_UP;
                            break;
                        case SDLK_DOWN:
                            snake_dir = DIRECTION_DOWN;
                            break;
                        default:
                            inputReceived = false;
                            break;
                        }
                    }

                    switch (e.key.keysym.sym)
                    {
                    case SDLK_SPACE:
                        growCount = 1;
                        break;
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
