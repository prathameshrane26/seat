CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -O3

# Detect OS: Windows uses -lws2_32, Linux/Docker does not need extra socket libraries
ifeq ($(OS),Windows_NT)
    LIBS = -lws2_32
    TARGET = server.exe
else
    LIBS = 
    TARGET = server.exe
endif

all: $(TARGET)

$(TARGET): src/main.o src/optimizer.o src/parser.o src/arena.o src/mongoose.o src/sqlite3.o
	$(CC) $^ -o $@ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)