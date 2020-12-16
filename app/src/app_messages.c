#include "app_messages.h"

void handle_client(t_result* result){
    if (result->operacion == MENSAJES){
        int tipo_mensaje = atoi(result->mensajes->mensajes[0]);
        switch(tipo_mensaje){
            case handshake_cliente: 
                handle_handshake_cliente(result->socket, result->mensajes->mensajes[1], atoi(result->mensajes->mensajes[2]), atoi(result->mensajes->mensajes[3]));
            break;
            case handshake_restaurante:
                handle_handshake_restaurante(result->socket, result->mensajes->mensajes[1], atoi(result->mensajes->mensajes[2]), atoi(result->mensajes->mensajes[3]), result->mensajes->mensajes[4], result->mensajes->mensajes[5]);
            break;
            case consultar_restaurantes:
                handle_consultar_restaurantes(result->socket);
            break;
            case seleccionar_restaurante:
                handle_seleccionar_restaurante(result->socket, result->mensajes->mensajes[1], result->mensajes->mensajes[2]);
            break;
            case consultar_platos: 
                handle_consultar_platos(result->socket, result->identificador_cliente);
            break;
            case crear_pedido:
                handle_crear_pedido(result->socket, result->identificador_cliente);
            break;
            case anadir_plato:
                handle_anadir_plato(result->socket, result->identificador_cliente, result->mensajes->mensajes[1], result->mensajes->mensajes[2]);
            break;
            case plato_listo:
                handle_plato_listo(result->socket, result->mensajes->mensajes[1], result->mensajes->mensajes[2], result->mensajes->mensajes[3]);
            break;
            case confirmar_pedido:
                handle_confirmar_pedido(result->socket, result->identificador_cliente, result->mensajes->mensajes[1]);
            break;
            case consultar_pedido:
                handle_consultar_pedido(result->socket, result->identificador_cliente, result->mensajes->mensajes[1]);
            break;

        }

    }
    
    return;
}

void handle_handshake_restaurante(int socket, char* nombre_restaurante, int pos_x, int pos_y, char* ip, char* puerto){
    t_restaurante* restaurante = buscar_restaurante_lista(nombre_restaurante);
            
    if (restaurante == NULL){
        restaurante = nuevo_restaurante(socket, nombre_restaurante, pos_x, pos_y, ip, puerto);
        pushbacklist(&lista_restaurantes, restaurante);
        
        if(!strcmp(nombre_restaurante, "Resto Default")){
            restaurante->platos = listaPlatosDefault;
        } else {
            t_modulo modulo_restaurante;
            modulo_restaurante.ip = ip;
            modulo_restaurante.puerto = puerto;
            modulo_restaurante.identificacion = "APP";
            modulo_restaurante.socket = 0;
            restaurante->platos = *enviar_mensaje_consultar_platos(&modulo_restaurante, nombre_restaurante);
        }
    } else {
        restaurante->socket = socket;
        restaurante->ip = string_new();
        string_append(&restaurante->ip, ip);
        restaurante->puerto = string_new();
        string_append(&restaurante->puerto, puerto);
    }
    
    send_message_socket(socket, "OK");
}

t_restaurante* nuevo_restaurante(int socket, char* nombre_restaurante, int pos_x, int pos_y, char* ip, char* puerto){
    t_restaurante* restaurante = malloc(sizeof(t_restaurante));
    restaurante->socket = socket;
    restaurante->nombre_restaurante = string_new();
    restaurante->platos = listaPlatosDefault;
    restaurante->posicion.posx = pos_x;
    restaurante->posicion.posy = pos_y;
    restaurante->ip = string_new();
    string_append(&restaurante->ip, ip);
    restaurante->puerto = string_new();
    string_append(&restaurante->puerto, puerto);
    string_append(&restaurante->nombre_restaurante, nombre_restaurante);

    return restaurante;
}

