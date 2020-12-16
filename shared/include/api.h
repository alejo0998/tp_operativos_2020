#ifndef API_H
#define API_H
#include "server.h"
#include "list.h"
#include <commons/string.h>

typedef enum tipo_mensaje{
    consultar_restaurantes = 1,
    seleccionar_restaurante = 2,
    obtener_restaurante = 3,
    consultar_platos = 4,
    crear_pedido = 5,
    guardar_pedido = 6,
    anadir_plato = 7,
    guardar_plato = 8,
    confirmar_pedido = 9,
    plato_listo = 10,
    consultar_pedido = 11,
    obtener_pedido = 12,
    finalizar_pedido = 13,
    terminar_pedido = 14,
    obtener_receta = 15,
    handshake_cliente = 16,
    handshake_restaurante = 17
} tipo_mensaje_t;


typedef struct {
    char* ip;
    char* puerto;
    int socket;
    char* identificacion;
} t_modulo;

typedef struct{
    char* receta;
    char* precio;
} receta_precio;

typedef struct {
    List* afinidades;
    char* pos_x;
    char* pos_y;
    List* recetas_precio;
    char* cantidad_hornos;
    char* cantidad_pedidos;
    char* cantidad_cocineros;
} r_obtener_restaurante;

typedef struct{
    char* comida;
    char* cantidad_total;
    char* cantidad_lista;    
} informacion_comidas;

typedef struct{
    char* nombre_paso;
    int ciclo_cpu;
}t_paso;

typedef struct {
    char* restaurante;
    char* estado;
    List* info_comidas;
} r_consultar_pedido;


typedef struct {
    char* estado;
    List* info_comidas;
} r_obtener_pedido;






int obtener_numero_mensaje(char* mensaje_tipo);

List* obtener_receta_precios(char* platos, char* precioplatos);


char** obtener_array_mensajes(char* array_mensaje);
List* obtener_list_mensajes(char* array_mensaje);
char ** list_a_char(List lista);
List* separar_por_comillas_lista(char** string_separado_por_espacios);

char** enviar_mensaje_consultar_restaurantes(t_modulo* modulo);
char* enviar_mensaje_seleccionar_restaurante(t_modulo* modulo, char* cliente, char* restaurante);
r_obtener_restaurante* enviar_mensaje_obtener_restaurante(t_modulo* modulo, char* restaurante);
List* enviar_mensaje_consultar_platos(t_modulo* modulo, char* restaurante);
char* enviar_mensaje_crear_pedido(t_modulo* modulo);
char* enviar_mensaje_guardar_pedido(t_modulo* modulo, char* restaurante, char* id_pedido);
char* enviar_mensaje_anadir_plato(t_modulo* modulo, char* plato, char* id_pedido);
char* enviar_mensaje_guardar_plato(t_modulo* modulo, char* restaurante, char* id_pedido, char* comida, char* cantidad);
char* enviar_mensaje_confirmar_pedido(t_modulo* modulo,char* id_pedido, char* restaurante);
char* enviar_mensaje_plato_listo(t_modulo* modulo, char* restaurante, char* id_pedido, char* comida);
char* enviar_mensaje_consultar_pedido(t_modulo* modulo, char* id_pedido);
r_obtener_pedido* enviar_mensaje_obtener_pedido(t_modulo* modulo, char* id_pedido,char* restaurante);
char* enviar_mensaje_finalizar_pedido(t_modulo* modulo, char* id_pedido,char* restaurante);
char* enviar_mensaje_terminar_pedido(t_modulo* modulo, char* id_pedido,char* restaurante);

int enviar_mensaje_modulo(t_modulo* modulo, char** mensaje, int cantidadMensajes);

char ** separar_por_comillas(char** string_separado_por_espacios);


List* obtener_informacion_comidas(char* platos,char* cantidades_listas, char* cantidades_totales);
List* enviar_mensaje_obtener_receta(t_modulo* modulo, char* nombre_plato);

#endif