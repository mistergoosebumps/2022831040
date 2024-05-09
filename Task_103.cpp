#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define CIRCLE_RADIUS 30
#define CIRCLE_SPEED 5
#define BLINK_DURATION 10

bool initializeSDL(SDL_Window** window, SDL_Renderer** renderer) 
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) 
    {
        printf("SDL initialization failed: %s\n", SDL_GetError());
        
        return false;
    }

    *window = SDL_CreateWindow("Circle Collision", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

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

void drawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius) 
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

    int circle1X = 0;

    int circle1Y = SCREEN_HEIGHT / 2;

    int circle2X = SCREEN_WIDTH / 2;

    int circle2Y = 0;

    bool collided = false;

    int blinkTimer = 0;

    while (running) 
    {
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
                        circle2Y -= CIRCLE_SPEED;
                        break;
                    case SDLK_DOWN:
                        circle2Y += CIRCLE_SPEED;
                        break;
                    case SDLK_LEFT:
                        circle2X -= CIRCLE_SPEED;
                        break;
                    case SDLK_RIGHT:
                        circle2X += CIRCLE_SPEED;
                        break;
                    default:
                        break;
                }
            }
        }

        circle1X += CIRCLE_SPEED;

        if (circle1X > SCREEN_WIDTH + CIRCLE_RADIUS) 
        {
            circle1X = -CIRCLE_RADIUS;
        }

        int dx = circle1X - circle2X;

        int dy = circle1Y - circle2Y;

        int distanceSquared = dx * dx + dy * dy;

        int combinedRadius = CIRCLE_RADIUS * 2;

        bool collision = distanceSquared <= combinedRadius * combinedRadius;

        if (collision && !collided) 
        {
            collided = true;

            circle1X -= CIRCLE_SPEED;

            blinkTimer = BLINK_DURATION;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        drawCircle(renderer, circle1X, circle1Y, CIRCLE_RADIUS);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

        drawCircle(renderer, circle2X, circle2Y, CIRCLE_RADIUS);

        if (collided) 
        {
            if (blinkTimer > 0) 
            {
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);

                drawCircle(renderer, circle2X, circle2Y, CIRCLE_RADIUS);

                blinkTimer--;
            } 
            else 
            {
                collided = false;

                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

                drawCircle(renderer, circle2X, circle2Y, CIRCLE_RADIUS);
            }
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}