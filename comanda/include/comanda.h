#ifndef COMANDA_H
#define COMANDA_H
#include <stdio.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <stdbool.h>
#include <signal.h>
#include "shared_utils.h"
#include "server.h"
#include "tests.h"
#include "api.h"
#include "pedidos_mensajes.h" //Por handle_client


// VARIABLES Y ESTRUCTURAS
typedef struct {
    char* puerto_escucha;
    char* esquema_memoria;
    int tamanio_memoria;
    int tamanio_swap;
    int frecuencia_compactacion;
    char* algoritmo_reemplazo;
    char* ruta_log;
} t_comanda_config;

t_comanda_config* comanda_config;

// FUNCIONES
void comanda_init(t_comanda_config** comanda_config, t_log** logger);
void comanda_finally(t_comanda_config* comanda_config, t_log* logger);
t_comanda_config* comanda_config_loader(char* path_config_file);
void comanda_config_parser(t_config* config, t_comanda_config* comanda_config);
void comanda_destroy(t_comanda_config* comanda_config);

#endif