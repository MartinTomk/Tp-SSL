#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*BOOL*/
#define TRUE 1
#define FALSE 0

/*Errors*/
#define OK 0
#define ERROR_ARCHIVE -1
#define ERROR_NULL_PTR -1

/*<=========================HTML========================>*/

#define NOMBRE_HTML_DESTINO "Listado.html"

    /*Identificadores Columnas, Filas, Tablas */


#define NOT_TAG 99

#define COL_TAG 30
#define COL_TAG_END 31

#define ROW_TAG 32
#define ROW_TAG_END 33
#define ROW_READ 20

#define TABLE_TAG 10
#define TABLE_TAG_END 11
#define TABLE_HEADER 110
#define NOT_TABLE 100

#define COLOR_ROW 120
#define NOT_COLOR_ROW 121

    /*Nombre de los tags*/
#define COL_TAG_HTML_END "</td>"
#define COL_TAG_HTML "<td>"
#define ROW_TAG_HTML "<tr>"
#define ROW_TAG_HTML_END "</tr>"
#define TABLE_TAG_HTML "<table>"
#define TABLE_TAG_HTML_END "</table>"
#define TABLE_TAG_BORDER_HTML "<table border=\"1\">"
#define HEADER_ROW_HTML "<tr>\n<td>"
#define HEADER_ROW_COLOR_HTML "<tr style=\"background-color:#ff0000\">\n<td>"
#define FOOTER_ROW_HTML "</td>\n</tr>\n"
#define SEPARADOR_HTML "</td><td>"
    /*Colors*/
#define RED 256
/*<=========================CSV========================>*/
#define NOMBRE_CSV_DESTINO "Listado.csv"

#define SEPARADOR_CSV ";"

#define CANTIDAD_CAMPOS_CSV 6 /*Cantidad de campos que se requieren*/
#define CANTIDAD_CAMPOS_HTML 16
#define CANTIDAD_CAMPOS_SCREEN 16

/* Longitud Vectores*/
#define LENGHT_CAMPO 50
#define LENGHT_TABLE_V 5
#define LENGHT_STYLE 50
#define MAX_LENGHT 20
/*Campos */
#define CANTIDAD_CAMPOS 40
#define PRIMER_CAMPO "Especie"
#define CANTIDAD_NUMEROS 13

/* Formato MENU/OPCIONES */
#define SEPARADOR_SCREEN " ; "
#define OLD_SEPARATOR ','
#define NEW_SEPARATOR '.'
#define OPT_SEPARATOR ')'
#define MSG_OPT_SEPARATOR ':'

/*Parametros*/
#define MAX_INTENTOS 3
#define VAR_MIN 0.5

/*Variacion*/
#define VAR_TRUE 1
#define VAR_FALSE 0
/*Posiciones SCREEEN*/
#define VAR_POS 7 /*Posicion de VAR en dicc_encabezado_screen*/


/*Reemplazos*/
#define NOT_FOUND -1
#define REPLACE_OK 0

/*Posiciones HTML*/
#define POS_VENTA 3 /*Estas pociones son realativas al vector dicc_encabezado_html */
#define POS_COMPRA 4
#define POS_APERTURA 8

/*Options define*/

#define OPT_LIST_VAR_SCREEN '1'
#define OPT_LIST_CSV '2'
#define OPT_LIST_HTML '3'
#define OPT_LIST_EXIT '4'
#define OPT_YES '1'
#define OPT_NO '2'

/*USER MESSAGGES*/
    /*MENU*/
#define MSG_LIST_VAR_SCREEN "Listar en pantalla las acciones cuya variacion fue mayor a "
#define MSG_LIST_CSV "Exportar listado en un archivo CSV"
#define MSG_LIST_HTML "Exportar listado en un archivo HTML resaltando con color las filas cuya apertura es mayor al precio compra/venta"
#define MSG_LIST_EXIT "Salir del programa"
    /*MENU REALIZAR OTRA ACCION*/
#define MSG_OPT_YES "Si"
#define MSG_OPT_NO "Cualquier otra tecla cerrara el programa"
    /*Informacion al usuario*/
