#include <stdio.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>
#include "shared_utils.h"
#include "server.h"
#include "tests.h"
#include "api.h"
#include "memoria_principal.h" //Por handle de memoria

typedef struct mensaje{
    int socket;
    char* restaurant;
    char* idPedido;
    char* plato;
    char* cantidadPlato;

} l_mensaje;


void handle_client(t_result* result);
void handle_guardar_pedidos(t_result*);
void handle_guardar_plato(t_result*);
void handle_confirmar_pedido(t_result *);
void handle_plato_listo(t_result*);
void handle_obtener_pedido(t_result *);

void handle_finalizar_pedido(t_result *);

int existe_restaurante(char* restaurante);

int guardar_plato_en_memoria(char* nombreResto, char* idPedido, char* cantidadPlato, char* plato);
int guardar_pedido_en_memoria(char* restaurante, char* id_pedido);
int confirmar_pedido_en_memoria(char* restaurante, char* id_pedido);
l_segmento* obtener_pedido_en_memoria(char* nombreResto, char* id_pedido);
int plato_listo_en_memoria(char* restaurante, char* id_pedido, char* plato);
int finalizar_pedido_en_memoria(char* restaurante, char* id_pedido);
