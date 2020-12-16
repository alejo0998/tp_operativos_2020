#include "repartidor.h"

void repartir_pedidos(t_repartidor* repartidor){
    while (1) {
        sem_wait(repartidor->nuevo_pedido);
        repartir_pedido(repartidor);
    }
}

void repartir_pedido(t_repartidor* repartidor){

    buscar_datos_pedido(repartidor);
    
    ir_hacia_restaurante(repartidor);

    esperar_pedido(repartidor);

    ir_hacia_cliente(repartidor);

    entregar_pedido(repartidor);

}

void buscar_datos_pedido(t_repartidor* repartidor){
    t_posicion posicion_restaurante;
    if (!strcmp(repartidor->pcb_actual->restaurante, "Resto Default")){
        posicion_restaurante.posx = app_config->posicion_rest_default_x;
        posicion_restaurante.posy = app_config->posicion_rest_default_y;
    } else {
        t_restaurante *restaurante = buscar_restaurante_lista(repartidor->pcb_actual->restaurante);
        posicion_restaurante.posx = restaurante->posicion.posx;
        posicion_restaurante.posy = restaurante->posicion.posy;
    }
    
    t_posicion posicion_cliente;

    t_cliente* cliente = buscar_cliente_lista(repartidor->pcb_actual->cliente);
    
    //TODO: CAMBIAR POR LAS POSICIONES REALES
    posicion_cliente.posx = cliente->posicion.posx;
    posicion_cliente.posy = cliente->posicion.posy;

    t_pedido* pedido = malloc(sizeof(t_pedido));
    pedido->posicion_cliente = posicion_cliente;
    pedido->posicion_restaurante = posicion_restaurante;

    repartidor->pedido = pedido;
}

void ir_hacia_restaurante(t_repartidor* repartidor){
    t_pedido* pedido = repartidor->pedido;

    while(!misma_posicion(repartidor->posicion, pedido->posicion_restaurante)){
        
        sem_wait(repartidor->ciclo_cpu);
        repartidor->pcb_actual->rafaga_anterior += 1;

        avanzar_hacia(repartidor, pedido->posicion_restaurante);
    }
}

void esperar_pedido(t_repartidor* repartidor){

    r_obtener_pedido* pedido;

    pedido = enviar_mensaje_obtener_pedido(&modulo_comanda, string_itoa(repartidor->pcb_actual->id_pedido), repartidor->pcb_actual->restaurante);

    if(!strcmp(repartidor->pcb_actual->restaurante, "Resto Default") || (pedido != NULL && comparar_platos(pedido))){

        return;

    }

    t_pedido_espera* pedido_espera = buscar_pedido_espera(string_itoa(repartidor->pcb_actual->id_pedido), repartidor->pcb_actual->restaurante);

    desuscribirse_clock(repartidor->ciclo_cpu);

    pasar_a_block(repartidor);

    sem_wait(pedido_espera->semaforo);

    free(pedido_espera);

    pasar_a_ready(repartidor);

}

void ir_hacia_cliente(t_repartidor* repartidor){
    t_pedido* pedido = repartidor->pedido;

    while(!misma_posicion(repartidor->posicion, pedido->posicion_cliente)){
        
        sem_wait(repartidor->ciclo_cpu);
        repartidor->pcb_actual->rafaga_anterior += 1;

        avanzar_hacia(repartidor, pedido->posicion_cliente);

    }

}

void entregar_pedido(t_repartidor* repartidor){
    desuscribirse_clock(repartidor->ciclo_cpu);
    sem_post(sem_grado_multiprocesamiento);
    char string_log[100];
    sprintf(string_log, "ENTREGO PEDIDO: Repartidor %d", repartidor->id);
    log_info(logger, string_log);
    enviar_final_pedido(repartidor->pcb_actual->restaurante, repartidor->pcb_actual->id_pedido);
    pushbacklist(&repartidores_libres, repartidor);
    sem_post(sem_entrenador_libre);
}

bool misma_posicion(t_posicion posicion1, t_posicion posicion2){
    return posicion1.posx == posicion2.posx && posicion1.posy == posicion2.posy;
}

void avanzar_hacia(t_repartidor* repartidor, t_posicion destino){

    
    if (repartidor->posicion.posx > destino.posx){
        mover_hacia_izquierda(repartidor);
    } else if(repartidor->posicion.posx < destino.posx) {
        mover_hacia_derecha(repartidor);
    } else if (repartidor->posicion.posy > destino.posy) {
        mover_hacia_abajo(repartidor);
    } else {
        mover_hacia_arriba(repartidor);
    }
    
    char log_string[100];
    sprintf(log_string, "El repartidor %i se movió a la posición %i - %i", repartidor->id, repartidor->posicion.posx, repartidor->posicion.posy);
    log_info(logger, log_string);

    cansarse(repartidor);

}

void mover_hacia_izquierda(t_repartidor* repartidor){
    repartidor->posicion.posx -= 1;
}

void mover_hacia_derecha(t_repartidor* repartidor){
    repartidor->posicion.posx += 1;
}

void mover_hacia_abajo(t_repartidor* repartidor){
    repartidor->posicion.posy -= 1;
}

void mover_hacia_arriba(t_repartidor* repartidor){
    repartidor->posicion.posy += 1;
}

void cansarse(t_repartidor* repartidor){
    repartidor->cansancio += 1;

    if (esta_cansado(repartidor)){
        descansar(repartidor);
    }
}

bool esta_cansado(t_repartidor* repartidor){
    return repartidor->cansancio >= repartidor->frecuencia_de_descanso;
}

void descansar(t_repartidor* repartidor){

    pasar_a_block(repartidor);

    char string_log[100];
    sprintf(string_log, "DESCANSA: Repartidor %d", repartidor->id);
    log_info(logger, string_log);

    for (int i = 0; i < repartidor->tiempo_de_descanso; i++){
        sem_wait(repartidor->ciclo_cpu);
    }
    desuscribirse_clock(repartidor->ciclo_cpu);
    repartidor->cansancio = 0;
    pasar_a_ready(repartidor);
}

void pasar_a_ready(t_repartidor* repartidor){
    
    pushbacklist(&pcb_ready, repartidor->pcb_actual);
    sem_post(sem_pcb_ready);
}

void pasar_a_block(t_repartidor* repartidor){
    sem_post(sem_grado_multiprocesamiento);
}

void desuscribirse_clock(sem_t* ciclo_cpu){
    for (IteratorList it = beginlist(suscriptores_cpu); it != NULL ; it = nextlist(it)){
        if ((sem_t*)dataiterlist(it) == ciclo_cpu){
            popiterlist(&suscriptores_cpu, it);
            break;
        }
    }
}