#define MSG_FILE_EXPORT_SUCCESS "El archivo ha sido creado exitosamente"
#define MSG_OPT "Opcion"

    /*Error*/
#define MSG_OPT_ERROR "La opcion ingresada es incorrecta"
#define MSG_ERROR_INTENTOS "Ha excedido la cantidad maxima de intentos, ejecute nuevamente"

/*Aqui se puede escribir que campos mostrar editando la variable y su MACRO correspondiente*/
char dicc_encabezado_csv[CANTIDAD_CAMPOS_CSV][LENGHT_CAMPO] = {"Especie","Precio Compra","Precio Venta","Apertura","Maximo","Minimo"};
char dicc_encabezado_html[CANTIDAD_CAMPOS_HTML][LENGHT_CAMPO] = {"Especie","Vto.","Cant. Nominal","Precio Compra","Precio Venta","Cant. Nominal","Ultimo","Variacion %","Apertura","Maximo","Minimo","Cierre Ant.","Vol. Nominal","Monto Operado ($)","Cant. Ope.","Hora Cotizacion"};
char dicc_encabezado_screen[CANTIDAD_CAMPOS_SCREEN][LENGHT_CAMPO] = {"Especie","Vto.","Cant. Nominal","Precio Compra","Precio Venta","Cant. Nominal","Ultimo","Variacion %","Apertura","Maximo","Minimo","Cierre Ant.","Vol. Nominal","Monto Operado ($)","Cant. Ope.","Hora Cotizacion"};

typedef enum state_t {
        INIT = 0,
        MAIN_MENU = 1,
        LIST_VAR_SCREEN = 2,
        LIST_CV_CSV = 3,
        LIST_CV_HTML = 4,
        EXIT = 5
}state_t;

/*Funciones para el WGET*/
FILE *popen(const char *command,const char *mode);
int pclose(FILE *stream);

/*Reconocer HTML ORIGEN*/
int reconocer_fila_columna (FILE* archivo_origen);
int reconocer_tabla (FILE* archivo_origen);

/*Lectura Escritura*/
int leer_campo(FILE* archivo_origen,char* campo);
int leer_fila(FILE* archivo_origen,char campos_vector [] [LENGHT_CAMPO],int cantidad_campos,int* campos_leidos);
int imprimir_campos_pedidos (char campos[][LENGHT_CAMPO],int cantidad_campos,char* separador,int posiciones_pedidas[],int cantidad_posiciones,FILE* archivo_destino);

/*Verificar que lo listado cumpla la condicion*/
int verificar_variacion(char campos[][LENGHT_CAMPO],int cantidad_campos,float variacion_min,int posicion_variacion);
int reconocer_col_pedidas (char dicc_encabezado[][LENGHT_CAMPO],int cant_col_encabezado,int posiciones_col[],char campos[][LENGHT_CAMPO],int cantidad_campos);
int verificar_compraventa_apertura(char campos[][LENGHT_CAMPO],int cantidad_campos,int posiciones_col[CANTIDAD_CAMPOS_CSV]);

/*Formato*/
int reemplazar_caracter(char* str,char caracter,char nuevo_caracter);
char corregir_acentos (char c);

/*Funciones Principales*/

int crear_tabla_datos (FILE* archivo_origen,FILE* archivo_destino,char dicc_encabezado[][LENGHT_CAMPO],int cantidad_campos,int (*funcion)(int,int,char[][LENGHT_CAMPO],int,char[][LENGHT_CAMPO],int[],FILE*));
/*Opciones*/
state_t realizar_otra_accion(void);
state_t ingresar_opcion (void);

/*Dibujar Menus*/
void dibujar_menu(void);

/*Funciones que guardan los datos en un archivo*/
int listar_pantalla(int div_aux,int campos_leidos,char campos[][LENGHT_CAMPO],int cantidad_campos,char encabezado_screen[][LENGHT_CAMPO],int posiciones_col[],FILE* archivo_destino);
int exportar_csv(int div_aux,int campos_leidos,char campos[][LENGHT_CAMPO],int cantidad_campos,char encabezado_screen[][LENGHT_CAMPO],int posiciones_col[],FILE* archivo_destino);
int exportar_html(int div_aux,int campos_leidos,char campos[][LENGHT_CAMPO],int cantidad_campos,char encabezado_html[][LENGHT_CAMPO],int posiciones_col[],FILE* archivo_destino);


