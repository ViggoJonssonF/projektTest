#include "engine.h"
#include <SDL.h>
#include "defs.h"

void handleInput(bool *quit, bool *placingBird, SDL_Rect iconRect, int *money, int *numPlacedBirds, Bird superbird, Bird placedBirds[]) {
    SDL_Event event;
    // Beräkna relativa gränser (ungefär 45,3% och 54,7% av WINDOW_WIDTH)
    int leftBoundary = (int)(WINDOW_WIDTH * 0.453125);
    int rightBoundary = (int)(WINDOW_WIDTH * 0.546875);
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
                // Om musen är inom de relativa gränserna – förhindra placering
                if (mx >= leftBoundary && mx <= rightBoundary) {
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