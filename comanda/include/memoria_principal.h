#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <stdbool.h>
#include "shared_utils.h"
#include <commons/bitarray.h>
#include "memoria_swap.h"
#include "server.h"
#include "tests.h"
#include "api.h"
#include "list.h"



typedef struct segmento{

    //int numeroSegmento;
    char* idPedido;
    int estadoPedido; // 0 EN PROCESO ; 1 LISTO ; 2 TERMINADO
    List* punteroTablaPaginas;
    
} l_segmento;

typedef struct frame{

    uint32_t cantidadPlato;
    uint32_t cantidadLista;
    char plato[24];

} l_frame;

typedef struct pagina{

    int numPagina;
    int bitUso;
    int bitPresencia;
    int bitModificado;
    void *frame;
    void *swap;

} l_pagina;

typedef struct proceso{

    char* nombreResto;
    List* punteroTablaSegmentos;

} l_proceso;

// MEMORIA PRINCIPAL

void iniciarMemoria();
l_proceso *crearProceso(char *);
int crearSegmento(l_proceso*, char* idPedido);
void crear_pagina2(l_segmento*, int cantidad, char plato[24]);
void terminarPlatoPagina(l_pagina *pagina);
int platos_listos(l_segmento* segmento);
void agregar_plato_pedido(l_pagina* pagina, int cantidad);
void confirmar_pedido_segmento(l_segmento *segmento);
void terminar_pedido_segmento(l_segmento*);
void eliminarSegmento(l_proceso*,l_segmento*);
void desalojarPedido(l_proceso *resto, l_segmento* segmento);
void pasarPaginasAPrincipal(l_segmento*);
void ocuparFrame(void*, t_bitarray *, List);
void desocuparFrame(void*, t_bitarray *, List);
char* obtenerEstadoPedido(int estado);
void pasarSupremo(l_pagina* paginaSwap);
void imprimirBitMap();
void imprimirTodo();
void imprimirMemoria();
void *puntero_memoria_principal;

// SEMAFOROS

sem_t* sem_mutex_algoritmos;
sem_t* sem_ocupar_frame;
sem_t* sem_mutex_swap_libre;
sem_t* sem_mutex_eliminar_segmento;
sem_t* sem_mutex_num_pagina;
// FINDS

void *frameLibre();
void *frameLibreSwap();
l_proceso *find_resto_lista(char*);
l_segmento *find_segmento_lista(char* idSegmento, List *segmentos);
l_pagina* plato_en_pagina(char* plato, List* lista);

// SWAP

void escribirArchivo(void*);
void modificarPagina(l_pagina*);
void quitarSiExiste(l_pagina*);
int pasarAPrincipal(l_pagina*);
void imprimirSwap();
void imprimirLista();

// ALGORITMOS

void *ejecutarAlgoritmo();
void *ejecutarLRU();
void *ejecutarClockMej();
void* desalojarDePrincipal(l_pagina*);


List tablaRestaurantes;
List tablaFrames;
char* punteroBitMap;
t_bitarray *bitMap;
int numeroPaginaGlobal;

List pilaPaginasAlgoritmos;