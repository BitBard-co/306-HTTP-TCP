CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2
LDFLAGS = 

SRC = src/main.c src/client.c src/http.c src/util.c
OBJ = $(SRC:.c=.o)
BIN = bin/sensor_client

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -Iinclude -c $< -o $@

clean:
	rm -f $(OBJ) $(BIN)

run:
	./bin/sensor_client httpshayan.org 80 /post 22.4 550e8400-e29b-41d4-a716-446655440000 raw
