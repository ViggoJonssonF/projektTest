#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_timer.h>
#include "engine.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1000
#define MAX_BALLOONS 100
#define MAX_BIRDS 10

// Linjär interpolationsfunktion
static float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

// Hjälpfunktion: beräknar avstånd mellan två punkter
static float distanceBetween(int x1, int y1, int x2, int y2) {
    float dx = (float)(x2 - x1);
    float dy = (float)(y2 - y1);
    return sqrtf(dx * dx + dy * dy);
}

/*
  Två banor (path-arrayer) – vänster och höger.
  Justera koordinaterna så att de passar din design.
*/

// Vänster bana (exempel)
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

// Höger bana (exempel, spegelvänd)
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
// Eftersom båda banorna har samma antal element
static int numPoints = sizeof(pathPointsLeft) / sizeof(pathPointsLeft[0]);

// --- Structar ---
// Balloon representerar en ballong som följer en bana
typedef struct {
    int hp;
    float speed;
    float x, y;
    int currentSegment;
    float segmentProgress;
    SDL_Texture *texture;
    int type;       // 0 = röd, 1 = blå, 2 = gul
    bool active;
    int side;       // 0 = vänster, 1 = höger
} Balloon;

// Bird representerar en superbird (fågel) som spelaren placerar
typedef struct {
    int damage;        
    float range;       
    float attackSpeed; 
    float x, y;
    float attackTimer; // Hanterar attackintervall
} Bird;

