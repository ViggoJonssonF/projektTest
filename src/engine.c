#include "engine.h"
#include <stdio.h>

Mix_Music *popSound = NULL;

bool initAudio() {
    // Initiera SDL_mixer: frekvens, format, antal kanaler, chunk size
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return false;
    }
    // Ladda pop-ljudet från en MP3-fil istället för WAV
    popSound = Mix_LoadMUS("resources/pop.mp3");
    if (!popSound) {
        printf("Failed to load pop sound! SDL_mixer Error: %s\n", Mix_GetError());
        return false;
    }
    return true;
}


void playPopSound() {
    if (popSound) {
        Mix_PlayMusic(popSound, 1);
    }
}


void cleanupAudio() {
    if (popSound) {
        Mix_FreeMusic(popSound);
        popSound = NULL;
    }
    Mix_CloseAudio();
}


bool initSDL(SDL_Window **window, SDL_Renderer **renderer, int windowWidth, int windowHeight) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return false;
    }
    
    // Initiera SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("IMG_Init Error: %s\n", IMG_GetError());
        SDL_Quit();
        return false;
    }
    
    *window = SDL_CreateWindow("TEST PROJEKT",
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                               windowWidth, windowHeight,
                               0);
    if (!*window) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return false;
    }
    
    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!*renderer) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return false;
    }
    return true;
}



SDL_Texture* loadTexture(SDL_Renderer *renderer, const char *path) {
    SDL_Surface *surface = IMG_Load(path);
    if (!surface) {
        printf("IMG_Load Error: %s\n", SDL_GetError());
        return NULL;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        printf("SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());
    }
    return texture;
}

SDL_Texture* loadImage(SDL_Renderer *renderer, const char *path) {
    SDL_Texture* texture = loadTexture(renderer, path);
    if (!texture) {
        printf("Error: failed to load image from %s\n", path);
    }
    return texture;
}

void cleanup(SDL_Window *window, SDL_Renderer *renderer, SDL_Texture *texture) {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}
