#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
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

bool initializeSDL(SDL_Window** window, SDL_Renderer** renderer, TTF_Font** font) 
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) 
    {
        printf("SDL initialization failed: %s\n", SDL_GetError());

        return false;
    }

    if (TTF_Init() != 0) 
    {
        printf("SDL_ttf initialization failed: %s\n", TTF_GetError());

        return false;
    }

    *window = SDL_CreateWindow("Sneaky Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
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

    *font = TTF_OpenFont("arial.ttf", 24); // for the selection of font for scoreboard and game over message

    if (*font == NULL) 
    {
        printf("Font loading failed: %s\n", TTF_GetError());

        return false;
    }

    return true;
}

void renderText(SDL_Renderer* renderer, TTF_Font* font, const char* message, int x, int y, SDL_Color color) 
{
    SDL_Surface* surface = TTF_RenderText_Solid(font, message, color);

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect textRect = {x, y, surface->w, surface->h};

    SDL_RenderCopy(renderer, texture, NULL, &textRect);

    SDL_FreeSurface(surface);

    SDL_DestroyTexture(texture);
}

void placeFood(int* x, int* y) 
{
    *x = (rand() % (SCREEN_WIDTH / BLOCK_SIZE)) * BLOCK_SIZE;

    *y = (rand() % (SCREEN_HEIGHT / BLOCK_SIZE)) * BLOCK_SIZE;
}

bool checkSelfCollision(SnakeSegment* snake, int snakeLength) 
{
    for (int i = 1; i < snakeLength; ++i) 
    {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) 
        {
            return true; // Collision detected
        }
    }

    return false;
}

int main(int argc, char* argv[]) 
{
    SDL_Window* window = NULL;

    SDL_Renderer* renderer = NULL;

    TTF_Font* font = NULL;

    if (!initializeSDL(&window, &renderer, &font)) 
    {
        return 1;
    }

    srand(time(NULL));

    bool running = true;

    SDL_Event event;

    // Snake setup

    SnakeSegment snake[200];

    int snakeLength = 3;

    int snakeDirX = 1, snakeDirY = 0;

    // Initializing snake position

    for (int i = 0; i < snakeLength; ++i) 
    {
        snake[i].x = SCREEN_WIDTH / 2 - i * BLOCK_SIZE;

        snake[i].y = SCREEN_HEIGHT / 2;
    }

    // Regular food setup

    int foodX, foodY;

    placeFood(&foodX, &foodY);

    // Bonus food setup

    int bonusFoodX = -BLOCK_SIZE; // Start outside screen

    int bonusFoodY = -BLOCK_SIZE;

    bool bonusFoodActive = false;

    // Score setup

    int score = 0;

    int foodsEaten = 0;

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

        // Game logic: Move the snake
        
        Uint32 currentTime = SDL_GetTicks();

        if (currentTime - lastTime > 100) 
        { 
            // Control the snake speed

            lastTime = currentTime;

            // Move snake body

            for (int i = snakeLength - 1; i > 0; --i) 
            {
                snake[i] = snake[i - 1];
            }

            // Move snake head

            snake[0].x += snakeDirX * BLOCK_SIZE;

            snake[0].y += snakeDirY * BLOCK_SIZE;

            // Check collision with regular food

            if (snake[0].x == foodX && snake[0].y == foodY) 
            {
                score += 10;

                snakeLength++;

                foodsEaten++;

                placeFood(&foodX, &foodY);

                // Activate bonus food after every 5 foods eaten

                if (foodsEaten % 5 == 0) 
                {
                    placeFood(&bonusFoodX, &bonusFoodY);

                    bonusFoodActive = true;
                }
            }

            // Check collision with bonus food

            if (bonusFoodActive && snake[0].x == bonusFoodX && snake[0].y == bonusFoodY) 
            {
                score += 50; // Bonus score

                snakeLength += 2; // Bonus growth

                bonusFoodActive = false;

                bonusFoodX = -BLOCK_SIZE;

                bonusFoodY = -BLOCK_SIZE;
            }

            // Check collision with boundaries

            if (snake[0].x < 0 || snake[0].x >= SCREEN_WIDTH || snake[0].y < 0 || snake[0].y >= SCREEN_HEIGHT) 
            {
                running = false;
            }

            // Check collision with itself

            if (checkSelfCollision(snake, snakeLength)) 
            {
                running = false;
            }
        }

        // Rendering

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        SDL_RenderClear(renderer);

        // Render regular food

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

        SDL_Rect foodRect = {foodX, foodY, BLOCK_SIZE, BLOCK_SIZE};

        SDL_RenderFillRect(renderer, &foodRect);

        // Render bonus food

        if (bonusFoodActive) 
        {
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

            SDL_Rect bonusFoodRect = {bonusFoodX, bonusFoodY, BLOCK_SIZE, BLOCK_SIZE};

            SDL_RenderFillRect(renderer, &bonusFoodRect);
        }

        // Render snake

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

        for (int i = 0; i < snakeLength; ++i) 
        {
            SDL_Rect segmentRect = {snake[i].x, snake[i].y, BLOCK_SIZE, BLOCK_SIZE};

            SDL_RenderFillRect(renderer, &segmentRect);
        }

        // Render score

        char scoreText[32];

        sprintf(scoreText, "Score: %d", score);

        renderText(renderer, font, scoreText, 10, 10, (SDL_Color){255, 255, 255, 255});

        SDL_RenderPresent(renderer);
    }

    // Game over screen

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    SDL_RenderClear(renderer);

    renderText(renderer, font, "Game Over", SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2 - 20, (SDL_Color){255, 0, 0, 255});

    char finalScore[32];

    sprintf(finalScore, "Final Score: %d", score);

    renderText(renderer, font, finalScore, SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2 + 20, (SDL_Color){255, 255, 255, 255});
    
    SDL_RenderPresent(renderer);

    SDL_Delay(3000); // Pause for 3 seconds

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
