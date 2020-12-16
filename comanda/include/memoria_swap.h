#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <stdbool.h>
#include "shared_utils.h"
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <commons/bitarray.h>
#include "server.h"
#include "tests.h"
#include "api.h"
#include "list.h"

void iniciarMemoriaSwap();
int max (int x, int y);

void *puntero_memoria_swap;
int archivoSwap;
t_bitarray *bitMapSwap;
List tablaSwap;
void* punteroBitMapSwap;

//LOS PONGO ACA PORQUE ES LA ULTIMA BIBLIOTECA

char* algoritmo;
int tamanioMemoria;
int tamanioSwap;
int tamanioBitMapPrincipal;
int tamanioBitMapSwap;

IteratorList iteradorClockMej;
int inicioClockMej;

t_log* logger;
sem_t* semaforo_contador;

