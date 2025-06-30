# Version for now doesnt matter
FROM ubuntu:22.04

# This sets the working directory inside the container to /app
WORKDIR /app

# This needs to change a lot wwhen we getting libraries
RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    make

COPY . .

RUN mkdir build && cd build && cmake .. && make

WORKDIR /app/build

CMD ["./app"]