int main()
{
    state_t status;
    FILE* pagina;
    FILE* archivo_destino;

    status = MAIN_MENU;
    while (status != EXIT) {
        if (status == MAIN_MENU) {
            dibujar_menu();
            status = ingresar_opcion();
        }
        if (status == LIST_VAR_SCREEN) {
            pagina = popen("wget -q -O - https://52.67.80.139/test/lideres-bcba_limpio.html --no-check-certificate","r");
            crear_tabla_datos(pagina,stdout,dicc_encabezado_screen,CANTIDAD_CAMPOS_SCREEN,listar_pantalla);
            status = realizar_otra_accion();
            pclose(pagina);

        }
        if (status == LIST_CV_CSV) {
            pagina = popen("wget -q -O - https://52.67.80.139/test/lideres-bcba_limpio.html --no-check-certificate","r");
            archivo_destino = fopen(NOMBRE_CSV_DESTINO,"w");
            crear_tabla_datos(pagina,archivo_destino,dicc_encabezado_csv,CANTIDAD_CAMPOS_CSV,exportar_csv);
            printf("%s \n",MSG_FILE_EXPORT_SUCCESS);
            status = realizar_otra_accion();
            fclose(archivo_destino);
            pclose(pagina);
        }
        if (status == LIST_CV_HTML) {
            pagina = popen("wget -q -O - https://52.67.80.139/test/lideres-bcba_limpio.html --no-check-certificate","r");
            archivo_destino = fopen(NOMBRE_HTML_DESTINO,"w");
            fprintf(archivo_destino,"%s \n",TABLE_TAG_BORDER_HTML);
            crear_tabla_datos(pagina,archivo_destino,dicc_encabezado_html,CANTIDAD_CAMPOS_HTML,exportar_html);
            fprintf(archivo_destino,"%s \n",TABLE_TAG_HTML_END);
            printf("%s \n",MSG_FILE_EXPORT_SUCCESS);
            status = realizar_otra_accion();
            fclose(archivo_destino);
            pclose(pagina);
        }
    }
    return 0;

}
/* FUNCIONES DE RECONOCIMIENTO DE LA PAGINA */

int reconocer_fila_columna (FILE* archivo_origen) {
    char c;
    int id_end;
    c = getc(archivo_origen);
    if (c == '\n'){
        c = getc(archivo_origen);
    }
    if (c != '<') {
            if (c == EOF) {
                return EOF;
            }
            else return NOT_TAG;
    }
    c = getc(archivo_origen);
    if (c != '/' && c != 't'){
            if (c == EOF) {
                return EOF;
            }
            else return NOT_TAG;
    }
    if (c == '/') {
        c = getc(archivo_origen);
        id_end = 1; /*Id identifica si es un tag de apertura o cierre */
    }
    else id_end = 0;
    if (c != 't') {
            if (c == EOF) {
                return EOF;
            }
            else return NOT_TAG;
    }
    c = getc(archivo_origen);
    if (c == 'r') {
        c = getc(archivo_origen);
        if ( c == '>') {
            return ROW_TAG + id_end;
        }
        else  {
            if (c == EOF) {
                return EOF;
            }
            else return NOT_TAG; /*No se consideran tablas del tipo <td class="clasetabla">*/
        }
    }
    if (c == 'd') {
        c = getc(archivo_origen);
        if (c == '>') {
            return COL_TAG + id_end;
        }
        else  {
            if (c == EOF) {
                return EOF;
            }
            else return NOT_TAG; /*No se consideran tablas del tipo <td class="clasetabla">*/
        }
    }
    if (c == EOF) {
            return EOF;
    }
    else return NOT_TAG; /*No se consideran tablas del tipo <td class="clasetabla">*/
}
int reconocer_tabla (FILE* archivo_origen) {
    char c;
    int id_end;
    int i = 0;
    char table_v[LENGHT_TABLE_V] = {'t','b','o','d','y'};

    c = getc(archivo_origen);
    if ( c != '<') {
        if (c == EOF){
            return EOF;
        }
        else return NOT_TAG;
    }
    c = getc(archivo_origen);

    if ( c != '/' && c != 't') {
        if (c == EOF){
            return EOF;
        }
        else return NOT_TAG;
    }
    if (c == '/') {
        c = getc(archivo_origen);

        id_end = 1;
    }
    else id_end = 0;

    while (c == table_v[i] && i < LENGHT_TABLE_V && c != EOF) {
        c = getc(archivo_origen);
        i++;
    }
    if (i == LENGHT_TABLE_V) {
        return TABLE_TAG + id_end;
    }
       else if (c == EOF) {
        return EOF;
       }
        return NOT_TAG;

}

