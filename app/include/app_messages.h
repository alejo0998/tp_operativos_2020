#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <semaphore.h>
#include "shared_utils.h"
#include "server.h"
#include "list.h"
#include "api.h"
#include "tests.h"
//#include "config_app.h"
//#include "config_handle.h"
#include "planificador.h"

int iniciador_de_planificacion;

void handle_consultar_restaurantes(int socket);
int relacionar(char* restaurante, char* cliente);
void handle_seleccionar_restaurante(int socket, char* restaurante, char* cliente);
char* obtener_id_pedido(t_restaurante*);
void  handle_crear_pedido(int, char*);
void handle_handshake_cliente(int socket, char* id_cliente, int, int);
void handle_handshake_restaurante(int socket, char* nombre_restaurante, int pos_x, int pos_y, char* ip, char* puerto);
t_restaurante* nuevo_restaurante(int socket, char* nombre_restaurante, int pos_x, int pos_y, char* ip, char* puerto);
void handle_consultar_platos(int, char*);
void handle_anadir_plato(int,char*, char*, char*);
int plato_en_restaurante(char* plato, List lista);
void handle_confirmar_pedido(int, char*, char*);
void handle_plato_listo(int, char*, char*, char*);
void handle_consultar_pedido(int,char*, char*);
char* armar_string_consultar_pedido(r_consultar_pedido* );
char* obtener_restaurantes();
char* obtener_platos(List);
int obtener_id_default(t_restaurante* restaurante);
void handle_client(t_result* result);
void inicializar_pedido_semaforo(char* id_pedido, char*);