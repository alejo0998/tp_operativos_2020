#include "sindicato.h"

int main(void){
	sindicato_init(&sindicato_config, &logger);
	get_or_create_fs();

	printf("Imprimiendo el path %s", sindicato_config->ruta_log);

	iniciar_servidor_desacoplado();
	sleep(1);
	iniciar_consola();

	sindicato_finally(sindicato_config, logger);
	return 0;
}

void iniciar_servidor_desacoplado(){

	pthread_t thread;

	pthread_create(&thread,NULL,(void*)iniciar_servidor_sindicato, NULL);
	pthread_detach(thread);
}

void iniciar_servidor_sindicato(){

	//TODO: Hacer que reciba ip y puerto de config
	iniciar_servidor(sindicato_config->ip, sindicato_config->puerto_escucha, handle_client);

}

void handle_client(t_result* result){

	char* mensaje = string_new();
	string_append(&mensaje, "[Handle] Se recibió el mensaje: ");
	for (int i = 0; i < *result->mensajes->size; i++){
		string_append(&mensaje, result->mensajes->mensajes[i]);
		string_append(&mensaje, " "); 
	}

	log_info(logger, mensaje);
	free(mensaje);

	if (result->operacion == MENSAJES){
		int tipo_mensaje = atoi(result->mensajes->mensajes[0]);
		switch(tipo_mensaje)
		{
			case consultar_platos:
				//OK
				handle_consultar_platos(result->socket, result->mensajes->mensajes[1]);
				break;
			case guardar_pedido:
				//OK
				handle_guardar_pedido(result->socket, result->mensajes->mensajes[1], result->mensajes->mensajes[2]);
				break;
			case guardar_plato:
				//OK
				handle_guardar_plato(result->socket, result->mensajes->mensajes[1], result->mensajes->mensajes[2], result->mensajes->mensajes[3], result->mensajes->mensajes[4]);
				break;
			case confirmar_pedido:
				//OK
				handle_confirmar_pedido(result->socket, result->mensajes->mensajes[1], result->mensajes->mensajes[2]);
				break;
			case obtener_pedido:
				//OK
				handle_obtener_pedido(result->socket, result->mensajes->mensajes[1], result->mensajes->mensajes[2]);
				break;
			case obtener_restaurante:
				//OK
				handle_obtener_restaurante(result->socket, result->mensajes->mensajes[1]);
				break;
			case plato_listo:
				handle_plato_listo(result->socket, result->mensajes->mensajes[1], result->mensajes->mensajes[2], result->mensajes->mensajes[3]);
				break;
			case obtener_receta:
				//OK
				handle_obtener_receta(result->socket, result->mensajes->mensajes[1]);
				break;
			case terminar_pedido:
				//OK
				handle_terminar_pedido(result->socket, result->mensajes->mensajes[1], result->mensajes->mensajes[2]);
				break;
			case handshake_restaurante:
				//OK
				handle_handshake_restaurante(result->socket);
				break;
			case handshake_cliente:
				//OK
				handle_handshake_cliente(result->socket);
				break;
			default:
				handle_error(result->socket);
				break;
		}
	}

	liberar_conexion(result->socket);
	return;
}

void handle_handshake_restaurante(int socket){
	send_message_socket(socket, "OK");
}

void handle_handshake_cliente(int socket){
	send_message_socket(socket, "OK");
}


