cmake_minimum_required(VERSION 3.10)

project(NetworkProtocoles VERSION 1.0)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_C_EXTENSIONS OFF)


add_executable(NetworkProtocoles
    HttpsProtocole/client.c
    HttpsProtocole/main.c
    HttpsProtocole/server.c
    TCPProtocole/tcp.c
    UDPProtocole/udp.c
)