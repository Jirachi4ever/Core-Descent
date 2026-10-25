/*Integrantes: Eydan Bastidas C.I: 31.677.825 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef char StringLargo[100];
typedef char StringCorto [30];

enum {NMONSTRUOS = 10};
enum {NOBJETOS = 10};

typedef struct{
    StringCorto nombre;
    int vida, ataque, defensa, exp;
}Jugador;

typedef struct{
    int seed, piso, sala, cant_pisos, total_salas;
}Ubicacion;

typedef struct {
    StringCorto name;
    int id, hp, atk, def, rarity;
}Entity; // En desarrollo

typedef struct{
    StringCorto name, effect;
    int id, value;
}Item;

typedef Item Items [NOBJETOS];

typedef Entity Entities [NMONSTRUOS];

typedef struct{
    Jugador player;
    Ubicacion ubi;
    Entities bestiary, oponente;
    Items objetos;
}Gamestate;

Gamestate cargar_config(); 

void cargar_bestiario(); 

void cargar_objeto();

void menu();

void menu_combate();

void menu_fueradecombate();

void seed(int semilla);

Gamestate insertar_nombre (Gamestate state);

Gamestate SeleccionarEnemigo(Gamestate state);

Gamestate avanzarSala (Gamestate state);

void iniciarCombate(Gamestate state); // En desarrollo

int calcularDaño(int ataque_atacante, int defensa_defensor); // En desarrollo

void otorgarRecompensa(Gamestate state, Items objetos, int total_objetos, Entities oponente); // En desarrollo

void verificarSubidaNivel(Gamestate state); // En desarrollo

/*

Modulos por configurar y concretar

Módulo 3:
void iniciarCombate(Estadojuego *estado, Entidad *enemigo);
int calcularDaño(int ataque_atacante, int defensa_defensor);
void otorgarRecompensa(EstadoJuego *estado, Objeto catalogo[],
int total_objetos, char tipo_enemigo[]);
void verificarSubidaNivel(EstadoJuego *estado);

Módulo 4:
void usar_objeto(EstadoJuego *estado);

Módulo 5:
void guardar_partida(EstadoJuego *estado);
int cargar_partida(EstadoJuego *estado);
void registrar_highscore(EstadoJuego estado, bool victoria);
*/

int main(){

    Gamestate state;
    int op;
    StringLargo s;
    
    bool inmenu = true;

    cargar_bestiario(state.bestiary);
    cargar_objeto(state.objetos);

    /* Revisión de Bestiario (Para pruebas)
    for(int i; i < 10; i++){
        printf("%s %d %d %d %d %d\n",
        state.bestiary[i].name,
        state.bestiary[i].id,
        state.bestiary[i].hp,
        state.bestiary[i].def,
        state.bestiary[i].atk,
        state.bestiary[i].rarity);
    }*/

    /* Revisión de Objetos (Para pruebas)
    for(int i = 0; i < 5; i++){
        printf("%s %d %d %s\n",
        state.objetos[i].name,
        state.objetos[i].id,
        state.objetos[i].value,
        state.objetos[i].effect);
    }*/

    menu();
    scanf("%d", &op);
    
    while(inmenu == true){
        switch(op)
        {
            case 1:
                inmenu = false;
                state = cargar_config(state); // Se carga toda la configuración 

                srand(state.ubi.seed); // Empieza la aleatorización por semilla
                
                state = insertar_nombre(state); // El usuario ingresa su nombre
                
                //Verificación de estadisticas (Para pruebas)
                /*printf("Datos\nNombre: %s\nVida: %d\nAtaque: %d\nDefensa: %d\nExperiencia: %d\nPiso: %d\nSala: %d\n",
                state.player.nombre,state.player.vida,state.player.ataque,state.player.defensa,
                state.player.exp,state.ubi.piso,state.ubi.sala);*/ 
                
                printf("Una vez en la mazmorra notas algo moviendose en la oscuridad...\n");
                printf("Piensas en volver, más tu cometido no te permite retroceder.\n");

                state = avanzarSala(state);
                state = SeleccionarEnemigo(state);

                printf("¡%s ha aparecido!\n", state.oponente[0].name);

            break;

            case 2:
                
                printf("No hay archivos para cargar.\n");
                
            break;

            case 3:
                    printf("No hay puntuaciones por mostrar.\n");
            break;

            case 4:
                inmenu = false;
            break;

            default:
                printf("ERROR! Opcion no valida.\n");
            break;

        }

        if(inmenu == true){
            printf("Seleccione otra opción.");
            scanf("%d", &op);
        }

    }

return 0;
}

