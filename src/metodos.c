#include "http.h"
#include "metodos.h"
#include "fs.h"
#include "http.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

void handle_request(http_request_t *request, char *document_root, int enviar_body) {
    char path_completo[512];
    struct stat stat_archivo;

    fs_result_t resultado = validar_ruta(request->uri, document_root, path_completo, sizeof(path_completo), &stat_archivo);


    if (resultado < 0) {
        if (resultado == -1) enviar_error(request, 404, "Not Found");
        else if (resultado == -2) enviar_error(request, 500, "Internal Server Error");
        else if (resultado == -3) enviar_error(request, 403, "Forbidden");
        return;
    }
    
    if (S_ISDIR(stat_archivo.st_mode)) {
        strncat(path_completo, "/index.html", sizeof(path_completo) - strlen(path_completo) - 1);
        if (stat(path_completo, &stat_archivo) < 0) {
            enviar_error(request, 403, "Forbidden");
            return;
        }
    }
    if (servir_archivo(request->sockfd, path_completo, enviar_body) != FS_SUCCESS) {
        printf("Aviso: No se pudo completar el envío de %s\n", path_completo);
    }

}


void handle_GET(http_request_t *request, char *document_root) {
        handle_request(request, document_root, 1); 
    }

void handle_HEAD(http_request_t *request, char *document_root) {
    handle_request(request, document_root, 0); 
}



