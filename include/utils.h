#ifndef UTILS_H
#define UTILS_H
#include <sys/stat.h>
#include <stdlib.h>   

const char* obtener_tipo_mime(const char *path);
int es_request_valido(char *uri, char *version);
int parsear_peticion(char *buffer, char *metodo_http, char *uri, char* version);
int es_version_valida(char *version_http, const char **versiones_validas, int num_versiones);
int validar_ruta(int sockfd, char *uri, char *document_root, char *path_completo, size_t path_len, struct stat *stat_archivo );
ssize_t enviar_todo(int sockfd, const void *buf, size_t len);
ssize_t recv_headers(int sockfd, char *buffer, size_t max);
void enviar_error(int sockfd, int codigo, char *mensaje);
void servir_archivo(int sockfd, char *path); 
void get_fecha(char *buf, size_t tamanio);

#endif
