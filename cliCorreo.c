/* ****************************************************************************
 * UAM - EPS - Ingeniería de Tecnologías y Servicios de Telecomunicación
 * Arquitectura de Redes I - Practica.
 * Profesores.:	Pedro María Santiago del Río (pedro.santiago@uam.es)
 *      	Javier Ramos (javier.ramos@uam.es)
 *
 * Proyecto...: Redes-3213-Practica2B
 * File.......: cliCorreo.c
 * Link Git...: https://github.com/kamihouse/Cliente-Correo-SMTP
 *
 * Author.....: Thiago Pereira Rosa	(thiago.pereirarosa@estudiante.uam.es)
 *              Isaac Gonzalez Gonzalez	(isaac.gonzalez@estudiante.uam.es)
 *
 * Implementación de un cliente sencillo de correo.
 * Created on 20 de Novembro de 2012, 21:27
 *************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT 25

void error(const char *msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]) {
    int sockfd, n, codigo;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char sender[256], data[1024], data1[256];
    char buffer[256], entrada[256], descripcion[256];
    char *ordUser, *ordName;

    if ((argv[1]) == NULL){
        error("--> Error 1:\n--> Falta el argumento nº 1 - (Destinatario)\n");
    }

    if ((argv[2]) == NULL){
        error("--> Error 2:\n--> Falta el argumento nº 2 - (Asunto)\n");
    }

    data[0] = '\0';

    // Ordenador usuario y host, Pode ser USER
    ordUser = getenv("LOGNAME");
    ordName = getenv("HOST");
    strcpy(sender, ordUser);
    strcat(sender, "@");
    strcat(sender, ordName);

    printf("- - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n"
            "-  # UAM - EPS - Redes 1 - Cliente Correo\t\t-\n"
            "-  Usuario: %s - HostN: %s\t-\n"
            "-  Remitente: %s\t\t-\n"
            "-  Introduzca abajo el Contenido del Correo\t\t-\n"
            "-  PUNTA: Acabe con un 'punto' y pulsa Enter\t\t-\n"
            "- - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n", ordUser, ordName, sender);

    //Leemos el contenido del mensaje
    while (strcmp(data1, ".\n") != 0){
        fgets(data1, 256, stdin);
        strcat(data, data1);
    }

    server = gethostbyname("127.0.0.1");

    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy((char *)&serv_addr.sin_addr.s_addr,
            (char *)server->h_addr,
            server->h_length);
    serv_addr.sin_port = htons(PORT);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    bzero(buffer, 256);

    n = read(sockfd, entrada, sizeof(entrada));
    sscanf(entrada, "%d %s", &codigo, descripcion);
    if (codigo >= 400){
	error(descripcion);
    }

    // Enviando HELO
    sprintf(buffer,"HELO %s\n", server->h_name);
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0)
	error("Error: HELO\n");
    n = read(sockfd, entrada, sizeof(entrada));
    if (n < 0)
	error("Error:  HELO\n");
    sscanf(entrada,"%d %s", &codigo, descripcion);
    if (codigo >= 400){
	error(descripcion);
    }

    // Enviando MAIL FROM
    sprintf(buffer,"MAIL FROM: <%s>\n", sender);
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0)
	error("Error: MAIL FROM\n");
    n = read(sockfd, entrada, sizeof(entrada));
    if (n < 0)
	error("Error: MAIL FROM\n");
    sscanf(entrada,"%d %s", &codigo, descripcion);
    if (codigo >= 400){
	error(descripcion);
    }

    // Enviando RCPT TO
    sprintf(buffer,"RCPT TO: <%s>\n", argv[1]);
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0)
	error("Error: RCPT TO\n");
    n = read(sockfd, entrada, sizeof(entrada));
    if (n < 0)
	error("Error: RCPT TO\n");
    sscanf(entrada,"%d %s", &codigo, descripcion);
    if (codigo >= 400){
	error(descripcion);
    }

    // Enviando DATA
    sprintf(buffer,"DATA\r\n");
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0)
	error("Error: DATA\n");
    n = read(sockfd, entrada, sizeof(entrada));
    if (n < 0)
	error("Error: Data\n");
    sscanf(entrada,"%d %s", &codigo, descripcion);
    if (codigo >= 400){
	error(descripcion);
    }

    // Enviando FROM, TO, SUBJECT, DATA
    sprintf(buffer,"from: %s\r\nto: %s\r\nsubject: %s\r\n%s", sender, argv[1], argv[2], data);
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0)
	error("Error\n");

    sprintf(buffer,"\r\n.\r\n");
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0)
	error("Error: DATA 2\n");
    n = read(sockfd, entrada, sizeof(entrada));
    if (n < 0)
	error("Error: Data 2\n");
    sscanf(entrada,"%d %s", &codigo, descripcion);
    if (codigo >= 400){
	error(descripcion);
    }

    // Enviando QUIT
    sprintf(buffer,"QUIT\r\n");
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0)
	error("Error: DATA 2\n");
    n = read(sockfd, entrada, sizeof(entrada));
    if (n < 0)
	error("Error: Data 2\n");
    sscanf(entrada,"%d %s", &codigo, descripcion);
    if (codigo >= 400){
	error(descripcion);
    }

    close(sockfd);
    return 0;
}
