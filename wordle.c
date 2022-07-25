#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h> //Para poder usar toupper.
#include <windows.h> // para poder ejecutar colores en CMD
#include <conio.h>

#define MAX_PARTIDAS 8
#define MIN_PARTIDAS 1
#define INTENTOS 6
#define LETRAS 6
#define ARCHIVOEXTERNO "palabras.txt"
#define MAX_INTENTOS 6

//Puntuaciones
#define START 5000
#define MAXPOINTS 5000
#define PASSLINE 500 
#define CLETTER 100 //Correct letter
#define GLETTER 50 //Good letter
#define PAL_ADIVINADA 2000

#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define RESET "\x1B[0m"

//manera en la que el programa cuando se ejecuta .exe pueda mostrar los colores en cmd
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING  0x0004
#endif
static HANDLE stdoutHandle;
static DWORD outModeInit;

int sesiones, acierto = 0, ck[5];

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

void promedio(int *psesiones, int sesiones){
    
    int suma;
    float prom;

    for (int i = 1; i <= sesiones; i++)
    {
        suma = suma + psesiones[i];
    }

    prom = suma / sesiones;

    printf("El promedio de tu puntaje fue de: %.2f ptos.\n",prom);
}

void maxAmin(int *psesiones, int sesiones){
    int min, max;

    min = max = psesiones[1];

    for (int i = 1; i <= sesiones; i++)
    {
        if (min > psesiones[i])
        {
            min = psesiones[i];
            if (max < psesiones[i])
            {
                max = psesiones[i];
            }
        }
    }
    
    printf("El maximo puntaje que obtuviste fue de: %i ptos.\n", max);
    printf("El minimo puntaje que obtuviste fue de: %i ptos.\n", min);
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

int puntaje(int psesion,int points,int partidaActual){

    int puntajeSesiones;

    if(points == MAXPOINTS){
        puntajeSesiones = START + MAXPOINTS + PAL_ADIVINADA;
        return puntajeSesiones;
    }
    else if(points == PASSLINE){
        puntajeSesiones = psesion - PASSLINE;
        return puntajeSesiones;
    }
    else if (points == CLETTER){
        puntajeSesiones = psesion + CLETTER;
        return puntajeSesiones;
    }
    else if (points == GLETTER){
        puntajeSesiones = psesion + GLETTER;
        return puntajeSesiones;
    }
    else if (points == PAL_ADIVINADA){
        puntajeSesiones = psesion + PAL_ADIVINADA;
        return puntajeSesiones;
    }
}

int intento(int psesion,char *p,int attemp){
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

        if(attemp == 1){
            return 1;
        }
        acierto = 1;
        psesion = puntaje(psesion,PAL_ADIVINADA,attemp);
        return psesion;
    }
    else{
        psesion = puntaje(psesion,PASSLINE,attemp);

        while (i < largo && palabra[i] != 10)
        {
            if (p[i] == palabra[i] && p[i] != 10)
            {   
                if (ck[i] == 0)
                {
                    psesion = puntaje(psesion,CLETTER,attemp);
                }
                ck[i] = 1;
        
                printf(RESET);
                printf("%s%c",KGRN,p[i]);
                printf(RESET);
            }
            else if(palabra[i] != 10){

                int val = palabra[i];

                if(strchr(p,val) != NULL){
                    psesion = puntaje(psesion,GLETTER,attemp);
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
        return psesion;
    }
}

void partidas(int partidas,char * p) {

    int pos, resultado;
    int psesiones[sesiones];

    for (int n = 1; n <= sesiones; n++)
    {
        psesiones[n] = START;
    }
    for (int f = 0; f < 5; f++)
    {
        ck[f] = 0;
    }
    
    for(int i = 1; i <= partidas;i++){
        srand(clock());
        pos = rand()%totalLines()+1;
        getWordInLine(p,pos);

        printf("Partida %i / %i \n", i,partidas);
        
        for (int attemp = 1; attemp <= MAX_INTENTOS; attemp++)
        {
            printf("Intento %i / %i \n", attemp,MAX_INTENTOS);

            fflush(stdin);
            resultado = intento(psesiones[i],p,attemp);
           
            if (resultado == 1)
            {
                attemp = MAX_INTENTOS;
                psesiones[i] = puntaje(psesiones[i],MAXPOINTS,i);
            }
            else if (acierto == 1){
                attemp = MAX_INTENTOS;
                psesiones[i] = resultado;
            }
            else if (acierto == 0 && attemp == MAX_INTENTOS)
            {
                psesiones[i] = 0;
            }
            else{
                psesiones[i] = resultado;
            }
        }
        if(partidas >= 1){
            printf("La palabra era: %s%s \n",KGRN, p);
            printf(RESET);
            if(i != partidas){
                char decision;
                printf("Desea seguir jugando? [N - exit]: ");
                getchar();
                scanf("%c",&decision);
                
                if(decision == 'N' || decision == 'n'){
                    for (int m = 1; m <= sesiones; m++)
                    {
                        printf("P. %i = %i pts. \n",m,psesiones[m]);
                        sesiones = i;
                    }
                    return;
                }
                printf("Siguiente ronda! \n");
            }
        }
        acierto = 0;
    }
    for (int m = 1; m <= sesiones; m++)
    {
        printf("P. %i = %i pts. \n",m,psesiones[m]);
    } printf("\n");
    maxAmin(psesiones,sesiones);
    promedio(psesiones,sesiones);
    printf("\n");
}

int main() {
    setupConsole();
    int i;
    int totaljuegos;
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