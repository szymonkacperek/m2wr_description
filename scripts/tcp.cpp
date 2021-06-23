#include "inc/tcp.h"

void TcpEnable(int listening, sockaddr_in* hint, sockaddr_in* client, socklen_t clientSize, int clientSocket){
    /* Creating a socket */
    listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1){
        std::cout<<"Can't create a socket!";
    } else {
        std::cout << "Socket created. Value of 'listening': " << listening;
    }

    /* Bind the socket to IP / port */
    hint->sin_family = AF_INET;
    hint->sin_port = htons(54000);
    inet_pton(AF_INET, "0.0.0.0", &hint->sin_addr);

    if (bind(listening, (sockaddr*) &hint, sizeof(hint)) == -1){
        std::cout << "Can't bind to IP/port\n";
    }

    /* Mark the socket for listening in */ 
    if (listen(listening, SOMAXCONN) == -1){
        std::cout << "Can't listen"; 
    }

    /* Accept a call */
    clientSize = sizeof(client);
    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];

    /* Create a socket and turn into non-blocking */
    clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
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
        inet_ntop(AF_INET, &client->sin_addr, host, NI_MAXHOST);
        std::cout << host << " connected on " << ntohs(client->sin_port) << std::endl;
    }
    memset(svc, 0, NI_MAXSERV);
}
