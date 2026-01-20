#ifndef NET_H
#define NET_H

#include <sys/types.h>

ssize_t enviar_todo(int sockfd, const void *buf, size_t len);
ssize_t recv_headers(int sockfd, char *buffer, size_t max);

#endif