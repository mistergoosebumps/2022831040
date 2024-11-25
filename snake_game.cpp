#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define BLOCK_SIZE 20
#define SNAKE_SPEED 5

typedef struct SnakeSegment 
{
    int x, y;
} SnakeSegment;

void renderText(SDL_Renderer* renderer, const char* message, int x, int y, SDL_Color color);

bool initializeSDL(SDL_Window** window, SDL_Renderer** renderer) 
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) 
    {
        printf("SDL initialization failed: %s\n", SDL_GetError());
        return false;
    }

    *window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (*window == NULL) 
    {
        printf("Window creation failed: %s\n", SDL_GetError());
        return false;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (*renderer == NULL) 
    {
        printf("Renderer creation failed: %s\n", SDL_GetError());
        return false;
    }

    return true;
}

int main(int argc, char* argv[]) 
{
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    if (!initializeSDL(&window, &renderer)) 
    {
        return 1;
    }

    srand(time(NULL));

    bool running = true;
    SDL_Event event;

    // Snake setup
    SnakeSegment snake[100];
    int snakeLength = 5;
    int snakeDirX = 1, snakeDirY = 0;

    // Initialize snake position
    for (int i = 0; i < snakeLength; ++i) 
    {
        snake[i].x = SCREEN_WIDTH / 2 - i * BLOCK_SIZE;
        snake[i].y = SCREEN_HEIGHT / 2;
    }

    // Food setup
    int foodX = (rand() % (SCREEN_WIDTH / BLOCK_SIZE)) * BLOCK_SIZE;
    int foodY = (rand() % (SCREEN_HEIGHT / BLOCK_SIZE)) * BLOCK_SIZE;

    // Score setup
    int score = 0;

    // Timing variables
    Uint32 lastTime = SDL_GetTicks();

    while (running) 
    {
        // Event handling
        while (SDL_PollEvent(&event)) 
        {
            if (event.type == SDL_QUIT) 
            {
                running = false;
            } 

            else if (event.type == SDL_KEYDOWN) 
            {
                switch (event.key.keysym.sym) 
                {
                    case SDLK_UP:
                        if (snakeDirY == 0) 
                        {
                            snakeDirX = 0;
                            snakeDirY = -1;
                        }
                        break;

                    case SDLK_DOWN:
                        if (snakeDirY == 0) 
                        {
                            snakeDirX = 0;
                            snakeDirY = 1;
                        }
                        break;

                    case SDLK_LEFT:
                        if (snakeDirX == 0) 
                        {
                            snakeDirX = -1;
                            snakeDirY = 0;
                        }
                        break;
                    case SDLK_RIGHT:
                        if (snakeDirX == 0) 
                        {
                            snakeDirX = 1;
                            snakeDirY = 0;
                        }

                        break;
                }
            }
        }

        // Game logic: For moving the snake
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastTime > 100) 
        { 
            // For controlling the snake speed
            lastTime = currentTime;

            // For moving snake body
            for (int i = snakeLength - 1; i > 0; --i) 
            {
                snake[i] = snake[i - 1];
            }

            // For moving snake head
            snake[0].x += snakeDirX * BLOCK_SIZE;
            snake[0].y += snakeDirY * BLOCK_SIZE;

            // for checking collision with food
            if (snake[0].x == foodX && snake[0].y == foodY) 
            {
                score += 10;
                snakeLength++;
                foodX = (rand() % (SCREEN_WIDTH / BLOCK_SIZE)) * BLOCK_SIZE;
                foodY = (rand() % (SCREEN_HEIGHT / BLOCK_SIZE)) * BLOCK_SIZE;
            }

            // for checking collision with boundaries
            if (snake[0].x < 0 || snake[0].x >= SCREEN_WIDTH || snake[0].y < 0 || snake[0].y >= SCREEN_HEIGHT) 
            {
                running = false;
            }
        }

        // Rendering
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Render food
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect foodRect = {foodX, foodY, BLOCK_SIZE, BLOCK_SIZE};
        SDL_RenderFillRect(renderer, &foodRect);

        // Render snake
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        for (int i = 0; i < snakeLength; ++i) 
        {
            SDL_Rect segmentRect = {snake[i].x, snake[i].y, BLOCK_SIZE, BLOCK_SIZE};

            SDL_RenderFillRect(renderer, &segmentRect);
        }

        // Render score
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        char scoreText[32];

        sprintf(scoreText, "Score: %d", score);

        renderText(renderer, scoreText, 10, 10, (SDL_Color){255, 255, 255, 255});

        SDL_RenderPresent(renderer);
    }

    // Game over screen

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    SDL_RenderClear(renderer);

    renderText(renderer, "Game Over", SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2 - 20, (SDL_Color){255, 0, 0, 255});

    char finalScore[32];

    sprintf(finalScore, "Final Score: %d", score);

    renderText(renderer, finalScore, SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2 + 20, (SDL_Color){255, 255, 255, 255});

    SDL_RenderPresent(renderer);

    SDL_Delay(3000);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void renderText(SDL_Renderer* renderer, const char* message, int x, int y, SDL_Color color) 
{
    
}
