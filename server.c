#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "http_handler.h"


#define BACKLOG 10 // cant de peticiones pendientes en listen()
#define PORT 3490

void error(const char *msg){
  perror(msg);
  exit(1);
}

int main(int argc, char *argv[])
{
  int sockfd; // file descriptor principal
  int newsockfd; // file descriptor del cliente
  int port;
  socklen_t clilen;
  struct sockaddr_in serv_addr, cli_addr; //solo ipv4

  /*
   * TODO: cambiar sockaddre_in a getaddrinfo para que soporte ipv4 e ipv6
   *
   * bg dixit: 
   * un puntero a struct sockaddr_in puede convertirse en un puntero a
    struct sockaddr y viceversa. Así que aunque connect() quiera una struct sockaddr*, puedes
      usar una struct sockaddr_in y convertirla en el último momento. (ver capitulo 3)
   *
  */

  sockfd = socket(AF_INET, SOCK_STREAM, 0); //SYSCALL

  if (sockfd < 0)
    error("error al crear socket");

  memset(&serv_addr,0,sizeof(serv_addr)); // aca tambien se 'blanquea' el campo sin_zero de sockaddr_in
  port = PORT;

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);
  serv_addr.sin_addr.s_addr = INADDR_ANY;


  if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) //syscall BIND y casteo a sockaddr
          error("error en bind");

  listen(sockfd, BACKLOG);

  printf("escuchando %d", port);


  while (1) {
    clilen = (sizeof(cli_addr));
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

    if(newsockfd < 0)
      error("error en accept");

    cliente_handler(newsockfd);
    close(newsockfd);

  }

  close(sockfd);
  return 0;
  
}
