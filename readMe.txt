/* ****************************************************************************
 * UAM - EPS - Ingeniería de Tecnologías y Servicios de Telecomunicación
 * Arquitectura de Redes I - Practica.
 * Profesores.:	Pedro María Santiago del Río (pedro.santiago@uam.es)
 *      	Javier Ramos (javier.ramos@uam.es)
 *
 * Proyecto...: Redes-3213-Practica2B
 * File.......: readMe.txt
 * Link Git...: https://github.com/kamihouse/Cliente-Correo-SMTP
 *
 * Author.....: Thiago Pereira Rosa	(thiago.pereirarosa@estudiante.uam.es)
 *              Isaac Gonzalez Gonzalez	(isaac.gonzalez@estudiante.uam.es)
 *
 * Implementación de un cliente sencillo de correo usando SMTP y Telnet.
 * Created on 20 de Novembro de 2012, 21:27
 *************************************************************************** */

1) Archivos:

    - uuencode
    - mime.types
    - cliCorreo.c
    - makefile
    - readMe.txt

2) Cómo Instalar:

    - Copie todos los archivos en un directorio de su elección.
    - Ejecute el comando 'make' en el terminal.
    - Espere a compilar todos los archivos.

1) Cómo Ejecutar:

    - Ejecutar el cliente con el comando './cliCorreo' con 1º, 2º y 3º Argumentos.
        - 1º Argumento: Remitente, Ej -> usuario@localhost
        - 2º Argumento: Destinatario, Ej -> usuario@localhost
        - 2º Argumento: Asunto

        - Informe el Nombre del Adjunto
            - El archivo deve estar el no mismo fichero de la aplicacion.

        - Informe el contenido del mensaje
            - Para finalizar la mensaje informa un punto (.) y pulsa enter
