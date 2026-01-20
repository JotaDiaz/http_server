// src/http.c
#include "http.h"
#include "net.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

int parsear_peticion(char *buffer, http_request_t *request){
    int leidos = sscanf(buffer, "%15s %255s %15s", request->metodo, request->uri, request->version);

    if(leidos < 3)
        return -1;
    
    return 0;
    
}


int es_request_valido(http_request_t *request) {  
    if (request->uri[0] != '/') {
        printf("uri debe empezar con / : %s\n", request->uri);
        return 0;
    }
    if (strncmp(request->version, "HTTP/", 5) != 0) {
        printf("no es http: %s\n", request->version);
        return 0;
    }
    return 1;
}


void get_fecha(char *buf, size_t tamanio) {
    time_t now;
    struct tm *tm_info;
    
    time(&now);
    tm_info = gmtime(&now);
    strftime(buf, tamanio, "%a, %d %b %Y %H:%M:%S GMT", tm_info);
}


void enviar_error(http_request_t *request, int codigo, char *mensaje) {    
    char header[512];
    char body[512];
    char fecha_str[64];

    get_fecha(fecha_str, sizeof(fecha_str));

    int body_len = snprintf(
        body, sizeof(body),
        "<html><body><h1>Error %d</h1><p>%s</p></body></html>",
        codigo, mensaje
    );

    int header_len = snprintf(
        header, sizeof(header),
        "HTTP/1.0 %d %s\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %d\r\n"
        "Date: %s\r\n"
        "Server: JoaquinHTTPServer/1.0\r\n"
        "Connection: close\r\n"
        "\r\n",
        codigo, mensaje, body_len, fecha_str
    );

    printf("RESPUESTA header %s\n", header);
    printf("RESPUESTA BODY %s\n", body);

    enviar_todo(request->sockfd, header, header_len);
    enviar_todo(request->sockfd, body, body_len);
}