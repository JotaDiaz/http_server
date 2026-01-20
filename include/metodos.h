#ifndef METODOS_H
#define METODOS_H

#include "http.h"

void handle_request(http_request_t *request, char *document_root, int enviar_body);
void handle_GET(http_request_t *request, char *document_root);
void handle_HEAD(http_request_t *request, char *document_root);

#endif