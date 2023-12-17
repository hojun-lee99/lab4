#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

int main() {
    int server_socket, client_sockets[MAX_CLIENTS];
    fd_set read_fds;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);

    // 소켓 초기화
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 서버 주소 설정
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(12345);  // 포트번호 12345 사용

    // 바인딩
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // 리스닝
    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port 12345...\n");

    // 클라이언트 소켓 배열 초기화
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        client_sockets[i] = 0;
    }

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(server_socket, &read_fds);

        // 최대 소켓 디스크립터 구하기
        int max_sd = server_socket;
        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (client_sockets[i] > 0) {
                FD_SET(client_sockets[i], &read_fds);
                if (client_sockets[i] > max_sd) {
                    max_sd = client_sockets[i];
                }
            }
        }

        // select를 사용하여 활성화된 소켓 디스크립터 확인
        int activity = select(max_sd + 1, &read_fds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            perror("Select error");
        }

        // 새 클라이언트 연결 처리
        if (FD_ISSET(server_socket, &read_fds)) {
            int new_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);
            if (new_socket < 0) {
                perror("Accept failed");
                exit(EXIT_FAILURE);
            }

            // 연결된 클라이언트 소켓을 배열에 추가
            for (int i = 0; i < MAX_CLIENTS; ++i) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = new_socket;
                    printf("New connection, socket fd is %d, IP is: %s, Port: %d\n",
                           new_socket, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                    break;
                }
            }
        }

        // 클라이언트로부터 메시지 수신 및 방송
        for (int i = 0; i < MAX_CLIENTS; ++i) {
            int sd = client_sockets[i];
            if (FD_ISSET(sd, &read_fds)) {
                char buffer[BUFFER_SIZE];
                memset(buffer, 0, sizeof(buffer));
                int valread = read(sd, buffer, BUFFER_SIZE);

                if (valread <= 0) {
                    // 연결이 끊어진 경우
                    printf("Host disconnected, socket fd is %d\n", sd);
                    close(sd);
                    client_sockets[i] = 0;
                } else {
                    // 클라이언트로부터 메시지를 수신하고 모든 클라이언트에게 방송
                    printf("Client %d: %s", i + 1, buffer);
                    for (int j = 0; j < MAX_CLIENTS; ++j) {
                        int dest_sd = client_sockets[j];
                        if (dest_sd != 0 && dest_sd != sd) {
                            send(dest_sd, buffer, strlen(buffer), 0);
                        }
                    }
                }
            }
        }
    }

    return 0;
}
