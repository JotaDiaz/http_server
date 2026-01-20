#ifndef FS_H
#define FS_H

#include <sys/stat.h>
#include <stddef.h>
const char* obtener_tipo_mime(const char *path);

int validar_ruta_safe(char *uri, char *document_root, 
                      char *path_completo, size_t path_len, 
                      struct stat *stat_archivo,
                      char *path_sanitizado_buf,      
                      char *real_doc_root_buf);        

int validar_ruta(int sockfd, char *uri, char *document_root, 
                 char *path_completo, size_t path_len, 
                 struct stat *stat_archivo);

void servir_archivo(int sockfd, char *path);

#endif