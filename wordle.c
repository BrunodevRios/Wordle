#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h> //Para poder usar toupper.
#include <windows.h> // para poder ejecutar colores en cmd

#define MAX_PARTIDAS 8
#define MIN_PARTIDAS 1
#define INTENTOS 6
#define LETRAS 6
#define ARCHIVOEXTERNO "palabras.txt"
#define MAX_INTENTOS 6

#define START 5000
#define MAXPOINTS 10000
#define PASSLINE 500 
#define CLETTER 100 //Correct letter
#define GLETTER 50 //Good letter

#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define RESET "\x1B[0m"

//manera en la que el programa cuando se ejecuta .exe pueda mostrar los colores en cmd
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING  0x0004
#endif

static HANDLE stdoutHandle;
static DWORD outModeInit;

//funcion necesarias para mostrar colores en cmd
void setupConsole(void) {
    DWORD outMode = 0;
    stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    if(stdoutHandle == INVALID_HANDLE_VALUE) {
        exit(GetLastError());
    }
    
    if(!GetConsoleMode(stdoutHandle, &outMode)) {
        exit(GetLastError());
    }

    outModeInit = outMode;
    
    // Enable ANSI escape codes
    outMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

    if(!SetConsoleMode(stdoutHandle, outMode)) {
        exit(GetLastError());
    }   
}

//funcion necesarias para mostrar colores en cmd
void restoreConsole(void) {
    // Reset colors
    printf("\x1b[0m");  
    
    // Reset console mode
    if(!SetConsoleMode(stdoutHandle, outModeInit)) {
        exit(GetLastError());
    }
}

int totalLines(){
    FILE * file;
    int numeroLineas = 0;
    char ch;

    file = fopen(ARCHIVOEXTERNO, "r");

    while(!feof(file))
    {
        ch = fgetc(file);
        if(ch == '\n')
        {
            numeroLineas++;
        }
    }
    return numeroLineas;
}

void getWordInLine(char *p,int pos) {

    char * line = NULL;
    size_t len;
    size_t read;
    FILE * fp;

    fp = fopen(ARCHIVOEXTERNO, "r");

    if (fp == NULL){
        exit(EXIT_FAILURE);
    }
    
    int i=1;

    while ((read = getline(&line, &len, fp)) != -1) {
        if (i==pos) {
            strcpy(p, line);
            return;
        }
        i++;
    }
    fclose(fp);
}

int intento(char *p){
    char palabra[6], pal_ingresada[6];
    int i = 0,j = 0, u = 0;

    scanf("%[^\n]s",&palabra);

    int largo = strlen(palabra);

    while(largo != 5){
        printf("Por favor ingrese una palabra con un largo de 5 letras.\n");
        scanf("%s",&pal_ingresada);
        largo = strlen(pal_ingresada);
    }

    for (int k = 0; k < largo; k++)
    {
        palabra[k] = toupper(palabra[k]);
    }

    strcat(palabra,"\n");

    int value = strcmp(palabra,p);

    if (value == 0)
    {
        printf("%s%s\n" RESET, KGRN,p);
        printf("Has acertado la palabra secreta \n");
        printf("\n");
        return 1;
    }
    else{
        while (i < largo && palabra[i] != 10)
        {
            if (p[i] == palabra[i] && p[i] != 10)
            {   
                printf(RESET);
                printf("%s%c",KGRN,p[i]);
                printf(RESET);
            }
            else if(palabra[i] != 10){

                int val = palabra[i];

                if(strchr(p,val) != NULL){

                    printf(RESET);
                    printf("%s%c",KYEL,palabra[i]);
                    printf(RESET);
                    val = 0;
                }
                else{
                    printf(RESET);
                    printf("_");
                }
            }
            i++;
        }
        printf("\n"RESET);  
    }
}

void partidas(int partidas,char * p) {
    int pos, resultado;
    for(int i = 1; i <= partidas;i++){
        srand(clock());
        pos = rand()%totalLines()+1;
        getWordInLine(p,pos);

        printf("Partida %i / %i \n", i,partidas);
        
        for (int j = 1; j <= MAX_INTENTOS; j++)
        {
            printf("     Intento %i / %i \n", j,MAX_INTENTOS);

            fflush(stdin);
            resultado = intento(p);
           
            if (resultado == 1)
            {
                j = MAX_INTENTOS;
            }
        }
        if(partidas >= 1){
            printf("La palabra era: %s%s \n",KGRN, p);
            printf(RESET);
            if(i != partidas)
                printf("Siguiente ronda! \n");
        }
    }
}

int main() {
    setupConsole();
    int i;
    int totaljuegos;
    int sesiones;
    int palabramisteriosa[LETRAS];
    char p[6];

    printf("Hola, bienvenido a Wordle\n");
    printf("Porfavor ingresa la cantidad de sesiones que deseas jugar entre 1 y 8. \n");
    printf("Recuerda evitar ingresar palabras donde se repitan letras, este Wordle trabaja con palabras totalmente heterogramas. \n");
    printf(">");
    
    scanf("%i", &sesiones);
    while(sesiones < MIN_PARTIDAS || sesiones > MAX_PARTIDAS){
        printf("No te hagas el vivo, pon dentro del rango mencionado, porfavor! \n");
        printf(">");
        scanf("%i", &sesiones);
    } 
    
    partidas(sesiones,p);

    printf("Fin de la partida\n Gracias por jugar!!\n");

    restoreConsole();
    
    system("pause");

    return 0;
} 