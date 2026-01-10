#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "utils.h"

void enviar_error(int sockfd, int codigo, char *mensaje){
    char respuesta[1024];
    sprintf(respuesta, "HTTP/1.0 %d %s\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Error %d</h1><p>%s</p></body></html>", codigo, mensaje, codigo, mensaje);
    send(sockfd, respuesta, strlen(respuesta), 0);
}

int es_request_valido(char *uri, char *version) {  
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
    int leidos = sscanf(buffer, "%s %s %s", metodo_http, uri, version);
    return (leidos >= 3);
}
