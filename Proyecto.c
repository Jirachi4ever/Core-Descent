/* 11/09/2026 Integrantes: Eydan Bastidas C.I: 31.677.825-Victoria Torres C.I: 32.768.115 seccion: 2*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>

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
    float prom_sala;
}Ubicacion;

typedef struct {
    StringCorto name;
    int id, hp, atk, def, rarity;
}Entity;

typedef struct{
    StringCorto name, effect;
    int id, value;
}Items;

typedef Items Catalogo [NOBJETOS];

typedef Entity Entities [NMONSTRUOS];

typedef struct{
    Jugador player;
    Ubicacion ubi;
    Entities bestiary, oponente;
    Catalogo objetos, inv;
}Gamestate;

Gamestate cargar_config(); 

void cargar_bestiario(); 

void cargar_objeto();

void menu();

void menu_combate();

void menu_fueradecombate();

void menu_inventario(Gamestate state);

void seed(int semilla);

Gamestate insertar_nombre (Gamestate state);

Gamestate SeleccionarEnemigo(Gamestate state);

Gamestate avanzarSala (Gamestate state);

void iniciarCombate(Gamestate state, bool incombat);

int calcularDaño(int ataque_atacante, int defensa_defensor); // Falta Critico

Gamestate otorgarRecompensa(Gamestate state, Catalogo objetos, Entities oponente, int total_inv); // En desarrollo

void verificarSubidaNivel(Gamestate state); // En desarrollo

void guardar_partida(Gamestate estado); // faltan pruebas

/*

Modulos por configurar y concretar

Módulo 3 (Actualmente en desarrollo):
void iniciarCombate(Estadojuego *estado, Entidad *enemigo); // Completo
int calcularDaño(int ataque_atacante, int defensa_defensor); // Completo
void otorgarRecompensa(EstadoJuego *estado, Objeto catalogo[], 
int total_objetos, char tipo_enemigo[]); // Completo
void verificarSubidaNivel(EstadoJuego *estado); // En desarrollo

Módulo 4:
void usar_objeto(EstadoJuego *estado);

Módulo 5:
void guardar_partida(EstadoJuego *estado);
int cargar_partida(EstadoJuego *estado);
void registrar_highscore(EstadoJuego estado, bool victoria);
*/

