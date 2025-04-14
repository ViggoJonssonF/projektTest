#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <SDL.h>
#include <SDL_timer.h>
#include <SDL_ttf.h>
#include "defs.h"
#include "engine.h"
//hej
int main(int argc, char **argv) {
    // Definiera banpunkter för fiender
    static SDL_Point pathPointsLeft[] = {
        { (int)(WINDOW_WIDTH / 3.84), 0 },
        { (int)(WINDOW_WIDTH / 3.84), (int)(WINDOW_HEIGHT * 0.09) },
        { (int)(WINDOW_WIDTH / 10.67), (int)(WINDOW_HEIGHT * 0.09) },
        { (int)(WINDOW_WIDTH / 10.67), (int)(WINDOW_HEIGHT * 0.20) },
        { (int)(WINDOW_WIDTH / 6.857), (int)(WINDOW_HEIGHT * 0.20) },
        { (int)(WINDOW_WIDTH / 2.341), (int)(WINDOW_HEIGHT * 0.20) },
        { (int)(WINDOW_WIDTH / 2.341), (int)(WINDOW_HEIGHT * 0.35) },
        { (int)(WINDOW_WIDTH / 3.2),   (int)(WINDOW_HEIGHT * 0.35) },
        { (int)(WINDOW_WIDTH / 3.2),   (int)(WINDOW_HEIGHT * 0.61) },
        { (int)(WINDOW_WIDTH / 2.526), (int)(WINDOW_HEIGHT * 0.61) },
        { (int)(WINDOW_WIDTH / 2.526), (int)(WINDOW_HEIGHT * 0.90) },
        { (int)(WINDOW_WIDTH / 13.714),(int)(WINDOW_HEIGHT * 0.90) },
        { (int)(WINDOW_WIDTH / 13.714),(int)(WINDOW_HEIGHT * 0.65) },
        { (int)(WINDOW_WIDTH / 4.364), (int)(WINDOW_HEIGHT * 0.65) },
        { (int)(WINDOW_WIDTH / 4.364), WINDOW_HEIGHT }
    };
    
    static SDL_Point pathPointsRight[] = {
        { (int)(WINDOW_WIDTH / 1.352), 0 },
        { (int)(WINDOW_WIDTH / 1.352), (int)(WINDOW_HEIGHT * 0.09) },
        { (int)(WINDOW_WIDTH / 1.103), (int)(WINDOW_HEIGHT * 0.09) },
        { (int)(WINDOW_WIDTH / 1.103), (int)(WINDOW_HEIGHT * 0.20) },
        { (int)(WINDOW_WIDTH / 1.170), (int)(WINDOW_HEIGHT * 0.20) },
        { (int)(WINDOW_WIDTH / 1.745), (int)(WINDOW_HEIGHT * 0.20) },
        { (int)(WINDOW_WIDTH / 1.745), (int)(WINDOW_HEIGHT * 0.35) },
        { (int)(WINDOW_WIDTH / 1.455), (int)(WINDOW_HEIGHT * 0.35) },
        { (int)(WINDOW_WIDTH / 1.455), (int)(WINDOW_HEIGHT * 0.61) },
        { (int)(WINDOW_WIDTH / 1.655), (int)(WINDOW_HEIGHT * 0.61) },
        { (int)(WINDOW_WIDTH / 1.655), (int)(WINDOW_HEIGHT * 0.90) },
        { (int)(WINDOW_WIDTH / 1.078), (int)(WINDOW_HEIGHT * 0.90) },
        { (int)(WINDOW_WIDTH / 1.078), (int)(WINDOW_HEIGHT * 0.65) },
        { (int)(WINDOW_WIDTH / 1.297), (int)(WINDOW_HEIGHT * 0.65) },
        { (int)(WINDOW_WIDTH / 1.297), WINDOW_HEIGHT }
    };
    
    int numPoints = sizeof(pathPointsLeft) / sizeof(pathPointsLeft[0]);
    
    SDL_Window  *window = NULL;
    SDL_Renderer *renderer = NULL;
    if (!initSDL(&window, &renderer, WINDOW_WIDTH, WINDOW_HEIGHT))
        return 1;
    
    if (!initAudio()) {
        cleanup(window, renderer, NULL);
        return 1;
    }
    
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
    
    Mix_Music *bgm = Mix_LoadMUS("resources/gamesound.mp3");
    
    SDL_Texture *mapTexture = loadImage(renderer, "resources/map.png");
    if (!mapTexture) {
        TTF_CloseFont(font);
        TTF_Quit();
        cleanup(window, renderer, NULL);
        return 1;
    }
    
    SDL_Texture *dartTexture = loadImage(renderer, "resources/dart.png");
    if (!dartTexture) {
        SDL_DestroyTexture(mapTexture);
        TTF_CloseFont(font);
        TTF_Quit();
        cleanup(window, renderer, NULL);
        return 1;
    }

    SDL_Texture *bulletTexture = loadImage(renderer, "resources/bullet.png");
    if (!bulletTexture) {
        SDL_DestroyTexture(mapTexture);
        TTF_CloseFont(font);
        TTF_Quit();
        cleanup(window, renderer, NULL);
        return 1;
    }
    
    // Ladda fiende-texturer
    SDL_Texture *enemyTextures[3];
    enemyTextures[0] = loadImage(renderer, "resources/redbloon.png");
    enemyTextures[1] = loadImage(renderer, "resources/bluebloon.png");
    enemyTextures[2] = loadImage(renderer, "resources/yellowbloon.png");
    for (int i = 0; i < 3; i++) {
        if (!enemyTextures[i]) {
            for (int j = 0; j < i; j++) {
                SDL_DestroyTexture(enemyTextures[j]);
            }
            SDL_DestroyTexture(mapTexture);
            SDL_DestroyTexture(dartTexture);
            SDL_DestroyTexture(bulletTexture);
            TTF_CloseFont(font);
            TTF_Quit();
            cleanup(window, renderer, NULL);
            return 1;
        }
    }
    
    // Ladda de tre karaktärernas texturer (tornens utseende)
    SDL_Texture *superbird1Texture = loadImage(renderer, "resources/superbird1.png");
    SDL_Texture *batbird1Texture   = loadImage(renderer, "resources/batbird1.png");
    SDL_Texture *brownbird1Texture = loadImage(renderer, "resources/brownbird1.png");
    if (!superbird1Texture || !batbird1Texture || !brownbird1Texture) {
        SDL_DestroyTexture(mapTexture);
        SDL_DestroyTexture(dartTexture);
        SDL_DestroyTexture(bulletTexture);
        TTF_CloseFont(font);
        TTF_Quit();
        cleanup(window, renderer, NULL);
        return 1;
    }
    
    // Ladda ikonbilderna för de tre karaktärerna
    SDL_Texture *superbird1icon = loadImage(renderer, "resources/superbird1icon.png");
    SDL_Texture *batbird1icon   = loadImage(renderer, "resources/batbird1icon.png");
    SDL_Texture *brownbird1icon = loadImage(renderer, "resources/brownbird1icon.png");
    if (!superbird1icon || !batbird1icon || !brownbird1icon) {
        SDL_DestroyTexture(mapTexture);
        SDL_DestroyTexture(dartTexture);
        SDL_DestroyTexture(bulletTexture);
        SDL_DestroyTexture(superbird1Texture);
        SDL_DestroyTexture(batbird1Texture);
        SDL_DestroyTexture(brownbird1Texture);
        TTF_CloseFont(font);
        TTF_Quit();
        cleanup(window, renderer, NULL);
        return 1;
    }
    
    // Skapa tre Bird-prototyper (tornsalternativ) med attack-animationer
    Bird superbird1;
    superbird1.damage = 1;
    superbird1.range = WINDOW_WIDTH * 0.1f;
    superbird1.attackSpeed = 6.0f;
    superbird1.cost = 200;
    superbird1.projectileTexture = bulletTexture;
    superbird1.texture = superbird1Texture;
    superbird1.baseTexture = superbird1Texture;
    superbird1.attackTexture = loadImage(renderer, "resources/superbird1attack.png");
    superbird1.attackAnimTimer = 0.0f;
    
    Bird batbird1;
    batbird1.damage = 1;
    batbird1.range = WINDOW_WIDTH * 0.1f;
    batbird1.attackSpeed = 4.0f;
    batbird1.cost = 300;
    batbird1.projectileTexture = dartTexture;
    batbird1.texture = batbird1Texture;
    batbird1.baseTexture = batbird1Texture;
    batbird1.attackTexture = loadImage(renderer, "resources/batbird1attack.png");
    batbird1.attackAnimTimer = 0.0f;
    
    Bird brownbird1;
    brownbird1.damage = 3;
    brownbird1.range = WINDOW_WIDTH * 0.3f;
    brownbird1.attackSpeed = 1.2f;
    brownbird1.cost = 400;
    brownbird1.projectileTexture = dartTexture;
    brownbird1.texture = brownbird1Texture;
    brownbird1.baseTexture = brownbird1Texture;
    brownbird1.attackTexture = loadImage(renderer, "resources/brownbird1attack.png");
    brownbird1.attackAnimTimer = 0.0f;
    
    // Sätt ihop tornsalternativen i en array
    Bird towerOptions[3];
    towerOptions[0] = superbird1;
    towerOptions[1] = batbird1;
    towerOptions[2] = brownbird1;
    
    // Placera ikonbilderna centrerat horisontellt med jämnt vertikalt mellanrum (i mitten av skärmen)
    SDL_Rect iconRects[3];
    SDL_Texture *towerIcons[3] = { superbird1icon, batbird1icon, brownbird1icon };
    int spacing = 20;
    for (int i = 0; i < 3; i++) {
        SDL_QueryTexture(towerIcons[i], NULL, NULL, &iconRects[i].w, &iconRects[i].h);
        // Gör ikonerna 3 gånger mindre
        iconRects[i].w /= 3;
        iconRects[i].h /= 3;
    }
    int totalIconsHeight = iconRects[0].h + iconRects[1].h + iconRects[2].h + spacing * 2;
    int startY = WINDOW_HEIGHT / 2 - totalIconsHeight / 2;
    for (int i = 0; i < 3; i++) {
        iconRects[i].x = WINDOW_WIDTH / 2 - iconRects[i].w / 2;
        iconRects[i].y = startY;
        startY += iconRects[i].h + spacing;
    }
    
    int money = 500;
    float moneyTimer = 0.0f;
    SDL_Rect mapRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    
    Enemy enemies[MAX_ENEMIES];
    int numEnemiesActive = 0;
    Projectile projectiles[MAX_PROJECTILES];
    int numProjectiles = 0;
    Bird placedBirds[MAX_PLACED_BIRDS];
    int numPlacedBirds = 0;
    bool placingBird = false;
    int selectedOption = -1;
    float birdRotations[MAX_PLACED_BIRDS] = {0};
    int leftPlayerHP = 10;
    int rightPlayerHP = 10;
    
    SDL_Rect baseEnemyRect;
    SDL_QueryTexture(enemyTextures[0], NULL, NULL, &baseEnemyRect.w, &baseEnemyRect.h);
    baseEnemyRect.w /= 5.2;
    baseEnemyRect.h /= 5.2;
    
    Uint32 lastTime = SDL_GetTicks();
    float spawnTimer = 0.0f;
    int enemySpawnCounter = 0;
    bool quit = false;
    
    while (!quit) {
        handleInputMulti(&quit, &placingBird, iconRects, 3, &selectedOption, &money, &numPlacedBirds, towerOptions, placedBirds);
        
        Uint32 currentTime = SDL_GetTicks();
        float dt = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        
        moneyTimer += dt;
        if (moneyTimer >= 10.0f) {
            moneyTimer -= 10.0f;
            money += 150;
        }
        
        spawnTimer += dt;
        if (spawnTimer >= 5.0f) {
            spawnTimer -= 5.0f;
            if (numEnemiesActive <= MAX_ENEMIES - 2) {
                int type = enemySpawnCounter % 3;
                enemySpawnCounter++;
                
                // Vänster fiende
                Enemy leftEnemy;
                leftEnemy.side = 0;
                leftEnemy.speed = 300.0f;
                leftEnemy.currentSegment = 0;
                leftEnemy.segmentProgress = 0.0f;
                leftEnemy.active = true;
                leftEnemy.x = (float)pathPointsLeft[0].x;
                leftEnemy.y = (float)pathPointsLeft[0].y;
                leftEnemy.type = type;
                if (type == 0) {
                    leftEnemy.hp = 1;
                    leftEnemy.texture = enemyTextures[0];
                } else if (type == 1) {
                    leftEnemy.hp = 3;
                    leftEnemy.texture = enemyTextures[1];
                } else {
                    leftEnemy.hp = 5;
                    leftEnemy.texture = enemyTextures[2];
                }
                enemies[numEnemiesActive++] = leftEnemy;
                
                // Höger fiende
                Enemy rightEnemy;
                rightEnemy.side = 1;
                rightEnemy.speed = 300.0f;
                rightEnemy.currentSegment = 0;
                rightEnemy.segmentProgress = 0.0f;
                rightEnemy.active = true;
                rightEnemy.x = (float)pathPointsRight[0].x;
                rightEnemy.y = (float)pathPointsRight[0].y;
                rightEnemy.type = type;
                if (type == 0) {
                    rightEnemy.hp = 1;
                    rightEnemy.texture = enemyTextures[0];
                } else if (type == 1) {
                    rightEnemy.hp = 3;
                    rightEnemy.texture = enemyTextures[1];
                } else {
                    rightEnemy.hp = 5;
                    rightEnemy.texture = enemyTextures[2];
                }
                enemies[numEnemiesActive++] = rightEnemy;
            }
        }
        
        updateEnemies(enemies, &numEnemiesActive, dt, pathPointsLeft, pathPointsRight, numPoints, enemyTextures, &leftPlayerHP, &rightPlayerHP);
        updateProjectiles(projectiles, &numProjectiles, dt);
        updateBirds(placedBirds, numPlacedBirds, enemies, numEnemiesActive, projectiles, &numProjectiles, dt, enemyTextures);
        calculateBirdRotations(placedBirds, numPlacedBirds, enemies, numEnemiesActive, birdRotations);
        
        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
        SDL_RenderClear(renderer);
        
        renderMap(renderer, mapTexture, mapRect);
        renderEnemies(renderer, enemies, numEnemiesActive, baseEnemyRect);
        renderProjectiles(renderer, projectiles, numProjectiles);
        renderBirds(renderer, placedBirds, numPlacedBirds, birdRotations);
        renderUI(renderer, font, money, leftPlayerHP, rightPlayerHP, WINDOW_WIDTH);
        
        // Rendera tornikonerna
        for (int i = 0; i < 3; i++) {
            SDL_RenderCopy(renderer, towerIcons[i], NULL, &iconRects[i]);
        }
        
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
        
        bool gameOver = false;
        char gameOverText[64];
        if (leftPlayerHP <= 0) {
            gameOver = true;
            sprintf(gameOverText, "Player 1 loses!");
        } else if (rightPlayerHP <= 0) {
            gameOver = true;
            sprintf(gameOverText, "Player 2 loses!");
        }
        if (gameOver) {
            SDL_Color red = {255, 0, 0, 255};
            SDL_Surface *gameOverSurface = TTF_RenderText_Solid(font, gameOverText, red);
            if (gameOverSurface) {
                SDL_Texture *gameOverTexture = SDL_CreateTextureFromSurface(renderer, gameOverSurface);
                SDL_FreeSurface(gameOverSurface);
                if (gameOverTexture) {
                    int tw, th;
                    TTF_SizeText(font, gameOverText, &tw, &th);
                    SDL_Rect gameOverRect = { WINDOW_WIDTH / 2 - tw / 2, WINDOW_HEIGHT / 2 - th / 2 - 30, tw, th };
                    SDL_Rect bgRect = { gameOverRect.x - 10, gameOverRect.y - 10, gameOverRect.w + 20, gameOverRect.h + 20 };
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    SDL_RenderFillRect(renderer, &bgRect);
                    SDL_RenderCopy(renderer, gameOverTexture, NULL, &gameOverRect);
                    SDL_DestroyTexture(gameOverTexture);
                }
            }
            SDL_RenderPresent(renderer);
            SDL_Delay(3000);
            break;
        }
    }
    
    SDL_DestroyTexture(mapTexture);
    SDL_DestroyTexture(dartTexture);
    SDL_DestroyTexture(bulletTexture);
    SDL_DestroyTexture(superbird1Texture);
    SDL_DestroyTexture(batbird1Texture);
    SDL_DestroyTexture(brownbird1Texture);
    SDL_DestroyTexture(superbird1icon);
    SDL_DestroyTexture(batbird1icon);
    SDL_DestroyTexture(brownbird1icon);
    for (int i = 0; i < 3; i++) {
        SDL_DestroyTexture(enemyTextures[i]);
    }
    TTF_CloseFont(font);
    TTF_Quit();
    cleanupAudio();
    Mix_FreeMusic(bgm);
    bgm = NULL;
    cleanup(window, renderer, NULL);
    
    return 0;
}