Gamestate cargar_config(Gamestate state){ // Cargado de la configuración general de juego
    FILE *config;

    config = fopen("config.txt", "r");

    if (config == NULL){
        printf("ERROR. No se pudo abrir config.txt");
        return state;
    }

    //Cargado de datos, Semilla, Cantidad de Pisos, Total de Salas, Stats y "ubicación" del jugador
    fscanf(config, "%d %d %d",&state.ubi.seed,&state.ubi.cant_pisos,&state.ubi.total_salas); 
    fscanf(config, "%d %d %d %d %d %d",&state.player.vida,&state.player.ataque,&state.player.defensa,
        &state.player.exp,&state.ubi.piso,&state.ubi.sala);
    fclose(config);
    return state;
}

void cargar_bestiario(Entities bestiary){ // Cargado de los Monstruos
    FILE *bestiario;

    bestiario = fopen("bestiary.txt", "r");
    
    if (bestiario == NULL){
        printf("ERROR. No se pudo abrir bestiary.txt");
        return;
    }

    for (int i = 0; i < 10; i++){
        char linea[100];
        fgets(bestiary[i].name, sizeof bestiary[i].name, bestiario);
        bestiary[i].name[strcspn(bestiary[i].name, "\r\n")] = '\0';

        fgets(linea, sizeof linea, bestiario);
        sscanf(linea, "%d %d %d %d %d",
        &bestiary[i].id,
        &bestiary[i].hp,
        &bestiary[i].atk,
        &bestiary[i].def,
        &bestiary[i].rarity);
    }
    
    fclose(bestiario);
}

void cargar_objeto(Items objetos){ // Cargado de los Objetos
    FILE *lista_objetos;

    lista_objetos = fopen("items.txt", "r");
    
    if (lista_objetos == NULL){
        printf("ERROR. No se pudo abrir items.txt");
        return;
    }

    for (int i = 0; i < 5; i++){
        char linea[100];
        fgets(objetos[i].name, sizeof objetos[i].name, lista_objetos);
        objetos[i].name[strcspn(objetos[i].name, "\r\n")] = '\0';
             
        fgets(linea, sizeof linea, lista_objetos);
        sscanf(linea, "%d %d %19s",
        &objetos[i].id,
        &objetos[i].value,
        objetos[i].effect);
                
    }
    
    fclose(lista_objetos);
}

Gamestate SeleccionarEnemigo(Gamestate state){
    Entities enemigo_posible;
    int cantidad_posible = 0, id_random;
    state.ubi.sala = 1;

    if(state.ubi.sala == 4){
        for (int i = 0; i < 10; i++){
            if(state.bestiary[i].rarity == 3){ // Sólo si la rareza del monstruo es 3 (Épico)
                enemigo_posible[cantidad_posible] = state.bestiary[i];
                cantidad_posible++;
            }
        }
    }

    if(state.ubi.sala == 3){
        for (int i = 0; i < 10; i++){
            if(state.bestiary[i].rarity == 2){ // Sólo si la rareza del monstruo es 2 (Raro)
                enemigo_posible[cantidad_posible] = state.bestiary[i];
                cantidad_posible++;
            }
        }
    } else if(state.ubi.sala < 3){

        for (int i = 0; i < 10; i++){
            if(state.bestiary[i].rarity == 1){ // Sólo si la rareza del monstruo es 1 (Común)
                enemigo_posible[cantidad_posible] = state.bestiary[i];
                cantidad_posible++;
            }
        }
    }

    if (cantidad_posible > 0) {
        int indice_random = rand() % cantidad_posible;
        state.oponente[0] = enemigo_posible[indice_random];
    }
return state;
}

Gamestate avanzarSala (Gamestate state){
    state.ubi.sala = state.ubi.sala + 1;

    return state;
}

void menu(){
    printf("<===========================================>\n");
    printf("[1] - Iniciar nuevo juego\n");
    printf("[2] - Continuar\n");
    printf("[3] - Highscores\n");
    printf("[4] - Salir\n");
    printf("<===========================================>\n");
}

void menu_fueradecombate(){
    printf("<===========================================>\n");
    printf("[1] - Avanzar\n");
    printf("[2] - Mochila\n");
    printf("[3] - Guardar y salir\n");
    printf("<===========================================>\n");
}

void menu_combate(){

    
    printf("<===========================================>\n");
    printf("[1] - Atacar\n");
    printf("[2] - Mochila\n");
    printf("[3] - Huir\n");
    printf("<===========================================>\n");
}

void iniciarCombate(Gamestate state){

}

int calcularDaño(int ataque_atacante, int defensa_defensor){ // En desarrollo

}

void otorgarRecompensa(Gamestate state, Items objetos, int total_objetos, Entities oponente){

}

void verificarSubidaNivel(Gamestate state){
    
}

Gamestate insertar_nombre(Gamestate state) { // Nombre del jugador
    int d;
    while ((d = getchar()) != '\n' && d != EOF);
    
    printf("Ingrese el nombre del jugador\n");

    fgets(state.player.nombre, sizeof(state.player.nombre), stdin);
    state.player.nombre[strcspn(state.player.nombre, "\n")] = '\0';
    return state;
}