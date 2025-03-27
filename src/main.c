#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_timer.h>
#include <SDL_image.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1000

int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }
    
    SDL_Window* window = SDL_CreateWindow("TEST PROJEKT",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!window) {
        printf("Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
                                                 SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    // Ladda bilden "redbird.png" från resources-mappen
    SDL_Surface* surface = IMG_Load("resources/redbird.png");
    if (!surface) {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    SDL_Rect birdRect;
    SDL_QueryTexture(texture, NULL, NULL, &birdRect.w, &birdRect.h);
    // Placera fågeln så att dess center hamnar i fönstrets mitt.
    birdRect.x = WINDOW_WIDTH / 2 - birdRect.w / 2;
    birdRect.y = WINDOW_HEIGHT / 2 - birdRect.h / 2;
    
    bool quit = false;
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                quit = true;
        }
        
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        
        // Eftersom fågeln är centrerad i fönstret, är dess rotationspunkt (bildens center)
        // samma som fönstrets center.
        float centerX = WINDOW_WIDTH / 2.0f;
        float centerY = WINDOW_HEIGHT / 2.0f;
        float deltaX = mouseX - centerX;
        float deltaY = mouseY - centerY;
        
        // Beräkna vinkeln från center till musen (i grader)
        float angle = atan2(deltaY, deltaX) * 180 / M_PI;
        // Eftersom den oroterade bilden har näbben (den del som ska peka mot musen)
        // vid top-center, justera med +90 grader så att rotationen blir korrekt.
        angle += 90;
        
        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
        SDL_RenderClear(renderer);
        
        // Rotera runt bildens center
        SDL_Point pivot = { birdRect.w / 2, birdRect.h / 2 };
        SDL_RenderCopyEx(renderer, texture, NULL, &birdRect, angle, &pivot, SDL_FLIP_NONE);
        
        SDL_RenderPresent(renderer);
        SDL_Delay(1000 / 60);
    }
    
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