/*Funciones de Lectura/Escritura */

int leer_campo(FILE* archivo_origen, char* campo) {

    char c[2] = {' ','\0'};
    c[0] = getc(archivo_origen);
    while (c[0] != '<'){
        if (c[0] < 0) {
            c[0] = corregir_acentos(c[0]);
        }
        strcat(campo,c);
        c[0] = getc(archivo_origen);
    }
    while (c[0] != '>'){
        c[0] = getc(archivo_origen);
    }

    return 0;

    }
int leer_fila(FILE* archivo_origen,char campos_vector [] [LENGHT_CAMPO],int cantidad_campos,int* campos_leidos){
    char campo[LENGHT_CAMPO] = "";
    int tag;
    int i = 0;

    getc(archivo_origen); /*Limpia un caracter*/
    if(reconocer_fila_columna (archivo_origen) != COL_TAG){
        return NOT_TAG;
    }
    leer_campo(archivo_origen,campo);
    strcpy(campos_vector[i],campo);
    strcat(campos_vector[i],"\0");
    i = 1;
    tag = reconocer_fila_columna(archivo_origen);
    while (tag != COL_TAG){
        tag = reconocer_fila_columna(archivo_origen);
    }
    while (tag != ROW_TAG_END && i < cantidad_campos) {

        memset(campo,0,LENGHT_CAMPO);
        leer_campo(archivo_origen,campo);
        strcpy(campos_vector[i],campo);
        strcat(campos_vector[i],"\0");
        i++;
        tag = reconocer_fila_columna(archivo_origen);
    }
    if (!strcmp(campos_vector[0],PRIMER_CAMPO)){
        *campos_leidos = i;
        return TABLE_HEADER;
    }
    return ROW_READ;
}

int imprimir_campos_pedidos (char campos[][LENGHT_CAMPO],int cantidad_campos,char* separador,int posiciones_pedidas[],int cantidad_posiciones,FILE* archivo_destino) {
    int i;
    if (!archivo_destino){
        return ERROR_ARCHIVE;
    }
    i = 0;
    while (i < cantidad_posiciones-1) {
        fprintf(archivo_destino,"%s%s",campos[posiciones_pedidas[i]],separador);
        i++;
    }
    fprintf(archivo_destino,"%s",campos[posiciones_pedidas[i]]);
    return OK;

}


/* Modificadores de Formato */

int reemplazar_caracter(char* str,char caracter,char nuevo_caracter) {
    int i = 0;
    short int reemplazado;

    if (str == NULL) {
        return ERROR_NULL_PTR;
    }
    while ( str[i] != '\0') {
        if(str[i] == caracter) {
            str[i] = nuevo_caracter;
            reemplazado = TRUE;
        }
        i++;
    }
    if (reemplazado) {
        return REPLACE_OK;
    }
    else return NOT_FOUND;
}

/*Los siguientes valores fueron obtenidos de evaluar la ejecucion repetidas veces observando el valor numerico que represanta cada caracter*/

char corregir_acentos (char c){
    if (c == -61){ /*Oberve que este caracter aparece en las lecturas*/
        return '\0';
    }
    if (c == -102){ /*Ú*/
        return 'U';
    }
    if (c == -77){ /*ó*/
        return 'o';
    }
    if (c == -95) { /*á*/
        return 'a';
    }
    if (c == -83) { /*í*/
        return 'i';
    }
    else return c; /*Solo modifique los utilizados en los encabezados por ende si llega una é, por ejemplo, no lo modifica*/
}

/*Funciones para cumplir condiciones de listado*/

