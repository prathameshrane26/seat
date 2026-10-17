CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -O3
SRC = src/main.c src/optimizer.c src/parser.c src/arena.c src/mongoose.c src/sqlite3.c
OBJ = $(SRC:.c=.o)
TARGET = server.exe

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) -lws2_32

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)
	