int main(int argc, char **argv) {
    SDL_Window  *window   = NULL;
    SDL_Renderer *renderer = NULL;
    
    if (!initSDL(&window, &renderer, WINDOW_WIDTH, WINDOW_HEIGHT)) {
        return 1;
    }
    if (!initAudio()) {
        cleanup(window, renderer, NULL);
        return 1;
    }
    
    // Ladda bakgrund (map.png)
    SDL_Texture *mapTexture = loadImage(renderer, "resources/map.png");
    if (!mapTexture) {
        cleanup(window, renderer, NULL);
        return 1;
    }
    
    // Ladda fågelns bild (superbird.png) – används för att rita ut de placerade fåglarna
    SDL_Texture *birdTexture = loadImage(renderer, "resources/superbird.png");
    if (!birdTexture) {
        SDL_DestroyTexture(mapTexture);
        cleanup(window, renderer, NULL);
        return 1;
    }

    // Ladda superbird-ikonen (superbirdicon.png)
    SDL_Texture *birdIcon = loadImage(renderer, "resources/superbirdicon.png");
    if (!birdIcon) {
        SDL_DestroyTexture(mapTexture);
        SDL_DestroyTexture(birdTexture);
        cleanup(window, renderer, NULL);
        return 1;
    }
    SDL_Rect iconRect;
    SDL_QueryTexture(birdIcon, NULL, NULL, &iconRect.w, &iconRect.h);
    iconRect.w /= 1;
    iconRect.h /= 1;
    iconRect.x = WINDOW_WIDTH / 2 - iconRect.w / 2;
    iconRect.y = WINDOW_HEIGHT / 2 - iconRect.h / 2;
    
    // Ladda ballong-texturer (röd, blå, gul)
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
            cleanup(window, renderer, NULL);
            return 1;
        }
    }
    
    SDL_Rect mapRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    
    // Sätt upp en initial birdRect (för de placerade fåglarna) – vi använder birdTexture storlek
    SDL_Rect birdRect;
    SDL_QueryTexture(birdTexture, NULL, NULL, &birdRect.w, &birdRect.h);
    birdRect.w /= 2;
    birdRect.h /= 2;

    // Denna rektangel används inte för placerade fåglar (de har sina egna positioner), men vi ritar ikonen separat.
    
    // Initiera array för placerade fåglar
    #define MAX_BIRDS 10
    Bird placedBirds[MAX_BIRDS];
    int numPlacedBirds = 0;
    bool placingBird = false; // Är vi i placeringsläge för en ny fågel?
    
    // Ballong-storlek (baserad på redbloon-textur, skala ned)
    SDL_Rect baseBalloonRect;
    SDL_QueryTexture(balloonTextures[0], NULL, NULL, &baseBalloonRect.w, &baseBalloonRect.h);
    baseBalloonRect.w /= 5.2;
    baseBalloonRect.h /= 5.2;
    
    // Hantera ballonger
    Balloon balloons[MAX_BALLOONS];
    int numBalloonsActive = 0;
    
    Uint32 lastTime = SDL_GetTicks();
    float spawnTimer = 0.0f;
    int balloonSpawnCounter = 0;
    
    // Vi behåller en attack-timer per ballongmål (för tidigare logik) – den används inte längre för fåglar,
    // eftersom nu varje placerad fågel har sin egen attackTimer.
    float globalAttackTimer = 0.0f; // ej längre använd
    
    bool quit = false;
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            // Om spelaren klickar med vänster musknapp:
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int mx = event.button.x;
                int my = event.button.y;
                // Om vi inte är i placeringsläge, kontrollera om klicket sker på ikonen
                if (!placingBird) {
                    if (mx >= iconRect.x && mx <= iconRect.x + iconRect.w &&
                        my >= iconRect.y && my <= iconRect.y + iconRect.h) {
                        // Aktivera placeringsläge
                        placingBird = true;
                    }
                } else {
                    // Vi är i placeringsläge – placera en ny superbird på klickpositionen
                    if (numPlacedBirds < MAX_BIRDS) {
                        Bird newBird;
                        newBird.damage = 1;
                        newBird.range = 400.0f;
                        newBird.attackSpeed = 1.0f;
                        newBird.x = (float)mx;
                        newBird.y = (float)my;
                        newBird.attackTimer = 0.0f;
                        placedBirds[numPlacedBirds++] = newBird;
                    }
                    placingBird = false;
                }
            }
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }
        
        Uint32 currentTime = SDL_GetTicks();
        float dt = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        
        // Spawna ballonger (två samtidigt, en för varje bana) var 5:e sekund
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
        
        // Uppdatera ballonger (använder rätt bana beroende på side)
        for (int i = 0; i < numBalloonsActive; i++) {
            if (!balloons[i].active) continue;
            SDL_Point *path = (balloons[i].side == 0) ? pathPointsLeft : pathPointsRight;
            int pathCount = numPoints;
            if (pathCount > 1 && balloons[i].currentSegment < pathCount - 1) {
                float segDist = distanceBetween(
                    path[balloons[i].currentSegment].x, path[balloons[i].currentSegment].y,
                    path[balloons[i].currentSegment + 1].x, path[balloons[i].currentSegment + 1].y
                );
                float moveAmount = (balloons[i].speed * dt) / segDist;
                balloons[i].segmentProgress += moveAmount;
                while (balloons[i].segmentProgress >= 1.0f && balloons[i].currentSegment < pathCount - 2) {
                    balloons[i].segmentProgress -= 1.0f;
                    balloons[i].currentSegment++;
                    segDist = distanceBetween(
                        path[balloons[i].currentSegment].x, path[balloons[i].currentSegment].y,
                        path[balloons[i].currentSegment + 1].x, path[balloons[i].currentSegment + 1].y
                    );
                }
                float x1 = (float)path[balloons[i].currentSegment].x;
                float y1 = (float)path[balloons[i].currentSegment].y;
                float x2 = (float)path[balloons[i].currentSegment + 1].x;
                float y2 = (float)path[balloons[i].currentSegment + 1].y;
                balloons[i].x = lerp(x1, x2, balloons[i].segmentProgress);
                balloons[i].y = lerp(y1, y2, balloons[i].segmentProgress);
                if (balloons[i].currentSegment >= pathCount - 1) {
                    balloons[i].active = false;
                }
            }
        }
        
        // Ta bort inaktiva ballonger
        for (int i = 0; i < numBalloonsActive; ) {
            if (!balloons[i].active) {
                for (int j = i; j < numBalloonsActive - 1; j++) {
                    balloons[j] = balloons[j + 1];
                }
                numBalloonsActive--;
            } else {
                i++;
            }
        }
        
        // För varje placerad fågel, välj ett mål och hantera attack
        for (int i = 0; i < numPlacedBirds; i++) {
            placedBirds[i].attackTimer += dt;
            // Välj mål för denna fågel: den ballong inom dess range med högst progress
            Balloon *target = NULL;
            float bestProgress = -1.0f;
            for (int j = 0; j < numBalloonsActive; j++) {
                float dx = balloons[j].x - placedBirds[i].x;
                float dy = balloons[j].y - placedBirds[i].y;
                float dist = sqrtf(dx*dx + dy*dy);
                if (dist <= placedBirds[i].range) {
                    float progress = balloons[j].currentSegment + balloons[j].segmentProgress;
                    if (progress > bestProgress) {
                        bestProgress = progress;
                        target = &balloons[j];
                    }
                }
            }
            if (target != NULL && placedBirds[i].attackTimer >= (1.0f / placedBirds[i].attackSpeed)) {
                placedBirds[i].attackTimer = 0.0f;
                playPopSound();
                target->hp -= placedBirds[i].damage;
                if (target->hp <= 0) {
                    target->active = false;
                } else {
                    if (target->type == 2 && target->hp <= 3) {
                        target->type = 1;
                        target->hp = 3;
                        target->texture = balloonTextures[1];
                    } else if (target->type == 1 && target->hp <= 1) {
                        target->type = 0;
                        target->hp = 1;
                        target->texture = balloonTextures[0];
                    }
                }
            }
        }
        
        // Rita superbirden (placerade fåglar)
        // Vi beräknar rotation för varje fågel baserat på dess eget mål
        // För enkelhet, låt oss räkna ut en rotation för varje fågel om den har ett mål
        // Annars ritas den med rotation 0
        // Vi sparar rotationen i en array (samma ordning som i placedBirds)
        float birdRotations[MAX_BIRDS] = {0};
        for (int i = 0; i < numPlacedBirds; i++) {
            // Sök efter mål för placedBirds[i]
            Balloon *target = NULL;
            float bestProgress = -1.0f;
            for (int j = 0; j < numBalloonsActive; j++) {
                float dx = balloons[j].x - placedBirds[i].x;
                float dy = balloons[j].y - placedBirds[i].y;
                float dist = sqrtf(dx*dx + dy*dy);
                if (dist <= placedBirds[i].range) {
                    float progress = balloons[j].currentSegment + balloons[j].segmentProgress;
                    if (progress > bestProgress) {
                        bestProgress = progress;
                        target = &balloons[j];
                    }
                }
            }
            if (target != NULL) {
                float dx = target->x - placedBirds[i].x;
                float dy = target->y - placedBirds[i].y;
                birdRotations[i] = atan2(dy, dx) * 180.0f / M_PI + 90.0f;
            } else {
                birdRotations[i] = 0.0f;
            }
        }
        
        // Rendering
        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
        SDL_RenderClear(renderer);
        
        SDL_RenderCopy(renderer, mapTexture, NULL, &mapRect);
        
        // Rita ballongerna
        for (int i = 0; i < numBalloonsActive; i++) {
            SDL_Rect bRect;
            bRect.w = baseBalloonRect.w;
            bRect.h = baseBalloonRect.h;
            bRect.x = (int)(balloons[i].x - bRect.w / 2);
            bRect.y = (int)(balloons[i].y - bRect.h / 2);
            SDL_RenderCopy(renderer, balloons[i].texture, NULL, &bRect);
        }
        
        // Rita placerade fåglar
        for (int i = 0; i < numPlacedBirds; i++) {
            SDL_Rect br;
            // Använd samma dimensioner som birdTexture
            br.w = birdRect.w;
            br.h = birdRect.h;
            br.x = (int)(placedBirds[i].x - br.w / 2);
            br.y = (int)(placedBirds[i].y - br.h / 2);
            SDL_Point pivot = { br.w / 2, br.h / 2 };
            SDL_RenderCopyEx(renderer, birdTexture, NULL, &br, birdRotations[i], &pivot, SDL_FLIP_NONE);
        }
        
        // Rita superbird-ikonen (alltid i mitten)
        SDL_RenderCopy(renderer, birdIcon, NULL, &iconRect);
        
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    
    SDL_DestroyTexture(mapTexture);
    SDL_DestroyTexture(birdTexture);
    SDL_DestroyTexture(birdIcon);
    for (int i = 0; i < 3; i++) {
        SDL_DestroyTexture(balloonTextures[i]);
    }
    cleanupAudio();
    cleanup(window, renderer, NULL);
    return 0;
}
