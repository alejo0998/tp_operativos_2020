#ifndef SINDICATO_CONFIG_H
#define SINDICATO_CONFIG_H

#include "sindicato_commons.h"

// VARIABLES Y ESTRUCTURAS
typedef struct {
    char* puerto_escucha;
    char* punto_montaje;
    char* ruta_log;
    char* block_size;
    char* blocks;
    char* magic_number;
    char* ip;
} t_sindicato_config;

t_sindicato_config* sindicato_config;
t_log* logger;

// FUNCIONES
void sindicato_init(t_sindicato_config** sindicato_config, t_log** logger);
void sindicato_finally(t_sindicato_config* sindicato_config, t_log* logger);
t_sindicato_config* sindicato_config_loader(char* path_config_file);
void sindicato_config_parser(t_config* config, t_sindicato_config* sindicato_config);
void sindicato_destroy(t_sindicato_config* sindicato_config);

#endif
