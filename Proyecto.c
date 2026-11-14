/* 11/09/2026 Integrantes: Eydan Bastidas C.I: 31.677.825-Victoria Torres C.I: 32.768.115 seccion: 2*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

typedef char StringLargo[100];
typedef char StringCorto [30];

enum {NMONSTRUOS = 11};
enum {NOBJETOS = 6};

typedef struct{
    StringCorto nombre;
    int vida, ataque, defensa, nivel, exp, exp_cap;
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

typedef struct{
    StringCorto nombre;
    int puntuacion;
}Highscore;

Gamestate cargar_config(Gamestate state, bool *valida); 

void cargar_bestiario(Entities bestiary);

Gamestate cargar_config(Gamestate state, bool *valida);

void menu();

void menu_highscores(void);

void menu_combate(Gamestate state, int vida_actual);

void menu_fueradecombate();

Gamestate fueradecombate(Gamestate state, int combat_victory, bool *ingame);

void menu_inventario(Gamestate state);

Gamestate mochila(Gamestate state, bool en_combate, int *vida_actual, int vida_max);

void seed(int semilla);

Gamestate insertar_nombre (Gamestate state);

Gamestate SeleccionarEnemigo(Gamestate state);

Gamestate avanzarSala (Gamestate state, int combat_victory);

Gamestate iniciarCombate(Gamestate state, int *victory);

int calcularDaño(int ataque_atacante, int defensa_defensor);

Gamestate otorgarRecompensa(Gamestate state); // Completo (Por testear)

Gamestate verificarSubidaNivel(Gamestate state); // En desarrollo

Gamestate usar_objeto(Gamestate state, int ranura, bool en_combate, int *vida_actual, int vida_max);

void guardar_partida(Gamestate state, bool *guardado);

Gamestate cargar_partida(Gamestate state, bool *cargada);

void jugar(Gamestate state);

void registrar_highscore(Gamestate state, bool victoria);

void cargar_objeto(Catalogo objetos);
/*

Modulos por configurar y concretar

Módulo 3 (Fase de Pruebas):
void iniciarCombate(Estadojuego *estado, Entidad *enemigo); // Completo
int calcularDaño(int ataque_atacante, int defensa_defensor); // Completo
void otorgarRecompensa(EstadoJuego *estado, Objeto catalogo[], 
int total_objetos, char tipo_enemigo[]); // Completo
void verificarSubidaNivel(EstadoJuego *estado); // Completo

Módulo 4 (Fase de Pruebas):
void usar_objeto(EstadoJuego *estado); // Completo

Módulo 5 (Fase de Pruebas):
void guardar_partida(EstadoJuego *estado);
int cargar_partida(EstadoJuego *estado);
void registrar_highscore(EstadoJuego estado, bool victoria);
*/

int main(){

    Gamestate state = {0};
    int op;
    bool main_menu = true, cargada, config_valida;

    cargar_bestiario(state.bestiary);
    cargar_objeto(state.objetos);    

    /* Revisión de Bestiario (Para pruebas)
    for(int i; i < NMONSTRUOS; i++){
        printf("%s %d %d %d %d %d %d %d%d %d %d %d\n",
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


    while(main_menu){
        switch(op)
        {
            case 1:
                state = cargar_config(state, &config_valida); // Se carga toda la configuración

                if (!config_valida) {
                break;
                }

                state.ubi.prom_sala = state.ubi.total_salas / state.ubi.cant_pisos;
                main_menu = false;

                srand(state.ubi.seed); // Empieza la aleatorización por semilla
                
                state = insertar_nombre(state); // El usuario ingresa su nombre
                
                /*Verificación de estadisticas (Para pruebas)
                printf("Datos\nNombre: %s\nVida: %d\nAtaque: %d\nDefensa: %d\nNivel: %d\nExperiencia: %d\nPiso: %d\nSala: %d\nCap: %d\n",
                state.player.nombre,state.player.vida,state.player.ataque,state.player.defensa,
                state.player.nivel,state.player.exp,state.ubi.piso,state.ubi.sala,state.player.exp_cap);*/
                
                jugar(state);
            break;

            case 2:
                state = cargar_partida(state, &cargada);
                if (cargada) {
                    main_menu = false;
                    srand(state.ubi.seed);
                    jugar(state);
                }
            break;

            case 3:
                menu_highscores();
            break;

            case 4:
                main_menu = false;
            break;

            default:
                printf("ERROR! Opcion no valida.\n");
            break;

        }

        if(main_menu == true){
            printf("Seleccione otra opción.");
            scanf("%d", &op);
        }
        

    }

