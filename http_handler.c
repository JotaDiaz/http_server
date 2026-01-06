#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include "http_handler.h"


#define TAMANO_BUFFER 4096


void cliente_handler(int sockfd){
  char buffer[TAMANO_BUFFER];
  int n;

  memset(buffer,0, TAMANO_BUFFER);

  n = recv(sockfd, buffer, TAMANO_BUFFER -1, MSG_NOSIGNAL);

  if (n < 0){
    perror("error en recv");
    return;
  }

  printf("request recibida ---");

  char *response = "HTTP/1.0 200 OK\r\n"
                     "Content-Type: text/html\r\n"
                     "\r\n"
                     "<html><body><h1>Hola Mundo!</h1><p>Este es mi servidor basico.</p></body></html>";         

  n = send(sockfd,response,strlen(response), 0);

  if(n < 0){
    perror("error en send");
  }
}


