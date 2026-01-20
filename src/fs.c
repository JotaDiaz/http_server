// src/fs.c
#include "fs.h"
#include "http.h"
#include "net.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

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

int validar_ruta(char *uri, char *document_root, char *path_completo, size_t path_len, struct stat *stat_archivo) {

    char path_sanitizado[PATH_MAX];
    char real_doc_root[PATH_MAX];
    
    
    snprintf(path_completo, path_len, "%s%s", document_root, uri);

    if (!realpath(path_completo, path_sanitizado)) {
        return FS_ERROR_NOT_FOUND;
    }

    if (!realpath(document_root, real_doc_root)) {
        return FS_ERROR_INTERNAL_SERVER;
    }

    if (strncmp(path_sanitizado, real_doc_root, strlen(real_doc_root)) != 0) {
        return FS_ERROR_FORBIDDEN;
    }

    if (stat(path_sanitizado, stat_archivo) < 0) {
        return FS_ERROR_NOT_FOUND;
    }

    strncpy(path_completo, path_sanitizado, path_len - 1);
    path_completo[path_len - 1] = '\0';
    
    return FS_SUCCESS;
}

fs_result_t servir_archivo(int sockfd, char *path, int enviar_cuerpo) {
    char fecha_str[64];
    get_fecha(fecha_str, sizeof(fecha_str));
    FILE *archivo = fopen(path, "rb");
    
    if (!archivo) {
        printf("error en fopen de servir_archivo \n");
        //enviar_error(sockfd, 404, "Not Found"); //TODO: SACAR DE ACA
        return FS_ERROR_NOT_FOUND;
    }

    struct stat sbuf;
    if (stat(path, &sbuf) < 0) {
        fclose(archivo);
        return FS_ERROR_NOT_FOUND;
    }    
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

    if(enviar_todo(sockfd, headers, strlen(headers)) < 0) {
        //enviar_error(sockfd, 500, "Internal Server Error"); //TODO: SACAR DE ACA
        fclose(archivo);
        return FS_ERROR_INTERNAL_SERVER;
    }

    printf("RESPUESTA header:\n%s\n", headers);

    if(!enviar_cuerpo){
        fclose(archivo);
        return FS_SUCCESS;
    }
    
    unsigned char file_buffer[4096]; 
    size_t bytes_leidos;

    while ((bytes_leidos = fread(file_buffer, 1, sizeof(file_buffer), archivo)) > 0) {
        if(enviar_todo(sockfd, file_buffer, bytes_leidos) < 0) {
            perror("Error enviando archivo");
            break;
        }
    }

    fclose(archivo);
    printf("archivo servido: %s (%ld bytes, %s)\n", path, filesize, mime_type);

    return FS_SUCCESS;
}