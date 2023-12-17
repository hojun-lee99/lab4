#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAX_REQUEST_SIZE 2048
#define MAX_RESPONSE_SIZE 2048

void handleGET(int clientSocket, const char *path) {
    char response[MAX_RESPONSE_SIZE];
    FILE *file = fopen(path, "r");

    if (file == NULL) {
        sprintf(response, "HTTP/1.1 404 Not Found\r\n\r\n");
    } else {
        sprintf(response, "HTTP/1.1 200 OK\r\n\r\n");
        char buffer[MAX_RESPONSE_SIZE];
        while (fgets(buffer, sizeof(buffer), file) != NULL) {
            strcat(response, buffer);
        }
        fclose(file);
    }

    send(clientSocket, response, strlen(response), 0);
}

void handlePOST(int clientSocket, const char *path, const char *data) {
    // Implement your own POST handling logic here
    // This is a simple example and does not handle data or perform any action
    char response[MAX_RESPONSE_SIZE];
    sprintf(response, "HTTP/1.1 200 OK\r\n\r\nPOST request received at %s", path);
    send(clientSocket, response, strlen(response), 0);
}

void handleRequest(int clientSocket, const char *request) {
    char method[10], path[255];
    sscanf(request, "%s %s", method, path);

    if (strcmp(method, "GET") == 0) {
        handleGET(clientSocket, path);
    } else if (strcmp(method, "POST") == 0) {
        // Extract data from POST request
        char *dataStart = strstr(request, "\r\n\r\n");
        if (dataStart != NULL) {
            dataStart += 4;  // Move past the "\r\n\r\n"
            handlePOST(clientSocket, path, dataStart);
        } else {
            // Invalid POST request
            char response[MAX_RESPONSE_SIZE];
            sprintf(response, "HTTP/1.1 400 Bad Request\r\n\r\n");
            send(clientSocket, response, strlen(response), 0);
        }
    } else {
        // Unsupported method
        char response[MAX_RESPONSE_SIZE];
        sprintf(response, "HTTP/1.1 501 Not Implemented\r\n\r\n");
        send(clientSocket, response, strlen(response), 0);
    }
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    char request[MAX_REQUEST_SIZE];

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
        // Accept incoming connection
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket == -1) {
            perror("Error accepting connection");
            continue;
        }

        printf("Client connected, IP: %s, Port: %d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        // Receive request from client
        ssize_t bytesRead = recv(clientSocket, request, sizeof(request), 0);
        if (bytesRead > 0) {
            // Null-terminate the request string
            request[bytesRead] = '\0';

            // Handle the request
            handleRequest(clientSocket, request);
        }

        // Close the client socket
        close(clientSocket);
        printf("Client disconnected.\n");
    }

    // Close the server socket
    close(serverSocket);

    return 0;
}
