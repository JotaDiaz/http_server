// src/net.c
#include "net.h"
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>

ssize_t enviar_todo(int sockfd, const void *buf, size_t len) {
    size_t total = 0;
    const char *p = buf;
    
    while (total < len) {
        ssize_t enviados = send(sockfd, p + total, len - total, 0);
        if (enviados <= 0) return -1;
        total += enviados;
    }
    
    return total;
}

ssize_t recv_headers(int sockfd, char *buffer, size_t max) {
    size_t total = 0;
    while (total < max - 1) {
        ssize_t n = recv(sockfd, buffer + total, max - 1 - total, 0);
        
        if (n < 0) return -1;
        if (n == 0) break;
        
        total += n;
        buffer[total] = '\0';
        
        if (strstr(buffer, "\r\n\r\n")) return total;
    }
    
    return -1;
}