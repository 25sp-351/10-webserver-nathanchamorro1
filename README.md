The server handles each client connection in a separate thread and processes basic HTTP/1.1 requests.
It can handle static files, performing calculations, and handling sleep commands.

I did a sleep function but I couldn't figure out how to do pipelining.

## Files

- server.c
- request.c
- responses.c
- request.h
- responses.h
- Makefile
- README.md

## Executing

To compile both programs, please run:

make
