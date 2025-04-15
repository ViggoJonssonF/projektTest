#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_net.h>


#define SERVER_PORT 2000
#define BUFFER_SIZE 512
#define MAX_ENEMIES 5
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600


// Enemy struct
typedef struct
{
    float x, y;
} EnemyData;


// Packet from server
typedef struct
{
    int numEnemies;
    EnemyData enemies[MAX_ENEMIES];
} ServerData;


int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    SDLNet_Init();


    SDL_Window *window = SDL_CreateWindow("Tower Defense Client", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);


    UDPsocket clientSocket = SDLNet_UDP_Open(0);
    UDPpacket *packet = SDLNet_AllocPacket(BUFFER_SIZE);


    // Send initial packet to server
    IPaddress serverAddress;
    SDLNet_ResolveHost(&serverAddress, "127.0.0.1", SERVER_PORT);


    packet->address = serverAddress;
    packet->len = 4;
    ((int *)packet->data)[0] = 1234;
    SDLNet_UDP_Send(clientSocket, -1, packet);


    printf("[CLIENT] Sent initial packet to server.\n");


    int running = 1;
    SDL_Event event;
    static int connected = 0; // Lägg till här!


    while (running)
    {
        // Receive game state from server
        if (SDLNet_UDP_Recv(clientSocket, packet)){
            if (!connected) {
                printf("[CLIENT] NICE Connection established with server!\n");
                connected = 1;
            }


            ServerData gameState;
            memcpy(&gameState, packet->data, sizeof(ServerData));


            // printf("[CLIENT] Game state received:\n");
            // for (int i = 0; i < gameState.numEnemies; i++) {
            //     printf("  Enemy %d position: (%.2f, %.2f)\n", i, gameState.enemies[i].x, gameState.enemies[i].y);
            // }


            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);


            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            for (int i = 0; i < gameState.numEnemies; i++){
                SDL_Rect enemyRect = {(int)gameState.enemies[i].x, (int)gameState.enemies[i].y, 20, 20};
                SDL_RenderFillRect(renderer, &enemyRect);
            }


            SDL_RenderPresent(renderer);
        }


        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = 0;
        }


        SDL_Delay(50);
    }


    SDLNet_FreePacket(packet);
    SDLNet_UDP_Close(clientSocket);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDLNet_Quit();
    SDL_Quit();


    return 0;
}