void handle_handshake_cliente(int socket, char* id_cliente, int pos_x, int pos_y){
    t_cliente* cliente = buscar_cliente_lista(id_cliente);
            
    if (cliente == NULL){
        cliente = malloc(sizeof(t_cliente));
        cliente->socket = socket;
        cliente->id_cliente = string_new();
        string_append(&cliente->id_cliente, id_cliente);
        cliente->posicion.posx = pos_x;
        cliente->posicion.posy = pos_y;
        pushbacklist(&lista_clientes, cliente);
    } else {
        cliente->socket = socket;
    }
    
    send_message_socket(socket, "OK");
}

void handle_crear_pedido(int socket, char* id_cliente){

    char* id_pedido;

    char* respuesta[1];

    t_cliente* cliente = buscar_cliente_lista(id_cliente);

    t_restaurante* restaurante = cliente->restaurante;

    if(!strcmp(restaurante->nombre_restaurante, "Resto Default")){

        id_pedido = string_itoa(obtener_id_default(restaurante));
        respuesta[0] = id_pedido;
        sem_post(sem_pedido_default);

    } else {
        id_pedido = obtener_id_pedido(restaurante);
        respuesta[0] = id_pedido;
        sem_post(sem_id_pedido);
    }

    if(strcmp(respuesta[0], "FAIL")){
        respuesta[0] = enviar_mensaje_guardar_pedido(&modulo_comanda, restaurante->nombre_restaurante, respuesta[0]);
    } if(strcmp(respuesta[0], "FAIL")){
        inicializar_pedido_semaforo(id_pedido, restaurante->nombre_restaurante);
    }
    send_messages_socket(socket, respuesta, 1);

}

int obtener_id_default(t_restaurante* restaurante){

    sem_wait(sem_pedido_default);

    id_pedido_default += 1;

    return id_pedido_default;
    

}

char* obtener_id_pedido(t_restaurante* restaurante){

    sem_wait(sem_id_pedido);

    t_modulo modulo_restaurante;
    modulo_restaurante.ip = restaurante->ip;
    modulo_restaurante.puerto = restaurante->puerto;
    modulo_restaurante.identificacion = "APP";
    modulo_restaurante.socket = 0;

    char* id_pedido = enviar_mensaje_crear_pedido(&modulo_restaurante);

    return id_pedido;
}

void handle_seleccionar_restaurante(int socket, char* cliente, char* restaurante){

    char* respuesta[1];

    if (relacionar(restaurante, cliente) != -1){
        respuesta[0] = "OK";
    } else {
        respuesta[0] = "FAIL";
    }

    send_messages_socket(socket, respuesta, 1);

}

int relacionar(char* nombre_restaurante, char* id_cliente){

    t_restaurante* restaurante = buscar_restaurante_lista(nombre_restaurante);

    if (restaurante == NULL){
        return -1;
    }

    t_cliente* cliente = buscar_cliente_lista(id_cliente);
    if (cliente == NULL){
        return -1;
    }

    cliente->restaurante = restaurante;

    return 0;
}

void handle_consultar_restaurantes(int socket){

    char* restaurantes = obtener_restaurantes();
    char* respuesta[1] = {restaurantes};

    send_messages_socket(socket, respuesta, 1);

}

char* obtener_restaurantes(){

    char * restaurantes = string_new();

    string_append(&restaurantes, "[");
    int primero = 1;
    for (IteratorList iter = beginlist(lista_restaurantes); iter != NULL; iter = nextlist(iter)){
        if (primero == 1){
            primero = 0;
        } else {
            string_append(&restaurantes, ",");
        }
        t_restaurante* restaurante = (t_restaurante*) iter->data;
        string_append(&restaurantes, restaurante->nombre_restaurante);

    }

    string_append(&restaurantes, "]");

    return restaurantes;
}

void handle_consultar_platos(int socket, char* idCliente){

    List lista_platos;
    initlist(&lista_platos);

    t_cliente* cliente = buscar_cliente_lista(idCliente);

    t_restaurante* restaurante = cliente->restaurante;

    if(!strcmp(restaurante->nombre_restaurante, "Resto Default")){

        printf("PLATOS : %s", obtener_platos(listaPlatosDefault));


        return;

    }

    t_modulo modulo_restaurante;
    modulo_restaurante.ip = restaurante->ip;
    modulo_restaurante.puerto = restaurante->puerto;
    modulo_restaurante.identificacion = "APP";
    modulo_restaurante.socket = 0;

    lista_platos = *enviar_mensaje_consultar_platos(&modulo_restaurante, NULL);
    
    
    send_message_socket(socket, obtener_platos(lista_platos));

}

