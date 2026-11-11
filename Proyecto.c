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
    int vida, ataque, defensa, exp, nivel;
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

typedef struct {
    Jugador player;
    int piso;
    int cuartos;
    int enemigos;
    int objetos;
} Resultado;

typedef Items Catalogo [NOBJETOS];

typedef Entity Entities [NMONSTRUOS];

typedef struct{
    Jugador player;
    Ubicacion ubi;
    Entities bestiary, oponente;
    Catalogo objetos, inv;
}Gamestate;

Gamestate cargar_config(Gamestate state); 

void cargar_bestiario(Entity *bestiary); 

void cargar_objeto(Items *objetos);

void menu();

void menu_combate(Gamestate state);

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

int cargarPartida(Gamestate *state);

void registrar_highscore(Gamestate state,int victoria,int cuartos_explorados,int enemigos_derrotados,int objetos_consumidos);

void mostrar_highscores();
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
void guardar_partida(EstadoJuego *estado); //desarrollo
int cargar_partida(EstadoJuego *estado); 
void registrar_highscore(EstadoJuego estado, bool victoria);
*/

int main(){

    Gamestate state = {0};
    int op, carga;
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
                state.player.nivel=1;
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
                carga = cargarPartida(&state);
                if (cargada == 1) {
                    printf("Continuando la partida...\n");
                } else {
                    printf("No se pudo cargar la partida.\n");
                }
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
            remove("partida_guardada.txt"); //elimina la partida guardada
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

void mostrar_highscores(void) {
    FILE *archivo;

    /* Arreglo que conserva las ultimas cinco victorias leidas. */
    Resultado ultimas[5];

    /* Registro auxiliar para leer una partida del archivo. */
    Resultado actual;

    /* Arreglo de caracteres para leer una linea a la vez. */
    StringLargo linea;

    int victoria; 
    int cantidad; 
    int correcto; 
    int i;        

    cantidad = 0;
    correcto = 1;

    archivo = fopen("highscore.txt", "r");

    if (archivo == NULL) {
        printf("No hay resultados disponibles ");
        printf("o no se pudo abrir highscore.txt.\n");
    } else {
        while (
            correcto == 1 &&
            fgets(linea, sizeof(linea), archivo) != NULL
        ) {
            //Primera linea: convierte el texto en un entero.Debe obtener exactamente un valor.//
             
            if (sscanf(linea, "%d", &victoria) != 1) {
                correcto = 0;
            } else {
                if (victoria != 0 && victoria != 1) {
                    correcto = 0;
                }
            }

            /* Segunda linea: nombre del jugador. */
            if (correcto == 1) {
                if (fgets(linea, sizeof(linea), archivo) != NULL) {

                    /* Elimina el salto de linea del texto */
                    linea[strcspn(linea, "\r\n")] = '\0';
                    //validacion
                    if (
                        strlen(linea) <
                        sizeof(actual.player.nombre)
                    ) {
                        strcpy(actual.player.nombre, linea);
                    } else {
                        correcto = 0;
                    }
                } else {
                    correcto = 0;
                }
            }

            //estadisticas de la partida//
            if (correcto == 1) {
                if (fgets(linea, sizeof(linea), archivo) != NULL) {
                    if (sscanf(linea,"%d %d %d %d %d %d %d %d %d",&actual.piso,&actual.cuartos,&actual.enemigos,&actual.objetos,&actual.player.vida,
                        &actual.player.ataque,&actual.player.defensa,&actual.player.nivel,&actual.player.exp) != 9) {
                        /* No se pudieron obtener los nueve datos. */
                        correcto = 0;
                    }
                } else {
                    correcto = 0;
                }
            }

            if (correcto == 1 && victoria == 1) {
                if (cantidad < 5) {

                    /* Todavia queda una posicion disponible. */
                    ultimas[cantidad] = actual;
                    cantidad++;

                } else {
                    /*
                     * Si ya hay cinco victorias, desplaza
                     * las posiciones y descarta la mas antigua.
                     */
                    for (i = 0; i < 4; i++) {
                        ultimas[i] = ultimas[i + 1];
                    }

                    /* Guarda la nueva victoria al final. */
                    ultimas[4] = actual;
                }
            }
        }

        if (ferror(archivo)) {
            correcto = 0;
        }
        fclose(archivo);

        if (correcto == 0) {
            printf("\nEl archivo contiene datos incompletos ");
            printf("o no se pudo leer correctamente.\n");
            printf("Se mostraran los resultados validos ");
            printf("leidos hasta ese punto.\n");
        }

        if (cantidad == 0) {
            printf("\nNo hay victorias disponibles para mostrar.\n");
        } else {
            printf("\n===== ULTIMAS CINCO VICTORIAS =====\n");

            for (i = cantidad - 1; i >= 0; i--) {
                printf("\n----------------------------------\n");

                printf(
                    "Nombre: %s\n",
                    ultimas[i].player.nombre
                );

                printf("Piso alcanzado: %d\n", ultimas[i].piso);
                printf("Cuartos explorados: %d\n", ultimas[i].cuartos);
                printf("Enemigos derrotados: %d\n", ultimas[i].enemigos);
                printf("Objetos consumidos: %d\n", ultimas[i].objetos);

                printf("Vida final: %d\n", ultimas[i].player.vida);
                printf("Ataque final: %d\n", ultimas[i].player.ataque);
                printf("Defensa final: %d\n", ultimas[i].player.defensa);
                printf("Nivel final: %d\n", ultimas[i].player.nivel);
                printf("Experiencia: %d\n", ultimas[i].player.exp);
            }

            printf("----------------------------------\n");
        }
    }
}
void mostrar_highscores(void) {
    FILE *archivo;
    Resultado ultimas[5];
    Resultado actual;
    StringLargo linea;
    int victoria;
    int cantidad;
    int correcto;
    int i;

    cantidad = 0;
    correcto = 1;

    archivo = fopen("highscore.txt", "r");

    if (archivo == NULL) {
        printf("No hay resultados disponibles ");
        printf("o no se pudo abrir highscore.txt.\n");
    } else {
        while (
            correcto == 1 &&
            fgets(linea, sizeof(linea), archivo) != NULL
        ) {
            if (sscanf(linea, "%d", &victoria) != 1) {
                correcto = 0;
            } else {
                if (victoria != 0 && victoria != 1) {
                    correcto = 0;
                }
            }

            if (correcto == 1) {
                if (fgets(linea, sizeof(linea), archivo) != NULL) {
                    linea[strcspn(linea, "\r\n")] = '\0';

                    if (strlen(linea) < sizeof(actual.nombre)) {
                        strcpy(actual.nombre, linea);
                    } else {
                        correcto = 0;
                    }
                } else {
                    correcto = 0;
                }
            }

            if (correcto == 1) {
                if (fgets(linea, sizeof(linea), archivo) != NULL) {
                    if (
                        sscanf(
                            linea,
                            "%d %d %d %d %d %d %d %d %d",
                            &actual.piso,
                            &actual.cuartos,
                            &actual.enemigos,
                            &actual.objetos,
                            &actual.vida,
                            &actual.ataque,
                            &actual.defensa,
                            &actual.nivel,
                            &actual.exp
                        ) != 9
                    ) {
                        correcto = 0;
                    }
                } else {
                    correcto = 0;
                }
            }

            if (correcto == 1 && victoria == 1) {
                if (cantidad < 5) {
                    ultimas[cantidad] = actual;
                    cantidad++;
                } else {
                    for (i = 0; i < 4; i++) {
                        ultimas[i] = ultimas[i + 1];
                    }

                    ultimas[4] = actual;
                }
            }
        }

        if (ferror(archivo)) {
            correcto = 0;
        }

        fclose(archivo);

        if (correcto == 0) {
            printf("\nEl archivo contiene datos incompletos ");
            printf("o no se pudo leer correctamente.\n");
            printf("Se mostraran los resultados validos ");
            printf("leidos hasta ese punto.\n");
        }

        if (cantidad == 0) {
            printf("\nNo hay victorias disponibles para mostrar.\n");
        } else {
            printf("\n===== ULTIMAS CINCO VICTORIAS =====\n");

            for (i = cantidad - 1; i >= 0; i--) {
                printf("\n----------------------------------\n");
                printf("Nombre: %s\n", ultimas[i].nombre);
                printf("Piso alcanzado: %d\n", ultimas[i].piso);
                printf("Cuartos explorados: %d\n", ultimas[i].cuartos);
                printf("Enemigos derrotados: %d\n", ultimas[i].enemigos);
                printf("Objetos consumidos: %d\n", ultimas[i].objetos);
                printf("Vida final: %d\n", ultimas[i].vida);
                printf("Ataque final: %d\n", ultimas[i].ataque);
                printf("Defensa final: %d\n", ultimas[i].defensa);
                printf("Nivel final: %d\n", ultimas[i].nivel);
                printf("Experiencia: %d\n", ultimas[i].exp);
            }

            printf("----------------------------------\n");
        }
    }
}

void mostrar_highscores(void) {
    FILE *archivo;

    //conserva las ultimas cinco victorias leidas//
    Resultado ultimas[5];

    // Registro auxiliar //
    Resultado actual;
    StringLargo linea;

    int victoria; 
    int cantidad; 
    int correcto; 
    int i;       
    cantidad = 0;
    correcto = 1;

    archivo = fopen("highscore.txt", "r");

    if (archivo == NULL) {
        printf("No hay resultados disponibles ");
        printf("o no se pudo abrir highscore.txt.\n");
    } else {
        while (
            correcto == 1 &&
            fgets(linea, sizeof(linea), archivo) != NULL
        ) {
            //convierte el texto en un entero.* Debe obtener exactamente un valor.//
            if (sscanf(linea, "%d", &victoria) != 1) {
                correcto = 0;
            } else {
                if (victoria != 0 && victoria != 1) {
                    correcto = 0;
                }
            }

            /* Segunda linea: nombre del jugador. */
            if (correcto == 1) {
                if (fgets(linea, sizeof(linea), archivo) != NULL) {
                    linea[strcspn(linea, "\r\n")] = '\0';

                    // Comprueba que el nombre quepa en el arreglo
                    if (
                        strlen(linea) <
                        sizeof(actual.player.nombre)
                    ) {
                        strcpy(actual.player.nombre, linea);
                    } else {
                        correcto = 0;
                    }
                } else {
                    /* El registro esta incompleto. */
                    correcto = 0;
                }
            }

            /* Tercera linea: estadisticas de la partida. */
            if (correcto == 1) {
                if (fgets(linea, sizeof(linea), archivo) != NULL) {
                    //indica la direccion de cada campo donde sscanf debe almacenar un numero.
                    if (
                        sscanf(
                            linea,
                            "%d %d %d %d %d %d %d %d %d",
                            &actual.piso,
                            &actual.cuartos,
                            &actual.enemigos,
                            &actual.objetos,
                            &actual.player.vida,
                            &actual.player.ataque,
                            &actual.player.defensa,
                            &actual.player.nivel,
                            &actual.player.exp
                        ) != 9
                    ) {
                        /* No se pudieron obtener los nueve datos. */
                        correcto = 0;
                    }
                } else {
                    correcto = 0;
                }
            }

            /*
             * Solo agrega registros completos de victorias.
             * Las derrotas se leen, pero no se muestran.
             */
            if (correcto == 1 && victoria == 1) {
                if (cantidad < 5) {

                    /* Todavia queda una posicion disponible. */
                    ultimas[cantidad] = actual;
                    cantidad++;

                } else {
                    /*
                     * Si ya hay cinco victorias, desplaza
                     * las posiciones y descarta la mas antigua.
                     */
                    for (i = 0; i < 4; i++) {
                        ultimas[i] = ultimas[i + 1];
                    }

                    /* Guarda la nueva victoria al final. */
                    ultimas[4] = actual;
                }
            }
        }

        /* Distingue un error de lectura de un fin normal del archivo. */
        if (ferror(archivo)) {
            correcto = 0;
        }

        /* El archivo ya no se necesita para mostrar los datos. */
        fclose(archivo);

        if (correcto == 0) {
            printf("\nEl archivo contiene datos incompletos ");
            printf("o no se pudo leer correctamente.\n");
            printf("Se mostraran los resultados validos ");
            printf("leidos hasta ese punto.\n");
        }

        if (cantidad == 0) {
            printf("\nNo hay victorias disponibles para mostrar.\n");
        } else {
            printf("\n===== ULTIMAS CINCO VICTORIAS =====\n");

            /*
             * Recorre desde la ultima posicion ocupada
             * hasta la primera para mostrar primero
             * la victoria mas reciente.
             */
            for (i = cantidad - 1; i >= 0; i--) {
                printf("\n----------------------------------\n");

                printf(
                    "Nombre: %s\n",
                    ultimas[i].player.nombre
                );

                printf("Piso alcanzado: %d\n", ultimas[i].piso);
                printf("Cuartos explorados: %d\n", ultimas[i].cuartos);
                printf("Enemigos derrotados: %d\n", ultimas[i].enemigos);
                printf("Objetos consumidos: %d\n", ultimas[i].objetos);

                printf("Vida final: %d\n", ultimas[i].player.vida);
                printf("Ataque final: %d\n", ultimas[i].player.ataque);
                printf("Defensa final: %d\n", ultimas[i].player.defensa);
                printf("Nivel final: %d\n", ultimas[i].player.nivel);
                printf("Experiencia: %d\n", ultimas[i].player.exp);
            }

            printf("----------------------------------\n");
        }
    }
}

    
}
void guardar_partida(Gamestate estado){
    FILE *salida;
    int i;
    int error;

    salida = fopen("partida_guardada.txt", "w");

    if (salida == NULL) {
         printf("ERROR. No se pudo abrir el archivo de guardado.\n");

    } else {

        /* Atributos: vida, ataque y defensa*/
        fprintf(salida,"%d %d %d\n",estado.player.vida,estado.player.ataque,estado.player.defensa);

        /* Nivel actual y experiencia*/
        fprintf(salida,"%d %d\n",estado.player.exp,estado.player.nivel);

        /* Ubicacion: piso y cuarto*/
        fprintf(salida,"%d %d\n",estado.ubi.piso,estado.ubi.sala);

        /* Inventario: guardar cada posicion del arreglo */
        for (i = 0; i < NOBJETOS; i++) {
            fprintf(salida, "%s\n",estado.inv[i].name);
            fprintf(salida, "%s\n",estado.inv[i].effect);

            fprintf(salida,"%d %d\n",estado.inv[i].id,estado.inv[i].value);
        }

        error = ferror(salida);

        if (fclose(salida) != 0) {
            error = 1;
        }

        if (error == 0) {
            printf("Partida guardada correctamente.\n");
        } else {
            printf("ERROR. No se pudo guardar toda la partida.\n");
        }
    }

}

