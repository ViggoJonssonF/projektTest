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
} Balloon;

typedef struct {
    float x, y;
    float vx, vy;
    float speed;
    SDL_Texture *texture;
    bool active;
    float angle;
    // Nya fält för att spara start och målposition
    float startX;
    float startY;
} Projectile;

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
void handleInput(bool *quit, bool *placingBird, SDL_Rect iconRect, int *money, int *numPlacedBirds, Bird superbird, Bird placedBirds[]);

// -------------------------
// Deklarationer för uppdateringsfunktioner (game logic)
// -------------------------
void updateBalloons(Balloon balloons[], int *numBalloonsActive, float dt, SDL_Point pathLeft[], SDL_Point pathRight[], int numPoints, SDL_Texture *balloonTextures[]);
void updateProjectiles(Projectile projectiles[], int *numProjectiles, float dt);
void updateBirds(Bird placedBirds[], int numPlacedBirds, Balloon balloons[], int numBalloonsActive, 
                 Projectile projectiles[], int *numProjectiles, float dt, 
                 SDL_Texture *dartTexture, SDL_Texture *balloonTextures[]);
void calculateBirdRotations(Bird placedBirds[], int numPlacedBirds, Balloon balloons[], int numBalloonsActive, float birdRotations[]);

// -------------------------
// Deklarationer för renderingsfunktioner
// -------------------------
void renderMap(SDL_Renderer *renderer, SDL_Texture *mapTexture, SDL_Rect mapRect);
void renderBalloons(SDL_Renderer *renderer, Balloon balloons[], int numBalloonsActive, SDL_Rect baseBalloonRect);
void renderProjectiles(SDL_Renderer *renderer, Projectile projectiles[], int numProjectiles);
void renderBirds(SDL_Renderer *renderer, Bird placedBirds[], int numPlacedBirds, SDL_Texture *birdTexture, float birdRotations[]);
void renderUI(SDL_Renderer *renderer, TTF_Font *font, int money, int windowWidth);
