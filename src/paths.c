#include "paths.h"
Paths createPaths(void) {
    Paths p;
    p.nmbrOfPoints = NUM_POINTS;
    p.points.left[0]  = (SDL_Point){ (int)(WINDOW_WIDTH / 3.84), 0 };
    p.points.left[1]  = (SDL_Point){ (int)(WINDOW_WIDTH / 3.84), (int)(WINDOW_HEIGHT * 0.09) };
    p.points.left[2]  = (SDL_Point){ (int)(WINDOW_WIDTH / 10.67), (int)(WINDOW_HEIGHT * 0.09) };
    p.points.left[3]  = (SDL_Point){ (int)(WINDOW_WIDTH / 10.67), (int)(WINDOW_HEIGHT * 0.20) };
    p.points.left[4]  = (SDL_Point){ (int)(WINDOW_WIDTH / 6.857), (int)(WINDOW_HEIGHT * 0.20) };
    p.points.left[5]  = (SDL_Point){ (int)(WINDOW_WIDTH / 2.341), (int)(WINDOW_HEIGHT * 0.20) };
    p.points.left[6]  = (SDL_Point){ (int)(WINDOW_WIDTH / 2.341), (int)(WINDOW_HEIGHT * 0.35) };
    p.points.left[7]  = (SDL_Point){ (int)(WINDOW_WIDTH / 3.2),   (int)(WINDOW_HEIGHT * 0.35) };
    p.points.left[8]  = (SDL_Point){ (int)(WINDOW_WIDTH / 3.2),   (int)(WINDOW_HEIGHT * 0.61) };
    p.points.left[9]  = (SDL_Point){ (int)(WINDOW_WIDTH / 2.526), (int)(WINDOW_HEIGHT * 0.61) };
    p.points.left[10] = (SDL_Point){ (int)(WINDOW_WIDTH / 2.526), (int)(WINDOW_HEIGHT * 0.90) };
    p.points.left[11] = (SDL_Point){ (int)(WINDOW_WIDTH / 13.714), (int)(WINDOW_HEIGHT * 0.90) };
    p.points.left[12] = (SDL_Point){ (int)(WINDOW_WIDTH / 13.714), (int)(WINDOW_HEIGHT * 0.65) };
    p.points.left[13] = (SDL_Point){ (int)(WINDOW_WIDTH / 4.364), (int)(WINDOW_HEIGHT * 0.65) };
    p.points.left[14] = (SDL_Point){ (int)(WINDOW_WIDTH / 4.364), WINDOW_HEIGHT };
    p.points.right[0]  = (SDL_Point){ (int)(WINDOW_WIDTH / 1.352), 0 };
    p.points.right[1]  = (SDL_Point){ (int)(WINDOW_WIDTH / 1.352), (int)(WINDOW_HEIGHT * 0.09) };
    p.points.right[2]  = (SDL_Point){ (int)(WINDOW_WIDTH / 1.103), (int)(WINDOW_HEIGHT * 0.09) };
    p.points.right[3]  = (SDL_Point){ (int)(WINDOW_WIDTH / 1.103), (int)(WINDOW_HEIGHT * 0.20) };
    p.points.right[4]  = (SDL_Point){ (int)(WINDOW_WIDTH / 1.170), (int)(WINDOW_HEIGHT * 0.20) };
    p.points.right[5]  = (SDL_Point){ (int)(WINDOW_WIDTH / 1.745), (int)(WINDOW_HEIGHT * 0.20) };
    p.points.right[6]  = (SDL_Point){ (int)(WINDOW_WIDTH / 1.745), (int)(WINDOW_HEIGHT * 0.35) };
    p.points.right[7]  = (SDL_Point){ (int)(WINDOW_WIDTH / 1.455), (int)(WINDOW_HEIGHT * 0.35) };
    p.points.right[8]  = (SDL_Point){ (int)(WINDOW_WIDTH / 1.455), (int)(WINDOW_HEIGHT * 0.61) };
    p.points.right[9]  = (SDL_Point){ (int)(WINDOW_WIDTH / 1.655), (int)(WINDOW_HEIGHT * 0.61) };
    p.points.right[10] = (SDL_Point){ (int)(WINDOW_WIDTH / 1.655), (int)(WINDOW_HEIGHT * 0.90) };
    p.points.right[11] = (SDL_Point){ (int)(WINDOW_WIDTH / 1.078), (int)(WINDOW_HEIGHT * 0.90) };
    p.points.right[12] = (SDL_Point){ (int)(WINDOW_WIDTH / 1.078), (int)(WINDOW_HEIGHT * 0.65) };
    p.points.right[13] = (SDL_Point){ (int)(WINDOW_WIDTH / 1.297), (int)(WINDOW_HEIGHT * 0.65) };
    p.points.right[14] = (SDL_Point){ (int)(WINDOW_WIDTH / 1.297), WINDOW_HEIGHT };
    return p;
}