int verificar_variacion(char campos[][LENGHT_CAMPO],int cantidad_campos,float variacion_min,int posicion_variacion){
        float variacion;

        reemplazar_caracter(campos[posicion_variacion],',','.');
        variacion = atof(campos[posicion_variacion]);
        if (variacion > variacion_min || variacion < (variacion_min*-1.00)){
            reemplazar_caracter(campos[posicion_variacion],'.',',');
            return VAR_TRUE;
        }
        reemplazar_caracter(campos[posicion_variacion],'.',',');
        return VAR_FALSE;
}
int reconocer_col_pedidas (char dicc_encabezado[][LENGHT_CAMPO],int cant_col_encabezado,int posiciones_col[],char campos[][LENGHT_CAMPO],int cantidad_campos){
    int i;
    int j = 0;
    i = 0;
    while (i < cant_col_encabezado) {
        posiciones_col[i] = -1; /*Me aseguro que si hay campos con el mismo nombre no se pisen*/
        i++;                    /*Me paso con el campo Cant. Nominal*/
    }
    while (j < cant_col_encabezado) {
        i = 0;
        while (i < cantidad_campos) {
            if (!strcmp(campos[i],dicc_encabezado[j])&& (posiciones_col[j] == -1)){
                posiciones_col[j] = i;
                i = cantidad_campos;
            }
            i++;
        }
        j++;
    }
    return OK;
}
int verificar_compraventa_apertura(char campos[][LENGHT_CAMPO],int cantidad_campos,int posiciones_col[CANTIDAD_CAMPOS_CSV]) {
    float precio_compra, precio_venta, apertura;

    precio_compra = atof(campos[posiciones_col[POS_COMPRA]]);
    precio_venta = atof (campos[posiciones_col[POS_VENTA]]);
    apertura = atof(campos[posiciones_col[POS_APERTURA]]);
    if (precio_compra < apertura && precio_venta < apertura) {
        return COLOR_ROW;
    }
    else return NOT_COLOR_ROW;
}

/*Dibujar Menus*/

void dibujar_menu(void) {
    printf("%c %c %s %0.2f \n",OPT_LIST_VAR_SCREEN,OPT_SEPARATOR,MSG_LIST_VAR_SCREEN,VAR_MIN);
    printf("%c %c %s \n",OPT_LIST_CSV,OPT_SEPARATOR,MSG_LIST_CSV);
    printf("%c %c %s \n",OPT_LIST_HTML,OPT_SEPARATOR,MSG_LIST_HTML);
    printf("%c %c %s \n",OPT_LIST_EXIT,OPT_SEPARATOR,MSG_LIST_EXIT);
}

/*Funciones de seleccionar opcion MENU*/

state_t ingresar_opcion (void) {
    char opcion;
    int intentos = 0;

    while (intentos < MAX_INTENTOS) {
        printf ("%s%c ",MSG_OPT,MSG_OPT_SEPARATOR);
        scanf("%c",&opcion);
        fflush(stdin);
        if (opcion == OPT_LIST_VAR_SCREEN) {
        return LIST_VAR_SCREEN;
        }
        if (opcion == OPT_LIST_CSV) {
        return LIST_CV_CSV;
        }
        if (opcion == OPT_LIST_HTML) {
        return LIST_CV_HTML;
        }
        if (opcion == OPT_LIST_EXIT){
        return EXIT;
        }
        printf("%s \n",MSG_OPT_ERROR);
        intentos++;
        dibujar_menu();
    }
    printf("%s \n",MSG_ERROR_INTENTOS);
    return EXIT;
}
state_t realizar_otra_accion(void){
    char option;
    printf("¿Desea Realizar otra accion? \n");
    printf ("%c %c %s \n",OPT_YES,OPT_SEPARATOR,MSG_OPT_YES);
    printf ("%c %c %s \n",OPT_NO,OPT_SEPARATOR,MSG_OPT_NO);
    printf("%s%c",MSG_OPT,MSG_OPT_SEPARATOR);
    scanf("%c",&option);
    fflush(stdin);

    if (option == OPT_YES){
        return MAIN_MENU;
    }
    else return EXIT;
}

