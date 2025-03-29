#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <SDL.h>
#include <SDL_timer.h>
#include <SDL_ttf.h>
#include "defs.h"
#include "engine.h"

int main(int argc, char **argv) {
    // Definiera banpunkter för ballonger
    static SDL_Point pathPointsLeft[] = {
        { 500,   0 },
        { 500,  60 },
        { 180,  60 },
        { 180, 170 },
        { 280, 170 },
        { 820, 170 },
        { 820, 290 },
        { 600, 290 },
        { 600, 570 },
        { 760, 570 },
        { 760, 870 },
        { 140, 870 },
        { 140, 620 },
        { 440, 620 },
        { 440, 900 }
    };
    
    static SDL_Point pathPointsRight[] = {
        { 1420, 0 },
        { 1420, 60 },
        { 1740, 60 },
        { 1740, 170 },
        { 1640, 170 },
        { 1100, 170 },
        { 1100, 290 },
        { 1320, 290 },
        { 1320, 570 },
        { 1160, 570 },
        { 1160, 870 },
        { 1780, 870 },
        { 1780, 620 },
        { 1480, 620 },
        { 1480, 900 }
    };
    
    static int numPoints = sizeof(pathPointsLeft) / sizeof(pathPointsLeft[0]);
    SDL_Window  *window = NULL;
    SDL_Renderer *renderer = NULL;
    
    // Initiera SDL, skapa fönster och renderer
    if (!initSDL(&window, &renderer, WINDOW_WIDTH, WINDOW_HEIGHT)) {
        return 1;
    }
    
    // Initiera ljud
    if (!initAudio()) {
        cleanup(window, renderer, NULL);
        return 1;
    }
    
    // Initiera TTF
    if (TTF_Init() == -1) {
        printf("TTF_Init Error: %s\n", TTF_GetError());
        cleanup(window, renderer, NULL);
        return 1;
    }
    
    TTF_Font *font = TTF_OpenFont("resources/font.ttf", 24);
    if (!font) {
        printf("TTF_OpenFont Error: %s\n", TTF_GetError());
        TTF_Quit();
        cleanup(window, renderer, NULL);
        return 1;
    }
    
    // Ladda resurser (texturer)
    SDL_Texture *mapTexture = loadImage(renderer, "resources/map.png");
    if (!mapTexture) {
        TTF_CloseFont(font);
        TTF_Quit();
        cleanup(window, renderer, NULL);
        return 1;
    }
    
    SDL_Texture *birdTexture = loadImage(renderer, "resources/superbird.png");
    if (!birdTexture) {
        SDL_DestroyTexture(mapTexture);
        TTF_CloseFont(font);
        TTF_Quit();
        cleanup(window, renderer, NULL);
        return 1;
    }
    
    SDL_Texture *birdIcon = loadImage(renderer, "resources/superbirdicon.png");
    if (!birdIcon) {
        SDL_DestroyTexture(mapTexture);
        SDL_DestroyTexture(birdTexture);
        TTF_CloseFont(font);
        TTF_Quit();
        cleanup(window, renderer, NULL);
        return 1;
    }
    
    // Hämta birdIcon storlek och centrera den i fönstret
    SDL_Rect iconRect;
    SDL_QueryTexture(birdIcon, NULL, NULL, &iconRect.w, &iconRect.h);
    iconRect.x = WINDOW_WIDTH / 2 - iconRect.w / 2;
    iconRect.y = WINDOW_HEIGHT / 2 - iconRect.h / 2;
    
    // Ladda ballong-texturer (tre typer)
    SDL_Texture *balloonTextures[3];
    balloonTextures[0] = loadImage(renderer, "resources/redbloon.png");
    balloonTextures[1] = loadImage(renderer, "resources/bluebloon.png");
    balloonTextures[2] = loadImage(renderer, "resources/yellowbloon.png");
    for (int i = 0; i < 3; i++) {
        if (!balloonTextures[i]) {
            for (int j = 0; j < i; j++) {
                SDL_DestroyTexture(balloonTextures[j]);
            }
            SDL_DestroyTexture(mapTexture);
            SDL_DestroyTexture(birdTexture);
            SDL_DestroyTexture(birdIcon);
            TTF_CloseFont(font);
            TTF_Quit();
            cleanup(window, renderer, NULL);
            return 1;
        }
    }
    
    SDL_Texture *dartTexture = loadImage(renderer, "resources/dart.png");
    if (!dartTexture) {
        SDL_DestroyTexture(mapTexture);
        SDL_DestroyTexture(birdTexture);
        SDL_DestroyTexture(birdIcon);
        for (int i = 0; i < 3; i++) {
            SDL_DestroyTexture(balloonTextures[i]);
        }
        TTF_CloseFont(font);
        TTF_Quit();
        cleanup(window, renderer, NULL);
        return 1;
    }
    
    // Kartans rektangel (hela fönstret)
    SDL_Rect mapRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    
    // Initiera global superbird-mall (template)
    Bird superbird;
    superbird.damage = 1;
    superbird.range = 400.0f;
    superbird.attackSpeed = 0.7f;
    superbird.x = WINDOW_WIDTH / 2.0f;
    superbird.y = WINDOW_HEIGHT / 2.0f;
    superbird.attackTimer = 0.0f;
    superbird.cost = 200;
    superbird.projectileTexture = dartTexture;
    
    // Spel-ekonomi
    int money = 500;
    float moneyTimer = 0.0f;
    
    // Hämta basstorlek för ballonger (utgå från redbloon-textur)
    SDL_Rect baseBalloonRect;
    SDL_QueryTexture(balloonTextures[0], NULL, NULL, &baseBalloonRect.w, &baseBalloonRect.h);
    baseBalloonRect.w /= 5.2;
    baseBalloonRect.h /= 5.2;
    
    // Initiera arrays för ballonger, projektiler och placerade fåglar
    Balloon balloons[MAX_BALLOONS];
    int numBalloonsActive = 0;
    Projectile projectiles[MAX_PROJECTILES];
    int numProjectiles = 0;
    Bird placedBirds[MAX_PLACED_BIRDS];
    int numPlacedBirds = 0;
    bool placingBird = false;
    
    // Array för fågelrotationer
    float birdRotations[MAX_PLACED_BIRDS] = {0};
    
    // Tidsvariabler
    Uint32 lastTime = SDL_GetTicks();
    float spawnTimer = 0.0f;
    int balloonSpawnCounter = 0;
    
    bool quit = false;
    while (!quit) {
        // Hantera inmatning via vår input-modul
        handleInput(&quit, &placingBird, iconRect, &money, &numPlacedBirds, superbird, placedBirds);
        
        // Beräkna delta-tid
        Uint32 currentTime = SDL_GetTicks();
        float dt = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        
        // Uppdatera pengar: var 10:e sekund, lägg till 150 dollar
        moneyTimer += dt;
        if (moneyTimer >= 10.0f) {
            moneyTimer -= 10.0f;
            money += 150;
        }
        
        // Spawna ballonger: två stycken (en per bana) var 5:e sekund
        spawnTimer += dt;
        if (spawnTimer >= 5.0f) {
            spawnTimer -= 5.0f;
            if (numBalloonsActive <= MAX_BALLOONS - 2) {
                int type = balloonSpawnCounter % 3; // 0 = röd, 1 = blå, 2 = gul
                balloonSpawnCounter++;
                
                // Vänster ballong
                Balloon leftBalloon;
                leftBalloon.side = 0;
                leftBalloon.speed = 300.0f;
                leftBalloon.currentSegment = 0;
                leftBalloon.segmentProgress = 0.0f;
                leftBalloon.active = true;
                leftBalloon.x = (float)pathPointsLeft[0].x;
                leftBalloon.y = (float)pathPointsLeft[0].y;
                leftBalloon.type = type;
                if (type == 0) {
                    leftBalloon.hp = 1;
                    leftBalloon.texture = balloonTextures[0];
                } else if (type == 1) {
                    leftBalloon.hp = 3;
                    leftBalloon.texture = balloonTextures[1];
                } else {
                    leftBalloon.hp = 5;
                    leftBalloon.texture = balloonTextures[2];
                }
                balloons[numBalloonsActive++] = leftBalloon;
                
                // Höger ballong
                Balloon rightBalloon;
                rightBalloon.side = 1;
                rightBalloon.speed = 300.0f;
                rightBalloon.currentSegment = 0;
                rightBalloon.segmentProgress = 0.0f;
                rightBalloon.active = true;
                rightBalloon.x = (float)pathPointsRight[0].x;
                rightBalloon.y = (float)pathPointsRight[0].y;
                rightBalloon.type = type;
                if (type == 0) {
                    rightBalloon.hp = 1;
                    rightBalloon.texture = balloonTextures[0];
                } else if (type == 1) {
                    rightBalloon.hp = 3;
                    rightBalloon.texture = balloonTextures[1];
                } else {
                    rightBalloon.hp = 5;
                    rightBalloon.texture = balloonTextures[2];
                }
                balloons[numBalloonsActive++] = rightBalloon;
            }
        }
        
        // Uppdatera spel-logiken via våra game_logic-funktioner
        updateBalloons(balloons, &numBalloonsActive, dt, pathPointsLeft, pathPointsRight, numPoints, balloonTextures);
        updateProjectiles(projectiles, &numProjectiles, dt);
        updateBirds(placedBirds, numPlacedBirds, balloons, numBalloonsActive, projectiles, &numProjectiles, dt, dartTexture, balloonTextures);
        calculateBirdRotations(placedBirds, numPlacedBirds, balloons, numBalloonsActive, birdRotations);
        
        // Rendera allt med hjälp av render-funktionerna
        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
        SDL_RenderClear(renderer);
        
        renderMap(renderer, mapTexture, mapRect);
        renderBalloons(renderer, balloons, numBalloonsActive, baseBalloonRect);
        renderProjectiles(renderer, projectiles, numProjectiles);
        renderBirds(renderer, placedBirds, numPlacedBirds, birdTexture, birdRotations);
        renderUI(renderer, font, money, WINDOW_WIDTH);
        
        // Rita birdIcon (för placeringsläge)
        SDL_RenderCopy(renderer, birdIcon, NULL, &iconRect);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    
    // Städning av resurser
    SDL_DestroyTexture(mapTexture);
    SDL_DestroyTexture(birdTexture);
    SDL_DestroyTexture(birdIcon);
    for (int i = 0; i < 3; i++) {
        SDL_DestroyTexture(balloonTextures[i]);
    }
    SDL_DestroyTexture(dartTexture);
    TTF_CloseFont(font);
    TTF_Quit();
    cleanupAudio();
    cleanup(window, renderer, NULL);
    
    return 0;
}
