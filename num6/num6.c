#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#define MAX_CLIENTS 10
#define MAX_MESSAGE_SIZE 1024

int main() {
    int serverSocket, clientSockets[MAX_CLIENTS], maxClients = 0;
    struct sockaddr_in serverAddr, clientAddr;
    fd_set readfds;
    char message[MAX_MESSAGE_SIZE];

    // Create server socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Error creating server socket");
        exit(EXIT_FAILURE);
    }

    // Set up server address structure
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);

    // Bind server socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error binding server socket");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) == -1) {
        perror("Error listening for connections");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port 8080...\n");

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(serverSocket, &readfds);
        int maxfd = serverSocket;

        for (int i = 0; i < maxClients; ++i) {
            int clientSocket = clientSockets[i];
            if (clientSocket > 0) {
                FD_SET(clientSocket, &readfds);
                if (clientSocket > maxfd) {
                    maxfd = clientSocket;
                }
            }
        }

        // Use select to wait for activity on any of the sockets
        if (select(maxfd + 1, &readfds, NULL, NULL, NULL) == -1) {
            perror("Error in select");
            exit(EXIT_FAILURE);
        }

        // Check for new connection
        if (FD_ISSET(serverSocket, &readfds)) {
            int newSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, (socklen_t*)sizeof(clientAddr));
            if (newSocket == -1) {
                perror("Error accepting connection");
                exit(EXIT_FAILURE);
            }

            printf("New connection, socket fd is %d, ip is: %s, port: %d\n",
                   newSocket, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

            // Add new socket to array of client sockets
            for (int i = 0; i < MAX_CLIENTS; ++i) {
                if (clientSockets[i] == 0) {
                    clientSockets[i] = newSocket;
                    if (i >= maxClients) {
                        maxClients = i + 1;
                    }
                    break;
                }
            }
        }

        // Check for data from clients
        for (int i = 0; i < maxClients; ++i) {
            int clientSocket = clientSockets[i];
            if (FD_ISSET(clientSocket, &readfds)) {
                // Read data from client
                ssize_t bytesRead = recv(clientSocket, message, MAX_MESSAGE_SIZE, 0);
                if (bytesRead <= 0) {
                    // Connection closed or error
                    printf("Client disconnected, socket fd is %d\n", clientSocket);
                    close(clientSocket);
                    clientSockets[i] = 0;
                } else {
                    // Broadcast message to all clients
                    for (int j = 0; j < maxClients; ++j) {
                        if (clientSockets[j] > 0 && j != i) {
                            send(clientSockets[j], message, bytesRead, 0);
                        }
                    }
                }
            }
        }
    }

    // Close all client sockets
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        int clientSocket = clientSockets[i];
        if (clientSocket > 0) {
            close(clientSocket);
        }
    }

    // Close server socket
    close(serverSocket);

    return 0;
}
