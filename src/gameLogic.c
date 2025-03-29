#include "engine.h"
#include <math.h>
#include <stdio.h>
#include "defs.h"

// Hjälpfunktioner
static float distanceBetween(int x1, int y1, int x2, int y2) {
    float dx = (float)(x2 - x1);
    float dy = (float)(y2 - y1);
    return sqrtf(dx * dx + dy * dy);
}

static float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

void updateBalloons(Balloon balloons[], int *numBalloonsActive, float dt, SDL_Point pathLeft[], SDL_Point pathRight[], int numPoints, SDL_Texture *balloonTextures[]) {
    for (int i = 0; i < *numBalloonsActive; i++) {
        if (!balloons[i].active) continue;
        SDL_Point *path = (balloons[i].side == 0) ? pathLeft : pathRight;
        int pathCount = numPoints;
        if (pathCount > 1 && balloons[i].currentSegment < pathCount - 1) {
            float segDist = distanceBetween(path[balloons[i].currentSegment].x, path[balloons[i].currentSegment].y,
                                             path[balloons[i].currentSegment + 1].x, path[balloons[i].currentSegment + 1].y);
            float moveAmount = (balloons[i].speed * dt) / segDist;
            balloons[i].segmentProgress += moveAmount;
            while (balloons[i].segmentProgress >= 1.0f && balloons[i].currentSegment < pathCount - 2) {
                balloons[i].segmentProgress -= 1.0f;
                balloons[i].currentSegment++;
                segDist = distanceBetween(path[balloons[i].currentSegment].x, path[balloons[i].currentSegment].y,
                                          path[balloons[i].currentSegment + 1].x, path[balloons[i].currentSegment + 1].y);
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
    for (int i = 0; i < *numBalloonsActive;) {
        if (!balloons[i].active) {
            for (int j = i; j < *numBalloonsActive - 1; j++) {
                balloons[j] = balloons[j + 1];
            }
            (*numBalloonsActive)--;
        } else {
            i++;
        }
    }
}

void updateProjectiles(Projectile projectiles[], int *numProjectiles, float dt) {
    for (int i = 0; i < *numProjectiles; i++) {
        if (!projectiles[i].active) continue;
        projectiles[i].x += projectiles[i].vx * PROJECTILE_SPEED * dt;
        projectiles[i].y += projectiles[i].vy * PROJECTILE_SPEED * dt;
        
        // Nu enbart kontroll om dart går utanför skärmen
        if (projectiles[i].x < 0 || projectiles[i].x > WINDOW_WIDTH ||
            projectiles[i].y < 0 || projectiles[i].y > WINDOW_HEIGHT) {
            projectiles[i].active = false;
        }
    }
    for (int i = 0; i < *numProjectiles;) {
        if (!projectiles[i].active) {
            for (int j = i; j < *numProjectiles - 1; j++) {
                projectiles[j] = projectiles[j + 1];
            }
            (*numProjectiles)--;
        } else {
            i++;
        }
    }
}

void updateBirds(Bird placedBirds[], int numPlacedBirds, Balloon balloons[], int numBalloonsActive, 
    Projectile projectiles[], int *numProjectiles, float dt, 
    SDL_Texture *dartTexture, SDL_Texture *balloonTextures[]) {
    
    for (int i = 0; i < numPlacedBirds; i++) {
        placedBirds[i].attackTimer += dt;
        Balloon *target = NULL;
        float bestProgress = -1.0f;
        
        // Bestäm vilket lag fågeln tillhör:
        // Om x < 960 betraktas fågeln som vänsterlag.
        // Om x > 960 betraktas fågeln som högerlag.
        bool birdIsLeft = (placedBirds[i].x < 960);
        bool birdIsRight = (placedBirds[i].x > 960);
        
        // Loopa över alla ballonger
        for (int j = 0; j < numBalloonsActive; j++) {
            // Om fågeln är vänsterlag: ignorera ballonger med x > 1050.
            if (birdIsLeft && (balloons[j].x > 1050))
                continue;
            // Om fågeln är högerlag: ignorera ballonger med x < 870.
            if (birdIsRight && (balloons[j].x < 870))
                continue;
            
            float dx = balloons[j].x - placedBirds[i].x;
            float dy = balloons[j].y - placedBirds[i].y;
            float dist = sqrtf(dx * dx + dy * dy);
            
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
            
            if (*numProjectiles < MAX_PROJECTILES) {
                Projectile newProj;
                newProj.startX = placedBirds[i].x;
                newProj.startY = placedBirds[i].y;
                newProj.x = newProj.startX;
                newProj.y = newProj.startY;
                newProj.speed = PROJECTILE_SPEED;
                newProj.texture = dartTexture;
                newProj.active = true;
                
                float dx = target->x - placedBirds[i].x;
                float dy = target->y - placedBirds[i].y;
                float mag = sqrtf(dx * dx + dy * dy);
                if (mag != 0) {
                    newProj.vx = dx / mag;
                    newProj.vy = dy / mag;
                    newProj.angle = atan2(dy, dx) * 180.0f / M_PI + 90.0f;
                } else {
                    newProj.vx = 0;
                    newProj.vy = 0;
                    newProj.angle = 0;
                }
                projectiles[(*numProjectiles)++] = newProj;
            }
            
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
}







void calculateBirdRotations(Bird placedBirds[], int numPlacedBirds, Balloon balloons[], int numBalloonsActive, float birdRotations[]) {
    for (int i = 0; i < numPlacedBirds; i++) {
        Balloon *target = NULL;
        float bestProgress = -1.0f;
        
        // Bestäm vilket lag fågeln tillhör:
        bool birdIsLeft = (placedBirds[i].x <= 870);
        bool birdIsRight = (placedBirds[i].x >= 1050);
        
        // Om fågeln inte är på någon av sidorna, sätt rotationen till 0.
        if (!birdIsLeft && !birdIsRight) {
            birdRotations[i] = 0.0f;
            continue;
        }
        
        for (int j = 0; j < numBalloonsActive; j++) {
            // Filtrera ballonger utifrån fågelns lag.
            if (birdIsLeft && (balloons[j].x > 870))
                continue;
            if (birdIsRight && (balloons[j].x < 1050))
                continue;
            
            float dx = balloons[j].x - placedBirds[i].x;
            float dy = balloons[j].y - placedBirds[i].y;
            float dist = sqrtf(dx * dx + dy * dy);
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
}
