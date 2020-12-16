#include "pedidos_mensajes.h"

void handle_client(t_result* result){

    //sleep(10);

    int tipo_mensaje = atoi(result->mensajes->mensajes[0]);
    if (tipo_mensaje == guardar_pedido){ // NOMBRE_RESTAURANTE ID_PEDIDO

        handle_guardar_pedidos(result);

    } else if (tipo_mensaje == guardar_plato){ // NOMBRE_RESTAURANTE ID_PEDIDO PLATO CANTIDAD_PLATO
        
        handle_guardar_plato(result);

    } else if (tipo_mensaje == confirmar_pedido){ // NOMBRE_RESTAURANTE ID_PEDIDO 
        
        handle_confirmar_pedido(result);

    } else if (tipo_mensaje == plato_listo){ //  NOMBRE_RESTAURANTE ID_PEDIDO PLATO
        
        handle_plato_listo(result);

    } else if (tipo_mensaje == obtener_pedido){ // NOMBRE_RESTAURANTE ID_PEDIDO
        
        handle_obtener_pedido(result);

    } else if (tipo_mensaje == finalizar_pedido){ //  NOMBRE_RESTAURANTE ID_PEDIDO
        
        handle_finalizar_pedido(result);

    }else if (tipo_mensaje == handshake_cliente){
        send_message_socket(result->socket, "OK");
        liberar_conexion(result->socket);
        printf("Se conecto el cliente con el id: %s \n", result->mensajes->mensajes[1]);
    }
}

int existe_restaurante(char* restaurante){
    
    IteratorList iterador = NULL;
    l_proceso* resto = NULL;

    for(iterador = beginlist(tablaRestaurantes);iterador!=NULL;iterador = nextlist(iterador)){
        resto = dataiterlist(iterador);

        if(!strcmp(resto->nombreResto,restaurante)){
            return 1;
        }
    }       

    return 0;

}

void handle_guardar_plato(t_result* result){

    char* respuesta[1];

    if (guardar_plato_en_memoria(result->mensajes->mensajes[1],result->mensajes->mensajes[2],result->mensajes->mensajes[4],result->mensajes->mensajes[3])){
        imprimirTodo();
        respuesta[0] = "OK";
    } else {
        respuesta[0] = "FAIL";
    }

    send_messages_socket(result->socket, respuesta, 1);
    liberar_conexion(result->socket);

}


void handle_guardar_pedidos(t_result* result){
    
    char* respuesta[1];

    if (guardar_pedido_en_memoria(result->mensajes->mensajes[1], result->mensajes->mensajes[2])){
        imprimirTodo();
        respuesta[0] = "OK";
    } else {
        respuesta[0] = "FAIL";
    }

    send_messages_socket(result->socket, respuesta, 1);
    liberar_conexion(result->socket);
}




void handle_confirmar_pedido(t_result* result){

    char* respuesta[1];

    if (confirmar_pedido_en_memoria(result->mensajes->mensajes[1], result->mensajes->mensajes[2])){
        imprimirTodo();
        respuesta[0] = "OK";
    } else {
        respuesta[0] = "FAIL";
    }

    send_messages_socket(result->socket, respuesta, 1);
    liberar_conexion(result->socket);

}

void handle_plato_listo(t_result* result){

    char* respuesta[1];

    if (plato_listo_en_memoria(result->mensajes->mensajes[1], result->mensajes->mensajes[2], result->mensajes->mensajes[3])){
        imprimirTodo();
        respuesta[0] = "OK";
    } else {
        respuesta[0] = "FAIL";
    }

    send_messages_socket(result->socket, respuesta, 1);
    liberar_conexion(result->socket);

}

void handle_obtener_pedido(t_result* result){

    IteratorList iterator = NULL;
    char* arrayReturn[4] = {string_new(), string_new(), string_new(), string_new()};
    // sem_wait(sem_mutex_obtener_pedido);
    l_segmento* segmento = obtener_pedido_en_memoria(result->mensajes->mensajes[1], result->mensajes->mensajes[2]);

    if(segmento != NULL){

        string_append(&arrayReturn[0], obtenerEstadoPedido(segmento->estadoPedido));

        for(iterator = beginlist(*segmento->punteroTablaPaginas); iterator != NULL; iterator = nextlist(iterator)){
            l_pagina* pagina = (l_pagina*) dataiterlist(iterator);
            l_frame* frame = pagina->frame;

            char* cantidad = string_itoa(frame->cantidadPlato);
            char* cantidadLista = string_itoa(frame->cantidadLista);

            string_append(&arrayReturn[1], frame->plato);
            string_append(&arrayReturn[1], ",");
            string_append(&arrayReturn[2], cantidad);
            string_append(&arrayReturn[2], ",");
            string_append(&arrayReturn[3], cantidadLista);
            string_append(&arrayReturn[3], ",");
        }
        
        send_messages_socket(result->socket, arrayReturn, 4);
        imprimirTodo();
    }else{
        string_append(&arrayReturn[0], "FAIL");
        send_messages_socket(result->socket, arrayReturn, 1);
        imprimirTodo();
    }
    liberar_conexion(result->socket);
    // sem_post(sem_mutex_obtener_pedido);
   
}