/*Funciones para Imprimir los datos en un archivo o mostrar en pantalla*/
int listar_pantalla(int div_aux,int campos_leidos,char campos[][LENGHT_CAMPO],int cantidad_campos,char encabezado_screen[][LENGHT_CAMPO],int posiciones_col[],FILE* archivo_destino) {

    if ( div_aux == ROW_READ || div_aux == TABLE_HEADER) {
        if (div_aux == TABLE_HEADER){
            reconocer_col_pedidas(encabezado_screen,campos_leidos,posiciones_col,campos,cantidad_campos);
            imprimir_campos_pedidos(campos,campos_leidos,SEPARADOR_SCREEN,posiciones_col,cantidad_campos,archivo_destino);
            printf("\n");
        }
        else {
            if (verificar_variacion(campos,campos_leidos,VAR_MIN,posiciones_col[VAR_POS])){
            imprimir_campos_pedidos(campos,campos_leidos,SEPARADOR_SCREEN,posiciones_col,cantidad_campos,archivo_destino);
            printf("\n");
            }
        }
    }
    return OK;
}
int exportar_csv(int div_aux,int campos_leidos,char campos[][LENGHT_CAMPO],int cantidad_campos,char encabezado_csv[][LENGHT_CAMPO],int posiciones_col[],FILE* archivo_destino) {

    if ( div_aux == ROW_READ || div_aux == TABLE_HEADER) {
        if (div_aux == TABLE_HEADER){
            reconocer_col_pedidas(encabezado_csv,CANTIDAD_CAMPOS_CSV,posiciones_col,campos,campos_leidos);
            imprimir_campos_pedidos(campos,campos_leidos,SEPARADOR_CSV,posiciones_col,CANTIDAD_CAMPOS_CSV,archivo_destino);
            fprintf(archivo_destino,"\n");
        }
        else {
            imprimir_campos_pedidos(campos,campos_leidos,SEPARADOR_CSV,posiciones_col,CANTIDAD_CAMPOS_CSV,archivo_destino);
            fprintf(archivo_destino,"\n");
        }
    }
    return OK;
}
int exportar_html(int div_aux,int campos_leidos,char campos[][LENGHT_CAMPO],int cantidad_campos,char encabezado_html[][LENGHT_CAMPO],int posiciones_col[],FILE* archivo_destino) {
    if ( div_aux == ROW_READ || div_aux == TABLE_HEADER) {
        if (div_aux == TABLE_HEADER){
            reconocer_col_pedidas(encabezado_html,CANTIDAD_CAMPOS_HTML,posiciones_col,campos,CANTIDAD_CAMPOS);
            fprintf(archivo_destino,"%s",HEADER_ROW_HTML);
            imprimir_campos_pedidos(campos,campos_leidos,SEPARADOR_HTML,posiciones_col,CANTIDAD_CAMPOS_HTML,archivo_destino);
            fprintf(archivo_destino,"%s",FOOTER_ROW_HTML);
        }
        else {
            if (verificar_variacion(campos,cantidad_campos,VAR_MIN,VAR_POS) == VAR_TRUE) {
            if (verificar_compraventa_apertura(campos,CANTIDAD_CAMPOS,posiciones_col) == COLOR_ROW) {
            fprintf(archivo_destino,"%s",HEADER_ROW_COLOR_HTML);
            imprimir_campos_pedidos(campos,campos_leidos,SEPARADOR_HTML,posiciones_col,CANTIDAD_CAMPOS_HTML,archivo_destino);
            }
            else {
                fprintf(archivo_destino,"%s",HEADER_ROW_HTML);
                imprimir_campos_pedidos(campos,campos_leidos,SEPARADOR_HTML,posiciones_col,CANTIDAD_CAMPOS_HTML,archivo_destino);
            }
            fprintf(archivo_destino,"%s",FOOTER_ROW_HTML);
            }
        }
    }
    return OK;
}

