#ifndef HTTP_H
#define HTTP_H

#include <sys/types.h>

int parsear_peticion(char *buffer, char *metodo_http, char *uri, char* version);
int es_request_valido(char *uri, char *version);
int es_version_valida(char *version_http, const char **versiones_validas, int num_versiones);
void enviar_error(int sockfd, int codigo, char *mensaje);
void get_fecha(char *buf, size_t tamanio);

#endif