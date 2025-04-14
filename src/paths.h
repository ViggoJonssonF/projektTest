#ifndef PATHS_H
#define PATHS_H
#include <SDL.h>
#include "defs.h"
#define NUM_POINTS 15
typedef struct {
    SDL_Point left[NUM_POINTS];
    SDL_Point right[NUM_POINTS];
} pathPair;
typedef struct {
    int nmbrOfPoints;
    pathPair points;
} Paths;
Paths createPaths(void);
#endif
