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

    http_request_t request = {0};
    request.sockfd = sockfd;

    
    memset(buffer, 0, TAMANO_BUFFER);
    
    printf("--- Esperando headers ---\n");
    n = recv_headers(sockfd, buffer, TAMANO_BUFFER - 1);

    if (n <= 0) {
        if (n < 0) enviar_error(&request, 400, "Bad Request");
        return;
    }
    
    if (parsear_peticion(buffer, &request) < 0) {
        printf("Fallo al parsear petición\n");
        enviar_error(&request, 400, "Bad Request");
        return;
    }

    if (!es_request_valido(&request)) {
        printf("Request inválido: URI o Versión incorrecta\n");
        enviar_error(&request, 400, "Bad Request");
        return;
    }

    if (strcasecmp(request.metodo, "GET") == 0) {
        handle_GET(&request, DOCUMENT_ROOT);
    }
    else if (strcasecmp(request.metodo, "HEAD") == 0) {
        enviar_error(&request, 501, "Not Implemented");
    }
    else {
        printf("Método no soportado: %s\n", request.metodo);
        enviar_error(&request, 501, "Not Implemented");
    }
}