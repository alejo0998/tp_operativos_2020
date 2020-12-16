#include "list.h"
#include "config_app.h"
#include <commons/string.h>
#include <stdlib.h>
#include "shared_utils.h"
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include "config_app.h"

typedef struct t_repartidor t_repartidor;

typedef struct {
    int id_pedido;
    char* restaurante;
    char* cliente;
    double estimacion;
    double estimacion_anterior;
    int rafaga_anterior;
    int ciclos_espera;
    double valorHRRN;
    t_repartidor* repartidor_actual;
} t_pcb;

List suscriptores_cpu;
sem_t* sem_pcb_new;
List pcb_new;
sem_t* sem_pcb_ready;
List pcb_ready;
sem_t* sem_grado_multiprocesamiento;
List pedidos_terminados;
sem_t* sem_ciclo_espera_HRRN;

typedef struct {
    int posx;
    int posy;
} t_posicion;

typedef struct {
    t_posicion posicion_restaurante;
    t_posicion posicion_cliente;
} t_pedido;

struct t_repartidor {
    int id;
    t_posicion posicion;
    int frecuencia_de_descanso;
    int tiempo_de_descanso;
    sem_t* nuevo_pedido;
    sem_t* espera_pedido;
    sem_t* ciclo_cpu;
    t_pedido* pedido;
    int cansancio;
    t_pcb* pcb_actual;
};

List repartidores_libres;
sem_t* sem_entrenador_libre;
sem_t* sem_mutex_sjf;

//MUEVO LOS STRUCT PARA UTILIZARLOS ACA
typedef struct {
    int socket;
    char* nombre_restaurante;
    List platos;
    t_posicion posicion;
    char* ip;
    char* puerto;
} t_restaurante;

typedef struct {
    int socket;
    char* id_cliente;
    t_posicion posicion;
    t_restaurante* restaurante;
} t_cliente;

typedef struct {
    char* restaurante;
    char* id_pedido;
    sem_t* semaforo;
} t_pedido_espera;

int id_pedido_default;
sem_t* sem_pedido_default;
sem_t* sem_id_pedido;

//MUEVO LAS LISTAS PARA UTILIZARLAS ACA
List lista_clientes;
List lista_restaurantes;
List lista_semaforos_pedidos;

//MUEVO LAS FUNCIONES PARA UTILIZARLAS ACA
t_restaurante* buscar_restaurante_lista(char* nombre_restaurante);
t_cliente* buscar_cliente_lista(char* id_cliente);
t_pedido_espera* buscar_pedido_espera(char* id_pedido, char*);
void eliminar_pedido_espera(char* id_pedido, char*);
void enviar_final_pedido(char*, int);
t_pcb* crear_pcb(char* restaurante, int id_pedido, char* id_cliente);
int comparar_platos(r_obtener_pedido*);