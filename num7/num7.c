#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_REQUEST_SIZE 8192
#define MAX_RESPONSE_SIZE 8192

void handle_get(int client_socket, const char *path) {
    char response[MAX_RESPONSE_SIZE];
    FILE *file = fopen(path, "r");

    if (file != NULL) {
        sprintf(response, "HTTP/1.1 200 OK\r\n\r\n");
        send(client_socket, response, strlen(response), 0);

        while (fgets(response, MAX_RESPONSE_SIZE, file) != NULL) {
            send(client_socket, response, strlen(response), 0);
        }

        fclose(file);
    } else {
        sprintf(response, "HTTP/1.1 404 Not Found\r\n\r\n");
        send(client_socket, response, strlen(response), 0);
    }
}

void handle_post(int client_socket, const char *path, const char *content) {
    char response[MAX_RESPONSE_SIZE];

    // 여기에서는 단순히 content를 출력합니다.
    printf("Received POST data:\n%s\n", content);

    // 클라이언트에 응답을 보냅니다.
    sprintf(response, "HTTP/1.1 200 OK\r\n\r\n");
    send(client_socket, response, strlen(response), 0);

    sprintf(response, "<html><body><h1>Received POST data:</h1><p>%s</p></body></html>", content);
    send(client_socket, response, strlen(response), 0);
}

void handle_request(int client_socket, const char *request) {
    char method[10], path[100], protocol[20];

    sscanf(request, "%s %s %s", method, path, protocol);

    if (strcmp(method, "GET") == 0) {
        handle_get(client_socket, path + 1);  // Skip the leading '/'
    } else if (strcmp(method, "POST") == 0) {
        char *content_start = strstr(request, "\r\n\r\n");
        if (content_start != NULL) {
            content_start += 4;  // Move past the empty line
            handle_post(client_socket, path + 1, content_start);
        } else {
            // Invalid POST request
            printf("Invalid POST request\n");
        }
    } else {
        // Unsupported method
        printf("Unsupported HTTP method: %s\n", method);
    }
}



int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    char request[MAX_REQUEST_SIZE];

    // 소켓 초기화
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 서버 주소 설정
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);  // 포트번호 8080 사용

    // 바인딩
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // 리스닝
    if (listen(server_socket, 10) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port 8080...\n");

    while (1) {
        // 클라이언트 연결 수락
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);
        if (client_socket == -1) {
            perror("Accept failed");
            continue;
        }

        // 클라이언트로부터 HTTP 요청 수신
        recv(client_socket, request, MAX_REQUEST_SIZE, 0);

        // HTTP 요청 처리
        handle_request(client_socket, request);

        // 클라이언트 소켓 닫기
        close(client_socket);
    }

    // 서버 소켓 닫기
    close(server_socket);

    return 0;
}
