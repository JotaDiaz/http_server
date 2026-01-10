#ifndef UTILS_H
#define UTILS_H

void enviar_error(int sockfd, int codigo, char *mensaje);
int es_request_valido(char *uri, char *version);
int parsear_peticion(char *buffer, char *metodo_http, char *uri, char* version);

#endif
