#ifndef RESPONSES_H
#define RESPONSES_H

#include "request.h"

// HTTP Response including headers, body, and the body length
typedef struct {
    char *headers;
    char *body;
    int body_length;
} HttpResponse;

// Builds response from request
void build_request(HttpRequest *req, HttpResponse *res);
// Sends response to socket
void send_request(int client_socket, HttpResponse *res);

#endif
