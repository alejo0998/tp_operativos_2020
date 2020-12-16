#include "mensajes_consola.h"

void leer_consola(t_log* logger,t_modulo* modulo) {

    pthread_t thread_mensaje;
	
	char* leido = readline(">");
    

	while(strncmp(leido, "", 1) != 0) {
        t_struct_mensajes* t_parametro_mensajes = malloc(sizeof(t_struct_mensajes));
        t_parametro_mensajes->modulo = modulo;
		log_info(logger, leido);
        t_parametro_mensajes->mensaje_completo = string_new();
        string_append(& t_parametro_mensajes->mensaje_completo, leido);
        pthread_create(&thread_mensaje,NULL,(void*)enviar_mensajes_por_consola, t_parametro_mensajes);
	    pthread_detach(thread_mensaje);
        //enviar_mensajes_por_consola(t_parametro_mensajes);
		//free(leido);
		leido = readline(">");
	}

	free(leido);
}

void enviar_mensajes_por_consola(t_struct_mensajes* mensaje){

    char** string_prueba = NULL;
    string_prueba = string_split(mensaje->mensaje_completo, " ");

    string_prueba = separar_por_comillas(string_prueba);

    if (string_prueba == NULL){
        return;
    }

    int numero_mensaje = obtener_numero_mensaje(string_prueba[0]);

    switch(numero_mensaje){

        case 1 : enviar_mensaje_consultar_restaurantes(mensaje->modulo);
        break;
        case 2 : enviar_mensaje_seleccionar_restaurante(mensaje->modulo, string_prueba[1], string_prueba[2]);
        break;
        case 3 : enviar_mensaje_obtener_restaurante(mensaje->modulo,string_prueba[1]);
        break;
        case 4 : enviar_mensaje_consultar_platos(mensaje->modulo,string_prueba[1]);
        break;
        case 5 : enviar_mensaje_crear_pedido(mensaje->modulo);
        break;
        case 6 : enviar_mensaje_guardar_pedido(mensaje->modulo,string_prueba[1], string_prueba[2]);
        break;
        case 7 : enviar_mensaje_anadir_plato(mensaje->modulo,string_prueba[1], string_prueba[2]);
        break;
        case 8 : enviar_mensaje_guardar_plato(mensaje->modulo,string_prueba[1], string_prueba[2],string_prueba[3], string_prueba[4]);
        break;
        case 9 : enviar_mensaje_confirmar_pedido(mensaje->modulo,string_prueba[1], string_prueba[2]);
        break;
        case 10 : enviar_mensaje_plato_listo(mensaje->modulo,string_prueba[1], string_prueba[2],string_prueba[3]);
        break;
        case 11 : enviar_mensaje_consultar_pedido(mensaje->modulo,string_prueba[1]);
        break;
        case 12 : enviar_mensaje_obtener_pedido(mensaje->modulo,string_prueba[1], string_prueba[2]);
        break;
        case 13 : enviar_mensaje_finalizar_pedido(mensaje->modulo,string_prueba[1], string_prueba[2]);
        break;
        case 14 : enviar_mensaje_terminar_pedido(mensaje->modulo,string_prueba[1], string_prueba[2]);
        break;
        case 15 : enviar_mensaje_obtener_receta(mensaje->modulo,string_prueba[1]);
        break;
        default : printf("No es un mensaje valido \n");

    }



}


//MOVIDA A SHARED
// char ** separar_por_comillas(char** string_separado_por_espacios){
//     List lista_separado_por_comillas;
//     initlist(&lista_separado_por_comillas);

//     for (int i = 0; string_separado_por_espacios[i] != NULL; i++){

//         if (string_starts_with(string_separado_por_espacios[i], "\"")){
//             if (string_ends_with(string_separado_por_espacios[i], "\"")){
//                 char* string_sin_comillas = string_substring(string_separado_por_espacios[i], 1, strlen(string_separado_por_espacios[i]) - 2);
//                 pushbacklist(&lista_separado_por_comillas, string_sin_comillas);
//             } else {
//                 char* string_concatenado = string_new();
//                 string_append(&string_concatenado, string_separado_por_espacios[i]);
//                 i++;
//                 int finalize_correctamente = 0;
//                 while(string_separado_por_espacios[i] != NULL){
//                     string_append(&string_concatenado, " ");
//                     string_append(&string_concatenado, string_separado_por_espacios[i]);
//                     if (string_ends_with(string_separado_por_espacios[i], "\"")){
//                         finalize_correctamente = 1;
//                         break;
//                     }
//                     i++;
//                 }
//                 if (finalize_correctamente == 1){
//                     char* string_sin_comillas = string_substring(string_concatenado, 1, strlen(string_concatenado) - 2);
//                     pushbacklist(&lista_separado_por_comillas, string_sin_comillas);
//                 } else {
//                     return NULL;
//                 }
//             }
//         } else {
//             pushbacklist(&lista_separado_por_comillas, string_separado_por_espacios[i]);
//         }

//     }

//     int size = sizelist(lista_separado_por_comillas);

//     char ** separado_por_comillas = malloc(sizeof(char*) * size);

//     for (int i = 0; i < size; i++){
//         char* elemento = popfrontlist(&lista_separado_por_comillas);
//         separado_por_comillas[i] = elemento;
//     }

//     return separado_por_comillas;

// }