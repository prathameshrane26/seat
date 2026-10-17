# Use a lightweight Debian base image with GCC and build utilities
FROM debian:bookworm-slim

# Install build dependencies and SQLite development libraries
RUN apt-get update && apt-get install -y \
    build-essential \
    libsqlite3-dev \
    && rm -rf /var/lib/apt/lists/*

# Set working directory inside the container
WORKDIR /app

# Copy backend and frontend source directories into the container
COPY backend/ ./backend/
COPY frontend/ ./frontend/

# Compile the C server (linking SQLite and omitting Windows-specific Winsock flags for Linux)
WORKDIR /app/backend
RUN gcc src/main.c src/optimizer.c src/parser.c src/arena.c src/mongoose.c -Iinclude -O3 -o server -lsqlite3

# Expose port 5000 for web traffic
EXPOSE 5000

# Run the compiled C server binary
CMD ["./server"]