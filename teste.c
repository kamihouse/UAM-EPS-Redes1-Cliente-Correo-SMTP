#include <stdio.h>
#include <stdlib.h>

int main(){
    system("whoami");
    char usuario[100];
    char *usuario1;

    printf("\nTestes:\n");
    //sprintf(usuario, "%d", system("whoami"));
    printf("-> %s \n", usuario);
    usuario1 = getenv("LOGNAME");
    printf("--> %s \n", usuario1);
    return 0;
}
