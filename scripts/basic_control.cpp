#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include <std_msgs/String.h>
#include <sstream>

/* tcp */
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

int main(int argc, char** argv){
    /* TCP: Creating a socket */
    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1) {
        std::cout << "[INFO] Can't create a socket!";
    } else {
        std::cout << "[INFO] Socket created. Value of 'listening': " << listening;
    }
    /* TCP: Bind the socket to IP / port */
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);
    if (bind(listening, (sockaddr*) &hint, sizeof(hint)) == -1) { std::cout <<
        "[INFO] Can't bind to IP/port"; }
    /* TCP: Mark the socket for listening in */ 
    if (listen(listening, SOMAXCONN) == -1) { std::cout << "[INFO] Can't listen\n"; }
    /* TCP: Accept a call */
    sockaddr_in client;
    socklen_t clientSize = sizeof(client);
    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];
    /* TCP: Create a socket and turn into non-blocking */
    int clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
    if (clientSocket == -1) { std::cout<<"[INFO] Problem with client connecting\n"; }
    fcntl(clientSocket, F_SETFL, O_NONBLOCK);
    /* TCP: Close the listening socket */
    close(listening); 
    memset(host, 0, NI_MAXHOST); int result = getnameinfo((sockaddr*)&client,
            sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0);
    if (result ){
        std::cout << host << " connected on " << svc << std::endl;
    } else {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        std::cout << host << " connected on " << ntohs(client.sin_port) << std::endl;
    }
    memset(svc, 0, NI_MAXSERV);
    /* TCP: Create storage variables */
    TPacket r_packet;
    r_packet.AppData.velocity.v = 0.0;
    r_packet.AppData.velocity.w = 0.0;
    char buf[4096];

    /* ROS: init, refresh rate, setup messages */
    ros::init(argc, argv, "basic_control");
    ros::NodeHandle n;
    ros::Publisher cmd_pub = n.advertise<geometry_msgs::Twist>("cmd_vel", 1000);
    ros::Rate loop_rate(1);
    int count = 0;
    geometry_msgs::Twist msg;
    while (ros::ok()) {
        /* TCP: Receive messages and check for errors */
        int bytes_received = recv(clientSocket, &r_packet, 4096, 0);
        if (bytes_received == -1) { printf("[WARN] No incoming messages.\n"); }
        if (bytes_received == 0) { printf("[INFO] Client disconnected.\n"); }
        if (bytes_received > 0) { printf("[INFO] Received packet.\n"); }

        /* ROS: send steering signals */
        msg.linear.x = r_packet.AppData.velocity.v;
        msg.angular.z = r_packet.AppData.velocity.w;
        cmd_pub.publish(msg);
        ros::spinOnce();
        loop_rate.sleep();
        ++count;
    }
    return 0;
}
