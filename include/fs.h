#ifndef FS_H
#define FS_H

#include <sys/stat.h>
#include <stddef.h>

typedef enum {
    FS_SUCCESS = 0,                    // OK
    FS_ERROR_NOT_FOUND = -1,          // 404 Not Found
    FS_ERROR_FORBIDDEN = -2,          // 403 Forbidden
    FS_ERROR_INTERNAL_SERVER = -3,    // 500 Internal Server Error
    FS_ERROR_NOT_IMPLEMENTED = -4     // 501 Not Implemented
} fs_result_t;

const char* obtener_tipo_mime(const char *path);

fs_result_t servir_archivo(int sockfd, char *path);
int validar_ruta(char *uri, char *document_root, char *path_completo, size_t path_len, struct stat *stat_archivo);

#endif