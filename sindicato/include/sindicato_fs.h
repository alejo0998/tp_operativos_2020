#ifndef SINDICATO_FS_H
#define SINDICATO_FS_H

#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>
#include <math.h>
#include <commons/bitarray.h>
#include <commons/string.h>
#include "sindicato_config.h"
#include <errno.h>

#define BIT_SIZE(x,y) ((x - 1) / y + 1);
#define	ES_TEST true

// VARIABLES Y ESTRUCTURAS
typedef enum {
	PENDIENTE,
	CONFIRMADO,
	TERMINADO,
} t_estado_pedido;

typedef struct {
	char* block_size;
	char* blocks;
	char* magic_number;
} t_sindicato_metadata;

typedef struct {
	int x;
	int y;
} t_posicion;

typedef struct {
	int cantidad_cocineros;
	t_posicion* posicion;
	t_list* afinidad_cocineros;
	t_list* platos;
	t_list* precio_platos;
	int cantidad_hornos;
} t_info;

typedef struct {
	t_estado_pedido estado_pedido;
	t_list* lista_platos;
	t_list* cantidad_platos;
	t_list* cantidad_lista;
	int precio_total;
} t_pedido;

typedef struct {
	int size;
	int initial_block;
} t_afip_file;

typedef struct {
	List* pasos;
	List* tiempo_paso;
} t_receta;

t_sindicato_metadata* sindicato_metadata;

pthread_mutex_t mutex_bitmap;

// FUNCIONES
char* get_path_restaurante(char* restaurante);
char* get_path_info_file(char* restaurante);
char* get_path_pedido_file(char* restaurante, char* id_pedido);
char* get_path_receta_file(char* nombre_receta);
void get_or_create_fs();
void crear_metadata_default();
int leer_metadata_afip();
void crear_bitmap();
int get_or_create_folder();
bool existe_archivo(char* archivo_path);
int necesita_recrearse(char * block_size, char * blocks, char * magic_number);
void crear_files();
bool existe_restaurante(char* restaurante);
bool existe_pedido(char* restaurante, char* nro_pedido);
bool existe_receta(char* receta);
t_info* create_info_config(char* restaurante);
t_pedido* create_pedido_config(char* restaurante, char* id_pedido);
t_receta* create_receta_config(char* nombre_receta);
t_list* get_platos(char* restaurante);
t_pedido* get_pedido(char* restaurante, char* id_pedido);
int create_pedido_file(char* path, t_afip_file* pedido_file);
bool create_afip_file(char* content, char* path);
List* obtenerBloquesActuales(uint32_t initial_block);
uint32_t getSiguienteBloque(uint32_t bloque);
FILE * get_or_create_file(char* path_file, char * mode);
void create_blocks();
char* get_receta_data(char* nombre_receta);
int get_data_size(FILE* fp);
char* data_to_char(char* data);
char* afip_file_to_char(char* path);
char* get_restaurante_data(char* restaurante);
char* get_pedido_data(char* restaurante, char* pedido);
bool update_afip_file(char* content, char* path);
#endif
