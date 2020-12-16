#ifndef RESTAURANTE_H
#define RESTAURANTE_H

#include "planificacion.h"

// VARIABLES Y ESTRUCTURAS



// FUNCIONES
void restaurante_init();
void restaurante_finally(t_restaurante_config* restaurante_config, t_log* logger);
t_restaurante_config* restaurante_config_loader(char* path_config_file);
void restaurante_config_parser(t_config* config, t_restaurante_config* restaurante_config);
void restaurante_destroy(t_restaurante_config* restaurante_config);
void handshake_init(t_modulo, t_modulo);

int asignar_pedido_id();
int asignar_pid();
int handshake_app(t_modulo* modulo);
int handshake_sindicato(t_modulo*);
void escuchar_mensajes_socket_desacoplado(int socket);
void escuchar_mensajes_socket(t_parameter* parametro);
void handle_client(t_result* result);
void inicializacion_default();
void handle_obtener_restaurante(r_obtener_restaurante* resultado);
int len_array(char** arrayInput);
void consultar_platos_f();
void handle_crear_pedido(int socket);
void handle_anadir_plato(t_result* result);
void handle_confirmar_pedido(t_result* result);
void handle_consultar_pedido(t_result* result);
void data_restaurante();
void ver_info_pedido(List* lista_pedidos);
void caso_uso();
void ver_estado_pcb();
void* escuchar_servidor(void* handle_client);
void modificar_pcb();
sem_t* sem_id;
sem_t* sem_mutex_confirmar_pedido;
char* armar_string_obtener_pedido(r_obtener_pedido* pedido);
void iniciar_servidor_desacoplado();


#endif