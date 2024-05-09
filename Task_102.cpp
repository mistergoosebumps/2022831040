#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define INITIAL_RADIUS 50
#define RADIUS_INCREMENT 2

bool initializeSDL(SDL_Window** window, SDL_Renderer** renderer) 
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) 
    {
        printf("SDL initialization failed: %s\n", SDL_GetError());

        return false;
    }

    *window = SDL_CreateWindow("Circle Drawing", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

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

void drawSolidCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius) 
{
    for (int x = -radius; x <= radius; x++) 
    {
        for (int y = -radius; y <= radius; y++) 
        {
            if (x*x + y*y <= radius*radius) 
            {
                SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
            }
        }
    }
}

int main(int argc, char* argv[]) 
{
    SDL_Window* window = NULL;

    SDL_Renderer* renderer = NULL;

    if (!initializeSDL(&window, &renderer)) 
    {
        return 1;
    }

    SDL_Event event;

    bool running = true;

    int radius = INITIAL_RADIUS;

    int centerX = SCREEN_WIDTH / 2;

    int centerY = SCREEN_HEIGHT / 2;

    while (running) 
    {
        while (SDL_PollEvent(&event)) 
        {
            if (event.type == SDL_QUIT) 
            {
                running = false;
            }
        }

        radius += RADIUS_INCREMENT;

        if (centerX - radius <= 0 || centerX + radius >= SCREEN_WIDTH || centerY - radius <= 0 || centerY + radius >= SCREEN_HEIGHT) 
        {
            radius = INITIAL_RADIUS;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255,255,255, 255);

        drawSolidCircle(renderer, centerX, centerY, radius);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);

    SDL_DestroyWindow(window);
    
    SDL_Quit();

    return 0;
}