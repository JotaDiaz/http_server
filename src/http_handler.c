#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include "http_handler.h"
#include "http.h"
#include "metodos.h" 
#include "net.h"     

#define TAMANO_BUFFER 4096
#define DOCUMENT_ROOT "./public"

void cliente_handler(int sockfd) {
    char buffer[TAMANO_BUFFER];
    int n;

    char metodo_http[16];
    char uri[256];
    char version[16];
    
    memset(buffer, 0, TAMANO_BUFFER);
    
    printf("--- Esperando headers ---\n");
    n = recv_headers(sockfd, buffer, TAMANO_BUFFER - 1);

    if (n <= 0) {
        if (n < 0) enviar_error(sockfd, 400, "Bad Request");
        return;
    }
    
    if (parsear_peticion(buffer, metodo_http, uri, version) < 0) {
        printf("Fallo al parsear petición\n");
        enviar_error(sockfd, 400, "Bad Request");
        return;
    }

    if (!es_request_valido(uri, version)) {
        printf("Request inválido: URI o Versión incorrecta\n");
        enviar_error(sockfd, 400, "Bad Request");
        return;
    }


    if (strcasecmp(metodo_http, "GET") == 0) {
        handle_GET(sockfd, uri, DOCUMENT_ROOT);
    }
    else if (strcasecmp(metodo_http, "HEAD") == 0) {
        enviar_error(sockfd, 501, "Not Implemented");
    }
    else {
        printf("Método no soportado: %s\n", metodo_http);
        enviar_error(sockfd, 501, "Not Implemented");
    }
}