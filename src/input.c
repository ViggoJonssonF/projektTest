#include "engine.h"
#include <SDL.h>
#include "defs.h"

void handleInput(bool *quit, bool *placingBird, SDL_Rect iconRect, int *money, int *numPlacedBirds, Bird superbird, Bird placedBirds[]) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            *quit = true;
        }
        if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
            int mx = event.button.x;
            int my = event.button.y;
            if (!*placingBird) {
                if (mx >= iconRect.x && mx <= iconRect.x + iconRect.w &&
                    my >= iconRect.y && my <= iconRect.y + iconRect.h) {
                    *placingBird = true;
                }
            } else {
                // Förhindra placering om x är mellan 870 och 1050
                if (mx >= 870 && mx <= 1050) {
                    // Du kan välja att visa ett meddelande eller bara ignorera placeringen.
                    *placingBird = false;
                    return;
                }
                
                // Placera en ny fågel om spelaren har råd
                if (*money >= superbird.cost && *numPlacedBirds < MAX_PLACED_BIRDS) {
                    Bird newBird = superbird;
                    newBird.x = (float)mx;
                    newBird.y = (float)my;
                    newBird.attackTimer = 0.0f;
                    placedBirds[(*numPlacedBirds)++] = newBird;
                    *money -= superbird.cost;
                }
                *placingBird = false;
            }
        }
    }
}