/*Busca y crea la tabla guardando los datos segun se indique en la funcion parametro*/
int crear_tabla_datos (FILE* archivo_origen,FILE* archivo_destino,char dicc_encabezado[][LENGHT_CAMPO],int cantidad_campos,int (*funcion)(int,int,char[][LENGHT_CAMPO],int,char[][LENGHT_CAMPO],int[],FILE*)) {

    char campos[CANTIDAD_CAMPOS][LENGHT_CAMPO];
    int aux, div_aux,campos_leidos;
    int posiciones_col[CANTIDAD_CAMPOS];

    if (!archivo_origen) {
        return ERROR_ARCHIVE;
    }
    if(!archivo_destino){
        return ERROR_ARCHIVE;
    }
    campos_leidos = 0;
    aux = reconocer_fila_columna(archivo_origen);
    while(aux != EOF){
        if (aux == ROW_TAG) {
            div_aux = leer_fila(archivo_origen,campos,CANTIDAD_CAMPOS,&campos_leidos);
            funcion(div_aux,campos_leidos,campos,cantidad_campos,dicc_encabezado,posiciones_col,archivo_destino);
        }
         aux = reconocer_fila_columna(archivo_origen);
    }
    return OK;
}


/*Funciones Antiguas que fueron modificadas por Codigo repetido*/