int main(){

    Gamestate state = {0};
    int op;
    StringLargo s;
    bool inmenu = true, incombat = false;

    cargar_bestiario(state.bestiary);
    cargar_objeto(state.objetos);

    /* Revisión de Bestiario (Para pruebas)
    for(int i; i < NMONSTRUOS; i++){
        printf("%s %d %d %d %d %d\n",
        state.bestiary[i].name,
        state.bestiary[i].id,
        state.bestiary[i].hp,
        state.bestiary[i].def,
        state.bestiary[i].atk,
        state.bestiary[i].rarity);
    }*/

    /* Revisión de Objetos (Para pruebas)
    for(int i = 0; i < NOBJETOS; i++){
        printf("%s %d %d %s\n",
        state.objetos[i].name,
        state.objetos[i].id,
        state.objetos[i].value,
        state.objetos[i].effect);
    }*/
    menu();
    scanf("%d", &op);


    while(inmenu){
        switch(op)
        {
            case 1:// nueva partida
                inmenu = false;
                state = cargar_config(state); // Se carga toda la configuración

                srand(state.ubi.seed); // Empieza la aleatorización por semilla
                
                state = insertar_nombre(state); // El usuario ingresa su nombre
                
                //Verificación de estadisticas (Para pruebas)
                printf("Datos\nNombre: %s\nVida: %d\nAtaque: %d\nDefensa: %d\nExperiencia: %d\nPiso: %d\nSala: %d\n",
                state.player.nombre,state.player.vida,state.player.ataque,state.player.defensa,
                state.player.exp,state.ubi.piso,state.ubi.sala);
                
                printf("Una vez en la mazmorra notas algo moviendose en la oscuridad...\n");
                printf("Piensas en volver, más tu cometido no te permite retroceder.\n");

                state = avanzarSala(state);

                state = SeleccionarEnemigo(state);

                printf("¡%s ha aparecido!\n", state.oponente[0].name);

                iniciarCombate(state,incombat);

            break;

            case 2: //cargar partida
                
              guardar_partida(state);
                
            break;

            case 3://highscore
                    printf("No hay puntuaciones por mostrar.\n");
            break;

            case 4://salir
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

    for (int i = 0; i < NMONSTRUOS; i++){
        StringLargo linea;
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

void cargar_objeto(Catalogo objetos){ // Cargado de los Objetos
    FILE *lista_objetos;

    lista_objetos = fopen("items.txt", "r");
    
    if (lista_objetos == NULL){
        printf("ERROR. No se pudo abrir items.txt");
        return;
    }

    for (int i = 0; i < NOBJETOS; i++){
        StringLargo linea;
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
    state.ubi.prom_sala = state.ubi.total_salas / state.ubi.cant_pisos;

    if(state.ubi.sala == state.ubi.prom_sala + 1){
        for (int i = 0; i < NMONSTRUOS; i++){
            if(state.bestiary[i].rarity == 3){ // Sólo si la rareza del monstruo es 3 (Épico)
                enemigo_posible[cantidad_posible] = state.bestiary[i];
                cantidad_posible++;
            }
        }
    }

    if(state.ubi.sala == state.ubi.prom_sala){
        for (int i = 0; i < NMONSTRUOS; i++){
            if(state.bestiary[i].rarity == 2){ // Sólo si la rareza del monstruo es 2 (Raro)
                enemigo_posible[cantidad_posible] = state.bestiary[i];
                cantidad_posible++;
            }
        }
    } else if(state.ubi.sala < state.ubi.prom_sala){

        for (int i = 0; i < NMONSTRUOS; i++){
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
    if(state.ubi.sala == state.ubi.prom_sala){
        state.ubi.piso = state.ubi.piso + 1;
        state.ubi.sala = 0;
    } else {
        state.ubi.sala = state.ubi.sala + 1;
    }

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

void menu_combate(Gamestate state){

    printf("<===========================================>\n");
    printf("%s: %d HP    %s: %d HP\n",state.player.nombre,state.player.vida
    ,state.oponente[0].name,state.oponente[0].hp);
    printf("<===========================================>\n");
    printf("[1] - Atacar\n");
    printf("[2] - Mochila\n");
    printf("[3] - Huir\n");
    printf("<===========================================>\n");
}

void menu_inventario(Gamestate state){

    for (int k = 0; k < 3; k++){
        if(state.inv[k].name[0] == '\0'){
            strcpy(state.inv[k].name, "Vacio");
        }
    }

    printf("<===========================================>\n");
    printf("[1] - %s\n", state.inv[0].name);
    printf("[2] - %s\n", state.inv[1].name);
    printf("[3] - %s\n", state.inv[2].name);
    printf("[4] - Salir\n");
    printf("<===========================================>\n");

}

void iniciarCombate(Gamestate state, bool incombat){ // En desarrollo
    incombat = true;
    int op2, atk_atacante, def_defensor, daño_infligido, total_inv = 3;

    menu_combate(state);
    scanf("%d", &op2);

    while (incombat){
        
        switch (op2)

        {
        case 1:

            atk_atacante = state.player.ataque, def_defensor = state.oponente[0].def;
            daño_infligido = calcularDaño(atk_atacante, def_defensor);
            state.oponente[0].hp = state.oponente[0].hp - daño_infligido;

            printf("¡Has infligido %d puntos de daño!\n", daño_infligido);

            atk_atacante = state.oponente[0].atk, def_defensor = state.player.defensa;
            daño_infligido = calcularDaño(atk_atacante, def_defensor);
            state.player.vida = state.player.vida - daño_infligido;

            printf("¡Has recibido %d puntos de daño!\n", daño_infligido);

            break;

        case 2:

            menu_inventario(state);

        break;

        case 3:

        break;
        
        default:
            break;
        }

        if(state.player.vida == 0 || state.oponente[0].hp == 0){
            incombat = false;

            if(state.player.vida == 0){printf("Has sido derrotado...\n");}
            if(state.oponente[0].hp == 0){printf("¡Has salido victorioso!\n");}

            break;
        }

    menu_combate(state);
    scanf("%d", &op2);
    }

}

int calcularDaño(int atk_atacante, int def_defensor){ // Probabilidad de critico por añadir (En desarrollo)
    int damage, defensa_real = (def_defensor > 1) ? def_defensor : 1;
    int base_dmg = 5;

    damage = (base_dmg * (atk_atacante/defensa_real));

    if (damage <= 0){
        damage = 1;
    }

    if ((rand() % 16) == 5){
        damage = damage * 2;
    }

    return damage;
}

Gamestate otorgarRecompensa(Gamestate state, Catalogo objetos, Entities oponente, int total_inv){

    bool recompensado = false;
    int id_random;

    if(state.oponente[0].rarity == 2){
        for(int l = 0; l < total_inv; l++){
            if (state.inv[l].id == '\0' && !recompensado){
                id_random = rand() % NOBJETOS;
                for (int m = 0; m < NOBJETOS; m++){
                    if(id_random == state.objetos[m].id){
                        state.inv[l] = state.objetos[m];
                    }
                }
                recompensado = true;    
            }
        }
    }

    if(state.oponente[0].rarity == 1){
        id_random = (rand() % 100) + 1;
        if(id_random <= 25){
            for(int l = 0; l < 3; l++){
                if (state.inv[l].id == '\0' && !recompensado){
                    id_random = rand() % NOBJETOS;
                    for (int m = 0; m < NOBJETOS; m++){
                        if(id_random == state.objetos[m].id){
                            state.inv[l] = state.objetos[m];
                        }
                    }
                    recompensado = true;    
                }
            }
        }
    }
    
}

void verificarSubidaNivel(Gamestate state){
    
}

Gamestate insertar_nombre(Gamestate state) { // Nombre del jugador
    int d;
    bool solo_espacios = true;
    while ((d = getchar()) != '\n' && d != EOF);
   
    printf("Ingrese el nombre del jugador\n");

    fgets(state.player.nombre, sizeof(state.player.nombre), stdin);
    state.player.nombre[strcspn(state.player.nombre, "\n")] = '\0';

    for (int i = 0; state.player.nombre[i] != '\0'; i++) {
        if (!isspace((unsigned char)state.player.nombre[i])) {
            solo_espacios = false;
            break;
        }
    }

    if (solo_espacios) {
        strcpy(state.player.nombre, "Sin nombre");
    }

    return state;
}
void guardar_partida(Gamestate estado){
    FILE *salida;
    int i;
    int error;

    archivo = fopen("partida_guardada.txt", "w");

    if (salida == NULL) {
         printf("ERROR. No se pudo abrir el archivo de guardado.\n");
        return;
    }

    /* Guardar nombre y estadisticas del jugador */
        fprintf(salida, "%s\n", estado.player.nombre);

        fprintf(archivo,"%d %d %d %d\n", estado.player.vida, estado.player.ataque,
            estado.player.defensa,estado.player.exp);

    /* Guardar configuracion y ubicacion */
        fprintf(salida,"%d %d %d %d %d %.9g\n",estado.ubi.seed, estado.ubi.piso,estado.ubi.sala,
            estado.ubi.cant_pisos,estado.ubi.total_salas,estado.ubi.prom_sala);

    /* Guardar el bestiario */
        for (i = 0; i < NMONSTRUOS; i++) {
            fprintf(salida, "%s\n", estado->bestiary[i].name);

            fprintf(salida,"%d %d %d %d %d\n",estado.bestiary[i].id,estado.bestiary[i].hp,estado.bestiary[i].atk,
                estado.bestiary[i].def,estado.bestiary[i].rarity);
    }

    /* Guardar el arreglo de oponentes */
        for (i = 0; i < NMONSTRUOS; i++) {
            fprintf(salida, "%s\n", estado->oponente[i].name);

            fprintf(salida,"%d %d %d %d %d\n",estado.oponente[i].id,estado.oponente[i].hp,estado.oponente[i].atk,
                estado.oponente[i].def,estado.oponente[i].rarity);
    }

    /* Guardar el catalogo de objetos */
        for (i = 0; i < NOBJETOS; i++) {
            fprintf(salida, "%s\n", estado.objetos[i].name);
            fprintf(salida, "%s\n", estado.objetos[i].effect);
            fprintf(salida,"%d %d\n",estado.objetos[i].id,estado.objetos[i].value);
    }

    /* Guardar todos los espacios del inventario */
        for (i = 0; i < NOBJETOS; i++) {
            fprintf(salida, "%s\n", estado.inv[i].name);
            fprintf(salida, "%s\n", estado.inv[i].effect);
            fprint(salida,"%d %d\n",estado.inv[i].id,estado.inv[i].value);
    }
    /* Revisar si alguna escritura fallo */
        error = ferror(salida);

    /* Cerrar siempre el archivo, incluso si hubo un error */
        if (fclose(salida) != 0) {
            error = 1;
        }

        if (error) {
            printf("ERROR. No se pudo guardar toda la partida.\n");
        } else {
            printf("Partida guardada correctamente.\n");
        }
    }
}
