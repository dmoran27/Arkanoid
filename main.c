/* 
 * File:   arkanoid.c
 * Author: Moran,Diana.
 * S.O: LINUX MINT.
 * Created on 6 de julio de 2015, 02:49 PM
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>

/* **************************************************
 * CONSTANTES
 ***************************************************** */

// Taño de las filas y las columnas de la parte de los bloques en el tablero 
#define NUM_FILAS_BLOQUES 6
#define NUM_COLUMNAS_BLOQUES 15

#define NUM_FILAS_TABLERO 20
#define NUM_COLUMNAS_TABLERO NUM_COLUMNAS_BLOQUES

// nombre de los archivos de donde se cargara el tablero
#define ARCHIVO_PARTIDA_NUEVA "arkanoid.txt"
#define ARCHIVO_PARTIDA_GUARDADA "arkanoid_guardado.txt"

// tamaño de la barra 
#define TAMANO_BARRA 20

// tiempo en el que se va a recargar la pantalla
#define TIEMPO_RECARGA_PANTALLA 80000

#define NUMERO_OPCIONES_MENU 3
#define OPCION_NUEVA_PARTIDA 1
#define OPCION_CARGAR_PARTIDA 2
#define OPCION_SALIR 3

/**
 * 
 */

/* **************************************************
 * VARIABLES GLOBALES
 ***************************************************** */
int tablero[NUM_FILAS_BLOQUES][NUM_COLUMNAS_BLOQUES];

int pantalla_max_y, pantalla_max_x;

int tamano_bloque;

int posicion_x_barra;
int posicion_y_barra;

int posicion_x_pelota;
int posicion_y_pelota;

int direccion_x_pelota = 1;
int direccion_y_pelota = 1;

int next_x_pelota = 0;
int next_y_pelota = 0;

/* **************************************************
 * DECLARACION DE FUNCIONES
 ***************************************************** */
void inicializar_tablero();
int cargar_tablero(char *nombre_archivo);
int salvar_tablero();

void inicializar_ncurses();
void mostrar_menu();
int seleccionar_opcion_menu();
void finalizar_ncurses();

void inicializar_juego(char *nombre_archivo);

void jugar();

void inicializar_barra();
void inicializar_pelota();

void mover_derecha();
void mover_izquierda();

void imprimir_bloques();
void mover_pelota();
void imprimir_barra();

void salir_juego();

/* **************************************************
 * PROGRAMA
 ***************************************************** */
int main () {
    inicializar_ncurses();
    
    int opcion;
    do {
        clear();
        refresh();
        
        nodelay(stdscr, FALSE);
        
        opcion = seleccionar_opcion_menu();
        
        if (opcion != OPCION_SALIR) {
        
            char *nombre_archivo;

            switch (opcion) {
                case OPCION_NUEVA_PARTIDA:
                    nombre_archivo = ARCHIVO_PARTIDA_NUEVA;
                    break;

                case OPCION_CARGAR_PARTIDA:
                    nombre_archivo = ARCHIVO_PARTIDA_GUARDADA;
                    break;
            }
        
            inicializar_juego(nombre_archivo);
        }
  

    } while (opcion != OPCION_SALIR);
    
    finalizar_ncurses();
    return 0;
}

/* **************************************************
 * IMPLEMENTACION DE FUNCIONES
 ***************************************************** */

/**
 * 
 */
void inicializar_tablero() {
    int i,j;
    for (i = 0; i < NUM_FILAS_TABLERO; i++) {
        for (j = 0; j < NUM_COLUMNAS_TABLERO; j++) {
            tablero[i][j] = 0;
        }
    }
}

/**
 * Carga la matriz desde el archivo
 * 
 * @return 1 en caso de que la cargue con exito 0 en caso contrario 
 */
int cargar_tablero(char *nombre_archivo){
    int i,j;
    FILE *archivo = fopen(nombre_archivo, "r");
    
    if (archivo == NULL){
        //printf("Error en el archivo N° %i, descripcion: %i", errno, strerror(errno));
        return 0;
    }
  
    for (i = 0; i < NUM_FILAS_BLOQUES; i++) {
        for (j = 0; j < NUM_COLUMNAS_BLOQUES; j++) {
            int valor = 0;
            fscanf(archivo, "%1d", &tablero[i][j]);   
        }
    }
    
    fclose(archivo);   
    return 1;
}

/**
 *  
 * @return 
 */
int salvar_tablero() {
    int i,j;
    FILE *archivo = fopen(ARCHIVO_PARTIDA_GUARDADA, "w");
    
    if (archivo == NULL){
       // printf("Error en el archivo N° %i, descripcion: %i", errno, strerror(errno));
        return 0;
    }
  
    for (i = 0; i < NUM_FILAS_BLOQUES; i++) {
        for (j = 0; j < NUM_COLUMNAS_BLOQUES; j++) {
            int valor = 0;
            fprintf(archivo, "%d", tablero[i][j]);
        }
        fprintf(archivo, "\n");
    }
    
    fclose(archivo);   
    return 1;
}

/**
 * 
 */
