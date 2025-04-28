#include "request.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>

int parse(int client_socket, HttpRequest *req)
{
    char buffer[4096];
    int bytes_read = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    
    if (bytes_read <= 0)
    {
        printf("Error: Could not read from socket.\n");
        return -1;
    }
    
    buffer[bytes_read] = '\0';
    
    printf("Received request:\n%s\n", buffer);
    req->method[0] = '\0';
    req->path[0] = '\0';
    
    sscanf(buffer, "%s %s", req->method, req->path);
    
    return 0;
}