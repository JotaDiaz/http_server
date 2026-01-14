#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "utils.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#define MAX_REQUEST 8192


void enviar_error(int sockfd, int codigo, char *mensaje)
{
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
        codigo,mensaje,body_len,fecha_str
    );

    printf("RESPUESTA header %s\n",header);
    printf("RESPUESTA BODY %s\n",body);

    enviar_todo(sockfd, header, header_len);
    enviar_todo(sockfd, body, body_len);
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

int es_version_valida(char *version_http, const char **versiones_validas, int num_versiones){
    for (int i = 0; i < num_versiones; i++) {
        if (strcmp(version_http, versiones_validas[i]) == 0)
            return 1;
    }
    return 0;
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


ssize_t enviar_todo(int sockfd, const void *buf, size_t len) {
    size_t total = 0;
    const char *p = buf;

    while (total < len) {
        ssize_t enviados = send(sockfd, p + total, len - total, 0);

        if (enviados <= 0) {
            return -1; 
        }

        total += enviados;
    }

    return total;
}

void get_fecha(char *buf, size_t tamanio){
    // formato RFC 1123
    time_t now;
    struct tm *tm_info;

    time(&now);
    tm_info = gmtime(&now);
    strftime(buf, tamanio, "%a, %d %b %Y %H:%M:%S GMT", tm_info);
}

void servir_archivo(int sockfd, char *path) {
    char fecha_str[64];
    get_fecha(fecha_str, sizeof(fecha_str));
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
        "Date: %s\r\n"              
        "Server: JoaquinHTTPServer/1.0\r\n"
        "\r\n", 
        mime_type, filesize, fecha_str);

    //send(sockfd, headers, strlen(headers), 0); //primero va el header
    if(enviar_todo(sockfd,headers,strlen(headers)) < 0)
        enviar_error(sockfd,500, "Internal Server Error"); 


    //el body va en paquetes de 4kb
    unsigned char file_buffer[4096]; 
    size_t bytes_leidos;

    while ((bytes_leidos = fread(file_buffer, 1, sizeof(file_buffer), archivo)) > 0) {

        if(enviar_todo(sockfd, file_buffer, bytes_leidos) < 0){
        //if (send(sockfd, file_buffer, bytes_leidos, 0) < 0) {
            perror("Error enviando archivo");
            break;
        }
    }

    fclose(archivo);
    printf("archivo servido: %s (%ld bytes, %s)\n", path, filesize, mime_type);
}



ssize_t recv_headers(int sockfd, char *buffer, size_t max) {
    size_t total = 0;

    while (total < max - 1) {
        printf("recv_header\n");
        ssize_t n = recv(sockfd, buffer + total, max - 1 - total, 0);

        if (n < 0)
            return -1;      

        if (n == 0)
            break;          

        total += n;
        buffer[total] = '\0';

        if (strstr(buffer, "\r\n\r\n"))
            return total;   
    }

    return -1; 
}
