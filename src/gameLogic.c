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

void updateEnemies(Enemy enemies[], int *numEnemiesActive, float dt, 
    SDL_Point pathLeft[], SDL_Point pathRight[], int numPoints, 
    SDL_Texture *enemyTextures[], int *leftPlayerHP, int *rightPlayerHP) {
    for (int i = 0; i < *numEnemiesActive; i++) {
        if (!enemies[i].active)
            continue;
        SDL_Point *path = (enemies[i].side == 0) ? pathLeft : pathRight;
        int pathCount = numPoints;
        if (pathCount > 1 && enemies[i].currentSegment < pathCount - 1) {
            float segDist = distanceBetween(
                path[enemies[i].currentSegment].x, path[enemies[i].currentSegment].y,
                path[enemies[i].currentSegment + 1].x, path[enemies[i].currentSegment + 1].y);
            float moveAmount = (enemies[i].speed * dt) / segDist;
            enemies[i].segmentProgress += moveAmount;
            while (enemies[i].segmentProgress >= 1.0f && enemies[i].currentSegment < pathCount - 2) {
                enemies[i].segmentProgress -= 1.0f;
                enemies[i].currentSegment++;
                segDist = distanceBetween(
                    path[enemies[i].currentSegment].x, path[enemies[i].currentSegment].y,
                    path[enemies[i].currentSegment + 1].x, path[enemies[i].currentSegment + 1].y);
            }
            float x1 = (float)path[enemies[i].currentSegment].x;
            float y1 = (float)path[enemies[i].currentSegment].y;
            float x2 = (float)path[enemies[i].currentSegment + 1].x;
            float y2 = (float)path[enemies[i].currentSegment + 1].y;
            enemies[i].x = lerp(x1, x2, enemies[i].segmentProgress);
            enemies[i].y = lerp(y1, y2, enemies[i].segmentProgress);
            // Beräkna basvinkeln så att "botten" av bilden pekar framåt (nedåt)
            float baseAngle = atan2f(y2 - y1, x2 - x1) * 180.0f / M_PI - 90.0f;
            enemies[i].angle = baseAngle; 
            
            // Om vi är i sista segmentet med full progress räknas fienden som nått slutet
            if ((enemies[i].currentSegment == pathCount - 2 && enemies[i].segmentProgress >= 1.0f) ||
                (enemies[i].currentSegment >= pathCount - 1)) {
                // Fiender på vänster sida drar HP från vänster spelare,
                // fiender på högersidan drar HP från höger spelare.
                if (enemies[i].side == 0) {
                    *leftPlayerHP -= enemies[i].hp;
                } else {
                    *rightPlayerHP -= enemies[i].hp;
                }
                enemies[i].active = false;
            }
        }
    }
    // Ta bort inaktiva fiender
    for (int i = 0; i < *numEnemiesActive;) {
        if (!enemies[i].active) {
            for (int j = i; j < *numEnemiesActive - 1; j++) {
                enemies[j] = enemies[j + 1];
            }
            (*numEnemiesActive)--;
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

void updateBirds(Bird placedBirds[], int numPlacedBirds, Enemy enemies[], int numEnemiesActive, 
    Projectile projectiles[], int *numProjectiles, float dt, 
    SDL_Texture *dartTexture, SDL_Texture *enemyTextures[]) {
    
    for (int i = 0; i < numPlacedBirds; i++) {
        placedBirds[i].attackTimer += dt;
        Enemy *target = NULL;
        float bestProgress = -1.0f;
        
        bool birdIsLeft = (placedBirds[i].x < 960);
        bool birdIsRight = (placedBirds[i].x > 960);
        
        for (int j = 0; j < numEnemiesActive; j++) {
            if (birdIsLeft && (enemies[j].x > 1050))
                continue;
            if (birdIsRight && (enemies[j].x < 870))
                continue;
            
            float dx = enemies[j].x - placedBirds[i].x;
            float dy = enemies[j].y - placedBirds[i].y;
            float dist = sqrtf(dx * dx + dy * dy);
            
            if (dist <= placedBirds[i].range) {
                float progress = enemies[j].currentSegment + enemies[j].segmentProgress;
                if (progress > bestProgress) {
                    bestProgress = progress;
                    target = &enemies[j];
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
                    target->texture = enemyTextures[1];
                } else if (target->type == 1 && target->hp <= 1) {
                    target->type = 0;
                    target->hp = 1;
                    target->texture = enemyTextures[0];
                }
            }
        }
    }
}

void calculateBirdRotations(Bird placedBirds[], int numPlacedBirds, Enemy enemies[], int numEnemiesActive, float birdRotations[]) {
    for (int i = 0; i < numPlacedBirds; i++) {
        Enemy *target = NULL;
        float bestProgress = -1.0f;
        
        bool birdIsLeft = (placedBirds[i].x <= 870);
        bool birdIsRight = (placedBirds[i].x >= 1050);
        
        if (!birdIsLeft && !birdIsRight) {
            birdRotations[i] = 0.0f;
            continue;
        }
        
        for (int j = 0; j < numEnemiesActive; j++) {
            if (birdIsLeft && (enemies[j].x > 870))
                continue;
            if (birdIsRight && (enemies[j].x < 1050))
                continue;
            
            float dx = enemies[j].x - placedBirds[i].x;
            float dy = enemies[j].y - placedBirds[i].y;
            float dist = sqrtf(dx * dx + dy * dy);
            if (dist <= placedBirds[i].range) {
                float progress = enemies[j].currentSegment + enemies[j].segmentProgress;
                if (progress > bestProgress) {
                    bestProgress = progress;
                    target = &enemies[j];
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
