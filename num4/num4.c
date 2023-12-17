#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define MAX_CLIENTS 5
#define MESSAGE_SIZE 256

// 공유 자원을 위한 구조체
typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    char message[MESSAGE_SIZE];
} SharedData;

SharedData shared_data;

// 클라이언트(자식) 쓰레드 함수
void *client_thread(void *arg) {
    int client_id = *((int *)arg);

    while (1) {
        pthread_mutex_lock(&shared_data.mutex);

        // 조건 변수를 사용하여 메시지를 기다림
        while (shared_data.message[0] == '\0') {
            pthread_cond_wait(&shared_data.cond, &shared_data.mutex);
        }

        // 메시지를 받고 출력
        printf("Client %d received message: %s\n", client_id, shared_data.message);

        // 메시지 초기화
        shared_data.message[0] = '\0';

        pthread_mutex_unlock(&shared_data.mutex);
    }

    return NULL;
}

// 서버(부모) 쓰레드 함수
void *server_thread(void *arg) {
    int client_ids[MAX_CLIENTS];
    pthread_t clients[MAX_CLIENTS];

    // 클라이언트(자식) 쓰레드들 생성
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        client_ids[i] = i + 1;
        pthread_create(&clients[i], NULL, client_thread, &client_ids[i]);
    }

    while (1) {
        char message[MESSAGE_SIZE];

        // 메시지 입력 받음
        printf("Enter message to broadcast: ");
        fgets(message, MESSAGE_SIZE, stdin);

        // 메시지를 모든 클라이언트에게 방송
        pthread_mutex_lock(&shared_data.mutex);

        for (int i = 0; i < MAX_CLIENTS; ++i) {
            // 메시지를 복사하고 조건 변수를 사용하여 클라이언트에게 알림
            strcpy(shared_data.message, message);
            pthread_cond_signal(&shared_data.cond);
        }

        pthread_mutex_unlock(&shared_data.mutex);
    }

    return NULL;
}

int main() {
    // 초기화
    shared_data.message[0] = '\0';
    pthread_mutex_init(&shared_data.mutex, NULL);
    pthread_cond_init(&shared_data.cond, NULL);

    pthread_t server_thread_id;
    pthread_create(&server_thread_id, NULL, server_thread, NULL);

    // 대기
    pthread_join(server_thread_id, NULL);

    return 0;
}
