#ifndef HTTP_H
#define HTTP_H

#include <sys/types.h>


typedef struct {
    int sockfd;
    char metodo[16];
    char uri[256];
    char version[16];
    char body[1024]; 
} http_request_t;



/* int parsear_peticion(char *buffer, char *metodo_http, char *uri, char* version);

 */

int parsear_peticion(char *buffer, http_request_t *request);
 //int es_request_valido(char *uri, char *version);
 int es_request_valido(http_request_t *request);
//int es_version_valida(char *version_http, const char **versiones_validas, int num_versiones);
void enviar_error(http_request_t *request, int codigo, char *mensaje);

void get_fecha(char *buf, size_t tamanio);

#endif