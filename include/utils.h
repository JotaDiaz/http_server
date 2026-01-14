#ifndef UTILS_H
#define UTILS_H

void enviar_error(int sockfd, int codigo, char *mensaje);
int es_request_valido(char *uri, char *version);
int parsear_peticion(char *buffer, char *metodo_http, char *uri, char* version);
const char* obtener_tipo_mime(const char *path);
void servir_archivo(int sockfd, char *path); 
ssize_t enviar_todo(int sockfd, const void *buf, size_t len);
ssize_t recv_headers(int sockfd, char *buffer, size_t max);
int es_version_valida(char *version_http, const char **versiones_validas, int num_versiones);
void get_fecha(char *buf, size_t tamanio);

#endif
