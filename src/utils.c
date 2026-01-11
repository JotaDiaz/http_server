#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "utils.h"
#include <sys/stat.h>

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


const char* obtener_tipo_mime(const char *path) {
    const char *extension = strrchr(path, '.');

    if (!extension) return "application/octet-stream";

    if (strcasecmp(extension, ".html") == 0 || strcasecmp(extension, ".htm") == 0) return "text/html";
    if (strcasecmp(extension, ".txt") == 0) return "text/plain";
    if (strcasecmp(extension, ".css") == 0) return "text/css";
    if (strcasecmp(extension, ".js") == 0)  return "application/javascript";
    if (strcasecmp(extension, ".jpg") == 0 || strcasecmp(extension, ".jpeg") == 0) return "image/jpeg";
    if (strcasecmp(extension, ".png") == 0) return "image/png";
    if (strcasecmp(extension, ".gif") == 0) return "image/gif";
    if (strcasecmp(extension, ".pdf") == 0) return "application/pdf";

    return "application/octet-stream"; 
}

void servir_archivo(int sockfd, char *path) {
    FILE *archivo = fopen(path, "rb");
    
    if (!archivo) {
        printf("error en fopen de servir_archivo \n");
        enviar_error(sockfd, 404, "Not Found "); 
        return;
    }

    struct stat sbuf;
    stat(path, &sbuf);
    long filesize = sbuf.st_size;

    const char *mime_type = obtener_tipo_mime(path);

    char headers[1024];
    snprintf(headers, sizeof(headers),
            "HTTP/1.0 200 OK\r\n"
            "Content-Type: %s\r\n"
            "Content-Length: %ld\r\n"
            "Connection: close\r\n"
            "\r\n", 
            mime_type, filesize);

    send(sockfd, headers, strlen(headers), 0); //primero va el header


    //el body va en paquetes de 4kb
    unsigned char file_buffer[4096]; 
    size_t bytes_leidos;

    while ((bytes_leidos = fread(file_buffer, 1, sizeof(file_buffer), archivo)) > 0) {
        if (send(sockfd, file_buffer, bytes_leidos, 0) < 0) {
            perror("Error enviando archivo");
            break;
        }
    }

    fclose(archivo);
    printf("archivo servido: %s (%ld bytes, %s)\n", path, filesize, mime_type);
}