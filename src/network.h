#ifndef NETWORK_H
#define NETWORK_H

#define BUFFER_SIZE 512
#define SERVER_PORT 2000
#define CLIENT_PORT 2001 // Fast port för klienten (viktigt för Windows!)

enum CommandType {
    COMMAND_INPUT = 1,
    COMMAND_STATE = 2
};

typedef struct {
    int command;
    float mouseX;
    float mouseY;
    int action;
} ClientInputPacket;

typedef struct {
    int command;
    int numEnemies;
    float enemyPositionsX[100];
    float enemyPositionsY[100];
} ServerStatePacket;

#endif
