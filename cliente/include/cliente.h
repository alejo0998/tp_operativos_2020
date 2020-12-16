#ifndef CLIENTE_H
#define CLIENTE_H
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <stdbool.h>
#include "mensajes_consola.h"

// VARIABLES Y ESTRUCTURAS
typedef struct {
    char* ip;
    char* puerto;
    char* ruta_log;
    int posicion_x;
    int posicion_y;
    char* id_cliente;
} t_cliente_config;

t_cliente_config* cliente_config;
t_log* logger;

// FUNCIONES
void cliente_init(t_cliente_config** cliente_config, t_log** logger);
void cliente_finally(t_cliente_config* cliente_config, t_log* logger);
t_cliente_config* cliente_config_loader(char* path_config_file);
void cliente_config_parser(t_config* config, t_cliente_config* cliente_config);
void cliente_destroy(t_cliente_config* cliente_config);
t_modulo * crear_modulo(char* ip, char* puerto);
t_modulo* get_modulo_config();
int handshake(t_modulo* modulo);
void escuchar_mensajes_socket_desacoplado(int socket);
void escuchar_mensajes_socket(t_parameter* parametro);
void handle_client(t_result* result);

#endif