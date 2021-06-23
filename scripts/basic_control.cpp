#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include <std_msgs/String.h>
#include <sstream>
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

void TcpEnable(){
    /* Creating a socket */
    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1){
        std::cout<<"Can't create a socket!";
    } else {
        std::cout << "Socket created. Value of 'listening': " << listening;
    }

    /* Bind the socket to IP / port */
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

    if (bind(listening, (sockaddr*) &hint, sizeof(hint)) == -1){
        std::cout << "Can't bind to IP/port";
    }

    /* Mark the socket for listening in */ 
    if (listen(listening, SOMAXCONN) == -1){
        std::cout << "Can't listen"; 
    }

    /* Accept a call */
    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];

    /* Create a socket and turn into non-blocking */
    int clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

    if (clientSocket == -1){
        std::cout<<"Problem with client connecting";
    }
    fcntl(clientSocket, F_SETFL, O_NONBLOCK);

    /* Close the listening socket */
    close(listening); 
    memset(host, 0, NI_MAXHOST);
    int result = getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0);
    if (result){
        std::cout << host << " connected on " << svc << std::endl;
    } else {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        std::cout << host << " connected on " << ntohs(client.sin_port) << std::endl;
    }
    memset(svc, 0, NI_MAXSERV);
}

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

void TcpReceive(int socket, TPacket packet){
    /* Receive messages */
    int bytes_received = recv(socket, &packet, 4096, 0);

    /* Error return value */
    if (bytes_received == -1){
        printf("[WARN] No incoming messages.\n");
        }

    /* Peer closed connection */
    if (bytes_received == 0){
        printf("[INFO] Client disconnected.\n");
    }

    /* Received data */
    if (bytes_received > 0){
        printf("[INFO] Received packet.\n");
    }
}
int main(int argc, char** argv){
    /* Creating a socket */
    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1){
        std::cout<<"Can't create a socket!";
    } else {
        std::cout << "Socket created. Value of 'listening': " << listening;
    }

    /* Bind the socket to IP / port */
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

    if (bind(listening, (sockaddr*) &hint, sizeof(hint)) == -1){
        std::cout << "Can't bind to IP/port";
    }

    /* Mark the socket for listening in */ 
    if (listen(listening, SOMAXCONN) == -1){
        std::cout << "Can't listen"; 
    }

    /* Accept a call */
    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];

    /* Create a socket and turn into non-blocking */
    int clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

    if (clientSocket == -1){
        std::cout<<"Problem with client connecting";
    }
    fcntl(clientSocket, F_SETFL, O_NONBLOCK);

    /* Close the listening socket */
    close(listening); 
    memset(host, 0, NI_MAXHOST);
    int result = getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0);
    if (result){
        std::cout << host << " connected on " << svc << std::endl;
    } else {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        std::cout << host << " connected on " << ntohs(client.sin_port) << std::endl;
    }
    memset(svc, 0, NI_MAXSERV);

    ros::init(argc, argv, "basic_control");
    ros::NodeHandle n;
    ros::Publisher cmd_pub = n.advertise<geometry_msgs::Twist>("cmd_vel", 1000);
    ros::Rate loop_rate(1);
    /* Create variables for the void functions to work */
    TPacket r_packet;
    r_packet.AppData.velocity.v = 0.0;
    r_packet.AppData.velocity.w = 0.0;
    char buf[4096];

    /* ROS */
    int count = 0;
    geometry_msgs::Twist msg;
    
    while(ros::ok()){
        /* TCP */
            /* Receive messages */
        int bytes_received = recv(clientSocket, &r_packet, 4096, 0);
            /* Error return value */
        if (bytes_received == -1){
            printf("[WARN] No incoming messages.\n");
            }
            /* Peer closed connection */
        if (bytes_received == 0){
            printf("[INFO] Client disconnected.\n");
        }
            /* Received data */
        if (bytes_received > 0){
            printf("[INFO] Received packet.\n");
        }

        /* ROS */
        msg.linear.x = r_packet.AppData.velocity.v;
        msg.angular.z = r_packet.AppData.velocity.w;
        cmd_pub.publish(msg);
        ros::spinOnce();
        loop_rate.sleep();
        ++count;
    }
    return 0;
}