void handle_finalizar_pedido(t_result* result){

    char* respuesta[1];

    if (finalizar_pedido_en_memoria(result->mensajes->mensajes[1], result->mensajes->mensajes[2])){
        imprimirTodo();
        respuesta[0] = "OK";
    } else {
        respuesta[0] = "FAIL";
    }

    send_messages_socket(result->socket, respuesta, 1);
    liberar_conexion(result->socket);

}


int guardar_plato_en_memoria(char* nombreResto, char* idPedido, char* cantidadPlato, char* plato){
    
    l_proceso *restoEnTabla = find_resto_lista(nombreResto);

    if(restoEnTabla == NULL){
        
        printf("El restaurante no esta en la tabla de restaurantes \n");
        
        return 0;
    }

    l_segmento *segmento = find_segmento_lista(idPedido, restoEnTabla->punteroTablaSegmentos);

    if(segmento == NULL){

        printf("El segmento no esta en la tabla de segmentos \n");
        
        return 0;

    }

    l_pagina* pagina_plato = plato_en_pagina(plato, segmento->punteroTablaPaginas);

    if(pagina_plato == NULL){
        crear_pagina2(segmento, atoi(cantidadPlato), plato); 
        return 1;
    }

    modificarPagina(pagina_plato);

    agregar_plato_pedido(pagina_plato,atoi(cantidadPlato));

    return 1;
};


int guardar_pedido_en_memoria(char* restaurante, char* id_pedido){

    l_proceso *restoEnTabla = find_resto_lista(restaurante);

    if(restoEnTabla == NULL){
        pushbacklist(&tablaRestaurantes, (void *) crearProceso(restaurante));
        restoEnTabla = backlist(tablaRestaurantes);
    }
    l_segmento *segmento = find_segmento_lista(id_pedido, restoEnTabla->punteroTablaSegmentos);

    if(segmento == NULL){
        return crearSegmento(restoEnTabla, id_pedido);
    }
    return 0;      

}

int confirmar_pedido_en_memoria(char* id_pedido, char* restaurante){

    l_proceso *restoEnTabla = find_resto_lista(restaurante);

    if(restoEnTabla == NULL){
        
        printf("El restaurante no esta en la tabla de restaurantes \n");
        
        return 0;
    }

    l_segmento *segmento = find_segmento_lista(id_pedido, restoEnTabla->punteroTablaSegmentos);

    if(segmento == NULL){
        printf("El pedido no esta en la tabla de pedidos del restaurante\n");
        return 0;
    }

    if(segmento->estadoPedido != 0){
        printf("El pedido ya esta confirmado o se encuentra terminado");
        return 0;
    }

    confirmar_pedido_segmento(segmento);

    return 1;
}



l_segmento* obtener_pedido_en_memoria(char* id_pedido, char* nombreResto){

    l_proceso *restoEnTabla = find_resto_lista(nombreResto);

    if(restoEnTabla == NULL){
        
        printf("El restaurante no esta en la tabla de restaurantes \n");
        
        return NULL;
    }

    l_segmento *segmento = find_segmento_lista(id_pedido, restoEnTabla->punteroTablaSegmentos);

    if(segmento == NULL){
        printf("El pedido no esta en la tabla de pedidos del restaurante\n");
        return NULL;
    }

    pasarPaginasAPrincipal(segmento);

    return segmento;
}

int plato_listo_en_memoria(char* nombreResto, char* idPedido, char* plato){
 
    l_proceso *restoEnTabla = find_resto_lista(nombreResto);

    if(restoEnTabla == NULL){
        
        printf("El restaurante no esta en la tabla de restaurantes \n");
        
        return 0;
    }

    l_segmento *segmento = find_segmento_lista(idPedido, restoEnTabla->punteroTablaSegmentos);

    if(segmento == NULL){

        printf("El segmento no esta en la tabla de segmentos \n");
        
        return 0;

    }

    if(segmento->estadoPedido == 0 || segmento->estadoPedido == 2){

        printf("El pedido no esta confirmado o ya esta terminado \n");
        
        return 0;

    }

    l_pagina* pagina_plato = plato_en_pagina(plato, segmento->punteroTablaPaginas);

    modificarPagina(pagina_plato);
 
    terminarPlatoPagina(pagina_plato);
    /*
    if(platos_listos(segmento)){
        terminar_pedido_segmento(segmento); 
    };
    */
    return 1;
}

int finalizar_pedido_en_memoria(char* id_pedido, char* restaurante){

    l_proceso *restoEnTabla = find_resto_lista(restaurante);

    if(restoEnTabla == NULL){
        
        printf("El restaurante no esta en la tabla de restaurantes \n");
        
        return 0;
    }

    l_segmento *segmento = find_segmento_lista(id_pedido, restoEnTabla->punteroTablaSegmentos);

    if(segmento == NULL){

        printf("El segmento no esta en la tabla de segmentos \n");
        
        return 0;

    }

    eliminarSegmento(restoEnTabla, segmento);

    return 1;
}