int seleccionar_opcion_menu() {    
    
    int opc = 0;
    int numero_opcion = 1;
    int opcion_seleccionada = 0;
    		
    WINDOW *menu_win = newwin(10, 30,7,25);
    
    keypad(menu_win, TRUE);
    mvprintw(0, 0, "Use las flechas para seleccionar una opcion del menu");
    refresh();
    
    mostrar_menu(menu_win, numero_opcion);
    
    while(TRUE) {	
        opc = wgetch(menu_win);
        switch(opc) {
            
            case KEY_UP:
                
                if(numero_opcion == 1) {
                    numero_opcion = NUMERO_OPCIONES_MENU;
                } else {
                    --numero_opcion;   
                }
                break;
                
            case KEY_DOWN:
            
                if(numero_opcion == NUMERO_OPCIONES_MENU) {
                    numero_opcion = 1;
                } else {
                    ++numero_opcion;
                } 
                break;
                
            case 10:
                opcion_seleccionada = numero_opcion;
                break;
        }
        
        if(opcion_seleccionada != 0) {
            return opcion_seleccionada;
        }
        
        mostrar_menu(menu_win, numero_opcion);
    }
}

/**
 * 
 */
void mostrar_menu(WINDOW *menu_win, int numero_opcion) {
    
    char *opciones_menu[] = {
        "Partida nueva",
        "Cargar partida",
        "Salir"
    };
    
    int x, y, i;	

    x = 2;
    y = 2;
    
    box(menu_win, 0, 0);
    
    for(i = 0; i < NUMERO_OPCIONES_MENU; ++i) {
        
        if(numero_opcion == i + 1) {	
        
            wattron(menu_win, A_REVERSE); 
            mvwprintw(menu_win, y, x, "%s", opciones_menu[i]);
            wattroff(menu_win, A_REVERSE);
            
        } else {
            mvwprintw(menu_win, y, x, "%s", opciones_menu[i]);
        }
    
        ++y;
    }
    wrefresh(menu_win); 
}

/**
 * 
 */
void inicializar_ncurses() {
    initscr();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    cbreak();
}

/**
 * 
 */
void finalizar_ncurses() {
    endwin();
    printf("Fin del juego . . . \n");
}

/**
 * 
 */
void inicializar_juego(char *nombre_archivo) {
    inicializar_tablero();
    
    
    if (!cargar_tablero(nombre_archivo)) {
        clear();
        mvprintw(0, 0, "Error al cargar el archivo que contiene el tablero. Presione una tecla para continuar . . .");
        refresh();
        getch();
        return;
    }
    
    // no bloquear la interfaz con getch
    nodelay(stdscr, TRUE);
    
    // obtener los valores maximos de la pantalla
    getmaxyx(stdscr, pantalla_max_y, pantalla_max_x);
  
    tamano_bloque = pantalla_max_x / NUM_COLUMNAS_BLOQUES;
    
    inicializar_barra();
    inicializar_pelota();
    
    jugar();
}

void inicializar_barra() {
    posicion_y_barra = pantalla_max_y - 1;
    posicion_x_barra = 0;
}

void inicializar_pelota() {
    posicion_y_pelota = pantalla_max_y - 2;
    posicion_x_pelota = 0;
    
    next_x_pelota = posicion_x_pelota;
    next_y_pelota = posicion_y_pelota;
}

void jugar() {

  // tecla que presiona el jugador
  int key;
  
  while(TRUE) { 
    key = getch();
    
    switch (key) {
        
        case KEY_LEFT:
            mover_izquierda();
            break;
            
        case KEY_RIGHT:
            mover_derecha();
            break;
        
        // Presiono la letra "g"
        case 103:
            salvar_tablero();
            break;
        
        // Presiono la letra "s"
        case 115:
            return;
    }
    
    usleep(TIEMPO_RECARGA_PANTALLA);
    clear();
    imprimir_bloques();
    imprimir_barra();
    mover_pelota();
    refresh();
    
    // verificar si persite 
    if (next_y_pelota == pantalla_max_y) {
        if (!(posicion_x_pelota >= posicion_x_barra && posicion_x_pelota <= posicion_x_barra + TAMANO_BARRA)) {
            return;
            mvprintw(3,3,"perdiste");
        }
    }
  }
}

void imprimir_bloques(){
   
    int i,j;
    for(i=0;i<NUM_FILAS_BLOQUES;i++){
        for(j=0; j<NUM_COLUMNAS_BLOQUES; j++){
            if(tablero[i][j]==0){
                mvprintw(i,j," "); 
            }
            if ( tablero[i][j]==1) {
                mvprintw(i, j, "*");
            }
/*
            if(tablero[i][j]==){
                mvprintw(i,j,"    ");
*/
                
            }
        }
        
    }


    
    
    

    
    

void mover_pelota() {
    
    mvprintw(posicion_y_pelota,posicion_x_pelota, "O");
    
    next_x_pelota = posicion_x_pelota + direccion_x_pelota;
    next_y_pelota = posicion_y_pelota + direccion_y_pelota;
    
    // condicionales del choque con las paredes
    if (next_x_pelota >= pantalla_max_x || next_x_pelota == 0) {
        direccion_x_pelota *= -1; 
    } else {
        posicion_x_pelota += direccion_x_pelota;
    }
    
    if (next_y_pelota >= pantalla_max_y || next_y_pelota == 0) {
        direccion_y_pelota *= -1; 
    } else {
        posicion_y_pelota += direccion_y_pelota;
    }
    
    
    
}
  
    


void mover_izquierda() {
    if (posicion_x_barra > 0) {
        --posicion_x_barra;
    }
}

void mover_derecha() {
    if ((posicion_x_barra + TAMANO_BARRA) < pantalla_max_x) {
        ++posicion_x_barra;
    }
}

void imprimir_barra() {
    mvprintw(posicion_y_barra, posicion_x_barra, "===================");
}
