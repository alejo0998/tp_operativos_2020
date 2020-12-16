#include "config_handle.h"

t_restaurante* buscar_restaurante_lista(char* nombre_restaurante){

    for (IteratorList iter = beginlist(lista_restaurantes); iter != NULL; iter = nextlist(iter)){
        t_restaurante* restaurante = (t_restaurante*) iter->data;

        if (strcmp(restaurante->nombre_restaurante, nombre_restaurante) == 0){
            return restaurante;
        }

    }

    return NULL;

}

t_cliente* buscar_cliente_lista(char* id_cliente){

    for (IteratorList iter = beginlist(lista_clientes); iter != NULL; iter = nextlist(iter)){
        t_cliente* cliente = (t_cliente*) iter->data;

        if (strcmp(cliente->id_cliente, id_cliente) == 0){
            return cliente;
        }

    }

    return NULL;

}

t_pedido_espera* buscar_pedido_espera(char* id_pedido, char* restaurante){

    for (IteratorList iter = beginlist(lista_semaforos_pedidos); iter != NULL; iter = nextlist(iter)){
        t_pedido_espera* pedido = (t_pedido_espera*) iter->data;

        if (!strcmp(pedido->restaurante, restaurante)){
            if(!strcmp(pedido->id_pedido, id_pedido)){
                return pedido;
            }
        }

    }

    return NULL;

}

void eliminar_pedido_espera(char* id_pedido, char* restaurante){

    for (IteratorList iter = beginlist(lista_semaforos_pedidos); iter != NULL; iter = nextlist(iter)){
        t_pedido_espera* pedido = (t_pedido_espera*) iter->data;

        if (!strcmp(pedido->restaurante, restaurante)){
            if (!strcmp(pedido->id_pedido, id_pedido)){
                popiterlist(&lista_semaforos_pedidos, iter);
                return;
            }
        }

    }

}

void enviar_final_pedido(char* restaurante, int id_pedido){

    enviar_mensaje_finalizar_pedido(&modulo_comanda, string_itoa(id_pedido), restaurante);

}

t_pcb* crear_pcb(char* restaurante, int id_pedido, char* id_cliente){
    t_pcb* pcb = malloc(sizeof(t_pcb));
    pcb->restaurante = restaurante;
    pcb->id_pedido = id_pedido;
    pcb->cliente = id_cliente;
    pcb->estimacion = app_config->estimacion_inicial;
    pcb->ciclos_espera = 0;
    pushbacklist(&pcb_new, pcb);
    sem_post(sem_pcb_new);
    return pcb;
}

int comparar_platos(r_obtener_pedido *pedido){

    IteratorList iterador;
    informacion_comidas *info;

    if(atoi(pedido->estado) == 2){
        return 1;
    }

    for(iterador = beginlist(*pedido->info_comidas);iterador != NULL; iterador = nextlist(iterador)){
        info = (informacion_comidas*)dataiterlist(iterador);

        if(*info->cantidad_lista != *info->cantidad_total){
            return 0;
        }
    }

    return 1;
}