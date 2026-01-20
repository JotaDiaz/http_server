#include "metodos.h"
#include "fs.h"
#include "http.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

void handle_GET(int sockfd, char *uri, char *document_root) {
    char path_completo[512];
    struct stat stat_archivo;

    if (validar_ruta(sockfd, uri, document_root, path_completo, sizeof(path_completo), &stat_archivo) < 0) {
        return;
    }

    if (S_ISDIR(stat_archivo.st_mode)) {
        strncat(path_completo, "/index.html", sizeof(path_completo) - strlen(path_completo) - 1);
        if (stat(path_completo, &stat_archivo) < 0) {
            enviar_error(sockfd, 403, "Forbidden");
            return;
        }
    }

    servir_archivo(sockfd, path_completo);
}