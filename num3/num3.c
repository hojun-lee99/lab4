#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 5

int buffer[BUFFER_SIZE];
sem_t mutex, empty, full;

// 세마포어 초기화
void init_semaphores() {
    sem_init(&mutex, 0, 1);  // 상호 배제를 위한 이진 세마포어
    sem_init(&empty, 0, BUFFER_SIZE);  // 버퍼의 빈 슬롯 개수를 나타내는 세마포어
    sem_init(&full, 0, 0);  // 버퍼의 찬 슬롯 개수를 나타내는 세마포어
}

// 생산자 스레드 함수
void *producer(void *arg) {
    int item;
    while (1) {
        item = rand() % 100;  // 아이템을 생성

        sem_wait(&empty);
        sem_wait(&mutex);

        // 임계 영역: 아이템을 버퍼에 추가
        printf("생산자가 아이템을 생산했습니다: %d\n", item);
        for (int i = 0; i < BUFFER_SIZE; ++i) {
            if (buffer[i] == 0) {
                buffer[i] = item;
                break;
            }
        }

        sem_post(&mutex);
        sem_post(&full);
    }
}

// 소비자 스레드 함수
void *consumer(void *arg) {
    int item;
    while (1) {
        sem_wait(&full);
        sem_wait(&mutex);

        // 임계 영역: 버퍼에서 아이템을 제거
        for (int i = 0; i < BUFFER_SIZE; ++i) {
            if (buffer[i] != 0) {
                item = buffer[i];
                buffer[i] = 0;
                break;
            }
        }

        printf("소비자가 아이템을 소비했습니다: %d\n", item);

        sem_post(&mutex);
        sem_post(&empty);

        // 아이템을 소비 (아이템을 처리)
        // ...

        // 처리 시간을 시뮬레이션하기 위해 sleep 사용
        usleep(100000);
    }
}

int main() {
    pthread_t producer_thread, consumer_thread;

    init_semaphores();

    // 여러 개의 생산자 및 소비자 스레드 생성
    pthread_create(&producer_thread, NULL, producer, NULL);
    pthread_create(&consumer_thread, NULL, consumer, NULL);

    // 스레드가 동작할 수 있도록 잠시 대기
    sleep(5);

    // 프로그램 종료 (실제 프로덕션 코드에서는 적절한 동기화를 사용할 것)
    exit(0);
}
