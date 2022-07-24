#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_PARTIDAS 8
#define MIN_PARTIDAS 1
#define INTENTOS 6
#define LETRAS 6
#define ARCHIVOEXTERNO "palabras.txt"
#define MAX_INTENTOS 6


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

void intento(char *p){
    char palabra[6];
    
    printf("%s\n", p);
    scanf("%s",&palabra);

    strcat(palabra,"\n");

    int value = strcmp(palabra,p);

    if (value == 0)
    {
        printf("Son iguales \n");
    }
    
}

void partidas(int partidas,char * p) {
    int pos;
    for(int i = 1; i <= partidas;i++){
        srand(clock());
        pos = rand()%totalLines()+1;
        getWordInLine(p,pos);

        printf("Partida %i / %i \n", i,partidas);
        
        for (int j = 1; j <= MAX_INTENTOS; j++)
        {
            printf("     Intento %i / %i \n", j,MAX_INTENTOS);

            fflush(stdin);
            intento(p);
        }
        
    }
    
}


int main() {
    int i;
    int totaljuegos;
    int sesiones;
    int palabramisteriosa[LETRAS];
    char p[6];
    
    printf("Hola, bienvenido a Wordle\n");
    printf("Porfavor ingresa la cantidad de sesiones que deseas jugar entre 1 y 8: \n");
    printf(">");
    
    scanf("%i", &sesiones);
    while(sesiones < MIN_PARTIDAS || sesiones > MAX_PARTIDAS){
        printf("No te hagas el vivo, pon dentro del rango mencionado, porfavor! \n");
        printf(">");
        scanf("%i", &sesiones);
    } 
    
    partidas(sesiones,p);
    return 0;
} 
