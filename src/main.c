#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_timer.h>
#include "engine.h"
#define M_PI 3.14159265358979323846


#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1000

int main(int argc, char **argv) {
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    
    // Initiera SDL, window och renderer
    if (!initSDL(&window, &renderer, WINDOW_WIDTH, WINDOW_HEIGHT)) {
        return 1;
    }
    
    SDL_Texture *texture = loadImage(renderer, "resources/superbird.png");
    if (!texture) {
        cleanup(window, renderer, texture);
        return 1;
    }

    
    // Hämta texturens dimensioner
    SDL_Rect birdRect;
    SDL_QueryTexture(texture, NULL, NULL, &birdRect.w, &birdRect.h);
    // Placera fågeln så att bildens center hamnar i mitten av fönstret
    birdRect.x = WINDOW_WIDTH / 2 - birdRect.w / 2;
    birdRect.y = WINDOW_HEIGHT / 2 - birdRect.h / 2;
    
    bool quit = false;
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                quit = true;
        }
        
        // Hämta musens position
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        
        // Beräkna vinkel från fönstrets center till musen (i grader)
        float centerX = WINDOW_WIDTH / 2.0f;
        float centerY = WINDOW_HEIGHT / 2.0f;
        float deltaX = mouseX - centerX;
        float deltaY = mouseY - centerY;
        float angle = atan2(deltaY, deltaX) * 180 / M_PI + 90;
        
        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
        SDL_RenderClear(renderer);
        
        // Rotera bilden kring dess center
        SDL_Point pivot = { birdRect.w / 2, birdRect.h / 2 };
        SDL_RenderCopyEx(renderer, texture, NULL, &birdRect, angle, &pivot, SDL_FLIP_NONE);
        
        SDL_RenderPresent(renderer);
        SDL_Delay(1000 / 60);
    }
    
    // Städning
    cleanup(window, renderer, texture);
    return 0;
}
