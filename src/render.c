#include "engine.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include "defs.h"

void renderMap(SDL_Renderer *renderer, SDL_Texture *mapTexture, SDL_Rect mapRect) {
    SDL_RenderCopy(renderer, mapTexture, NULL, &mapRect);
}

void renderEnemies(SDL_Renderer *renderer, Enemy enemies[], int numEnemiesActive, SDL_Rect baseEnemyRect) {
    for (int i = 0; i < numEnemiesActive; i++) {
        SDL_Rect eRect;
        eRect.w = baseEnemyRect.w;
        eRect.h = baseEnemyRect.h;
        eRect.x = (int)(enemies[i].x - eRect.w / 2);
        eRect.y = (int)(enemies[i].y - eRect.h / 2);
        SDL_Point pivot = { eRect.w / 2, eRect.h / 2 };
        SDL_RenderCopyEx(renderer, enemies[i].texture, NULL, &eRect, enemies[i].angle, &pivot, SDL_FLIP_NONE);
    }
}

void renderProjectiles(SDL_Renderer *renderer, Projectile projectiles[], int numProjectiles) {
    for (int i = 0; i < numProjectiles; i++) {
        SDL_Rect dRect;
        SDL_QueryTexture(projectiles[i].texture, NULL, NULL, &dRect.w, &dRect.h);
        dRect.x = (int)(projectiles[i].x - dRect.w / 2);
        dRect.y = (int)(projectiles[i].y - dRect.h / 2);
        SDL_Point dPivot = { dRect.w / 2, dRect.h / 2 };
        SDL_RenderCopyEx(renderer, projectiles[i].texture, NULL, &dRect, projectiles[i].angle, &dPivot, SDL_FLIP_NONE);
    }
}

void renderBirds(SDL_Renderer *renderer, Bird placedBirds[], int numPlacedBirds, SDL_Texture *birdTexture, float birdRotations[]) {
    for (int i = 0; i < numPlacedBirds; i++) {
        SDL_Rect br;
        SDL_QueryTexture(birdTexture, NULL, NULL, &br.w, &br.h);
        br.w /= 2;
        br.h /= 2;
        br.x = (int)(placedBirds[i].x - br.w / 2);
        br.y = (int)(placedBirds[i].y - br.h / 2);
        SDL_Point pivotBird = { br.w / 2, br.h / 2 };
        SDL_RenderCopyEx(renderer, birdTexture, NULL, &br, birdRotations[i], &pivotBird, SDL_FLIP_NONE);
    }
}

void renderUI(SDL_Renderer *renderer, TTF_Font *font, int money, int leftPlayerHP, int rightPlayerHP, int windowWidth) {
    char moneyText[64];
    sprintf(moneyText, "Money: $%d", money);
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, moneyText, white);
    if (textSurface) {
        SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_FreeSurface(textSurface);
        if (textTexture) {
            SDL_Rect textRect;
            TTF_SizeText(font, moneyText, &textRect.w, &textRect.h);
            textRect.x = windowWidth / 2 - textRect.w / 2;
            textRect.y = 50;
            SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
            SDL_DestroyTexture(textTexture);
        }
    }
    
    // Rendera HP-barer med max hp = 10
    SDL_Rect leftBarOutline = {50, 10, 200, 20};
    float leftFillWidth = 200 * (leftPlayerHP / 10.0f);
    SDL_Rect leftBarFill = {50, 10, (int)leftFillWidth, 20};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &leftBarOutline);
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &leftBarFill);
    
    SDL_Rect rightBarOutline = {windowWidth - 250, 10, 200, 20};
    float rightFillWidth = 200 * (rightPlayerHP / 10.0f);
    SDL_Rect rightBarFill = {windowWidth - 250, 10, (int)rightFillWidth, 20};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &rightBarOutline);
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &rightBarFill);
    
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}
