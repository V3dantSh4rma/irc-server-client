// IRC server implementation in C
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <string.h>
#include <processthreadsapi.h>

#define BUFFER 1024
#define MAX_USERS 100


SOCKET users[MAX_USERS] = {0};
int total_clients = 0;

DWORD WINAPI HandleClient(LPVOID lpParam)
{
    SOCKET client_socket = (SOCKET)lpParam;
    char buffer[BUFFER];
    puts("[SERVER] A Client just Connected to the server!");

    while(1)
    {
        int bytesRead = recv(client_socket, buffer, BUFFER, 0);
        if(bytesRead <= 0)
        {
            if(bytesRead == 0)
            {
                puts("[SERVER] A client just Disconnected from the server!");
            } else {
                printf("[ERROR] Error while receiving data. Error: %d", WSAGetLastError());
            }
            break;
        }

        
        buffer[bytesRead] = '\0';
        printf("Received: %s\n", buffer);

        for(int i = 0; i < total_clients; i++)
        {
            if(users[i] != client_socket)
            {
                send(users[i], buffer, bytesRead, 0);
            }
        }
    }

    closesocket(client_socket);
    return 0;
}

int main(int argc, char *argv[]){
    int port;

    if(argc == 1){
        puts("Usage: ./server 1234");
        exit(EXIT_FAILURE);
    }

    if(atoi(argv[1]) >= 0 && atoi(argv[1]) <= 65535)
    {
        port = atoi(argv[1]); 
    } else {
        puts("ERROR: The Port Number exceeds the TCP Port range.");
        exit(EXIT_FAILURE);
    }
    
    SOCKET server_socket = INVALID_SOCKET;
    SOCKET client_socket = INVALID_SOCKET;
    WSADATA wsaData;
    char buffer[BUFFER];

    if( WSAStartup(MAKEWORD(2,2), &wsaData) != 0 ){
        puts("[ERROR] Failed to start winsock.");
        exit(EXIT_FAILURE);
    }

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if(server_socket == INVALID_SOCKET)
    {
        puts("[ERROR] Invalid Socket.");
        exit(EXIT_FAILURE);
    }

    puts("[SOCKET] Socket initialised.");

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(bind(server_socket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR){
        puts("[ERROR] Unable to Bind the Socket Server.");
        exit(EXIT_FAILURE);
    }

    printf("[SERVER] Server Binded to Port: %d\n", port);

    if(listen(server_socket, SOMAXCONN) == SOCKET_ERROR)
    {
        printf("[ERROR] Unable to Listen. Error: %d\n", WSAGetLastError());
        closesocket(server_socket);
        exit(EXIT_FAILURE);
    }

    puts("[SERVER] TCP Server started. Waiting for incoming connections...");

    while (1) {
        client_socket = accept(server_socket, NULL, NULL);

        if(client_socket != INVALID_SOCKET)
        {
            if(total_clients < MAX_USERS)
            {
                users[total_clients] = client_socket;
                total_clients++;
                printf("[SOCKET] Number of clients: %d\n", total_clients);
            } else {
                puts("Maximum Users(100) Reached.");
                closesocket(client_socket);
            }
        }

        if (client_socket == INVALID_SOCKET) {
            printf("[ERROR] Failed to accept connections. Error: %d\n", WSAGetLastError());
            closesocket(client_socket);
            continue;
        }

        HANDLE thread = CreateThread(NULL, 0, HandleClient, (LPVOID)client_socket, 0, NULL);
        if(thread == NULL)
        {
           puts("[ERROR] Failed to create Thread.");
           closesocket(client_socket);
           continue;
        }
        CloseHandle(thread);
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}