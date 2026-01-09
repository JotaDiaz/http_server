#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include "http_handler.h"


#define TAMANO_BUFFER 4096
#define DOCUMENT_ROOT "./public"

void enviar_error(int sockfd, int codigo, char *mensaje){
    char respuesta[1024];
    sprintf(respuesta, "HTTP/1.0 %d %s\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Error %d</h1><p>%s</p></body></html>", codigo, mensaje, codigo, mensaje);
    send(sockfd, respuesta, strlen(respuesta), 0);
}


static int es_request_valido(char *uri, char *version) {  
    if (uri[0] != '/') {
        printf("uri debe empezar con / : %s\n", uri);
        return 0;
    }
  
    if (strncmp(version, "HTTP/", 5) != 0) {
        printf("no es http: %s\n", version);
        return 0;
    }
    
    return 1;
}


int parsear_peticion(char *buffer, char *metodo_http, char *uri, char* version ){
    // lee del buffer esperando recibir una ppeticion bien formada
    int leidos = sscanf(buffer, "%s %s %s", metodo_http, uri, version);
    return (leidos >= 3);
}


void cliente_handler(int sockfd){
    char buffer[TAMANO_BUFFER];
    int n;

  // variables para parsear la peticion
    char metodo_http[16];
    char uri[256];
    char version[16];
    char path_completo[512];

    memset(buffer,0, TAMANO_BUFFER);

    n = recv(sockfd, buffer, TAMANO_BUFFER -1, MSG_NOSIGNAL);

    if (n < 0){
        perror("error en recv");
        return;
    }

    printf("request recibida ---\n");

    if(!parsear_peticion(buffer, metodo_http, uri, version)){
        printf("fallo parsear peticion\n");
        enviar_error(sockfd,400,"bad request");
        return;
    }

    if (!es_request_valido(uri, version)) {
        printf("fallo es requiest valido\n");
        enviar_error(sockfd, 400, "Bad Request");
        return;
    }

    if (strcasecmp(metodo_http, "GET") != 0) {
        printf("no es GET\n");
        enviar_error(sockfd,501, "not implemented");
        return;
    }

    sprintf(path_completo, "%s%s", DOCUMENT_ROOT, uri); //arma la ruta donde el servidor deberia buscar 

    char body[1024];
    sprintf(body, 
        "<html><body>"
        "<h1>ok</h1>"
        "<p>uri de la peticion: <b>%s</b></p>"
        "<p>path_completo: <b>%s</b></p>"
        "</body></html>", 
        uri, path_completo);


    char response[2056];
    sprintf(response,"HTTP/1.0 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %ld\r\n"
        "\r\n"
        "%s", 
        strlen(body), body);

    n = send(sockfd,response,strlen(response), 0);

     if(n < 0){
      perror("error en send");
    }
}


