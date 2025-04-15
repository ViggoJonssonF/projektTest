#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>
#include <SDL_net.h>


#define MAX_CLIENTS 4
#define MAX_ENEMIES 5
#define SERVER_PORT 2000
#define BUFFER_SIZE 512
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600


// Enemy struct
typedef struct {
    float x, y;
} EnemyData;


// Packet to send game state
typedef struct {
    int numEnemies;
    EnemyData enemies[MAX_ENEMIES];
} ServerData;


typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    UDPsocket socket;
    UDPpacket *packet;
    IPaddress clients[MAX_CLIENTS];
    int numClients;
    ServerData gameState;
} Game;


void addClient(IPaddress address, Game *game) {
    for (int i = 0; i < game->numClients; i++) {
        if (address.host == game->clients[i].host && address.port == game->clients[i].port) {
            return;
        }
    }
    if (game->numClients < MAX_CLIENTS) {
        game->clients[game->numClients++] = address;
        printf("[SERVER] New client added! Total clients: %d\n", game->numClients);
    }
}


void sendGameState(Game *game) {
    memcpy(game->packet->data, &(game->gameState), sizeof(ServerData));
    game->packet->len = sizeof(ServerData);


    for (int i = 0; i < game->numClients; i++) {
        game->packet->address = game->clients[i];
        SDLNet_UDP_Send(game->socket, -1, game->packet);
    }
}


void updateEnemies(Game *game) {
    for (int i = 0; i < game->gameState.numEnemies; i++) {
        game->gameState.enemies[i].x += 1.0f;
        if (game->gameState.enemies[i].x > WINDOW_WIDTH)
            game->gameState.enemies[i].x = 0;
    }
}


void renderGame(Game *game) {
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255); // Svart bakgrund
    SDL_RenderClear(game->renderer);


    SDL_SetRenderDrawColor(game->renderer, 255, 0, 0, 255); // Röd för fiender
    for (int i = 0; i < game->gameState.numEnemies; i++) {
        SDL_Rect enemyRect = {
            (int)game->gameState.enemies[i].x,
            (int)game->gameState.enemies[i].y,
            20, 20
        };
        SDL_RenderFillRect(game->renderer, &enemyRect);
    }


    SDL_RenderPresent(game->renderer);
}


int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDLNet_Init();


    Game game = {0};
    game.window = SDL_CreateWindow("Tower Defense Server", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    game.renderer = SDL_CreateRenderer(game.window, -1, SDL_RENDERER_ACCELERATED);


    game.socket = SDLNet_UDP_Open(SERVER_PORT);
    game.packet = SDLNet_AllocPacket(BUFFER_SIZE);


    game.numClients = 0;
    game.gameState.numEnemies = MAX_ENEMIES;
    for (int i = 0; i < MAX_ENEMIES; i++) {
        game.gameState.enemies[i].x = 50.0f * i;
        game.gameState.enemies[i].y = 100.0f + i * 30.0f;
    }


    printf("[SERVER] Running on port %d...\n", SERVER_PORT);


    int running = 1;
    SDL_Event event;


    while (running) {
        // Ta emot nya klienter
        while (SDLNet_UDP_Recv(game.socket, game.packet)) {
            addClient(game.packet->address, &game);
        }


        // Uppdatera fiender
        updateEnemies(&game);


        // Skicka gamestate till alla klienter
        sendGameState(&game);


        // Rendera spelet på serverfönstret
        renderGame(&game);


        // Hantera SDL events (så vi kan stänga fönstret)
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
        }


        SDL_Delay(100);
    }


    SDLNet_FreePacket(game.packet);
    SDLNet_UDP_Close(game.socket);
    SDL_DestroyRenderer(game.renderer);
    SDL_DestroyWindow(game.window);
    SDLNet_Quit();
    SDL_Quit();


    return 0;
}
