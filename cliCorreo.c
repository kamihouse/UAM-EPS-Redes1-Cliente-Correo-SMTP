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

void correio(const char *remitente, const char *destino, const char *asunto, char *adjunto){
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[1000000];
    char buffer_aux[1000000];
    char hname[100];
    //puerto destino
    portno = PORT;
    //servidor de correo
    server = gethostbyname("127.0.0.1");
    if (server == NULL){
        fprintf(stderr, "ERROR, no such host\n");
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

    serv_addr.sin_port = htons(portno);

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    bzero(buffer,256);

    n = write(sockfd,buffer,strlen(buffer));

    if (n < 0)
         error("ERROR writing to socket");

    bzero(buffer,256);

    //codigo para enviar com hasta servidor
    char descripcion[255];
    int codigo;

    //Leer el OK del servir y despues envia HELO
    n= read(sockfd, buffer, sizeof(buffer));

    if(n < 0)
        error("Error: al recv");

    sscanf(buffer, "%d %s", &codigo, descripcion);

    if(codigo >= 400)
        error(descripcion);

    gethostname(hname, sizeof(hname));
    sprintf(buffer,"HELO %s\r\n", hname);
    write(sockfd, buffer, strlen(buffer));

    //envia el email del usuario
    n= read(sockfd, buffer, sizeof(buffer));

    if(n < 0)
        error("Error: al recv");

    sscanf(buffer, "%d %s", &codigo, descripcion);

    if(codigo >= 400)
        error(descripcion);

    sprintf(buffer,"MAIL FROM: <%s>\r\n", destino);
    write(sockfd, buffer, strlen(buffer));

    //envia el email del destinatario
    n = read(sockfd, buffer, sizeof(buffer));

    if(n < 0)
        error("Error: al recv");

    sscanf(buffer, "%d %s", &codigo, descripcion);

    if(codigo >= 400)
        error(descripcion);

    sprintf(buffer,"RCPT TO: <%s>\r\n", remitente);
    write(sockfd, buffer, strlen(buffer));

    //envia DATA
    n = read(sockfd, buffer, sizeof(buffer));

    if(n < 0)
        error("Error: al recv");

    sscanf(buffer, "%d %s", &codigo, descripcion);

    if(codigo >= 400)
        error(descripcion);

    sprintf(buffer,"DATA\r\n");
    write(sockfd, buffer, strlen(buffer));

    //envia la MENSAGE
    n = read(sockfd, buffer, sizeof(buffer));

    if(n<0)
        error("Error: al recv");

    sscanf(buffer, "%d %s", &codigo, descripcion);

    if(codigo >= 400)
        error(descripcion);

    //pedir la mensage
    char line[100];
    char msg[10000]={0};

    printf("\nEscriba el cuerpo del mensaje: \n");
    // sacar la mensang que el cliente va digitar
    while(fgets(line, 99, stdin) != NULL){
         strcat(msg, line);
    }
    //printf("\nasunto mod: %s\n", asunto);
    sprintf(buffer,"from: %s\r\nto:%s\r\nsubject: =?iso-8859-1?Q?%s?=\r\nMIME-Version: 1.0\r\nContent-Type: multipart/mixed;boundary=55445525221336464997999458455754\r\n\r\n--55445525221336464997999458455754\r\nContent-Type: text/plain; charset=ISO-8859-1;\r\nContent-Disposition: inline;\r\nContent-Transfer-Encoding: 8bit\r\n\r\n%s",remitente, destino, asunto, msg);
    if(adjunto != NULL){
        //sacar el tipo mime
        char *ext = (char *)malloc(sizeof(char) * 255);
        char *aux = (char *)malloc(sizeof(char) * 255);
        char *puntero_aux = (char *)malloc(sizeof(char) * 255);
        char *comando = (char *)malloc(sizeof(char) * 255);
        char *tipo_mime = (char *)malloc(sizeof(char) * 255);

        strcpy(aux,adjunto);
        puntero_aux = strtok(aux,".");//provoca una ruptura en el contenido de la string aux donde tienes punto

        while(puntero_aux != NULL){
           strcpy(ext,puntero_aux);
           puntero_aux = strtok(NULL,".");
        }

        sprintf(comando,"awk '{for(i=2;i<=NF;i++){if($i==\"%s\")print $1}}' mime.types",ext);

        FILE *archivo_mime;
        archivo_mime = popen(comando,"r");//executando en terminal el comando que está dentro de respuesta
        if(archivo_mime == NULL)
            error("ERROR ao abrir el archivo mime.types");

        fscanf(archivo_mime, "%s", tipo_mime);

        char *linea = (char *)malloc(sizeof(char) * 255);

        sprintf(buffer_aux,"--55445525221336464997999458455754\r\nContent-Type:%s;\r\nContent-Disposition:attachment;filename=\"%s\"\r\nContent-Transfer-Encoding: base64\r\n\r\n",tipo_mime,adjunto);
        strcat(buffer,buffer_aux);

        sprintf(buffer_aux,"uuencode -m %s %s", adjunto, adjunto);
        FILE *p = popen(buffer_aux, "r");
        if(p == NULL)
            error("Error ejecutando uuencode");

        fgets(linea, 1000, p);

        int ultimalinea=0;
        while(fgets(linea,1000,p) != NULL){
            strcat(buffer,linea);
            ultimalinea=strlen(linea);
        }

        buffer[strlen(buffer)-ultimalinea] = '\0';
        pclose(p);
    }

    strcat(buffer, "--55445525221336464997999458455754--\r\n.\r\n");

    write(sockfd, buffer, strlen(buffer));

    //envia QUIT
    n= read(sockfd, buffer, sizeof(buffer));

    if(n < 0)
        error("Error: al recv");

    sscanf(buffer, "%d %s", &codigo, descripcion);

    if(codigo >= 400)
        error(descripcion);

    sprintf(buffer,"QUIT\r\n");
    write(sockfd, buffer, strlen(buffer));

    //verificar la ultima mensage
    n= read(sockfd, buffer, sizeof(buffer));

    if(n < 0)
        error("Error: al recv");

    sscanf(buffer, "%d %s", &codigo, descripcion);

    if(codigo >= 400)
            error(descripcion);

    printf("\nMensage enviada!\n");
    close(sockfd);
}

int main(int argc, char *argv[]) {
    char sender[256];
    char *ordUser, *ordName, *lang;

    /*
     Deve - se verificar o seu SHELL para que ele exiba as variaveis de ambiente com SETENV
     */
    ordUser = getenv("LOGNAME");
    lang = getenv("LANG");
    ordName = getenv("COMMAND_MODE");
    strcpy(sender, ordUser);
    strcat(sender, "@");
    strcat(sender, ordName);


    if ((argc < 4) || (argc > 5)){
        printf("\n\tError: No obides del Remitente, Destino y Asunto\n"
                " \tEj: ./cliCorreo thiago@thiago.com thiago@thiago.com Assunto\n\n");
    } else {
        printf("- - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n"
            "-  # UAM - EPS - Redes 1 - Cliente Correo\t\t-\n"
            "-  Usuario: %s - HostN: %s \t\t\t-\n"
            "-  Remitente: %s \t\t\t\t-\n"
            "-  Lang: %s -> Cambiar: ISO-8859-1 \t\t-\n"
            "-  Introduzca abajo el Contenido del Correo\t\t-\n"
            "-  PUNTA: Acabe con un 'punto' y pulsa Enter\t\t-\n"
            "- - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n", ordUser, ordName, sender, lang);

        int i;
        char *aux = (char *)malloc(sizeof(char) * 255);
        char *aux2 = (char *)malloc(sizeof(char) * 255);
        char *asunto = (char *)malloc(sizeof(char) * 255);
        strcpy(aux,argv[3]);

        for(i = 0; i <= strlen(argv[3]); i++) {
            if ((unsigned char)aux[i] < 128) {
                sprintf(aux2, "%c", (unsigned char)aux[i]);
                strcat(asunto, aux2);
            }
            if ((unsigned char)aux[i] >= 128){
                sprintf(aux2,"=%02X",(unsigned char)aux[i]);
                strcat(asunto,aux2);
            }
        }
        if(argc == 4)
            correio(argv[1], argv[2], asunto, NULL);
	else
	    correio(argv[1], argv[2], asunto, argv[4]);
    }
    return 0;
}
