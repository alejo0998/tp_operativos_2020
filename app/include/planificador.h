#ifndef PLANIFICADOR_H
#define PLANIFICADOR_H

#include "list.h"
#include "config_app.h"
#include <commons/string.h>
#include <stdlib.h>
#include <math.h>
#include "shared_utils.h"
//#include "config_handle.h"
#include "repartidor.h"
#include <pthread.h>
#include <unistd.h>

void iniciar_planificador();
void iniciar_repartidores();
void pcb_prueba();
void iniciar_planificador_corto_plazo();
void iniciar_planificador_largo_plazo();
void planificar_largo_plazo();
void iniciar_clock();
void clock_cpu();
void planificar_corto_plazo_FIFO();
void planificar_corto_plazo_SJF();
void asignar_datos_pedido(t_pcb* pcb);
void planificar_corto_plazo_HRRN();
void aumentar_espera_HRRN();
t_repartidor* obtener_repartidor_mas_cercano(t_pcb* pcb);
#endif
