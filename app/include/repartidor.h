#ifndef REPARTIDOR_H
#define REPARTIDOR_H

#include "list.h"
#include "config_app.h"
#include <commons/string.h>
#include <stdlib.h>
#include "shared_utils.h"
#include <semaphore.h>
#include "config_handle.h"

void repartir_pedidos(t_repartidor* repartidor);
void repartir_pedido(t_repartidor* repartidor);
void ir_hacia_restaurante(t_repartidor* repartidor);
void esperar_pedido(t_repartidor* repartidor);
void ir_hacia_cliente(t_repartidor* repartidor);
void entregar_pedido(t_repartidor* repartidor);
bool misma_posicion(t_posicion posicion1, t_posicion posicion2);
void avanzar_hacia(t_repartidor* repartidor, t_posicion destino);
void mover_hacia_izquierda(t_repartidor* repartidor);
void mover_hacia_derecha(t_repartidor* repartidor);
void mover_hacia_abajo(t_repartidor* repartidor);
void mover_hacia_arriba(t_repartidor* repartidor);
void cansarse(t_repartidor* repartidor);
bool esta_cansado(t_repartidor* repartidor);
void descansar(t_repartidor* repartidor);
void buscar_datos_pedido(t_repartidor* repartidor);
void desuscribirse_clock(sem_t* ciclo_cpu);
void pasar_a_ready(t_repartidor* repartidor);
void pasar_a_block(t_repartidor* repartidor);
int pedido_terminado(int id_pedido);

#endif
