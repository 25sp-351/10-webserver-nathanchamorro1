#include "responses.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>

// Builds a response (i.e, 404) based on response.
static void response_status(HttpResponse *res, int status_code, const char *body, const char *status_text) {
    char header[1024];
    
    if (body != NULL) {
        res -> body = strdup(body);
        res -> body_length = strlen(body);
    } else {
        res -> body = NULL;
        res -> body_length = 0;
    }

    sprintf(header, "HTTP/1.1 %d %s\r\nContent-Type: text/html\r\nContent Length: %d\r\n\r\n",
            status_code, status_text, res->body_length);

    res->headers = strdup(header);
}

// Static file requests
static void handle_static(HttpResponse *res, const char *path) {
    char filepath[1024] = "static";
    strcat(filepath, path + 7);
    
    int file = open(filepath, O_RDONLY);
    
    if (file < 0) {
     response_status(res, 404, "<html><body><h1>404 Not Found</h1></body></html>", "Not Found");
        return;
    }
    
    // Get the file size
    struct stat st;
    fstat(file, &st);
    res -> body_length = st.st_size;
    
    // Read content of file
    res->body = malloc(res->body_length);
    read(file, res->body, res->body_length);
    close(file);
    
    // Looks at file extension and sets a file type.
    const char *content_type = "application/octet-stream";
    char *extension = strrchr(filepath, '.');
    if (extension) {
        if (strcasecmp(extension, ".html") == 0)
            content_type = "text/html";

        else if (strcasecmp(extension, ".txt") == 0)
            content_type = "text/plain";

        else if (strcasecmp(extension, ".jpg") == 0 || strcasecmp(extension, ".jpeg") == 0)
            content_type = "image/jpeg";
            
        else if (strcasecmp(extension, ".png") == 0)
            content_type = "image/png";
        else if (strcasecmp(extension, ".css") == 0)
            content_type = "text/css";

        else if (strcasecmp(extension, ".js") == 0)
            content_type = "application/javascript";
    }
    
    char header[512];
    sprintf(header,
            "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %d\r\n\r\n",
            content_type, res->body_length);
    res->headers = strdup(header);
}

// Calculator function
static void calculator(HttpResponse *res, const char *path) {

    
    char op[10];
    int num1, num2;
    
    int result = 0;

    sscanf(path, "/calc/%9[^/]/%d/%d", op, &num1, &num2);

    if (strcmp(op, "add") == 0) {
        result = num1 + num2;
    } else if (strcmp(op, "mul") == 0) {
        result = num1 * num2;
    } else if (strcmp(op, "div") == 0) {
        result = num1 / num2;
     } else if (strcmp(op, "sub") == 0) {
        result = num1 - num2;
    }
    
    char body[256];
    sprintf(body, "<html><body><h1>Result: %d</h1></body></html>", result);
    
    res -> body = strdup(body);
    res -> body_length = strlen(res->body);
    
    char header[512];
    sprintf(header,
            "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n",
            res->body_length);
    res->headers = strdup(header);
}

// Sleep function for pipelining
static void sleep_handler(HttpResponse *res, const char *path) {
    // Get seconds and limit
    int seconds = atoi(path + 7);
    if (seconds > 30) seconds = 30;
    
    // Sleeps for seconds inputted
    sleep(seconds);
    
    // Create response with fixed message
    const char *message = "Successfully slept.\n";
    res -> body = strdup(message);
    res -> body_length = strlen(message);
    
    // Set headers in one step
    res -> headers = malloc(128);
    sprintf(res -> headers, 
            "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: %d\r\n\r\n",
            res->body_length);
}

void build_request(HttpRequest *req, HttpResponse *res)
{
    // Set everything to NULL or 0
    res -> headers = NULL;
    res -> body = NULL;
    res -> body_length = 0;
    
    // Is it a GET request
    if (strcmp(req->method, "GET") != 0)
    {
        // If not GET, throw an error
        char error_msg[100];
        sprintf(error_msg, "HTTP/1.1 405 Missing GET\r\nContent-Length: 0\r\n\r\n");
        res -> headers = strdup(error_msg);
        return;
    }
    
    // Is it a static file
    if (strncmp(req->path, "/static/", 8) == 0)
    {
        handle_static(res, req->path);
    }
    // Is it a calculator request
    else if (strncmp(req->path, "/calc/", 6) == 0)
    {
        calculator(res, req->path);
    }
    // Is it a sleep request
    else if (strncmp(req -> path, "/sleep/", 7) == 0)
    {
        sleep_handler(res, req->path);
    }
    else
    {
        char error_msg[100];
        sprintf(error_msg, "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n");
        res->headers = strdup(error_msg);
    }
}

void send_request(int client_socket, HttpResponse *res) {
    // Send headers
    send(client_socket, res -> headers, strlen(res -> headers), 0);
    
    // If there's a body, send
    if (res->body_length > 0 && res->body) {
        send(client_socket, res->body, res->body_length, 0);
    }
}