FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    git \
    libssl-dev \
    libboost-all-dev \
    && rm -rf /var/lib/apt/lists/*

# removes cache
WORKDIR /app

RUN git clone https://github.com/zaphoyd/websocketpp.git /app/websocketpp

COPY . .

RUN cmake -S . -B build && cmake --build build

CMD ["./build/app"]

