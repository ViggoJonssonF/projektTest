#define ENGINE_H
#include <SDL.h>
#include <SDL_image.h>
#include <stdbool.h>
#include <SDL_mixer.h>  // Lägg till SDL_mixer

// Initierar SDL, skapar window och renderer. Returnerar true om allt gick bra.
bool initSDL(SDL_Window **window, SDL_Renderer **renderer, int windowWidth, int windowHeight);

// Laddar en textur från en given sökväg.
SDL_Texture* loadTexture(SDL_Renderer *renderer, const char *path);

// Wrapper för att ladda en bild från resources.
SDL_Texture* loadImage(SDL_Renderer *renderer, const char *path);

// Städar upp: förstör texture, renderer och window samt avslutar SDL.
void cleanup(SDL_Window *window, SDL_Renderer *renderer, SDL_Texture *texture);

// --- Nya ljudfunktioner ---
bool initAudio();       // Initierar SDL_mixer och laddar pop-ljudet.
void playPopSound();    // Spelar upp pop-ljudet.
void cleanupAudio();    // Städar upp ljudresurser.