int create_pedido(char* restaurante, char* id_pedido){

	char* content = "ESTADO_PEDIDO=Pendiente\nLISTA_PLATOS=[]\nCANTIDAD_PLATOS=[]\nCANTIDAD_LISTA=[]\nPRECIO_TOTAL=0";
	char* path_pedido = get_path_pedido_file(restaurante, id_pedido);

	bool created = create_afip_file(content, path_pedido);

	if (!created){
		log_error(logger, "[Create Pedido] No se pudo guardar el contenido");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

/* --- SUITES DE HANDLES --- */
void handle_consultar_platos(int socket, char* restaurante){
	//Verificar si el Restaurante existe dentro del File System. 
	//Para esto se deberá buscar dentro del directorio Restaurantes si existe un subdirectorio con el nombre del Restaurante. 
	//En caso de no existir se deberá informar dicha situación.
	if (!existe_restaurante(restaurante)){
		log_error(logger, "[Guardar Pedido] El restaurante no existe");
		char* respuesta[1] = {"El restaurante no existe."};
		send_messages_socket(socket, respuesta, 1);
		return;
	}

	//Obtener los platos que puede preparar dicho Restaurante del archivo info.AFIP.
	char* restaurante_char = get_restaurante_data(restaurante);
	restaurante_char = data_to_char(restaurante_char);
	char** restaurante_info = string_split(restaurante_char, " ");
	char* platos = sacar_corchetes(restaurante_info[3]);

	//Responder el mensaje indicando los platos que puede preparar el Restaurante.
	char* respuesta[1] = {platos};
	send_messages_socket(socket, respuesta, 1);

}

void handle_guardar_pedido(int socket, char* restaurante, char* id_pedido){
	char* respuesta[1];
	//Verificar si el Restaurante existe dentro del File System. 
	//Para esto se deberá buscar dentro del directorio Restaurantes si existe un subdirectorio con el nombre del Restaurante. 
	//En caso de no existir se deberá informar dicha situación.
	if (!existe_restaurante(restaurante)){
		log_error(logger, "[Guardar Pedido] El restaurante no existe");
		respuesta[0] = "El restaurante no existe.";
		send_messages_socket(socket, respuesta, 1);
		return;
	}

	//Verificar que el ID de pedido no exista para dicho restaurante.
	//En caso de existir se deberá informar sobre dicha situación.
	//En caso de que no exista, se deberá crear el archivo.
	if (existe_pedido(restaurante, id_pedido)){
		log_error(logger, "[Guardar Pedido] El pedido ya existe");
		respuesta[0] = "El pedido ya existe.";
		send_messages_socket(socket, respuesta, 1);
		return;
	}

	log_info(logger, "[Guardar Pedido] Se procede a crear el pedido");
	int resultado_operacion = create_pedido(restaurante, id_pedido);
	//Responder el mensaje indicando si se pudo realizar la operación correctamente (Ok/Fail).
	

	if (resultado_operacion == EXIT_SUCCESS){
		respuesta[0] = "Ok";
	} else {
		respuesta[0] = "Fail";
	}
	send_messages_socket(socket, respuesta, 1);
}

bool platos_iguales(char* plato, char* comida){
	if(!strcmp(plato, comida)){
		return true;
	} else {
		return false;
	}
}

void handle_guardar_plato(int socket, char* restaurante, char* id_pedido, char* comida, char* cantidad){
	//Verificar si el Restaurante existe dentro del File System. 
	//Para esto se deberá buscar dentro del directorio Restaurantes si existe un subdirectorio con el nombre del Restaurante. 
	//En caso de no existir se deberá informar dicha situación.
	if (!existe_restaurante(restaurante)) {
		log_error(logger, "[Guardar Plato] El restaurante no existe");
		char* respuesta[1] = {"El restaurante no existe."};
		send_messages_socket(socket, respuesta, 1);
		return;
	}

	//Verificar si el Pedido existe dentro del File System. 
	//Para esto se deberá buscar dentro del directorio del Restaurante si existe dicho pedido. 
	//En caso de no existir se deberá informar dicha situación.
	if (!existe_pedido(restaurante, id_pedido)){
		log_error(logger, "[Guardar Plato] El pedido ya existe");
		char* respuesta[1] = {"El pedido ya existe."};
		send_messages_socket(socket, respuesta, 1);
		return;
	}

	char* pedido_file = get_path_pedido_file(restaurante, id_pedido);

	FILE* fp = fopen(pedido_file, "rw+");

	int lock = flock(fileno(fp), LOCK_EX);

	if (lock == -1){
		log_error(logger, "NO PUDE CREAR EL LOCK");
	}

	char* pedido = get_pedido_data(restaurante, id_pedido);

	pedido =  data_to_char(pedido);

	char** pedido_info = string_split(pedido, " ");

	//Verificar que el pedido esté en estado “Pendiente”. En caso contrario se deberá informar dicha situación.
	if (strcmp(pedido_info[0], "Pendiente") != 0){
		log_error(logger, "[Guardar Plato] El pedido no esta en estado Pendiente");
		char* respuesta[1] = {"El pedido no esta en estado Pendiente"};
		send_messages_socket(socket, respuesta, 1);
		flock(fileno(fp), LOCK_UN);
		fclose(fp);
		return;
	}

	

	//Verificar si ese plato ya existe dentro del archivo.
	//En caso de existir, se deberán agregar la cantidad pasada por parámetro a la actual. 
	//En caso de no existir se deberá agregar el plato a la lista de Platos y anexar la cantidad que se tiene que cocinar de dicho plato y aumentar el precio total del pedido.
	char** platos = string_split(sacar_corchetes(pedido_info[1]), ",");
	char** cantidad_total = string_split(sacar_corchetes(pedido_info[2]), ",");
	char** cantidad_lista = string_split(sacar_corchetes(pedido_info[3]), ",");
	bool actualizado = false;
	char* platos_actualizado = string_new();
	char* cantidad_actualizada = string_new();
	char* cantidad_lista_actualizada = string_new();
	string_append(&platos_actualizado, "[");
	string_append(&cantidad_actualizada, "[");
	string_append(&cantidad_lista_actualizada, "[");
	int i = 0;
	while(platos[i] != NULL){
		if (strcmp(platos[i], comida) == 0){
			cantidad_total[i] = string_itoa(atoi(cantidad_total[i]) + atoi(cantidad));
			actualizado = true;
		}
		if (i != 0){
			string_append(&platos_actualizado, ",");
			string_append(&cantidad_actualizada, ",");
			string_append(&cantidad_lista_actualizada, ",");
		}
		string_append(&platos_actualizado, platos[i]);
		string_append(&cantidad_actualizada, cantidad_total[i]);
		string_append(&cantidad_lista_actualizada, cantidad_lista[i]);
		i++;
	}

	if (!actualizado){
		if (i != 0){
			string_append(&platos_actualizado, ",");
			string_append(&cantidad_actualizada, ",");
			string_append(&cantidad_lista_actualizada, ",");
		}
		string_append(&platos_actualizado, comida);
		string_append(&cantidad_actualizada, cantidad);
		string_append(&cantidad_lista_actualizada, "0");
	}
	string_append(&platos_actualizado, "]");
	string_append(&cantidad_actualizada, "]");
	string_append(&cantidad_lista_actualizada, "]");
	pedido_info[1] = platos_actualizado;
	pedido_info[2] = cantidad_actualizada;
	pedido_info[3] = cantidad_lista_actualizada;

	char* data_actualizada = string_new();
	string_append_with_format(&data_actualizada, "ESTADO_PEDIDO=%s\nLISTA_PLATOS=%s\nCANTIDAD_PLATOS=%s\nCANTIDAD_LISTA=%s\nPRECIO_TOTAL=%s", pedido_info[0], pedido_info[1], pedido_info[2], pedido_info[3], pedido_info[4]);

	char* path_pedido = get_path_pedido_file(restaurante, id_pedido);
	bool updated = update_afip_file(data_actualizada, path_pedido);

	flock(fileno(fp), LOCK_UN);
	fclose(fp);
	if (!updated){
		log_error(logger, "[Confirmar Pedido] No se pudo guardar el contenido actualizado");
		char* respuesta[1] = {"No se pudo guardar el contenido actualizado"};
		send_messages_socket(socket, respuesta, 1);
		return;
	}

	//Responder el mensaje indicando si se pudo realizar la operación correctamente (Ok/Fail).
	char* respuesta[1] = {"Ok"};

	send_messages_socket(socket, respuesta, 1);

}

void handle_confirmar_pedido(int socket, char* id_pedido, char* restaurante){
	//Verificar si el Restaurante existe dentro del File System. 
	//Para esto se deberá buscar dentro del directorio Restaurantes si existe un subdirectorio con el nombre del Restaurante. 
	//En caso de no existir se deberá informar dicha situación.
	if (!existe_restaurante(restaurante)){
		log_error(logger, "[Obtener Pedido] El restaurante no existe");
		char* respuesta[1] = {"El restaurante no existe."};
		send_messages_socket(socket, respuesta, 1);
		return;
	}

	//Verificar si el Pedido existe dentro del File System. 
	//Para esto se deberá buscar dentro del directorio del Restaurante si existe dicho pedido. 
	//En caso de no existir se deberá informar dicha situación.
	if (!existe_pedido(restaurante, id_pedido)){
		log_error(logger, "[Guardar Pedido] El pedido no existe");
		char* respuesta[1] = {"El pedido no existe."};
		send_messages_socket(socket, respuesta, 1);
		return;
	}

	char* pedido_file = get_path_pedido_file(restaurante, id_pedido);

	FILE* fp = fopen(pedido_file, "rw+");

	int lock = flock(fileno(fp), LOCK_EX);

	if (lock == -1){
		log_error(logger, "NO PUDE CREAR EL LOCK");
	}

	char* pedido = get_pedido_data(restaurante, id_pedido);

	pedido =  data_to_char(pedido);

	char** pedido_info = string_split(pedido, " ");

	//Verificar que el pedido esté en estado “Pendiente”. En caso contrario se deberá informar dicha situación.
	if (strcmp(pedido_info[0], "Pendiente") != 0){
		log_error(logger, "[Confirmar Pedido] El pedido no esta en estado Pendiente");
		char* respuesta[1] = {"El pedido no esta en estado Pendiente"};
		send_messages_socket(socket, respuesta, 1);
		flock(fileno(fp), LOCK_UN);
		fclose(fp);
		return;
	}

	//Cambiar el estado del pedido de “Pendiente” a “Confirmado” (se debe truncar el archivo en caso de ser necesario).
	pedido_info[0] = string_new();
	string_append(&pedido_info[0], "Confirmado");

	char* data_actualizada = string_new();
	string_append_with_format(&data_actualizada, "ESTADO_PEDIDO=%s\nLISTA_PLATOS=%s\nCANTIDAD_PLATOS=%s\nCANTIDAD_LISTA=%s\nPRECIO_TOTAL=%s", pedido_info[0], pedido_info[1], pedido_info[2], pedido_info[3], pedido_info[4]);

	char* path_pedido = get_path_pedido_file(restaurante, id_pedido);
	bool updated = update_afip_file(data_actualizada, path_pedido);

	flock(fileno(fp), LOCK_UN);
	fclose(fp);
	if (!updated){
		log_error(logger, "[Confirmar Pedido] No se pudo guardar el contenido actualizado");
		char* respuesta[1] = {"No se pudo guardar el contenido actualizado"};
		send_messages_socket(socket, respuesta, 1);
		return;
	}

	//Responder el mensaje indicando si se pudo realizar la operación correctamente (Ok/Fail).
	char* respuesta[1] = {"OK"};
	send_messages_socket(socket, respuesta, 1);
}

void handle_obtener_pedido(int socket, char* id_pedido, char* restaurante){

	//Verificar si el Restaurante existe dentro del File System. 
	//Para esto se deberá buscar dentro del directorio Restaurantes si existe un subdirectorio con el nombre del Restaurante. 
	//En caso de no existir se deberá informar dicha situación.
	if (!existe_restaurante(restaurante)){
		log_error(logger, "[Obtener Pedido] El restaurante no existe");
		char* respuesta[1] = {"El restaurante no existe."};
		send_messages_socket(socket, respuesta, 1);
		return;
	}

	//Verificar si el Pedido existe dentro del File System. 
	//Para esto se deberá buscar dentro del directorio del Restaurante si existe dicho pedido. 
	//En caso de no existir se deberá informar dicha situación.
	if (!existe_pedido(restaurante, id_pedido)){
		log_error(logger, "[Guardar Pedido] El pedido no existe");
		char* respuesta[1] = {"El pedido no existe."};
		send_messages_socket(socket, respuesta, 1);
		return;
	}
	
	char* pedido_file = get_path_pedido_file(restaurante, id_pedido);

	FILE* fp = fopen(pedido_file, "rw+");

	int lock = flock(fileno(fp), LOCK_EX);

	if (lock == -1){
		log_error(logger, "NO PUDE CREAR EL LOCK");
	}

	char* pedido = get_pedido_data(restaurante, id_pedido);
	pedido =  data_to_char(pedido);

	char** pedido_info = string_split(pedido, " ");
	char* estado = pedido_info[0];
	char* platos = sacar_corchetes(pedido_info[1]);
	char* cantidad_lista = sacar_corchetes(pedido_info[3]);
	char* cantidad_total = sacar_corchetes(pedido_info[2]);
	//Responder el mensaje indicando si se pudo realizar en conjunto con la información del pedido si correspondiera.

	char* respuesta[4] = {estado, platos, cantidad_lista, cantidad_total};
	send_messages_socket(socket, respuesta, 4);

	flock(fileno(fp), LOCK_UN);
	fclose(fp);
}

void handle_obtener_restaurante(int socket, char* restaurante){
	//Verificar si el Restaurante existe dentro del File System. 
	//Para esto se deberá buscar dentro del directorio Restaurantes si existe un subdirectorio con el nombre del Restaurante. 
	//En caso de no existir se deberá informar dicha situación.
	if (!existe_restaurante(restaurante)){
		log_error(logger, "[Obtener Restaurante] El restaurante no existe.");
		char* respuesta[1] = {"El restaurante no existe"};
		send_messages_socket(socket, respuesta, 1);
		return;
	}

	//Obtener todo los datos del archivo info.AFIP.
	// char* [7] = [afinidades] [pos x] [pos y] [platos] [precioplatos] [cantidadHornos] [cantidadPedidos] [cantidadCocineros]
	char* restaurante_char = get_restaurante_data(restaurante);
	restaurante_char = data_to_char(restaurante_char);
	char** restaurante_info = string_split(restaurante_char, " ");

	char** posiciones = string_get_string_as_array(restaurante_info[1]);
	char* afinidades = sacar_corchetes(restaurante_info[2]);
	char* platos = sacar_corchetes(restaurante_info[3]);
	char* precio_platos = sacar_corchetes(restaurante_info[4]);
	char* cantidad_hornos = restaurante_info[5];
	char* cantidad_cocineros =  restaurante_info[0];
	char* cantidad_pedidos = string_itoa(obtener_cantidad_pedidos(restaurante));

	char* respuesta[8] = {afinidades, posiciones[0], posiciones[1], platos, precio_platos, cantidad_hornos, cantidad_pedidos, cantidad_cocineros};
	send_messages_socket(socket, respuesta, 8);
	//Responder el mensaje indicando los datos del Restaurante.

}

int obtener_cantidad_pedidos(char* restaurante){
	char* restaurante_path = get_path_restaurante(restaurante);
	DIR* restaurante_dir = opendir(restaurante_path);
	struct dirent* entry;
    int pedidos = 0;
	while((entry=readdir(restaurante_dir))){
		pedidos++;
	}
	closedir(restaurante_dir);
	return pedidos > 1 ? pedidos-3 : 0;
}

void handle_plato_listo(int socket, char* restaurante, char* id_pedido, char* comida){

	//Verificar si el Restaurante existe dentro del File System. 
	//Para esto se deberá buscar dentro del directorio Restaurantes si existe un subdirectorio con el nombre del Restaurante. 
	//En caso de no existir se deberá informar dicha situación.
	if (!existe_restaurante(restaurante)){
		log_error(logger, "[Plato Listo] El restaurante no existe.");
		char* respuesta[1] = {"El restaurante no existe"};
		send_messages_socket(socket, respuesta, 1);
		return;
	}

	//Verificar si el Pedido existe dentro del File System. 
	//Para esto se deberá buscar dentro del directorio del Restaurante si existe dicho pedido. 
	//En caso de no existir se deberá informar dicha situación.
	if (!existe_pedido(restaurante, id_pedido)){
		log_error(logger, "[Plato Listo] El pedido no existe");
		char* respuesta[1] = {"El pedido no existe."};
		send_messages_socket(socket, respuesta, 1);
		return;
	}

	char* pedido_file = get_path_pedido_file(restaurante, id_pedido);

	FILE* fp = fopen(pedido_file, "rw+");

	int lock = flock(fileno(fp), LOCK_EX);

	if (lock == -1){
		log_error(logger, "NO PUDE CREAR EL LOCK");
	}

	char* pedido = get_pedido_data(restaurante, id_pedido);

	pedido =  data_to_char(pedido);

	char** pedido_info = string_split(pedido, " ");

	//Verificar que el pedido esté en estado “Confirmado”. 
	//En caso contrario se deberá informar dicha situación.
	if (strcmp(pedido_info[0], "Confirmado") != 0){
		log_error(logger, "[Plato Listo] El pedido no esta en estado Confirmado");
		char* respuesta[1] = {"El pedido no esta en estado Confirmado"};
		send_messages_socket(socket, respuesta, 1);
		flock(fileno(fp), LOCK_UN);
		fclose(fp);
		return;
	}

	//Verificar si ese plato ya existe dentro del archivo. 
	//En caso de existir, se deberá aumentar en uno la cantidad lista de ese plato. 
	//En caso contrario se deberá informar dicha situación.
	char** platos = string_split(sacar_corchetes(pedido_info[1]), ",");
	char** cantidad_lista = string_split(sacar_corchetes(pedido_info[3]), ",");
	bool actualizado = false;
	char* cantidad_lista_actualizada = string_new();
	string_append(&cantidad_lista_actualizada, "[");
	int i = 0;
	while(platos[i] != NULL){
		if (strcmp(platos[i], comida) == 0){
			cantidad_lista[i] = string_itoa(atoi(cantidad_lista[i]) + 1);
			actualizado = true;
		}
		if (i != 0){
			string_append(&cantidad_lista_actualizada, ",");
		}
		string_append(&cantidad_lista_actualizada, cantidad_lista[i]);
		i++;
	}

	if (!actualizado){
		log_error(logger, "[Plato Listo] No existe ese plato en ese archivo");
		char* respuesta[1] = {"No existe ese plato en ese archivo"};
		send_messages_socket(socket, respuesta, 1);
		flock(fileno(fp), LOCK_UN);
		fclose(fp);
		return;
	}
	string_append(&cantidad_lista_actualizada, "]");
	pedido_info[3] = cantidad_lista_actualizada;

	char* data_actualizada = string_new();
	string_append_with_format(&data_actualizada, "ESTADO_PEDIDO=%s\nLISTA_PLATOS=%s\nCANTIDAD_PLATOS=%s\nCANTIDAD_LISTA=%s\nPRECIO_TOTAL=%s", pedido_info[0], pedido_info[1], pedido_info[2], pedido_info[3], pedido_info[4]);

	char* path_pedido = get_path_pedido_file(restaurante, id_pedido);
	bool updated = update_afip_file(data_actualizada, path_pedido);

	flock(fileno(fp), LOCK_UN);
	fclose(fp);

	if (!updated){
		log_error(logger, "[Plato Listo] No se pudo guardar el contenido actualizado");
		char* respuesta[1] = {"No se pudo guardar el contenido actualizado"};
		send_messages_socket(socket, respuesta, 1);
		return;
	}

	//Responder el mensaje indicando si se pudo realizar la operación correctamente (Ok/Fail).
	char* respuesta[1] = {"Ok"};
	send_messages_socket(socket, respuesta, 1);
}

void handle_obtener_receta(int socket, char* comida){

	//Verificar si existe el plato dado dentro del directorio de recetas.
	//En caso de no existir, se deberá informar dicha situación.
	if (!existe_receta(comida)){
		log_error(logger, "[Obtener Receta] La receta no existe.");
		char* respuesta[1] = {"La receta no existe."};
		send_messages_socket(socket, respuesta, 1);
		return;
	}

	char* receta_char = data_to_char(get_receta_data(comida));

	char** receta_pasos_y_tiempos = string_split(receta_char, " ");
	char* receta_pasos = sacar_corchetes(receta_pasos_y_tiempos[0]);
	char* receta_tiempos = sacar_corchetes(receta_pasos_y_tiempos[1]);

	char* respuesta[3] = {comida, receta_pasos, receta_tiempos};

	//Responder el mensaje con la receta solicitada.
	send_messages_socket(socket, respuesta, 3);

	
}

char* sacar_corchetes(char* array){
	return string_substring(array, 1, strlen(array) - 2 );
}

void handle_terminar_pedido(int socket, char* id_pedido, char* restaurante){

	//Verificar si el Restaurante existe dentro del File System. 
	//Para esto se deberá buscar dentro del directorio Restaurantes si existe un subdirectorio con el nombre del Restaurante. 
	//En caso de no existir se deberá informar dicha situación.
	if (!existe_restaurante(restaurante)){
		log_error(logger, "[Terminar Pedido] El restaurante no existe");
		char* respuesta[1] = {"El restaurante no existe."};
		send_messages_socket(socket, respuesta, 1);
		return;
	}

	//Verificar si el Pedido existe dentro del File System. 
	//Para esto se deberá buscar dentro del directorio del Restaurante si existe dicho pedido. 
	//En caso de no existir se deberá informar dicha situación.
	if (!existe_pedido(restaurante, id_pedido)){
		log_error(logger, "[Terminar Pedido] El pedido no existe");
		char* respuesta[1] = {"El pedido no existe."};
		send_messages_socket(socket, respuesta, 1);
		return;
	}
	
	char* pedido_file = get_path_pedido_file(restaurante, id_pedido);

	FILE* fp = fopen(pedido_file, "rw+");

	int lock = flock(fileno(fp), LOCK_EX);

	if (lock == -1){
		log_error(logger, "NO PUDE CREAR EL LOCK");
	}

	char* pedido = get_pedido_data(restaurante, id_pedido);
	pedido =  data_to_char(pedido);
	char** pedido_info = string_split(pedido, " ");

	//Verificar que el pedido esté en estado “Confirmado”. En caso contrario se deberá informar dicha situación.
	if (strcmp(pedido_info[0], "Confirmado") != 0){ //TODO: CAMBIE ACA
		log_error(logger, "[Terminar Pedido] El pedido no esta en estado Confirmado");
		char* respuesta[1] = {"El pedido no esta en estado Confirmado"};
		send_messages_socket(socket, respuesta, 1);
		flock(fileno(fp), LOCK_UN);
		fclose(fp);
		return;
	}

	//Cambiar el estado del pedido a “Terminado” 
	pedido_info[0] = string_new();
	string_append(&pedido_info[0], "Terminado");

	char* data_actualizada = string_new();
	string_append_with_format(&data_actualizada, "ESTADO_PEDIDO=%s\nLISTA_PLATOS=%s\nCANTIDAD_PLATOS=%s\nCANTIDAD_LISTA=%s\nPRECIO_TOTAL=%s", pedido_info[0], pedido_info[1], pedido_info[2], pedido_info[3], pedido_info[4]);

	char* path_pedido = get_path_pedido_file(restaurante, id_pedido);
	bool updated = update_afip_file(data_actualizada, path_pedido);

	flock(fileno(fp), LOCK_UN);
	fclose(fp);

	if (!updated){
		log_error(logger, "[Terminar Pedido] No se pudo guardar el contenido actualizado");
		char* respuesta[1] = {"No se pudo guardar el contenido actualizado"};
		send_messages_socket(socket, respuesta, 1);
		return;
	}

	//Responder el mensaje indicando si se pudo realizar la operación correctamente (Ok/Fail).
	char* respuesta[1] = {"OK"};
	send_messages_socket(socket, respuesta, 1);
}

void handle_error(int socket){
	char* respuesta[1];
	respuesta[0] = "ERROR";
	send_messages_socket(socket, respuesta, 1);
}

void handle_crear_restaurante(char* nombre, char* cantidad_cocineros, char* posicion,
	char* afinidad_cocineros, char* platos, char* precio_platos, char* cantidad_hornos){

	//CHECK PARAMETROS
	if (nombre == NULL){
		printf("El nombre es obligatorio. \n");
		return;
	}

	if (cantidad_cocineros == NULL){
		printf("La cantidad de cocineros es obligatoria. \n");
		return;
	}

	if (posicion == NULL){
		printf("La posicion es obligatoria. \n");
		return;
	}

	if (afinidad_cocineros == NULL){
		printf("La afinidad de los cocineros es obligatoria. \n");
		return;
	}

	if (platos == NULL){
		printf("Los platos son obligatorios. \n");
		return;
	}

	if (precio_platos == NULL){
		printf("Los precios de los platos son obligatorios. \n");
		return;
	}

	if (cantidad_hornos == NULL){
		printf("La cantidad de hornos es obligatoria. \n");
		return;
	}

	//VERIFICAR QUE NO EXISTE EL RESTAURANTE
	if (existe_restaurante(nombre)){
		printf("Ya existe un restaurante con el nombre: %s \n", nombre);
		return;
	}

	get_or_create_folder(get_path_restaurante(nombre));
	char* content = string_new();
	string_append_with_format(&content, "CANTIDAD_COCINEROS=%s\n", cantidad_cocineros);
	string_append_with_format(&content, "POSICION=%s\n", posicion);
	string_append_with_format(&content, "AFINIDAD_COCINEROS=%s\n", afinidad_cocineros);
	string_append_with_format(&content, "PLATOS=%s\n", platos);
	string_append_with_format(&content, "PRECIO_PLATOS=%s\n", precio_platos);
	string_append_with_format(&content, "CANTIDAD_HORNOS=%s", cantidad_hornos);

	create_afip_file(content, get_path_info_file(nombre));
}

void handle_crear_receta(char* nombre, char* pasos, char* tiempo_pasos){

	//CHECK PARAMETROS
	if (nombre == NULL){
		printf("El nombre de la receta es obligatorio. \n");
		return;
	}

	if (pasos == NULL){
		printf("Los pasos de la receta son obligatorios. \n");
		return;
	}

	if (tiempo_pasos == NULL){
		printf("El tiempo de los pasos de la receta son obligatorios obligatoria. \n");
		return;
	}

	//VERIFICAR QUE NO EXISTE LA RECETA
	if (existe_receta(nombre)){
		printf("Ya existe una receta con el nombre: %s \n", nombre);
		return;
	}

	char* content = string_new();
	string_append_with_format(&content, "PASOS=%s\n", pasos);
	string_append_with_format(&content, "TIEMPO_PASOS=%s", tiempo_pasos);
	create_afip_file(content, get_path_receta_file(nombre));

}
/* --- END SUITES DE HANDLES --- */

void iniciar_consola(){
	do {
		printf("Ingrese alguno de los siguientes comandos: \n");
		//CrearRestaurante MiRestaurante 5 [4,5] [Milanesa] [Milanesa,Empanadas,Ensalada] [200,50,150] 2
		//CrearRestaurante MiRestaurante 5 [4, 5] [Milanesa] [Milanesa, Empanadas, Ensalada] [200, 50, 150] 2
		printf("1. CrearRestaurante [NOMBRE] [CANTIDAD_COCINEROS] [POSICION] [AFINIDAD_COCINEROS] [PLATOS] [PRECIO_PLATOS] [CANTIDAD_HORNOS] \n");
		//CrearReceta Milanesa [Trocear,Empanar,Reposar,Hornear] [4,5,3,10]
		//CrearReceta Milanesa [Trocear, Empanar, Reposar, Hornear] [4, 5, 3, 10]
		printf("2. CrearReceta [NOMBRE] [PASOS] [TIEMPO_PASOS] \n");
		printf("3. Exit \n");
		char * line = getlinefromconsole();
		process_line(line);
		if (strcmp("Exit\n", line) == 0){
			printf("Se reconoce el comando Exit: %s\n", line);
			break;
		}
	} while (1);
	
}

void process_line(char* line){
	if (string_length(line) <= 1){
		return;
	}
	line = string_substring_until(line, (string_length(line)-1));
	char** lineas = string_split(line, " ");
	char* array[8];
	int longitud = 0, long_aux = 0;
	while (lineas[long_aux] != NULL){
		char* aux_string = string_new();
		string_append(&aux_string, lineas[long_aux]);
		if (string_starts_with(lineas[long_aux], "[")){
			while(!string_ends_with(lineas[long_aux], "]")){
				long_aux++;
				string_append(&aux_string, lineas[long_aux]);
			}
		}
		array[longitud] = aux_string;
		long_aux++;
		longitud++;
	}

	if (strcmp("CrearRestaurante", array[0])== 0){
		printf("Se reconoce el comando Crear Restaurante: %s\n", line);
		if (longitud < 8){
			printf("No se tienen los suficientes parametros para llamar a Crear restaurante, se requieren 7 y se obtuvieron: %d\n", longitud - 1);
		}
		//OK
		handle_crear_restaurante(array[1], array[2], array[3], array[4], array[5], array[6], array[7]);
		return;
	}
	if (strcmp("CrearReceta", array[0])== 0){
		printf("Se reconoce el comando Crear Receta: %s\n", line);
		if (longitud < 4){
			printf("No se tienen los suficientes parametros para llamar a Crear restaurante, se requieren 7 y se obtuvieron: %d\n", longitud - 1);
		}
		//OK
		handle_crear_receta(array[1], array[2], array[3]);
		return;
	}
	if (strcmp("Exit\n", array[0]) != 0){
		printf("No se reconoce el comando ingresado: %s\n", line);
		return;
	}
}

char * getlinefromconsole(void) {
	char * line = malloc(100), * linep = line;
	size_t lenmax = 100, len = lenmax;
	int c;

	if(line == NULL)
		return NULL;

	for(;;) {
		c = fgetc(stdin);
		if(c == EOF)
			break;

		if(--len == 0) {
			len = lenmax;
			char * linen = realloc(linep, lenmax *= 2);

			if(linen == NULL) {
				free(linep);
				return NULL;
			}
			line = linen + (line - linep);
			linep = linen;
		}

		if((*line++ = c) == '\n')
			break;
	}
	*line = '\0';
	return linep;
}

