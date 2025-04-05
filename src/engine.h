#ifndef ENGINE_H
#define ENGINE_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <stdbool.h>
#include <SDL_ttf.h>

// -------------------------
// Typdefinitioner för spelet
// -------------------------
typedef struct {
    int damage;
    float range;
    float attackSpeed;
    float x, y;
    float attackTimer;
    int cost;
    SDL_Texture *projectileTexture;
    SDL_Texture *texture;      // Aktuell textur (kan ändras vid attack)
    SDL_Texture *baseTexture;  // Normal (base) textur
    SDL_Texture *attackTexture;// Attackversionens textur
    float attackAnimTimer;     // Timer för hur länge attackbilden visas
} Bird;

typedef struct {
    int hp;
    float speed;
    float x, y;            
    int currentSegment;    
    float segmentProgress; 
    SDL_Texture *texture;  
    int type;              
    bool active;
    int side;              
    float angle;           
} Enemy;

typedef struct {
    float x, y;
    float vx, vy;
    float speed;
    SDL_Texture *texture;
    bool active;
    float angle;
    float startX;
    float startY;
} Projectile;

// Struktur för tornsalternativ
typedef struct {
    Bird prototype;         // Egenskaper för tornet
    SDL_Texture *iconTexture; // Ikon för att välja tornet
    SDL_Rect iconRect;        // Position och storlek för ikonen
} TowerOption;

// -------------------------
// Deklarationer för SDL- och ljudfunktioner
// -------------------------
bool initSDL(SDL_Window **window, SDL_Renderer **renderer, int windowWidth, int windowHeight);
SDL_Texture* loadTexture(SDL_Renderer *renderer, const char *path);
SDL_Texture* loadImage(SDL_Renderer *renderer, const char *path);
void cleanup(SDL_Window *window, SDL_Renderer *renderer, SDL_Texture *texture);
bool initAudio();
void playPopSound();
void cleanupAudio();

// -------------------------
// Deklarationer för inputhantering
// -------------------------
void handleInputMulti(bool *quit, bool *placingBird, SDL_Rect iconRects[], int numIcons,
                      int *selectedOption, int *money, int *numPlacedBirds, Bird towerOptions[], Bird placedBirds[]);

// -------------------------
// Deklarationer för uppdateringsfunktioner (game logic)
// -------------------------
void updateEnemies(Enemy enemies[], int *numEnemiesActive, float dt, 
    SDL_Point pathLeft[], SDL_Point pathRight[], int numPoints, 
    SDL_Texture *enemyTextures[], int *leftPlayerHP, int *rightPlayerHP);
void updateProjectiles(Projectile projectiles[], int *numProjectiles, float dt);
void updateBirds(Bird placedBirds[], int numPlacedBirds, Enemy enemies[], int numEnemiesActive, 
                 Projectile projectiles[], int *numProjectiles, float dt, 
                 SDL_Texture *dartTexture, SDL_Texture *enemyTextures[]);
void calculateBirdRotations(Bird placedBirds[], int numPlacedBirds, Enemy enemies[], int numEnemiesActive, float birdRotations[]);

// -------------------------
// Deklarationer för renderingsfunktioner
// -------------------------
void renderMap(SDL_Renderer *renderer, SDL_Texture *mapTexture, SDL_Rect mapRect);
void renderEnemies(SDL_Renderer *renderer, Enemy enemies[], int numEnemiesActive, SDL_Rect baseEnemyRect);
void renderProjectiles(SDL_Renderer *renderer, Projectile projectiles[], int numProjectiles);
void renderBirds(SDL_Renderer *renderer, Bird placedBirds[], int numPlacedBirds, float birdRotations[]);
void renderUI(SDL_Renderer *renderer, TTF_Font *font, int money, int leftPlayerHP, int rightPlayerHP, int windowWidth);

#endif