char* obtener_platos(List lista){

    char * platos = string_new();

    string_append(&platos, "[");
    int primero = 1;
    for (IteratorList iter = beginlist(lista); iter != NULL; iter = nextlist(iter)){
        if (primero == 1){
            primero = 0;
        } else {
            string_append(&platos, ",");
        }
        char* nombrePlato = (char*) iter->data;
        string_append(&platos, nombrePlato);

    }

    string_append(&platos, "]");

    return platos;
}

void handle_anadir_plato(int socket, char* id_cliente, char* plato, char* id_pedido){

    char* respuesta[1];

    t_cliente* cliente = buscar_cliente_lista(id_cliente);

    t_restaurante* restaurante = cliente->restaurante;

    t_modulo modulo_restaurante;
    modulo_restaurante.ip = restaurante->ip;
    modulo_restaurante.puerto = restaurante->puerto;
    modulo_restaurante.identificacion = "APP";
    modulo_restaurante.socket = 0;

    if(!plato_en_restaurante(plato, restaurante->platos)){
        respuesta[0] = "FAIL";
        
        send_messages_socket(socket, respuesta, 1);
        return;

    }

    if(!strcmp(restaurante->nombre_restaurante, "Resto Default")){

        respuesta[0] = enviar_mensaje_guardar_plato(&modulo_comanda, restaurante->nombre_restaurante, id_pedido, plato, "1");

        send_messages_socket(socket, respuesta, 1);
        return;

    }
    
    respuesta[0] = enviar_mensaje_anadir_plato(&modulo_restaurante, plato, id_pedido);
    if(!strcmp(respuesta[0], "FAIL")){
        send_messages_socket(socket, respuesta, 1);
        return;
    }
    
    respuesta[0] = enviar_mensaje_guardar_plato(&modulo_comanda, restaurante->nombre_restaurante, id_pedido, plato, "1");
    // if(!strcmp(respuesta[0], "FAIL")){
    //     send_messages_socket(socket, respuesta, 1);
    //     return;
    // }

    send_messages_socket(socket, respuesta, 1);

}

int plato_en_restaurante(char* plato, List lista){
    
    for(IteratorList iter = beginlist(lista); iter != NULL; iter = nextlist(iter)){
        char* platoE = (char*) dataiterlist(iter);

        if(!strcmp(platoE, plato)){
            return 1;
        }
    }
    return 0;
}

void handle_plato_listo(int socket, char* restaurante, char* id_pedido, char* plato){

    char* respuesta[1];
    r_obtener_pedido* pedido;

    respuesta[0] = enviar_mensaje_plato_listo(&modulo_comanda, restaurante, id_pedido, plato);

    pedido = enviar_mensaje_obtener_pedido(&modulo_comanda, id_pedido, restaurante);

    if(pedido == NULL){
        respuesta[0] = "FAIL";
        send_messages_socket(socket, respuesta, 1);
        return;
    }

    if(comparar_platos(pedido)){
        t_pedido_espera* pedido_espera = buscar_pedido_espera(id_pedido, restaurante);
        eliminar_pedido_espera(id_pedido, restaurante);
        sem_post(pedido_espera->semaforo);
        printf("PEDIDO LISTO: %s\n", id_pedido);
    }

    send_messages_socket(socket, respuesta, 1);


}

