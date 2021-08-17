#include <stdio.h>
#include <stdlib.h>

int main()
{
    char nombre_archivo[50];
    char c;
    FILE* archivo_origen;
    FILE* archivo_destino;

    printf("Ingrese el nombre del archivo origen \n Nombre:  ");
    scanf("%s",nombre_archivo);
    archivo_origen = fopen(nombre_archivo,"r");
    fflush(stdin);
    if (!archivo_origen){
        printf("No se encontro archivo origen \n");
        return -1;
    }
    printf("Ingrese el nombre del archivo destino \n Nombre:  ");
    scanf("%s",nombre_archivo);
    archivo_destino = fopen(nombre_archivo,"w");
    fprintf(archivo_destino,"<table border=\"1\">\n<tr>\n<td>");
    c = getc(archivo_origen);

    while(c != EOF) {
        if (c == '\t') {
            fprintf(archivo_destino,"</td><td>");
            while (c == '\t' || c ==' '){
            c = getc(archivo_origen);
            }
        }
        if (c == '\n') {
            fprintf(archivo_destino,"</td>\n</tr>\n<tr>\n<td>");
            c = getc(archivo_origen);
        }
        fprintf(archivo_destino,"%c",c);
        c = getc(archivo_origen);
    }
    fprintf(archivo_destino,"</td>\n</tr>\n</table>");
    fclose(archivo_destino);
    fclose(archivo_origen);
    return 0;
}
