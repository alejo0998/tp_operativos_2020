#ifndef PLANIFICACION_H
#define PLANIFICACION_H
#include <stdio.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <stdbool.h>
#include <semaphore.h>
#include "server.h"
#include "tests.h"
#include "shared_utils.h"
#include "api.h"


//VARIABLES GLOBALES
List afinidades;
List platos_restaurante;
List lista_pedidos;
List lista_controladora_ready;
char* pos_x;
char* pos_y;
int cantidad_hornos;
int cantidad_pedidos;
int cantidad_platos;
int pid;
int cantidad_cocineros;
int socket_sindicato;
int socket_app;
t_modulo modulo_sindicato;
t_modulo modulo_app;
List recetas_precios;
sem_t* sem_exec;
sem_t* sem_block;
sem_t* sem_ready;
sem_t* sem_horno_libre;
sem_t* sem_finalizar_pedido;
int handshake_app_r;
int handshake_sindicato_r;



typedef enum estado_proceso{
    NEW = 0,
    READY = 1,
    EXEC = 2,
    BLOCKED = 3,
    BLOCKED_SUSPENDED = 4,
    EXIT = 5
} estado_proceso_t;


typedef struct {
    
    char* nombre;
    List* pasos;
    int cantidad_total;
    int cantidad_listo;
 
} t_plato;


typedef struct {

    int id_pedido;
    List platos;

} t_pedido;



typedef struct{

    char* afinidad;
    List pcb_espera;
    List cocineros;
    sem_t* sem_cocinero_libre;
    sem_t* sem_pcb_espera;
    
}t_ready;

typedef struct{

    int id_pedido;
    char* afinidad;
    int pid;
    estado_proceso_t estado;
    t_plato* plato;
    sem_t* ciclo_cpu;

} t_pcb;

typedef struct{
    int ocupado;
    t_pcb* pcb;
    sem_t* sem_horno;
} t_horno;


typedef struct{

    int ocupado;
    char* afinidad;
    t_pcb* pcb;
    sem_t* semaforo_exec;

}t_exec;

typedef struct{

    t_pcb* pcb;
    t_exec* exec;

}t_controlar_ready;

typedef struct {
    char* puerto_escucha;
    char* ip_sindicato;
    char* puerto_sindicato;
    char* ip_app;
    char* puerto_app;
    int quantum;
    char* ruta_log;
    char* algoritmo_planificador;
    char* nombre_restaurante;
    char* ip_escucha;
    int retardo_ciclo_cpu;
} t_restaurante_config;

t_restaurante_config* restaurante_config;

List colas_ready;
List colas_exit;
List colas_pcb;
List colas_exec;
List hornos;
List pcb_espera_horno;
List suscriptores_cpu;

void controlador_hornos();
void inicializar_colas();
void inicializar_colas_ready();
void inicializar_colas_exec();
void inicializar_colas_io();
void asignar_pcb_cocinero(t_pcb* pcb, t_exec* cocinero);
void controlador_ready(t_ready* cola);
void controlador_a_ready(t_ready* cola);
void desuscribirse_clock(sem_t* ciclo_cpu);
void clock_cpu();
void iniciar_clock();

void paso_ready(t_pcb* pcb);
int paso_exit(t_pcb* pcb);
void paso_exec(t_exec* cocinero);
void paso_a_exec_FIFO(t_exec* cocinero);
void paso_a_exec_RR(t_exec* cocinero);
int pedidos_finalizados();
int plato_general(char* nombre_plato);
t_ready* cola_por_afinidad(char* afinidad);
void paso_block(t_horno* horno);
void paso_a_block(t_horno* horno);
t_ready* cola_ready_cocinero(char* afinidad);
int es_paso_io(t_paso* paso);
int termino_pedido(int id_pedido);
int cola_ready_creada(char* afinidad);
char* obtener_estado(int estado);
char* afinidad_por_nombre_plato(char* nombre);

void planificacion();

t_log* logger;

t_exec* crear_exec();
t_ready* cola_ready_pcb(t_pcb* pcb);
t_paso* crear_paso(char* nombre_paso, int ciclo_cpu);
t_plato* crear_plato(char* nombre, List* pasos, int pedido_id, int cantidad_total, int cantidad_listo, int pid);
t_pedido* creacion_pedido(int id, List* platos);
t_pcb* crear_pcb(int id_pedido,int pid,t_plato* plato, char* afinidad);

#endif