void handle_confirmar_pedido(int socket, char* id_cliente, char* id_pedido){

    char* respuesta[1];
    // char* arrayReturn[1];

    // r_consultar_pedido pedido;
    // r_obtener_pedido *pedidoAux;

    t_cliente* cliente = buscar_cliente_lista(id_cliente);

    t_restaurante* restaurante = cliente->restaurante;

    t_modulo modulo_restaurante;
    modulo_restaurante.ip = restaurante->ip;
    modulo_restaurante.puerto = restaurante->puerto;
    modulo_restaurante.identificacion = "APP";
    modulo_restaurante.socket = 0;


    // pedidoAux = enviar_mensaje_obtener_pedido(&modulo_comanda, id_pedido, restaurante->nombre_restaurante);
    
    // if(pedidoAux != NULL){
    //     pedido.restaurante = NULL;
    //     pedido.estado = pedidoAux->estado;
    //     pedido.info_comidas = pedidoAux->info_comidas;

    //     arrayReturn[0] = armar_string_consultar_pedido(&pedido);
    // }
    if(!strcmp(restaurante->nombre_restaurante, "Resto Default")){

        respuesta[0] = enviar_mensaje_confirmar_pedido(&modulo_comanda, id_pedido, restaurante->nombre_restaurante);
            if(!strcmp(respuesta[0], "FAIL")){
                send_messages_socket(socket, respuesta, 1);
                return;
            }
        crear_pcb(restaurante->nombre_restaurante, atoi(id_pedido), id_cliente);

        send_messages_socket(socket, respuesta, 1);

        return;
    }
    
    respuesta[0] = enviar_mensaje_confirmar_pedido(&modulo_restaurante, id_pedido, restaurante->nombre_restaurante);
    if(!strcmp(respuesta[0], "FAIL")){
        send_messages_socket(socket, respuesta, 1);
        return;
    }

    respuesta[0] = enviar_mensaje_confirmar_pedido(&modulo_comanda, id_pedido, restaurante->nombre_restaurante);
    if(!strcmp(respuesta[0], "FAIL")){
        send_messages_socket(socket, respuesta, 1);
        return;
    }

    crear_pcb(restaurante->nombre_restaurante, atoi(id_pedido), id_cliente);

    send_messages_socket(socket, respuesta, 1);
    //liberar_conexion(socket);

}

void handle_consultar_pedido(int socket, char* id_cliente, char* id_pedido){

    r_consultar_pedido pedido;
    r_obtener_pedido *pedidoAux;
    char* arrayReturn[1];

    t_cliente* cliente = buscar_cliente_lista(id_cliente);

    t_restaurante* restaurante = cliente->restaurante;

    pedido.restaurante = restaurante->nombre_restaurante;

    pedidoAux = enviar_mensaje_obtener_pedido(&modulo_comanda, id_pedido, restaurante->nombre_restaurante);

    pedido.estado = pedidoAux->estado;
    pedido.info_comidas = pedidoAux->info_comidas;

    arrayReturn[0] = armar_string_consultar_pedido(&pedido);

    send_messages_socket(socket, arrayReturn, 1);

}

char* armar_string_consultar_pedido(r_consultar_pedido* pedido){


    char* arrayReturn = string_new();

    string_append(&arrayReturn, "{");
    if(pedido->restaurante != NULL){
        string_append(&arrayReturn, pedido->restaurante);
        string_append(&arrayReturn, ",");
    }
    string_append(&arrayReturn, pedido->estado);
    string_append(&arrayReturn, ",");

    string_append(&arrayReturn, "[");
    for (IteratorList iter = beginlist(*pedido->info_comidas); iter != NULL; iter = nextlist(iter)){
        string_append(&arrayReturn, "{");
        informacion_comidas* info = (informacion_comidas*) iter->data;
        string_append(&arrayReturn, info->comida);
        string_append(&arrayReturn, ",");
        string_append(&arrayReturn, info->cantidad_total);
        string_append(&arrayReturn, ",");
        string_append(&arrayReturn, info->cantidad_lista);
        string_append(&arrayReturn, "}");
        string_append(&arrayReturn, ",");

    }

    string_append(&arrayReturn, "]");

    string_append(&arrayReturn, "}");

    return arrayReturn;

}

void inicializar_pedido_semaforo(char* id_pedido, char* restaurante){
    t_pedido_espera* pedido_espera = malloc(sizeof(t_pedido_espera));
    
    pedido_espera->restaurante = string_new();
    string_append(&pedido_espera->restaurante, restaurante);
    pedido_espera->id_pedido = string_new();
    string_append(&pedido_espera->id_pedido, id_pedido);
    pedido_espera->semaforo = malloc(sizeof(sem_t));
    sem_init(pedido_espera->semaforo, 0, 0);

    pushfrontlist(&lista_semaforos_pedidos, pedido_espera);

}