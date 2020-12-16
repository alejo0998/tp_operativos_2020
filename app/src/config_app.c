#include "config_app.h"

t_app_config* app_config_loader(char* path_config_file, t_log** logger) {
    t_config* config = config_create(path_config_file);
    t_app_config* app_config = malloc(sizeof(t_app_config));

    app_config_parser(config, app_config);
    config_destroy(config);

    //INiCIALIZO MODULO DE COMANDA
    modulo_comanda.ip = app_config->ip_comanda;
    modulo_comanda.puerto = app_config->puerto_comanda;
    modulo_comanda.identificacion = "APP";
    modulo_comanda.socket = 0;

    *logger = init_logger(app_config->ruta_log, "APP", LOG_LEVEL_INFO);

    return app_config;
}


void app_config_parser(t_config* config, t_app_config* app_config) {
    app_config->ip_comanda = strdup(config_get_string_value(config, "IP_COMANDA"));
    app_config->puerto_comanda = strdup(config_get_string_value(config, "PUERTO_COMANDA"));
    app_config->puerto_escucha = strdup(config_get_string_value(config, "PUERTO_ESCUCHA"));
    app_config->grado_multiprocesamiento = config_get_int_value(config, "GRADO_DE_MULTIPROCESAMIENTO");
    app_config->algoritmo_planificacion = strdup(config_get_string_value(config, "ALGORITMO_DE_PLANIFICACION"));
    app_config->alpha = config_get_double_value(config, "ALPHA");
    app_config->estimacion_inicial = config_get_int_value(config, "ESTIMACION_INICIAL");
    app_config->repartidores = config_get_array_value(config, "REPARTIDORES");
    app_config->frecuencia_descanso = config_get_array_value(config, "FRECUENCIA_DE_DESCANSO");
    app_config->tiempo_descanso = config_get_array_value(config, "TIEMPO_DE_DESCANSO");
    app_config->ruta_log = strdup(config_get_string_value(config, "ARCHIVO_LOG"));
    app_config->platos_default = config_get_array_value(config, "PLATOS_DEFAULT");
    app_config->posicion_rest_default_x = config_get_int_value(config, "POSICION_REST_DEFAULT_X");
    app_config->posicion_rest_default_y = config_get_int_value(config, "POSICION_REST_DEFAULT_Y");
    app_config->retardo_ciclo_cpu = config_get_int_value(config, "RETARDO_CICLO_CPU");
}

void app_destroy(t_app_config* app_config) {
    free(app_config->ip_comanda);
    free(app_config->puerto_comanda);
    free(app_config->puerto_escucha);
    free(app_config->algoritmo_planificacion);
    free(app_config->ruta_log);
    free(app_config);
}