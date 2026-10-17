FROM debian:bookworm-slim

RUN apt-get update && apt-get install -y \
    build-essential \
    libsqlite3-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy the entire backend directory (which includes src, include, frontend, and Makefile)
COPY backend/ ./backend/

WORKDIR /app/backend

# Build the C server using make
RUN make

EXPOSE 5000

CMD ["./server.exe"]