#ifndef REQUEST_H
#define REQUEST_H

// HTTP Request after oarsing
typedef struct {
    char method[8];
    char path[1024];
} HttpRequest;

// Parses into HttpRequest struct
int parse(int client_socket, HttpRequest *req);

#endif
