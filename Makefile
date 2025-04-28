CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -pthread
TARGET = server

SRC = server.c request.c responses.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
