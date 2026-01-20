// src/http.c
#include "http.h"
#include "net.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

int parsear_peticion(char *buffer, char *metodo_http, char *uri, char* version) {
    int leidos = sscanf(buffer, "%15s %255s %15s", metodo_http, uri, version);
    return (leidos < 3) ? -1 : 0;
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

int es_version_valida(char *version_http, const char **versiones_validas, int num_versiones) {
    for (int i = 0; i < num_versiones; i++) {
        if (strcmp(version_http, versiones_validas[i]) == 0)
            return 1;
    }
    return 0;
}

void get_fecha(char *buf, size_t tamanio) {
    time_t now;
    struct tm *tm_info;
    
    time(&now);
    tm_info = gmtime(&now);
    strftime(buf, tamanio, "%a, %d %b %Y %H:%M:%S GMT", tm_info);
}

void enviar_error(int sockfd, int codigo, char *mensaje) {
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

    enviar_todo(sockfd, header, header_len);
    enviar_todo(sockfd, body, body_len);
}