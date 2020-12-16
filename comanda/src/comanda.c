#include "comanda.h"

int main(int argc, char *argv[]){

    comanda_init(&comanda_config, &logger);
	
    initlist(&tablaRestaurantes);
    initlist(&tablaFrames);
    initlist(&tablaSwap);
    initlist(&pilaPaginasAlgoritmos);

    double tamanioMemoriaPrincipal = comanda_config->tamanio_memoria;
    double tamanioMemoriaSwap = comanda_config->tamanio_swap;

    tamanioBitMapPrincipal = (tamanioMemoriaPrincipal/256)*8;
    tamanioBitMapSwap = (tamanioMemoriaSwap/256)*8;

	punteroBitMapSwap = malloc(max(1, comanda_config->tamanio_swap/256));
	punteroBitMap = malloc(max(1, comanda_config->tamanio_memoria/256));
    
	bitMap = bitarray_create_with_mode(punteroBitMap, max(1, (comanda_config->tamanio_memoria/256)), MSB_FIRST);
    bitMapSwap = bitarray_create_with_mode(punteroBitMapSwap, max(1, (comanda_config->tamanio_swap/256)), MSB_FIRST);

    puntero_memoria_principal = malloc(comanda_config->tamanio_memoria);

    algoritmo = comanda_config->algoritmo_reemplazo;
    tamanioMemoria = comanda_config->tamanio_memoria;
    tamanioSwap = comanda_config->tamanio_swap;

    semaforo_contador = malloc(sizeof(sem_t));
    sem_init(semaforo_contador, 0, 0);

    sem_mutex_algoritmos = malloc(sizeof(sem_t));
    sem_init(sem_mutex_algoritmos, 0, 1);

    sem_ocupar_frame = malloc(sizeof(sem_t));
    sem_init(sem_ocupar_frame, 0, 1);

    sem_mutex_swap_libre = malloc(sizeof(sem_t));
    sem_init(sem_mutex_swap_libre, 0, 1);

    sem_mutex_eliminar_segmento = malloc(sizeof(sem_t));
    sem_init(sem_mutex_eliminar_segmento, 0, 1);

    sem_mutex_num_pagina = malloc(sizeof(sem_t));
    sem_init(sem_mutex_num_pagina, 0, 1);
    
    inicioClockMej = 1;
    
    printf("Imprimiendo el path %s", comanda_config->ruta_log);

    signal(SIGUSR1,&imprimirBitMap);

    iniciarMemoria();

    iniciarMemoriaSwap();
    
    iniciar_servidor("127.0.0.1", "5001", handle_client);

    close(archivoSwap);

    comanda_finally(comanda_config, logger);
    return 0;
}

void comanda_init(t_comanda_config** comanda_config, t_log** logger){
    *comanda_config = comanda_config_loader("./cfg/comanda.config");
    *logger = init_logger((*comanda_config)->ruta_log, "comanda", LOG_LEVEL_INFO);
  
}

void comanda_finally(t_comanda_config* comanda_config, t_log* logger) {

    comanda_destroy(comanda_config);
    log_destroy(logger);
}

t_comanda_config* comanda_config_loader(char* path_config_file) {
    t_config* config = config_create(path_config_file);
    t_comanda_config* comanda_config = malloc(sizeof(t_comanda_config));

    comanda_config_parser(config, comanda_config);
    config_destroy(config);

    return comanda_config;
}

void comanda_config_parser(t_config* config, t_comanda_config* comanda_config) {
    comanda_config->puerto_escucha = strdup(config_get_string_value(config, "PUERTO_ESCUCHA"));
    comanda_config->esquema_memoria = strdup(config_get_string_value(config, "ESQUEMA_MEMORIA"));
    comanda_config->tamanio_memoria = config_get_int_value(config, "TAMANIO_MEMORIA");
    comanda_config->tamanio_swap = config_get_int_value(config, "TAMANIO_SWAP");
    comanda_config->frecuencia_compactacion = config_get_int_value(config, "FRECUENCIA_COMPACTACION");
    comanda_config->algoritmo_reemplazo = strdup(config_get_string_value(config, "ALGORITMO_REEMPLAZO"));
    comanda_config->ruta_log = strdup(config_get_string_value(config, "ARCHIVO_LOG"));
}

void comanda_destroy(t_comanda_config* comanda_config) {
    free(comanda_config->puerto_escucha);
    free(comanda_config->esquema_memoria);
    free(comanda_config->algoritmo_reemplazo);
    free(comanda_config->ruta_log);
    free(comanda_config);
}