return 0;
}

Gamestate cargar_config(Gamestate state, bool *valida){ // Cargado de la configuración general de juego
    FILE *config;
    *valida = false;
    config = fopen("config.txt", "r");

    if (config == NULL){
        printf("ERROR. No se pudo abrir config.txt");
        return state;
    }

    int leidos1 = fscanf(config, "%d %d %d",&state.ubi.seed,&state.ubi.cant_pisos,&state.ubi.total_salas);
    int leidos2 = fscanf(config, "%d %d %d %d %d %d %d %d",&state.player.vida,&state.player.ataque,&state.player.defensa,
        &state.player.nivel,&state.player.exp,&state.ubi.piso,&state.ubi.sala,&state.player.exp_cap);

    fclose(config);

    if (leidos1 != 3 || leidos2 != 8) {
        printf("config.txt está incompleto o tiene un formato inválido.\n");
  
        return state;
    }

    if (state.ubi.cant_pisos <= 0 || // validación de datos
        state.ubi.total_salas <= 0 ||
        state.ubi.total_salas % state.ubi.cant_pisos != 0 ||
        state.player.vida <= 0 ||
        state.player.ataque <= 0 ||
        state.player.defensa < 0 ||
        state.player.nivel < 1 ||
        state.player.exp < 0 ||
        state.player.exp_cap <= 0) {
        printf("config.txt contiene valores inválidos.\n");
        return state;
    }

    //Cargado de datos, Semilla, Cantidad de Pisos, Total de Salas, Stats y "ubicación" del jugador
     
    
    *valida = true;
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
    int cantidad_posible = 0;
    state.ubi.prom_sala = state.ubi.total_salas / state.ubi.cant_pisos;

    if(state.ubi.sala == state.ubi.prom_sala + 1){
        for (int i = 0; i < NMONSTRUOS; i++){
            if(state.bestiary[i].rarity == 3){ // Sólo si la rareza del monstruo es 3 (Épico)
                enemigo_posible[cantidad_posible] = state.bestiary[i];
                cantidad_posible++;
            }
        }
    } else if((state.ubi.piso == 1) && (state.ubi.sala == state.ubi.prom_sala - 1)){
        for (int i = 0; i < NMONSTRUOS; i++){
            if(state.bestiary[i].rarity == 2){ // Sólo si la rareza del monstruo es 2 (Raro)
                enemigo_posible[cantidad_posible] = state.bestiary[i];
                cantidad_posible++;
            }
        }
    } else if(state.ubi.sala == state.ubi.prom_sala){
        for (int i = 0; i < NMONSTRUOS; i++){
            if(state.bestiary[i].rarity == 2){ // Sólo si la rareza del monstruo es 2 (Raro)
                enemigo_posible[cantidad_posible] = state.bestiary[i];
                cantidad_posible++;
            }
        }
    } else {
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

Gamestate avanzarSala (Gamestate state, int combat_victory){

    if((state.ubi.piso == 1) && (state.ubi.sala == 0)){
        state.ubi.sala = state.ubi.sala + 1;
        printf("Avanzas una sala...\n");
        return state;
    }

    if(combat_victory == 1){
        if((state.ubi.piso == 1) && (state.ubi.sala == state.ubi.prom_sala - 1)){
            state.ubi.sala = 1;
            state.ubi.piso = state.ubi.piso + 1;
            printf("Avanzas al siguiente piso...\n");
        }else if (state.ubi.piso == state.ubi.cant_pisos) {
            state.ubi.sala = state.ubi.sala + 1;
            printf("Te adentras en la sala del guardián...\n");
            printf("Al entrar notas un miasma que nubla tus sentidos... \n");
        }else if(state.ubi.sala == state.ubi.prom_sala){
            state.ubi.piso = state.ubi.piso + 1;
            state.ubi.sala = 1;
            printf("Avanzas al siguiente piso...\n");
        } else {
            state.ubi.sala = state.ubi.sala + 1;
            printf("Avanzas una sala...\n");
        }
    }
    
    printf("Piso: %d Sala: %d \n", state.ubi.piso, state.ubi.sala);
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

Gamestate fueradecombate(Gamestate state, int combat_victory, bool *ingame){
    int op;
    bool inmenu = true, guardado;

    while(inmenu){
        menu_fueradecombate();
        scanf("%d", &op);

        switch (op)
        {
        case 1:

            state = avanzarSala(state, combat_victory);
            inmenu = false;

        break;

        case 2:

            state = mochila(state, false, NULL, 0);
            
        break;

        case 3:

            guardar_partida(state, &guardado);
            if(guardado){
                inmenu = false;
                *ingame = false;
            }

        break;

        default:

            printf("ERROR! Opcion no valida.\n");

        break;
        }
    }
return state;
}

void menu_combate(Gamestate state, int vida_actual){

    printf("<===========================================>\n");
    printf("%s: %d HP    %s: %d HP\n",
           state.player.nombre, vida_actual,
           state.oponente[0].name, state.oponente[0].hp);
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

Gamestate mochila(Gamestate state, bool en_combate, int *vida_actual, int vida_max){
    int op_items;
    bool inmenu = true;

    while(inmenu){
        menu_inventario(state);
        if (scanf("%d", &op_items) != 1) {
            printf("Entrada no valida.\n");
            return state;
        }

        switch(op_items)
        {
            case 1:
            case 2:
            case 3:
                state = usar_objeto(state, op_items - 1, en_combate,
                                    vida_actual, vida_max);
                inmenu = false;
            break;

            case 4:
                inmenu = false;
            break;

            default:
                printf("ERROR! Opcion no valida.\n");            
            break;
        }
    }
    return state;
}

Gamestate usar_objeto(Gamestate state, int ranura, bool en_combate,
                      int *vida_actual, int vida_max){
    if (ranura < 0 || ranura >= 3 || state.inv[ranura].id == 0) {
        printf("No hay item para utilizar.\n");
        return state;
    }

    if (strcmp(state.inv[ranura].effect, "Healing") == 0 ||
        strcmp(state.inv[ranura].effect, "Healing_MAX") == 0) {
        if (!en_combate || vida_actual == NULL) {
            printf("Solo puedes curarte durante un combate.\n");
            return state;
        }

        if (strcmp(state.inv[ranura].effect, "Healing_MAX") == 0) {
            *vida_actual = vida_max;
        } else {
            *vida_actual += state.inv[ranura].value;
            if (*vida_actual > vida_max) {
                *vida_actual = vida_max;
            }
        }
        printf("Vida actual: %d/%d.\n", *vida_actual, vida_max);
    } else if (strcmp(state.inv[ranura].effect, "Damage_Buff") == 0) {
        if (!en_combate) {
            printf("Este objeto solo puede usarse durante un combate.\n");
            return state;
        }

        state.player.ataque += state.inv[ranura].value;
        printf("Tu ataque aumenta en %d durante este combate.\n",
               state.inv[ranura].value);
    } else if (strcmp(state.inv[ranura].effect, "Defense_Buff") == 0) {
        if (!en_combate) {
            printf("Este objeto solo puede usarse durante un combate.\n");
            return state;
        }

        state.player.defensa += state.inv[ranura].value;
        printf("Tu defensa aumenta en %d durante este combate.\n",
               state.inv[ranura].value);
    } else if (strcmp(state.inv[ranura].effect, "Buff_Max_Damage") == 0) {
        if (en_combate) {
            printf("Este objeto solo puede usarse fuera de combate.\n");
            return state;
        }

        state.player.ataque += state.inv[ranura].value;
        printf("Tu ataque aumenta en %d permanentemente.\n",
               state.inv[ranura].value);
    } else {
        printf("Efecto desconocido: %s. No se consumo el objeto.\n",
               state.inv[ranura].effect);
        return state;
    }

    memset(&state.inv[ranura], 0, sizeof(state.inv[ranura]));
    return state;
}

Gamestate iniciarCombate(Gamestate state, int *combat_victory){
    bool incombat = true;
    int op2, atk_atacante, def_defensor, daño_infligido;
    int ataque_base = state.player.ataque;
    int defensa_base = state.player.defensa;
    int vida_completa = state.player.vida;
    int vida_actual = vida_completa;
    *combat_victory = 4; // Estado de espera (para evitar bucles)

    menu_combate(state, vida_actual);
    scanf("%d", &op2);

    while (incombat){
        
        switch (op2)

        {
            case 1:

                atk_atacante = state.player.ataque, def_defensor = state.oponente[0].def;
                daño_infligido = calcularDaño(atk_atacante, def_defensor);
                state.oponente[0].hp = state.oponente[0].hp - daño_infligido;

                printf("¡Has infligido %d puntos de daño!\n", daño_infligido);

                if(state.oponente[0].hp > 0){
                    atk_atacante = state.oponente[0].atk, def_defensor = state.player.defensa;
                    daño_infligido = calcularDaño(atk_atacante, def_defensor);
                    vida_actual = vida_actual - daño_infligido;
                    printf("¡Has recibido %d puntos de daño!\n", daño_infligido);
                }

            break;

            case 2:
                state = mochila(state, true, &vida_actual, vida_completa);
        break;

            case 3:
                if(rand() %100 < 15){
                    printf("¡Has escapado con éxito!\n+");
                    *combat_victory = 2;
                } else {
                    printf("Has fallado en escapar...\n");
                    atk_atacante = state.oponente[0].atk, def_defensor = state.player.defensa;
                    daño_infligido = calcularDaño(atk_atacante, def_defensor);
                    vida_actual = vida_actual - daño_infligido;
                    printf("¡Has recibido %d puntos de daño!\n", daño_infligido);
                }

            break;
            
            default:
                printf("ERROR! Opcion no valida.\n");
            break;
        }

        if((state.oponente[0].rarity == 3) && (state.oponente[0].hp <= 0)){ // Derrota del Jefe Final
            *combat_victory = 5;
            incombat = false;
            break;
        }

        if(vida_actual <= 0) { // Pierde el jugador
            *combat_victory = 0;
            incombat = false;
            break;
        }else if (state.oponente[0].hp <= 0){ // Derrota del enemigo
            *combat_victory = 1;
            incombat = false;
            break;
        } else if (*combat_victory == 2){ // Escape exitoso
            incombat = false;
            break;
        }

    menu_combate(state, vida_actual);
    scanf("%d", &op2);
    }

    state.player.ataque = ataque_base;
    state.player.defensa = defensa_base;
    return state;
}

int calcularDaño(int atk_atacante, int def_defensor){
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

Gamestate otorgarRecompensa(Gamestate state) {
    int exp_comun = 25;
    int exp_raro = 40;

    if (state.oponente[0].rarity == 2) {
        for (int l = 0; l < 3; l++) {
            if (state.inv[l].id == 0) {
                int indice = rand() % NOBJETOS;
                state.inv[l] = state.objetos[indice];
                printf("¡Has recibido %s!\n", state.inv[l].name);
                break;
            }
        }

        state.player.exp += exp_raro;
        printf("Obtienes %d puntos de experiencia\n", exp_raro);
    }

    if (state.oponente[0].rarity == 1) {
        if (rand() % 100 < 25) {
            for (int l = 0; l < 3; l++) {
                if (state.inv[l].id == 0) {
                    int indice = rand() % NOBJETOS;
                    state.inv[l] = state.objetos[indice];
                    printf("¡Has recibido %s!\n", state.inv[l].name);
                    break;
                }
            }
        }

        state.player.exp += exp_comun;
        printf("Obtienes %d puntos de experiencia\n", exp_comun);
    }

    return verificarSubidaNivel(state);
}

Gamestate verificarSubidaNivel(Gamestate state){ // Developing

    while (state.player.exp >= state.player.exp_cap){
        state.player.exp = state.player.exp - state.player.exp_cap;
        state.player.nivel++;
        state.player.ataque = state.player.ataque + 5;
        state.player.defensa = state.player.defensa + 5;
        state.player.vida = state.player.vida + 5;
        state.player.exp_cap = state.player.exp_cap + 25;
        printf("¡Has subido de nivel! +5 a todas las estadisticas.");
    }
    return state;
}

void guardar_partida(Gamestate state, bool *guardado){
    *guardado = false;

    FILE *out = fopen("partida_guardada.txt", "w");

    if (out == NULL) {
        printf("No se pudo guardar la partida");
        return;
    }

    if (fprintf(out, "%d %d %d\n%d %d %d %d %d %d %d %d\n%d %d %d\n",
                state.ubi.seed,
                state.ubi.cant_pisos,
                state.ubi.total_salas,
                state.player.vida,
                state.player.ataque,
                state.player.defensa,
                state.player.nivel,
                state.player.exp,
                state.ubi.piso,
                state.ubi.sala,
                state.player.exp_cap,
                state.inv[0].id,
                state.inv[1].id,
                state.inv[2].id) < 0) {
        printf("Error al escribir la partida");
        fclose(out);
        return;
    }

    if(fprintf(out, "%s",state.player.nombre) < 0){
        printf("Error al escribir el nombre");
        fclose(out);
        return;
    }

    if (fclose(out) == EOF) {
        printf("Error al cerrar el archivo de partida");
        return;
    }

    printf("Partida guardada.\n");
    *guardado = true;
}

Gamestate cargar_partida(Gamestate state, bool *cargada){
    FILE *in = fopen("partida_guardada.txt", "r");
    Gamestate partida = state;
    int ids_inventario[3];
    int conversiones;

    *cargada = false;
    if (in == NULL) {
        printf("No se pudo abrir partida_guardada.txt");
        return state;
    }

    conversiones = fscanf(in,
        "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %29[^\n]",
        &partida.ubi.seed,
        &partida.ubi.cant_pisos,
        &partida.ubi.total_salas,
        &partida.player.vida,
        &partida.player.ataque,
        &partida.player.defensa,
        &partida.player.nivel,
        &partida.player.exp,
        &partida.ubi.piso,
        &partida.ubi.sala,
        &partida.player.exp_cap,
        &ids_inventario[0],
        &ids_inventario[1],
        &ids_inventario[2],
        partida.player.nombre);

    if (conversiones != 15) {
        fprintf(stderr, "El archivo de guardado esta incompleto o tiene un formato invalido.\n");
        fclose(in);
        return state;
    }

    if (partida.ubi.cant_pisos <= 0 ||
        partida.ubi.total_salas <= 0 ||
        partida.ubi.piso < 1 ||
        partida.ubi.piso > partida.ubi.cant_pisos ||
        partida.ubi.sala < 0 ||
        partida.player.vida < 0 ||
        partida.player.exp_cap <= 0) {
        fprintf(stderr, "El archivo de guardado contiene valores invalidos.\n");
        fclose(in);
        return state;
    }

    if (fclose(in) == EOF) {
        printf("Error al cerrar el archivo de guardado");
        return state;
    }

    memset(partida.inv, 0, sizeof(partida.inv));
    for (int ranura = 0; ranura < 3; ranura++) {
        if (ids_inventario[ranura] == 0) {
            continue;
        }

        bool encontrado = false;
        for (int i = 0; i < NOBJETOS; i++) {
            if (partida.objetos[i].id == ids_inventario[ranura]) {
                partida.inv[ranura] = partida.objetos[i];
                encontrado = true;
                break;
            }
        }

        if (!encontrado) {
            fprintf(stderr, "El objeto con ID %d del guardado no existe en el catalogo.\n",
                    ids_inventario[ranura]);
            return state;
        }
    }

    partida.ubi.prom_sala =
        partida.ubi.total_salas / partida.ubi.cant_pisos;
    memset(partida.oponente, 0, sizeof(partida.oponente));
    *cargada = true;
    printf("Partida cargada.\n");
    return partida;
}

void jugar(Gamestate state){
    int combat_victory = 0;
    bool ingame = true, partida_finalizada = false, victoria;

    printf("Tu cometido te insta a seguir adelante.\n");
    state = fueradecombate(state, combat_victory, &ingame);
    if (!ingame) {
        return;
    }

    state = SeleccionarEnemigo(state);

    while (ingame) {
        printf("¡%s ha aparecido!\n", state.oponente[0].name);
        state = iniciarCombate(state, &combat_victory);

        if (combat_victory == 1) {
            state = otorgarRecompensa(state);
            state = fueradecombate(state, combat_victory, &ingame);
            if (ingame) {
                state = SeleccionarEnemigo(state);
            }
        } else if (combat_victory == 0) {
            ingame = false;
            partida_finalizada = true;
            victoria = false;
        } else if (combat_victory == 5) {
            printf("¡Has cumplido tu cometido y liberado la mazmorra de todos los males!\n");
            printf("Más el mal siempre está al acecho, puede que vuelvas a adentrarte en esta oscuridad...\n");
            ingame = false;
            partida_finalizada = true;
            victoria = true;
        } else {
            state = fueradecombate(state, combat_victory, &ingame);
            if (ingame) {
                state = SeleccionarEnemigo(state);
            }
        }
    }
    if (partida_finalizada) {
        registrar_highscore(state, victoria);
        remove("partida_guardada.txt");
    }
}

void menu_highscores(void){
    FILE *highscores = fopen("highscores.txt", "r");
    StringCorto nombre[5] = {{0}};
    int puntuación[5] = {0};
    char linea[128];
    int cantidad = 0, op;
    bool inmenu = true;

    if (highscores == NULL) {
        perror("No se pudo abrir highscores.txt");
        return;
    }

    while (cantidad < 5 && fgets(linea, sizeof linea, highscores) != NULL) {
        linea[strcspn(linea, "\r\n")] = '\0';

        char *separador = strrchr(linea, ' ');
        int puntos;
        char extra;
        if (separador == NULL || separador == linea ||
            sscanf(separador + 1, "%d %c", &puntos, &extra) != 1) {
            continue;
        }

        size_t largo_nombre = (size_t)(separador - linea);
        if (largo_nombre >= sizeof nombre[cantidad]) {
            continue;
        }

        *separador = '\0';
        strcpy(nombre[cantidad], linea);
        puntuación[cantidad] = puntos;
        cantidad++;
    }

    if (ferror(highscores)) {
        fprintf(stderr, "Error al leer highscores.txt\n");
        fclose(highscores);
        return;
    }
    if (fclose(highscores) == EOF) {
        fprintf(stderr, "Error al cerrar highscores.txt\n");
        return;
    }

    printf("<===========================================>\n");
    if (cantidad == 0) {
        printf("Todavía no hay puntuaciones registradas.\n");
    }
    for (int i = 0; i < cantidad; i++) {
        printf("[%d] - Nombre: %s Puntuación: %d\n",
               i + 1, nombre[i], puntuación[i]);
    }
    printf("[6] - Salir\n");
    printf("<===========================================>\n");
    printf("Seleccione una opción: ");
    if (scanf("%d", &op) != 1) {
        fprintf(stderr, "Entrada no válida.\n");
        return;
    }

    while(inmenu){
        switch (op)
        {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
                printf("El nombre de un gran guerrero tallado en piedra...\n");
            break;
            case 6:
                inmenu = false;
            break;
        
        default:
            printf("Opción no válida.\n");
            break;
        }
        if(inmenu){
            printf("Seleccione otra opción: ");
            if (scanf("%d", &op) != 1) {
                fprintf(stderr, "Entrada no válida.\n");
                return;
            }
        }
    }
}

void registrar_highscore(Gamestate state, bool victoria){
    Highscore registros[5], nuevo;
    int cantidad = 0;
    char linea[256];

    if (!victoria) {
        return;
    }

    FILE *archivo = fopen("highscores.txt", "a+");
    if (archivo == NULL) {
        printf("No se pudo abrir highscores.txt");
        return;
    }

    rewind(archivo);
    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        if (strchr(linea, '\n') == NULL && !feof(archivo)) {
            int caracter;
            while ((caracter = fgetc(archivo)) != '\n' && caracter != EOF) {
            }
            printf("Se ignoro una linea demasiado larga de highscores.txt.\n");
            continue;
        }

        linea[strcspn(linea, "\r\n")] = '\0';
        char *separador = strrchr(linea, ' ');
        if (separador == NULL) {
            continue;
        }

        if (strcmp(separador + 1, "Derrotado") == 0) {
            continue;
        }

        if (strcmp(separador + 1, "Victorioso") == 0) {
            int estadisticas[7];
            char extra;
            bool linea_valida = true;
            *separador = '\0';

            for (int i = 6; i >= 0; i--) {
                separador = strrchr(linea, ' ');
                if (separador == NULL ||
                    sscanf(separador + 1, "%d%c", &estadisticas[i],
                           &extra) != 1) {
                    linea_valida = false;
                    break;
                }
                *separador = '\0';
            }
            if (!linea_valida || linea[0] == '\0' ||
                strlen(linea) >= sizeof(nuevo.nombre)) {
                continue;
            }

            memcpy(nuevo.nombre, linea, strlen(linea) + 1);
            nuevo.puntuacion = estadisticas[3] * 10 +
                               estadisticas[5] * 20 +
                               estadisticas[6] * 10 + 100; //
        } else {
            int puntuacion;
            char extra;
            if (sscanf(separador + 1, "%d%c", &puntuacion, &extra) != 1 ||
                separador == linea ||
                (size_t)(separador - linea) >= sizeof(nuevo.nombre)) {
                continue;
            }
            *separador = '\0';
            memcpy(nuevo.nombre, linea, (size_t)(separador - linea) + 1);
            nuevo.puntuacion = puntuacion;
        }

        if (cantidad == 5) {
            for (int i = 1; i < 5; i++) {
                registros[i - 1] = registros[i];
            }
            cantidad--;
        }
        registros[cantidad] = nuevo;
        cantidad++;
    }

    if (ferror(archivo)) {
        printf("No se pudo leer highscores.txt");
        fclose(archivo);
        return;
    }
    if (fclose(archivo) == EOF) {
        printf("No se pudo cerrar highscores.txt");
        return;
    }

    strcpy(nuevo.nombre, state.player.nombre);
    nuevo.puntuacion = state.player.nivel * 10 +
                       state.ubi.piso * 20 +
                       state.ubi.sala * 10 + 100;

    if (cantidad == 5) {
        for (int i = 1; i < 5; i++) {
            registros[i - 1] = registros[i];
        }
        cantidad--;
    }
    registros[cantidad] = nuevo;
    cantidad++;

    FILE *highscores = fopen("highscores.txt", "w");
    if (highscores == NULL) {
        printf("No se pudo abrir highscores.txt para escribir");
        return;
    }

    for (int i = 0; i < cantidad; i++) {
        if (fprintf(highscores, "%s %d\n",
                    registros[i].nombre,
                    registros[i].puntuacion) < 0) {
            printf("No se pudo escribir en highscores.txt");
            fclose(highscores);
            return;
        }
    }
    if (fclose(highscores) == EOF) {
        printf("No se pudo cerrar highscores.txt");
    }
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