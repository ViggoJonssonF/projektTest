#include "engine.h"
#include <SDL.h>
#include "defs.h"

void handleInputMulti(bool *quit, bool *placingBird, SDL_Rect iconRects[], int numIcons,
                      int *selectedOption, int *money, int *numPlacedBirds, Bird towerOptions[], Bird placedBirds[]) {
    SDL_Event event;
    int leftBoundary = (int)(WINDOW_WIDTH * 0.453125);
    int rightBoundary = (int)(WINDOW_WIDTH * 0.546875);
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            *quit = true;
        }
        if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
            int mx = event.button.x;
            int my = event.button.y;
            if (!(*placingBird)) {
                for (int i = 0; i < numIcons; i++) {
                    if (mx >= iconRects[i].x && mx <= iconRects[i].x + iconRects[i].w &&
                        my >= iconRects[i].y && my <= iconRects[i].y + iconRects[i].h) {
                        *placingBird = true;
                        *selectedOption = i;
                        break;
                    }
                }
            } else {
                if (mx >= leftBoundary && mx <= rightBoundary) {
                    *placingBird = false;
                    *selectedOption = -1;
                    return;
                }
                if (*selectedOption != -1 &&
                    *money >= towerOptions[*selectedOption].cost &&
                    *numPlacedBirds < MAX_PLACED_BIRDS) {
                    Bird newBird = towerOptions[*selectedOption];
                    newBird.x = (float)mx;
                    newBird.y = (float)my;
                    newBird.attackTimer = 0.0f;
                    placedBirds[(*numPlacedBirds)++] = newBird;
                    *money -= towerOptions[*selectedOption].cost;
                }
                *placingBird = false;
                *selectedOption = -1;
            }
        }
    }
}
