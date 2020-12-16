#ifndef SINDICATO_H
#define SINDICATO_H

#include "sindicato_fs.h"
#include <unistd.h>
#include <sys/file.h>

// VARIABLES Y ESTRUCTURAS

// FUNCIONES
int guardar_pedido_en_afip(char* restaurante, char* id_pedido);
void handle_guardar_pedido(int socket, char* restaurante, char* id_pedido);
void handle_client(t_result* result);
void handle_consultar_platos(int socket, char* restaurante);
void handle_guardar_plato(int socket, char* restaurante, char* id_pedido, char* comida, char* cantidad);
void handle_confirmar_pedido(int socket, char* id_pedido,  char* restaurante);
void handle_obtener_pedido(int socket, char* id_pedido,  char* restaurante);
void handle_obtener_restaurante(int socket, char* restaurante);
void handle_plato_listo(int socket, char* restaurante,  char* id_pedido, char* comida);
void handle_obtener_receta(int socket, char* comida);
void handle_terminar_pedido(int socket, char* id_pedido,  char* restaurante);
void handle_error(int socket);
void iniciar_consola();
void iniciar_servidor_desacoplado();
void iniciar_servidor_sindicato();
char* getlinefromconsole(void);
void process_line(char* line);
void handle_crear_restaurante(char* nombre, char* cantidad_cocineros, char* posicion, char* afinidad_cocineros, char* platos, char* precio_platos, char* cantidad_hornos);
void handle_crear_receta(char* nombre, char* pasos, char* tiempo_pasos);
void handle_handshake_restaurante(int socket);
bool platos_iguales(char* plato, char* comida);
char* sacar_corchetes(char* array);
int obtener_cantidad_pedidos(char* restaurante);
void handle_handshake_cliente(int socket);
#endif