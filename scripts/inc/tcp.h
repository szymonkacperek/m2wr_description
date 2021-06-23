#ifndef TCP_H
#define TCP_H

#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <bitset>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

void TcpEnable(int* listening, sockaddr_in* hint, sockaddr_in* client, socklen_t* clientSize, int* clientSocket);

typedef struct{
    double v;
    double w;
}AppDataVelocities;

typedef struct{
    int i_ver;
    int i_content_id;
    union{
        AppDataVelocities velocity;
        unsigned char cTab[64];
    }AppData;
}TPacket;

#endif
