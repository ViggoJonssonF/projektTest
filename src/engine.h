#ifndef ENGINE_H
#define ENGINE_H

#include <SDL.h>
#include <SDL_image.h>
#include <stdbool.h>

// Initierar SDL, skapar window och renderer. Returnerar true om allt gick bra.
bool initSDL(SDL_Window **window, SDL_Renderer **renderer, int windowWidth, int windowHeight);

// Laddar en textur från en given sökväg.
SDL_Texture* loadTexture(SDL_Renderer *renderer, const char *path);

// Ny funktion: En wrapper för att ladda en bild från resources.
// Du kan anropa denna funktion för att ladda olika bilder framöver.
SDL_Texture* loadImage(SDL_Renderer *renderer, const char *path);

// Städar upp: förstör texture, renderer och window samt avslutar SDL.
void cleanup(SDL_Window *window, SDL_Renderer *renderer, SDL_Texture *texture);

#endif