/*
int exportar_variaciones_csv (FILE* archivo_origen) {

    char campos[CANTIDAD_CAMPOS][LENGHT_CAMPO];
    int aux, div_aux,campos_leidos;
    int posiciones_col[CANTIDAD_CAMPOS];
    FILE* archivo_destino;

    campos_leidos = 0;

    if (!archivo_origen) {

        return ERROR_ARCHIVE;
    }
    archivo_destino = fopen(NOMBRE_CSV_DESTINO,"w");
    aux = reconocer_fila_columna(archivo_origen);
    while(aux != EOF){
        if (aux == ROW_TAG) {
            div_aux = leer_fila(archivo_origen,campos,CANTIDAD_CAMPOS,&campos_leidos);
            if ( div_aux == ROW_READ || div_aux == TABLE_HEADER) {
                if (div_aux == TABLE_HEADER){
                    reconocer_col_pedidas(dicc_encabezado_csv,CANTIDAD_CAMPOS_CSV,posiciones_col,campos,campos_leidos);
                    imprimir_campos_pedidos(campos,campos_leidos,SEPARADOR_CSV,posiciones_col,CANTIDAD_CAMPOS_CSV,archivo_destino);
                    fprintf(archivo_destino,"\n");
                }
                else {
                    imprimir_campos_pedidos(campos,campos_leidos,SEPARADOR_CSV,posiciones_col,CANTIDAD_CAMPOS_CSV,archivo_destino);
                    fprintf(archivo_destino,"\n");
                }
            }
        }
        aux = reconocer_fila_columna(archivo_origen);
    }
    printf("%s \n",MSG_FILE_EXPORT_SUCCESS);
    return OK;
}*/
/*
int listar_variaciones (FILE* archivo_origen) {

    char campos[CANTIDAD_CAMPOS][LENGHT_CAMPO];
    int aux, div_aux,campos_leidos;
    int posiciones_col[CANTIDAD_CAMPOS_SCREEN];
    campos_leidos = 0;

    if (!archivo_origen) {
        return ERROR_ARCHIVE;
        }
   aux = reconocer_fila_columna(archivo_origen);
    while(aux != EOF){
        if (aux == ROW_TAG) {
            div_aux = leer_fila(archivo_origen,campos,CANTIDAD_CAMPOS,&campos_leidos);
            if ( div_aux == ROW_READ || div_aux == TABLE_HEADER) {
                if (div_aux == TABLE_HEADER){
                    reconocer_col_pedidas(dicc_encabezado_screen,campos_leidos,posiciones_col,campos,CANTIDAD_CAMPOS);
                    imprimir_campos_pedidos(campos,campos_leidos,SEPARADOR_SCREEN,posiciones_col,CANTIDAD_CAMPOS_SCREEN,stdout);
                    printf("\n");
                }
                else {
                    if(verificar_variacion(campos,CANTIDAD_CAMPOS,VAR_MIN,posiciones_col[VAR_POS])) {
                    imprimir_campos_pedidos(campos,campos_leidos,SEPARADOR_SCREEN,posiciones_col,CANTIDAD_CAMPOS_SCREEN,stdout);
                    printf("\n");
                    }
                }
            }
        }
        aux = reconocer_fila_columna(archivo_origen);
    }
    return OK;
}*//*
int exportar_variaciones_html (FILE* archivo_origen) {

    char campos[CANTIDAD_CAMPOS][LENGHT_CAMPO];
    int aux, div_aux,campos_leidos;
    int posiciones_col[CANTIDAD_CAMPOS];
    FILE* archivo_destino;

    campos_leidos = 0;
    if (!archivo_origen) {
        return ERROR_ARCHIVE;
    }
    archivo_destino = fopen(NOMBRE_HTML_DESTINO,"w");
    aux = reconocer_fila_columna(archivo_origen);
    fprintf(archivo_destino,"%s \n",TABLE_TAG_HTML);
    while(aux != EOF){
        if (aux == ROW_TAG) {
            div_aux = leer_fila(archivo_origen,campos,CANTIDAD_CAMPOS,&campos_leidos);
            exportar_html(div_aux,campos_leidos,campos,CANTIDAD_CAMPOS_HTML,dicc_encabezado_html,posiciones_col,archivo_destino);
        }
        aux = reconocer_fila_columna(archivo_origen);
    }

    fprintf(archivo_destino,"%s \n",TABLE_TAG_HTML_END);
    printf("%s \n",MSG_FILE_EXPORT_SUCCESS);
    return OK;
}*//*
int exportar_variaciones_csv (FILE* archivo_origen) {

    char campos[CANTIDAD_CAMPOS][LENGHT_CAMPO];
    int aux, div_aux,campos_leidos;
    int posiciones_col[CANTIDAD_CAMPOS];
    FILE* archivo_destino;

    campos_leidos = 0;

    if (!archivo_origen) {

        return ERROR_ARCHIVE;
    }
    archivo_destino = fopen(NOMBRE_CSV_DESTINO,"w");
    aux = reconocer_fila_columna(archivo_origen);
    while(aux != EOF){
        if (aux == ROW_TAG) {
            div_aux = leer_fila(archivo_origen,campos,CANTIDAD_CAMPOS,&campos_leidos);
            exportar_csv(div_aux,campos_leidos,campos,CANTIDAD_CAMPOS_CSV,dicc_encabezado_csv,posiciones_col,archivo_destino);
        }
        aux = reconocer_fila_columna(archivo_origen);
    }
    fclose(archivo_destino);
    printf("%s \n",MSG_FILE_EXPORT_SUCCESS);
    return OK;
}*/
/*int listar_variaciones (FILE* archivo_origen) {

    char campos[CANTIDAD_CAMPOS][LENGHT_CAMPO];
    int aux, div_aux,campos_leidos;
    int posiciones_col [CANTIDAD_CAMPOS_SCREEN];
    campos_leidos = 0;

    if (!archivo_origen) {
        return ERROR_ARCHIVE;
        }
   aux = reconocer_fila_columna(archivo_origen);
    while(aux != EOF){
        if (aux == ROW_TAG) {
            div_aux = leer_fila(archivo_origen,campos,CANTIDAD_CAMPOS,&campos_leidos);
                listar_pantalla(div_aux,campos_leidos,campos,CANTIDAD_CAMPOS_SCREEN,dicc_encabezado_screen,posiciones_col,stdout);
        }
        aux = reconocer_fila_columna(archivo_origen);
    }
    return OK;
}

*/
/*int imprimir_campos_pedidos_html (char campos[][LENGHT_CAMPO],int cantidad_campos,int posiciones_pedidas[],int cantidad_posiciones,int table_color, char style[],int long_style,FILE* archivo_destino) {
    int i;
    i = 0;
    if(!archivo_destino) {
        return ERROR_ARCHIVE;
    }
    if (table_color == COLOR_ROW) {
        fprintf(archivo_destino,"%s",style);
    }
    else fprintf(archivo_destino,"%s",ROW_TAG_HTML);
    while (i < cantidad_posiciones) {
        fprintf(archivo_destino,"%s %s %s",COL_TAG_HTML,campos[posiciones_pedidas[i]],COL_TAG_HTML_END);
        i++;
    }
    fprintf(archivo_destino,"%s \n",ROW_TAG_HTML_END);
    return OK;

}*/
