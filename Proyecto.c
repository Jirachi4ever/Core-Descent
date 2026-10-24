/*Integrantes: Eydan Bastidas C.I: 31.677.825 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef char StringLargo[100];
typedef char StringCorto [20];

typedef struct{
    StringCorto nombre;
    int vida, ataque, defensa, exp, piso;
}Jugador;

typedef struct{
    int piso_actual, sala_actual;
}ubicacion;

void cargar_config(); // En desarrollo

void cargar_bestiario(); // En desarrollo

void cargar_objeto(); // En desarrollo

void menu();

void menu_combate();

void menu_fueradecombate();

void seed(int semilla);

void insertar_nombre (Jugador *x);

/*

Modulos por configurar y concretar

Módulo 2:
void avanzarCuarto (EstadoJuego *estado, Entidad bestiario[], 
int total_enemigos, Objeto catalogo[], int total_objetos);
Entidad SeleccionarEnemigo(Entidad bestiario[], int total_enemigos, 
int es_cuarto_final);

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

    int op, seed_final;
    StringLargo semilla;
    FILE *guardado,*highscore;
    Jugador x;

    menu();
    guardado = fopen("partida_guardada.txt", "r");
    scanf("%d", &op);

    switch(op)
    {
        case 1:

            int c;
            while ((c = getchar()) != '\n' && c != EOF);

            srand(time(NULL));
            printf("Ingrese una semilla (Enter para generar una aleatoria)\n");
            fgets(semilla, sizeof(semilla), stdin);
            semilla[strcspn(semilla, "\n")] = '\0';

            if (strlen(semilla) == 0) {
                seed_final = rand() % 10000 + 1;
                printf("\nNo ingresaste ninguna seed. Se genero una automatica: %u\n", seed_final);
            } else {
                seed_final = (unsigned int)atoi(semilla);        
                printf("\nSe utilizara la seed ingresada: %u\n", seed_final);
            }

            seed(seed_final);
            insertar_nombre(&x);
            x.vida = 100, x.ataque = 5, x.defensa = 5, x.exp = 0, x.piso = 1;
            printf("Perfil:\nNombre: %s\nVida: %d\nAtaque: %d\nDefensa: %d\nExperiencia: %d\nPiso: %d\n", x.nombre, x.vida, x.ataque, x.defensa, x.exp, x.piso);

        break;

        case 2:
            if (guardado == NULL){
                printf("No hay archivos para cargar.\n");
            } else {

            }
        break;

        case 3:
            if (highscore == NULL){
                printf("No hay puntuaciones por mostrar.\n");
            } else {

            }
        break;

        case 4:
            
        break;

        default:
            printf("ERROR! Opcion no valida.\n");
        break;

    }

return 0;
}

void cargar_config(){

}

void cargar_bestiario(){

}

void cargar_objeto(){
    
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

void insertar_nombre(Jugador *x) {
    int d;
    while ((d = getchar()) != '\n' && d != EOF);
    
    printf("Ingrese el nombre del jugador\n");

    fgets(x->nombre, sizeof(x->nombre), stdin);
    x->nombre[strcspn(x->nombre, "\n")] = '\0';
}

void seed(int seed_final){
    int seed1, i;
    seed1 = seed_final;
    printf("Ejemplo de seed: %d\n", seed1);
    srand(seed1);
    for (i = 0; i < 5; i++) {
        printf("%d ", rand() % 100 + 1);
    }
    printf("\n\n");
}