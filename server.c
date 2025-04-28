#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "request.h"
#include "responses.h"

#define MAXPENDING 10

// Prints an error message and exits the program
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

// Handles each client connection in a separate thread
void *handle_client(void *arg)
{
    int client_socket = *(int *)arg;
    free(arg);


// Build request chooses how to reply based on request. Send Request sends headers and body to client.
    HttpRequest req;
    if (parse(client_socket, &req) == 0)
{
    HttpResponse res;
    build_request(&req, &res);
    send_request(client_socket, &res);
}
    close(client_socket);
    printf("Client disconnected\n");

    return NULL;
}

int main(int argc, char *argv[])
{
    int port = 80; // Default port

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-p") == 0 && i + 1 < argc)
        {
            port = atoi(argv[++i]);
        }
        else
        {
            fprintf(stderr, "Usage: %s [-p <port>]\n", argv[0]);
            exit(1);
        }
    }

    int server_socket;
    int client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Create TCP socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        error("socket");
    }

    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Set up server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // Bind socket to address and port
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        error("bind");
    }
    // Listens to incoming connections
    if (listen(server_socket, MAXPENDING) < 0)
    {
        error("listen");
    }

    printf("Server listening on port %d\n", port);
    // Accepts and handle client connections
    while (1)
    {
        if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len)) < 0)
        {
            error("accept");
        }

        // Allocate memory for client socket to pass into thread
        int *client_sock_ptr = malloc(sizeof(int));
        *client_sock_ptr = client_socket;


        // Create a thread to handle the client
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_client, client_sock_ptr) != 0)
        {
            perror("pthread_create");
            free(client_sock_ptr);
            close(client_socket);
        }
        else
        {
            pthread_detach(thread_id);
        }
    }

    close(server_socket);
    return 0;
}
