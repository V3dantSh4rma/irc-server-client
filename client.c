// IRC client imlpementation
#include <stdio.h>
#include <WinSock2.h>
#include <string.h>
#include <Windows.h>

#define BUFFER 1024
#define MAX_LIM 100

void getBroadcast(LPVOID socket)
{
    SOCKET client_socket = *(SOCKET*) socket;
    char buff[BUFFER];
    while(1)
    {
        int bytesRead = recv(client_socket, buff, BUFFER, 0);
        
        if (bytesRead <= 0)
        {
            if (bytesRead == 0)
            {
                puts("[SERVER] A client just Disconnected from the server!");
            } else {
                printf("[ERROR] Error while receiving data. Error: %d", WSAGetLastError());
            }
            break;        
        }
        
        buff[bytesRead] = '\0';
        printf("\n[SERVER] Received: \"%s\"\n", buff);
        printf(">> ");
    }
}

int main(int argc, char *argv[]){
    int port;

    if(argc == 1){
        puts("Usage: ./client 1234");
        exit(EXIT_FAILURE);
    }

    if(atoi(argv[1]) >= 0 && atoi(argv[1]) <= 65535)
    {
        port = atoi(argv[1]); 
    } else {
        puts("ERROR: The Port Number exceeds the TCP Port range.");
        exit(EXIT_FAILURE);
    }

    SOCKET client_socket = INVALID_SOCKET;
    WSADATA wsaData;
    char buffer[BUFFER];

    if( WSAStartup(MAKEWORD(2,2), &wsaData) != 0 ){
        puts("[ERROR] Failed to start winsock.");
        exit(EXIT_FAILURE);
    }

    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    if(client_socket == INVALID_SOCKET)
    {
        puts("[ERROR] Invalid Socket.");
        exit(EXIT_FAILURE);
    }

    puts("[SOCKET] Socket initialised.");

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(connect(client_socket, (struct sockaddr *)&server, sizeof(server)) != 0)
    {
        printf("[ERROR] Unable to connect to the server. Error: %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    puts("[SERVER] Connected to the server...");
    
    HANDLE thr = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) getBroadcast, &client_socket, 0, NULL);

    if(thr == NULL)
    {
        printf("[ERROR] Failed to create thread. Error: %d\n", WSAGetLastError());
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    printf("Enter the message to send to the server ('.exit' to quit)\n");

    while (1) {
        printf(">> ");
        fgets(buffer, BUFFER, stdin);
        
        buffer[strcspn(buffer, "\n")] = '\0';

        if (strcmp(buffer, ".exit") == 0) 
        {
            exit(EXIT_SUCCESS);
        }

        if (send(client_socket, buffer, strlen(buffer), 0) != SOCKET_ERROR) {
            puts("sent.");
        } else {
            printf("[ERROR] Error: %d\n", WSAGetLastError());
            break;
        }        
    }

    WaitForSingleObject(thr, INFINITE);
    CloseHandle(thr);
    closesocket(client_socket);
    WSACleanup();
    return 0;
}