int cargarPartida(Gamestate *state)
{
    FILE *archivo;
    Gamestate temporal;
    StringLargo linea;
    StringLargo nombre;
    StringLargo efecto;
    int correcto;
    int cargada;
    int i;

    cargada = 0;

    archivo = fopen("partida_guardada.txt", "r");

    if (archivo == NULL) {
        printf("No hay partida guardada o no se pudo abrir.\n");
    } else {
        temporal = *state;
        correcto = 1;

        /* Leer vida, ataque y defensa. */
        if (fgets(linea, sizeof linea, archivo) != NULL) { //guarda texto en linea
            if (sscanf(linea,"%d %d %d",&temporal.player.vida,&temporal.player.ataque,
                &temporal.player.defensa) != 3){ //guarda el texo en la variable corresponiente 
                correcto = 0; //confirmacion de que contiene los elementos esperados
            }
        } else {
            correcto = 0; // no tiene los elementos esperados
        }

        /* Leer experiencia */
        if (correcto == 1) {
            if (fgets(linea, sizeof linea, archivo) != NULL) { 
                if (sscanf(linea,"%d",&temporal.player.exp,&temporal.player.nivel) != 2) {
                    correcto = 0;
                }
            } else {
                correcto = 0;
            }
        }
        /* Leer piso y cuarto. */
        if (correcto == 1) {
            if (fgets(linea, sizeof linea, archivo) != NULL) {
                if (sscanf(linea,"%d %d",&temporal.ubi.piso,&temporal.ubi.sala) != 2) {
                    correcto = 0;
                }
            } else {
                correcto = 0;
            }
        }

        /* Leer cada posicion del inventario. */
        for (i = 0; i < NOBJETOS && correcto == 1; i++) {

            if (fgets(nombre, sizeof nombre, archivo) != NULL &&
                fgets(efecto, sizeof efecto, archivo) != NULL &&
                fgets(linea, sizeof linea, archivo) != NULL) {

                nombre[strcspn(nombre, "\r\n")] = '\0';
                efecto[strcspn(efecto, "\r\n")] = '\0';

                if (strlen(nombre) < sizeof temporal.inv[i].name &&
                    strlen(efecto) < sizeof temporal.inv[i].effect) {

                    strcpy(temporal.inv[i].name, nombre);
                    strcpy(temporal.inv[i].effect, efecto);

                    if (sscanf(linea,"%d %d",&temporal.inv[i].id,&temporal.inv[i].value) != 2) {
                        correcto = 0;
                    }
                } else {
                    correcto = 0;
                }
            } else {
                correcto = 0;
            }
        }

        /* Validaciones basicas. */
        if (correcto == 1) {
            if (temporal.player.vida <= 0 ||
                temporal.player.ataque < 0 ||
                temporal.player.defensa < 0 ||
                temporal.player.exp < 0 ||
                temporal.player.nivel < 1 ||
                temporal.ubi.piso < 1 ||
                temporal.ubi.piso > temporal.ubi.cant_pisos ||
                temporal.ubi.sala < 0) {

                correcto = 0;
            }
        }

        fclose(archivo);

        if (correcto == 1) {
            *state = temporal;
            cargada = 1;

            printf("Partida cargada correctamente.\n");
        } else {
            printf("El archivo esta incompleto o contiene datos invalidos.\n");
        }
    }

    return cargada;
}
