#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include "http_handler.h"
#include <sys/stat.h>
#include "utils.h" 

#define TAMANO_BUFFER 4096
#define DOCUMENT_ROOT "./public"

void cliente_handler(int sockfd){
    char buffer[TAMANO_BUFFER];
    int n;

  // variables para parsear la peticion
    char metodo_http[16];
    char uri[256];
    char version[16];
    char path_completo[512];
    struct stat stat_archivo;

    memset(buffer,0, TAMANO_BUFFER);

    //n = recv(sockfd, buffer, TAMANO_BUFFER -1, MSG_NOSIGNAL);
    n = recv_headers(sockfd,buffer, TAMANO_BUFFER -1);
    if (n <= 0) {
        enviar_error(sockfd, 400, "Bad Request");
        return;
    }

    printf("request recibida ---\n");

    if(!parsear_peticion(buffer, metodo_http, uri, version)){
        printf("fallo parsear peticion\n");
        enviar_error(sockfd,400,"Bad Request");
        return;
    }

    if (!es_request_valido(uri, version)) {
        printf("fallo es requiest valido\n");
        enviar_error(sockfd, 400, "Bad Request");
        return;
    }

    if (strcasecmp(metodo_http, "GET") != 0) {
        printf("no es GET\n");
        enviar_error(sockfd,501, "Not Implemented");
        return;
    }

    sprintf(path_completo, "%s%s", DOCUMENT_ROOT, uri); //arma la ruta donde el servidor deberia buscar 


    if(stat(path_completo, &stat_archivo) < 0){
        printf("archivo no encontrado en %s \n", path_completo);
        enviar_error(sockfd,404, "Not Found");
        return;
    }

    if (S_ISDIR(stat_archivo.st_mode)) {
        strncat(path_completo, "/index.html", sizeof(path_completo) - strlen(path_completo) - 1);
        if (stat(path_completo, &stat_archivo) < 0) {
            enviar_error(sockfd, 403, "Forbidden");
            return;
        }
    }
    servir_archivo(sockfd,path_completo);

/*     char body[1024];
    sprintf(body, 
        "<html><body>"
        "<h1>ok</h1>"
        "<p>uri de la peticion: <b>%s</b></p>"
        "<p>path_completo: <b>%s</b></p>"
        "</body></html>", 
        uri, path_completo);


    char response[2056];
    sprintf(response,"HTTP/1.0 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %ld\r\n"
        "\r\n"
        "%s", 
        strlen(body), body);
 */
/*     n = send(sockfd,response,strlen(response), 0);

     if(n < 0){
      perror("error en send");
    } */
}


