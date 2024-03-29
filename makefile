# ****************************************************************************
# UAM - EPS - Ingeniería de Tecnologías y Servicios de Telecomunicación
# Arquitectura de Redes I - Practica.
# Profesores.:	Pedro María Santiago del Río (pedro.santiago@uam.es)
#      	Javier Ramos (javier.ramos@uam.es)
#
# Proyecto...: Redes-3213-Practica2B
# File.......: makefile
# Link Git...: https://github.com/kamihouse/Cliente-Correo-SMTP
#
# Author.....: Thiago Pereira Rosa	(thiago.pereirarosa@estudiante.uam.es)
#              Isaac Gonzalez Gonzalez	(isaac.gonzalez@estudiante.uam.es)
#
# Implementación de un cliente sencillo de correo.
# Created on 20 de Novembro de 2012, 21:27
# ************************************************************************** */

# banderas de compilacion (PUEDE SER UTIL CAMBIARLAS)
CC = gcc
#ifdef ENTREGA
CFLAGS = -Wall -g -I .
LDLIBS = -lm
#else
CFLAGS = -Wall -g -I .
#-DENTREGA
LDLIBS = -lm
#endif

# fuentes a considerar (si no se cambia, todos los '.c' del directorio actual)
SOURCES = $(shell ls -1 *.c* | xargs)

# busca los ejecutables (todos los .c con metodo tipo 'int main')
EXEC_SOURCES = $(shell grep -l "^int main" $(SOURCES) | xargs)

# fich. de dependencia (.d) y nombres de ejecutables (los anteriores, sin '.c')
EXECS = $(shell echo $(EXEC_SOURCES) | sed -e 's:\.c[p]*::g')
DEPS = $(shell echo $(SOURCES) | sed -e 's:\.c[p]*:\.d:g')

all:	cliCorreo

# las dependencias (CAMBIA PARA CADA PRACTICA)
cliCorreo: cliCorreo.o

# receta para hacer un .d (dependencias automaticas de tipo .h para tus .o)
%.d : %.c
	@set -e; $(CC) -MM $(CFLAGS) $< \
	| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
	[ -s $@ ] || rm -f $@

#incluye las dependencias generadas
-include $(DEPS)

# receta para hacer un .o
%.o :	%.c
	@echo -n compilando objeto \'$<\'...
	@$(CC) $(CFLAGS) $< -c
	@echo [OK]

# receta para hacer un ejecutable (suponiendo resto de dependencias OK)
% :	%.o
	@echo -n compilando ejecutable \'$@\'...
	@$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)
	@echo [OK]

# limpieza
clean:
	@rm -f $(wildcard *.o *.d core* *.P) $(EXECS)

# ayuda (nunca viene mal)
help:
	@echo "Use: make <target> ..."
	@echo "Valid targets:"
	@$(MAKE) --print-data-base --question | sed -e "s:makefile::g" |\
	awk '/^[^.%][-A-Za-z0-9_]*:/	\
		{ print "   " substr($$1, 1, length($$1)-1) }'