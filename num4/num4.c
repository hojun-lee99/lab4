#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


#define MAX_CLIENTS 5

pthread_mutex_t mutex;
pthread_cond_t cond;

int messageQueue[MAX_CLIENTS];
int numClients = 0;

void *clientThread(void *arg) {
    int clientID = *(int *)arg;

    while (1) {
        // Simulate sending a message
        int message = rand() % 100;
        pthread_mutex_lock(&mutex);
        messageQueue[clientID] = message;
        printf("Client %d: Sent message %d\n", clientID, message);
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);

        // Simulate some delay between messages
        sleep(2);
    }
}

void *serverThread(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex);

        // Wait for a message from any client
        while (numClients == 0) {
            pthread_cond_wait(&cond, &mutex);
        }

        // Broadcast the message to all clients
        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (messageQueue[i] != -1) {
                printf("Server: Broadcasting message %d to Client %d\n", messageQueue[i], i);
                messageQueue[i] = -1;
            }
        }

        pthread_mutex_unlock(&mutex);
    }
}

int main() {
    pthread_t server, clients[MAX_CLIENTS];
    int clientIDs[MAX_CLIENTS];

    // Initialize mutex and condition variable
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    // Create server thread
    pthread_create(&server, NULL, serverThread, NULL);

    // Create client threads
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        clientIDs[i] = i;
        pthread_create(&clients[i], NULL, clientThread, &clientIDs[i]);
    }

    // Join threads (this won't actually happen in this example)
    pthread_join(server, NULL);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        pthread_join(clients[i], NULL);
    }

    // Cleanup
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
