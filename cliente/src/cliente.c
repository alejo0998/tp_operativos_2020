#include "cliente.h"

int main(int argc, char *argv[]){
    
    cliente_init(&cliente_config, &logger);
    log_info(logger, "Soy el CLIENTE! %s", mi_funcion_compartida());
    
   
    for (int i = 0; i < argc; i++){
        printf("%s\n", argv[i]);
    }

    t_modulo* modulo = get_modulo_config();

    if (modulo == NULL){
        printf("%s\n", "El modulo indicado no existe");
        cliente_finally(cliente_config, logger);
        return -1;
    }


    int resultado = handshake(modulo);

    printf("HANDSHAKE %d",resultado);

    if (resultado != 0){
        printf("%s\n", "No se pudo realizar la conexion con el cliente");
        cliente_finally(cliente_config, logger);
        return -1;
    }


    leer_consola(logger,modulo);

    cliente_finally(cliente_config, logger);

    return 0;
}

void cliente_init(t_cliente_config** cliente_config, t_log** logger){
    *cliente_config = cliente_config_loader("./cfg/cliente.config");
    *logger = init_logger((*cliente_config)->ruta_log, "CLIENTE", LOG_LEVEL_INFO);
}

void cliente_finally(t_cliente_config* cliente_config, t_log* logger) {
    cliente_destroy(cliente_config);
    log_destroy(logger);
}

t_cliente_config* cliente_config_loader(char* path_config_file) {
    t_config* config = config_create(path_config_file);
    t_cliente_config* cliente_config = malloc(sizeof(t_cliente_config));

    cliente_config_parser(config, cliente_config);
    config_destroy(config);
//
    return cliente_config;
}

void cliente_config_parser(t_config* config, t_cliente_config* cliente_config) {
    cliente_config->ip = strdup(config_get_string_value(config, "IP"));
    cliente_config->puerto = strdup(config_get_string_value(config, "PUERTO"));
    cliente_config->ruta_log = strdup(config_get_string_value(config, "ARCHIVO_LOG"));
    cliente_config->posicion_x = config_get_int_value(config, "POCISION_X");
    cliente_config->posicion_y = config_get_int_value(config, "POCISION_Y");
    cliente_config->id_cliente = strdup(config_get_string_value(config, "ID_CLIENTE"));
    
}

void cliente_destroy(t_cliente_config* cliente_config) {
    free(cliente_config->ip);
    free(cliente_config->puerto);
    free(cliente_config->ruta_log);
    free(cliente_config->id_cliente);
    free(cliente_config);
}

t_modulo* get_modulo_config(){
    return crear_modulo(cliente_config->ip, cliente_config->puerto);
}

t_modulo * crear_modulo(char* ip, char* puerto){
    t_modulo* modulo = malloc(sizeof(t_modulo));
    modulo->ip = ip;
    modulo->puerto = puerto;
    modulo->socket = 0;
    modulo->identificacion = cliente_config->id_cliente;
    return modulo;
}

int handshake(t_modulo* modulo){

    char* mensajes[4] = {string_itoa(handshake_cliente), cliente_config->id_cliente, string_itoa(cliente_config->posicion_x), string_itoa(cliente_config->posicion_y)};

    int socket = send_messages_and_return_socket(modulo->identificacion, modulo->ip, modulo->puerto, mensajes, 4);

    if (socket == -1){
        return -1;
    }

    char * mensaje = receive_simple_message(socket);

    if (mensaje == NULL){
        return -1;
    }

    printf("El handshake con el modulo fue correcto\n");

    escuchar_mensajes_socket_desacoplado(socket);
 
    return 0;
}

void escuchar_mensajes_socket_desacoplado(int socket){
    
    pthread_t thread;
    t_parameter* parametro = malloc(sizeof(t_parameter));

	parametro->socket = socket;
	parametro->f = handle_client;

	pthread_create(&thread,NULL,(void*)escuchar_mensajes_socket, parametro);
	pthread_detach(thread);

}

void escuchar_mensajes_socket(t_parameter* parametro){
    escuchar_socket_sin_conexion(&parametro->socket, parametro->f);
}

void handle_client(t_result* result){

    for(int i = 0; i < *result->mensajes->size; i++){
        printf("%s", result->mensajes->mensajes[i]);
    }
    printf("\n");

}

