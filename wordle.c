// LIBRERIAS INLCUIDAS
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // para funciones como strcpy().
#include <time.h> // para funciones del tiempo.
#include <ctype.h> //Para poder usar la funcion toupper..
#include <windows.h> // para poder ejecutar colores en CMD.
#include <conio.h> // para funciones como getch().

// MACROS UTILIZADOS PARA EL FUNCIONAMIENTO DE UNA PARTIDA.
#define MAX_PARTIDAS 8
#define MIN_PARTIDAS 1
#define INTENTOS 6
#define LETRAS 6
#define ARCHIVOEXTERNO "palabras.txt"
#define MAX_INTENTOS 6

// MACROS UTILIZADOS PARA LAS PUNTUACIONES.
#define INICIO 5000
#define MAXPUNTOS 5000
#define PASODELINEA 500 
#define LCORRECTA 100 //letra correcta.
#define ELETRA 50 //Existencia de letra.
#define PAL_ADIVINADA 2000

// MACROS QUE BUSQUE EN INTERNET PARA PODER IMPRIMIR COLORES AMARILLOS Y VERDES SIENDO FIEL AL WORDLE ORIGINAL.
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define RESET "\x1B[0m" // vuelve a la fuente del color predeterminado por la consola.

// MACROS QUE TAMBIEN TUVE QUE BUSCAR YA QUE AL MOMENTO DE CORRERLO EN CMD, NO ME MOSTRABA COLORES AL IGUAL QUE EN LA CONSOLA.
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING  0x0004
#endif
static HANDLE stdoutHandle;
static DWORD outModeInit;

// VARIABLES GLOBALES PARA QUE TODAS LAS FUNCIONES PUEDAN ACCEDER A ELLAS DE FORMA MAS FACIL
int sesiones, acierto = 0, ck[5];

//ck es una variable global para poder saber que letras de la palabra en cada partida fueron acertadas en el lugar correcto.
/* Esto nos va a ayudar al momento de determinar el puntaje. Ya que las letras que fueron acertadas en el lugar correspondiente
ya que el puntaje de las letras acertadas en el lugar correcto no son acumulables.
*/

//FUNCIONES DE LAS CUALES SE VAN A COMPONER NUESTRO WORDLE:

// FUNCION QUE BUSQUE EN INTERNET PARA PODER MOSTRAR COLORES EN CMD.
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

// FUNCION QUE BUSQUE EN INTERNET PARA PODER MOSTRAR COLORES EN CMD.
void restoreConsole(void) {
    // Reset de colores
    printf("\x1b[0m");  
    
    // Reset modo de consolas
    if(!SetConsoleMode(stdoutHandle, outModeInit)) {
        exit(GetLastError());
    }
}

// FUNCION QUE MEDIANTE UNA CUENTA MATEMATICA Y UN FOR CALCULA EL PROMEDIO DE LAS DIFERENTES PARTIDAS QUE JUGUEMOS.
void promedio(int *psesiones, int sesiones){
    
    int suma;
    float prom;

    for (int i = 1; i <= sesiones; i++) // el for es para que itere en cada sesion de juego y guarde los diferentes puntajes.
    {
        suma = suma + psesiones[i];
    }

    prom = suma / sesiones; 

    printf("El promedio de tu puntaje fue de: %.2f ptos.\n",prom);
}

// FUNCION QUE COMPARA, BUSCA Y ENCUENTRA EL PUNJATE MAS ALTO Y EL MAS BAJO (maximo y minimo).
void maxYmin(int *psesiones, int sesiones){
    int min, max;

    min = max = psesiones[1]; // psesiones[1] porque la matriz empieza desde 1

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

// FUNCION QUE BUSCA Y DETECTA LA CANTIDAD DE PALABRAS DE UN ARCHIVO EXTERNO (palabras.txt).
int totalLines(){
    FILE * file;
    int numeroLineas = 0;
    char ch;

    file = fopen(ARCHIVOEXTERNO, "r");

    while(!feof(file)) 
    {
        ch = fgetc(file); // lee caracter por caracter gracias al while
        if(ch == '\n')
        {
            numeroLineas++;
        }
    }
    return numeroLineas;
   
}

// FUNCION QUE NOS TRAE UNA PALABRA DEL ARCHIVO "palabras.txt".
void getWordInLine(char *p,int pos) {

    char * line = NULL;
    size_t len;
    size_t read; // valor de retorno de la funcion getline
    FILE * fp;
    int i=1;
    
    fp = fopen(ARCHIVOEXTERNO, "r");

    if (fp == NULL){
        exit(EXIT_FAILURE);
    }
    //getline lee una linea entera  del archivo almacenando el texto (incluyendo el carácter de nueva línea y el de terminación).
    while ((read = getline(&line, &len, fp)) != -1) { // mientras read no de -1 signfica que hay lineas de codigo por leer
        if (i==pos) {
            strcpy(p, line);
            return;
        }
        i++;
    }
    fclose(fp);
}

//FUNCION DONDE SE DEFINEN Y ALMACENAN LOS PUNTAJES .
int puntaje(int psesion,int points,int partidaActual){
    // Declaro un entero donde se guarden los puntajes de cada sesion.
    int puntajeSesiones;
    // Use los macros antes definidos para jugar con la logica de lo que pasa segun vayamos adivinando las letras.
    if(points == MAXPUNTOS){
        puntajeSesiones = INICIO + MAXPUNTOS + PAL_ADIVINADA;
        return puntajeSesiones;
    }
    else if(points == PASODELINEA){
        puntajeSesiones = psesion - PASODELINEA;
        return puntajeSesiones;
    }
    else if (points == LCORRECTA){
        puntajeSesiones = psesion + LCORRECTA;
        return puntajeSesiones;
    }
    else if (points == ELETRA){
        puntajeSesiones = psesion + ELETRA;
        return puntajeSesiones;
    }
    else if (points == PAL_ADIVINADA){
        puntajeSesiones = psesion + PAL_ADIVINADA;
        return puntajeSesiones;
    }
} // El return sirve para que se guarde el puntaje de sesion.

// FUNCION QUE TRABAJA LA LOGICA DENTRO DE CADA INTENTO JUGADO.
int intento(int psesion,char *p,int attemp){
    char palabra[6], pal_ingresada[6];
    int i = 0,j = 0, u = 0;

    scanf("%[^\n]s",&palabra); // Le pido que tome todos los caracteres expecto el salto de linea(\n).

    int largo = strlen(palabra);// strlen numero de caracteres de palabra.

    // Validador de solamente pueda ingresar 5 letras.
    while(largo != 5){
        printf("Por favor ingrese una palabra con un largo de 5 letras.\n");
        scanf("%s",&pal_ingresada);
        largo = strlen(pal_ingresada);
    }

    // Validador para poner las letras en mayuscula siempre [toupper()].
    for (int k = 0; k < largo; k++)
    {
        palabra[k] = toupper(palabra[k]);
    }

    strcat(palabra,"\n"); // srtcat: añade un bloque de memoria a otro.

    int value = strcmp(palabra,p);

    if (value == 0)
    {
        printf("%s%s\n" RESET, KGRN,p); // macros para agregar color.
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
        psesion = puntaje(psesion,PASODELINEA,attemp);

        while (i < largo && palabra[i] != 10) // en el codigo ACSII 10 = salto de linea.
        {
            if (p[i] == palabra[i] && p[i] != 10)
            {   
                if (ck[i] == 0)
                {
                    psesion = puntaje(psesion,LCORRECTA,attemp);
                }
                ck[i] = 1;
        
                printf(RESET);
                printf("%s%c",KGRN,p[i]); // macros para agregar color.
                printf(RESET);
            }
            else if(palabra[i] != 10){

                int val = palabra[i];

                if(strchr(p,val) != NULL){
                    psesion = puntaje(psesion,ELETRA,attemp);
                    printf(RESET);
                    printf("%s%c",KYEL,palabra[i]);// macros para agregar color.
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

// FUNCION QUE ELIGE LA PALABRA ALEATORIAMENTE, SUMA/RESTA PUNTAJES, INGRESA LETRAS.
void partidas(int partidas,char * p) {

    int pos, resultado;
    int psesiones[sesiones];

    for (int n = 1; n <= sesiones; n++) //Inicializa el puntaje de cada sesion de juego con el puntaje inicial.
    {
        psesiones[n] = INICIO;
    }

    //Este ciclo nos va a llenar la matriz int con 0. Esta matriz nos va a ayudar a identificar que letras ya fueron acertadas en el lugar correcto.
    for (int f = 0; f < 5; f++) 
    {
        ck[f] = 0;
    }
    // Este for elige las palabras aleatoriamente haciendo una cuenta de un numero aleatorio % el total de lineas mas uno ya que el contador de totalines() empieza desde 0.
    for(int i = 1; i <= partidas;i++){
        srand(clock());
        pos = rand()%totalLines()+1;
        getWordInLine(p,pos);

        printf("Partida %i / %i \n", i,partidas);


    for (int attemp = 1; attemp <= MAX_INTENTOS; attemp++)
        {
            printf("Intento %i / %i \n", attemp,MAX_INTENTOS);

            fflush(stdin); // Funcion que limpia la memoria del buffer.
           
            resultado = intento(psesiones[i],p,attemp);
            
            if (resultado == 1)
            {
                attemp = MAX_INTENTOS;
                psesiones[i] = puntaje(psesiones[i],MAXPUNTOS,i);
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
                char decision; //validador para seguir jugando o no.
                printf("Desea seguir jugando? [N - exit]: ");
                getchar();// getchar(): lee caracter por caracter.
                scanf("%c",&decision);
                // Si tocamos la n o N con estas lineas de codigo el juego corta ahi.
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
    
    maxYmin(psesiones,sesiones); // Resultados finales de la partida.
    promedio(psesiones,sesiones); // Resultados finales de la partida.
    printf("\n");                                       
}

int main() {
    setupConsole();
    int totaljuegos;
    int palabramisteriosa[LETRAS];
    char p[6];

    printf("Hola, bienvenido a Wordle\n");
    printf("Porfavor ingresa la cantidad de sesiones que deseas jugar entre 1 y 8. \n");
    printf("Recuerda colocar palabras de 5 letras como en el juego original \n");
    printf("Recuerda evitar ingresar palabras donde se repitan letras, este Wordle trabaja con palabras totalmente heterogramas. \n");
    printf(">");
    
    scanf("%i", &sesiones);
    while(sesiones < MIN_PARTIDAS || sesiones > MAX_PARTIDAS){
        printf("Pon dentro del rango mencionado, porfavor! \n");
        printf(">");
        scanf("%i", &sesiones);
    } 
    
    partidas(sesiones,p);

    printf("Fin de la partida\n Gracias por jugar!!\n");

    restoreConsole();
    
    system("pause");

    return 0;
} 