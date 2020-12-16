#include "sindicato_fs.h"

/* --- SUITES DE PATH O RUTAS --- */
char* get_path_restaurante(char* restaurante){
	char* path = string_new();
	string_append(&path, sindicato_config->punto_montaje);
	string_append(&path, "/Files/Restaurantes/");
	string_append(&path, restaurante);
	return path;
}

char* get_path_info_file(char* restaurante){
	char* path = string_new();
	string_append(&path, sindicato_config->punto_montaje);
	string_append(&path, "/Files/Restaurantes/");
	string_append(&path, restaurante);
	string_append(&path, "/Info.AFIP");
	return path;
}

char* get_path_pedido_file(char* restaurante, char* id_pedido){
	char* path = string_new();
	string_append(&path, sindicato_config->punto_montaje);
	string_append(&path, "/Files/Restaurantes/");
	string_append(&path, restaurante);
	string_append(&path, "/Pedido");
	string_append(&path, id_pedido);
	string_append(&path, ".AFIP");
	return path;
}

char* get_path_receta_file(char* nombre_receta){
	char* path = string_new();
	string_append(&path, sindicato_config->punto_montaje);
	string_append(&path, "/Files/Recetas/");
	string_append(&path, nombre_receta);
	string_append(&path, ".AFIP");
	return path;
}

char* get_path_bitmap_file(){
	char* path = string_new();
	string_append(&path, sindicato_config->punto_montaje);
	string_append(&path, "/Metadata/Bitmap.bin");
	return path;
}

char* get_path_block_file(uint32_t id){
	char* path = string_new();
	string_append(&path, sindicato_config->punto_montaje);
	string_append(&path, string_from_format("/Blocks/%d.AFIP", id));
	return path;
}
/* --- END SUITES DE PATH O RUTAS --- */

/* --- SUITES DE FILES PARSER --- */
/* -- REFACTOR -- */
t_list* strings_to_list(char** strings) {
	t_list* list = list_create();
	while (*strings != NULL) {
		list_add(list, strdup(*strings));
		strings++;
	}
	return list;
}

List* strings_to_List(char** strings) {
	List* list = malloc(sizeof(List));;
	initlist(list);
	while (*strings != NULL) {
		pushbacklist(list, strdup(*strings));
		strings++;
	}
	return list;
}

t_estado_pedido config_get_estado_pedido(t_config* config, char* key){
	if (!config_has_property(config, key)) exit(EXIT_FAILURE);

	char* value = config_get_string_value(config, key);
	if (strcmp(value, "Pendiente")){
		return PENDIENTE;
	} else if (strcmp(value, "Confirmado")){
		return CONFIRMADO;
	} else {//if (strcmp(valloc, "Terminado")){
		return TERMINADO;
	}
}
/* -- END REFACTOR -- */
t_posicion* get_position_from_config(t_config* config, char* key){
	char** posicion_str = config_get_array_value(config, key);
	t_posicion* posicion_aux = malloc(sizeof(posicion_str));
	posicion_aux->x = atoi(posicion_str[0]);
	posicion_aux->y = atoi(posicion_str[1]);
	return posicion_aux;
}

void info_file_parser(t_config* config, t_info* info_config){
	//preparing lists
	char** afinidad_cocineros_str = config_get_array_value(config, "AFINIDAD_COCINEROS");
	char** platos_str = config_get_array_value(config, "PLATOS");
	char** precio_platos_str = config_get_array_value(config, "PRECIO_PLATOS");

	info_config->cantidad_cocineros = config_get_int_value(config, "CANTIDAD_COCINEROS");
	info_config->posicion = get_position_from_config(config, "POSICION");
	info_config->afinidad_cocineros = strings_to_list(afinidad_cocineros_str);
	info_config->platos = strings_to_list(platos_str);
	info_config->precio_platos = strings_to_list(precio_platos_str);
	info_config->cantidad_hornos = config_get_int_value(config, "CANTIDAD_COCINEROS");
}

t_info* create_info_config(char* restaurante){
	char* path_info = get_path_info_file(restaurante);
	t_config* config = config_create(path_info);
	t_info* info_config = malloc(sizeof(t_info));

	info_file_parser(config, info_config);
	//TODO: config_destroy(config);
	return info_config;
}

void pedido_file_parser(t_config* config, t_pedido* pedido_config){
	//preparing lists
	char** lista_platos_str = config_get_array_value(config, "LISTA_PLATOS");
	char** cantidad_platos_str = config_get_array_value(config, "CANTIDAD_PLATOS");
	char** cantidad_lista_str = config_get_array_value(config, "CANTIDAD_LISTA");

	// pedido_config->estado_pedido = config_get_string_value(config, "ESTADO_PEDIDO");
	pedido_config->estado_pedido = config_get_estado_pedido(config, "ESTADO_PEDIDO");
	pedido_config->lista_platos = strings_to_list(lista_platos_str);
	pedido_config->cantidad_platos = strings_to_list(cantidad_platos_str);
	pedido_config->cantidad_lista = strings_to_list(cantidad_lista_str);
	pedido_config->precio_total = config_get_int_value(config, "PRECIO_TOTAL");
}

t_pedido* create_pedido_config(char* restaurante, char* id_pedido){
	char* path_pedido = get_path_pedido_file(restaurante, id_pedido);
	t_config* config = config_create(path_pedido);
	t_pedido* pedido_config = malloc(sizeof(t_pedido));

	pedido_file_parser(config, pedido_config);
	//TODO: config_destroy(config);
	return pedido_config;
}

/* --- END SUITES DE FILES PARSER--- */

/* --- SUITES DE FUNCIONES READ--- */
t_afip_file* read_afip_file(char* path){
	t_afip_file* afip_file = malloc(sizeof(t_afip_file));
	t_config* config = config_create(path);

	afip_file->initial_block = config_get_int_value(config, "INITIAL_BLOCK");
	afip_file->size = config_get_int_value(config, "SIZE");
	config_destroy(config);

	return afip_file;
}

char* get_block_data(uint32_t block){
	char* block_file_path = get_path_block_file(block);
	FILE* block_file = get_or_create_file(block_file_path, "rb");
	int size = get_data_size(block_file);
	char* data = calloc(size + 1 , sizeof(char));
	fread (data, size, 1, block_file);
	fclose(block_file);
	data[size] = '\0';
	return data;
}

int get_data_size(FILE* fp){
	fseek(fp, -sizeof(int32_t), SEEK_END);
	int size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	return size;
}

char* read_blocks(t_afip_file* afip_file){
	char* data = string_new();
	uint32_t siguiente = afip_file->initial_block;
	while(siguiente!=-1){
		// TODO: puede devolver EXIT_FAILURE
		string_append(&data, get_block_data(siguiente));
		siguiente = getSiguienteBloque(siguiente);
	}
	return data;
}

char* read_receta(char* nombre_receta){
	// t_afip_file* afip_file = read_afip_file(get_path_receta_file(nombre_receta));
	// char* data = read_blocks(afip_file);
	// t_receta* receta = malloc(sizeof(t_receta));
	// // receta->pasos = strings_to_List(pasos_str);
	// // receta->tiempo_paso = strings_to_List(tiempo_paso_str);
	// // TODO: hay que completar receta con la data
	return NULL;//receta;
}
/* --- END SUITES DE FUNCIONES READ--- */

void get_or_create_fs() {

	char * punto_de_montaje = sindicato_config->punto_montaje;

	char * metadata_adress = string_new();
	string_append(&metadata_adress, sindicato_config->punto_montaje);
	string_append(&metadata_adress, "/Metadata");

	char* metadata_afip_adress = string_new();
	string_append(&metadata_afip_adress, metadata_adress);
	string_append(&metadata_afip_adress, "/Metadata.AFIP");

	get_or_create_folder(punto_de_montaje);
	get_or_create_folder(metadata_adress);
	int necesita_recrearse = 0;


	if (existe_archivo(metadata_afip_adress) ) {
		necesita_recrearse = leer_metadata_afip(metadata_afip_adress);
	} else {
		necesita_recrearse = 1;
	}

	if (necesita_recrearse) {
		/* Crear solo con write, para que se pise lo anterior */
		crear_metadata_default(metadata_afip_adress);
		crear_bitmap();
		crear_files();
	}
}
/* < 0 si no puede crear el punto de montaje, 0 si ya existia previamente, > 0 si creo correctamente el punto de montaje*/
int get_or_create_folder(char* file_adress){

	DIR* folder_dir;

	if ((folder_dir = opendir(file_adress)) == NULL){
		int rv = mkdir(file_adress, 0777);
		if (rv == 0){
			log_info(logger, "Se creo la carpeta: %s", file_adress);
			closedir(folder_dir);
			return 1;
		}
		else {
			log_error(logger, "No se pudo crear la carpeta");
			return -1;
		}
	}
	else {
		closedir(folder_dir);
		return 0;
	}
}

void crear_metadata_default(char * metadata_afip_adress){
	FILE *fp;
	
	fp = fopen( metadata_afip_adress , "w" );

	char * block_size = string_new(); 
	string_append(&block_size, "BLOCK_SIZE=");
	string_append(&block_size, sindicato_config->block_size);
	string_append(&block_size, "\n");
    char * blocks = string_new(); 
	string_append(&blocks, "BLOCKS=");
	string_append(&blocks, sindicato_config->blocks);
	string_append(&blocks, "\n");
    char * magic_number = string_new(); 
	string_append(&magic_number, "MAGIC_NUMBER=");
	string_append(&magic_number, sindicato_config->magic_number);
	string_append(&magic_number, "\n");

	char * config = string_new();
	string_append(&config, block_size);
	string_append(&config, blocks);
	string_append(&config, magic_number);

	fputs(config, fp);
	fclose(fp);

	char log1[200];
	sprintf(log1, "[Crear Metadata] Se creó el archivo %s\n", metadata_afip_adress);
	log_info(logger, log1);
}

/* Devuelve 1 si necesita recrear el FS */
int leer_metadata_afip(char * metadata_afip_adress){
	t_config* metadata_afip_config = config_create(metadata_afip_adress);

	char * block_size = config_get_string_value(metadata_afip_config, "BLOCK_SIZE");
	char * blocks = config_get_string_value(metadata_afip_config, "BLOCKS");
	char * magic_number = config_get_string_value(metadata_afip_config, "MAGIC_NUMBER");

	printf("El tamaño de bloque es de %s\n", block_size);
	printf("La cantidad de bloques es %s\n", blocks);
	printf("El numero magico es %s\n", magic_number); 

	if (necesita_recrearse(block_size, blocks, magic_number)){
		return 1;
	}

	return 0;
}

int necesita_recrearse(char * block_size, char * blocks, char * magic_number){

	if (strcmp(block_size, sindicato_config->block_size) != 0 || strcmp(blocks, sindicato_config->blocks) != 0 || strcmp(magic_number, sindicato_config->magic_number) != 0) {
		return 1;
	}
	return 0;

}

FILE * get_or_create_file(char* path_file, char * mode){
	FILE* file = fopen(path_file, mode);
	if (file == NULL){
		log_error(logger, "[Get Or Create File] No se creo el archivo");
		exit(-1);
	}

	return file;
}
/* --- BITMAP --- */
FILE * get_or_create_bitmap_file(char * mode){
	char * bitmap_address = string_new();
	string_append(&bitmap_address, sindicato_config->punto_montaje);
	string_append(&bitmap_address, "/Metadata/Bitmap.bin");

	FILE * bitmap_file = fopen(bitmap_address, mode);

	if (bitmap_file == NULL){
		printf("ERROR DE LISTEN: %s | NUMERO DE ERRNO: %d\n", strerror(errno), errno);
		return bitmap_file;
	}

	free(bitmap_address);

	return bitmap_file;
}

void update_bitmap_file(t_bitarray * bitmap){
	FILE * bitmap_file = get_or_create_file(get_path_bitmap_file(), "wb");
	if (bitmap_file == NULL)
		log_error(logger, "No se pudo obtener 'bitmap file'");

	fwrite(bitmap->bitarray, sizeof(char), bitmap->size, bitmap_file);

	fclose(bitmap_file);
}

void crear_bitmap(){
	int blocks = atoi(sindicato_config->blocks);

	size_t bytes = BIT_SIZE(blocks, CHAR_BIT);
	char * bitarray = calloc(bytes, sizeof(char));
	t_bitarray	* bitmap = bitarray_create_with_mode(bitarray, bytes, LSB_FIRST);

	for(int pos=0; pos < blocks; pos++){
		//Limpia los bits del bitarray (Los pone en 0)
		bitarray_clean_bit(bitmap, pos);
	}
	update_bitmap_file(bitmap);

	char log1[200];
	sprintf(log1, "[Crear Bitmap] Se creó el archivo %s", get_path_bitmap_file());
	log_info(logger, log1);
	// bitarray_destroy(bitmap);
	
}

t_bitarray* get_bitarray(){
	FILE * bitmap_file = get_or_create_file(get_path_bitmap_file(), "rb");
	if (bitmap_file == NULL)
		log_error(logger, "No se pudo obtener 'bitmap file'");

	int blocks = atoi(sindicato_config->blocks);

	size_t bitarray_size = BIT_SIZE(blocks, CHAR_BIT);
	char* bitarray = malloc(bitarray_size);
	size_t read_bytes = fread(bitarray, 1, bitarray_size, bitmap_file);

	if (read_bytes != bitarray_size) {
		fclose(bitmap_file);
		free(bitarray);
		log_info(logger,"El Bitmap esta incompleto");
		return NULL;
	}

	fclose(bitmap_file);

	return bitarray_create_with_mode(bitarray, bitarray_size, LSB_FIRST);
}

void modify_block(t_bitarray * bitmap, bool status, int position){
	if(status) //si esta libre ese bloque
		bitarray_set_bit(bitmap, position);

	else   //si esta ocupado ese bloque
		bitarray_clean_bit(bitmap, position);
}

void take_block(int block_pos){
	// Setea 1 en la posicion indicada (bloque ocupado)
	pthread_mutex_lock(&mutex_bitmap);
	t_bitarray * bitmap = get_bitarray();
	modify_block(bitmap, true, block_pos);
	update_bitmap_file(bitmap);
	pthread_mutex_unlock(&mutex_bitmap);
}

void free_block(uint32_t block_pos){
	// Setea 0 en la posicion indicada (bloque libre)
	pthread_mutex_lock(&mutex_bitmap);
	t_bitarray * bitmap = get_bitarray();
	modify_block(bitmap, false, block_pos);
	update_bitmap_file(bitmap);
	pthread_mutex_unlock(&mutex_bitmap);
}
int get_available_block_in_bitmap(){
	int index = -1;
	bool bit = true;
	t_bitarray* bitmap = get_bitarray();
	int max_bit = bitarray_get_max_bit(bitmap);

	while((index<max_bit)&&(bit==true)){
		index++;
		bit = bitarray_test_bit(bitmap, index);
	}

	if (bit){
		log_error(logger, "[Get Available Block In Bitmap] No se obtuvo un bloque sin usar.");
		return -1;
	}
	take_block(index);
	return index;
}
/* --- END BITMAP --- */


bool existe_archivo(char* ruta_archivo){
	FILE *fp;

	if ((fp = fopen(ruta_archivo, "r")) == NULL){
		return false;
	} else {
		fclose(fp);
		return true;
	}
}

void crear_files(){
	char * files_adress = string_new();
	string_append(&files_adress, sindicato_config->punto_montaje);
	string_append(&files_adress, "/Files");

	char * restaurantes_adress = string_new();
	string_append(&restaurantes_adress, files_adress);
	string_append(&restaurantes_adress, "/Restaurantes");

	char * recetas_adress = string_new();
	string_append(&recetas_adress, files_adress);
	string_append(&recetas_adress, "/Recetas");

	char * bloques_adress = string_new();
	string_append(&bloques_adress, sindicato_config->punto_montaje);
	string_append(&bloques_adress, "/Blocks");

	get_or_create_folder(files_adress);
	get_or_create_folder(restaurantes_adress);
	get_or_create_folder(recetas_adress);
	get_or_create_folder(bloques_adress);
	create_blocks();
}

void create_blocks(){
	int cantidad_bloques = atoi(sindicato_config->blocks);
	for(int id=0; id < cantidad_bloques; id++){
		FILE * bloque = get_or_create_file(get_path_block_file(id), "w");
		fclose(bloque);
	}

	char log1[200];
	sprintf(log1, "[Crear Blocks] Se crearon los %d bloques\n", cantidad_bloques);
	log_info(logger, log1);


}

bool existe_restaurante(char* restaurante){
	DIR* folder_dir;

	char* path_restaurante = get_path_restaurante(restaurante);

	if ((folder_dir = opendir(path_restaurante)) == NULL){
		return false;
	} else {
		closedir(folder_dir);
		return true;
	}
}

bool existe_pedido(char* restaurante, char* nro_pedido){
	FILE *fp;

	char* path_pedido = get_path_pedido_file(restaurante, nro_pedido);

	if ((fp = fopen(path_pedido, "r")) == NULL){
		return false;
	} else {
		fclose(fp);
		return true;
	}
}

bool existe_receta(char* receta){
	FILE *fp;

	char* path_receta = get_path_receta_file(receta);

	if ((fp = fopen(path_receta, "r")) == NULL){
		return false;
	} else {
		fclose(fp);
		return true;
	}
}

int calculate_blocks_required(char* string){
	int string_size = string_length(string);
	int wearable_size = atoi(sindicato_config->block_size) - sizeof(uint32_t);
	return (string_size / wearable_size) + ((string_size % wearable_size) ? 1 : 0);
}

int get_available_block(){
	return get_available_block_in_bitmap();
}

bool save_afip_file(char* path, t_afip_file* afip_file){
	FILE* pedido = fopen(path, "w");
	if (pedido == NULL){
		log_error(logger, "[Save Afip File] No se creo el archivo de pedido");
		return false;
	}

	t_config* config = config_create(path);
	config_set_value(config, "SIZE", string_itoa(afip_file->size));
	config_set_value(config, "INITIAL_BLOCK", string_itoa(afip_file->initial_block));
	config_save_in_file(config, path);

	fclose(pedido);
	return true;
}

bool save_block(int initial, int next, char* content){
	FILE * bloque = get_or_create_file(get_path_block_file(initial), "w");
	if (bloque == NULL){
		log_error(logger, "[Save Block] No se obtuvo el archivo bloque");
		return false;
	}
	int finish_code = fwrite(content, strlen(content), 1, bloque);
	finish_code = fwrite(&next, sizeof(uint32_t), 1, bloque);
	fclose(bloque);
	if (finish_code < 0){
		return false;
	}
	return true;
}

bool save_in_blocks(int initial_block, char* content, int number_of_blocks){
	log_info(logger, "[Save In Block] Se procede a guardar en bloques");
	int block_size = atoi(sindicato_config->block_size) - sizeof(uint32_t);
	int next_block;
	int finish_code;

	List* bloques_actuales = obtenerBloquesActuales(initial_block);
	if (number_of_blocks > (int)sizelist(*bloques_actuales)){
		int faltante = number_of_blocks - (int)sizelist(*bloques_actuales);
		while(faltante>0){
			int available_block = get_available_block();
			uint32_t* block = malloc(sizeof(uint32_t));
			*block = available_block;
			pushbacklist(bloques_actuales, block);
			faltante--;
			char log1[200];
			sprintf(log1, "[Save In Block] Se asignó el nuevo bloque: %d a la cadena iniciada por %d\n", available_block, initial_block);
			log_info(logger, log1);
		}
	} else if (number_of_blocks < (int)sizelist(*bloques_actuales)){
		int restante = (int)sizelist(*bloques_actuales) - number_of_blocks;
		while(restante>0){
			uint32_t* block = popbacklist(bloques_actuales);
			free_block(*block);
			restante--;
			char log1[200];
			sprintf(log1, "[Save In Block] Se desasigno el bloque: %d a la cadena iniciada por %d\n", *block, initial_block);
			log_info(logger, log1);
			free(block);
		}
	}
	initial_block = *(int *)popfrontlist(bloques_actuales);
	for(int i=0; i<number_of_blocks; i++){
		char* sub_string = string_substring_until(content, block_size);
		if ((int)sizelist(*bloques_actuales)==0){
			next_block = -1;
		} else {
			content = string_substring_from(content, block_size);
			next_block = *(int *)popfrontlist(bloques_actuales);
		}
		finish_code = save_block(initial_block, next_block, sub_string);
		if(!finish_code){
			return false;
		}
		initial_block = next_block;
	}
	return true;
}

List* obtenerBloquesActuales(uint32_t initial_block){
	List* lista = malloc(sizeof(List));
	initlist(lista);

	uint32_t siguiente = initial_block;
	while (siguiente != -1){
		uint32_t* bloque_siguiente = malloc(sizeof(uint32_t));
		*bloque_siguiente = siguiente;
		pushbacklist(lista, bloque_siguiente);
		siguiente = getSiguienteBloque(siguiente);
	}
	
	return lista;
}


uint32_t getSiguienteBloque(uint32_t bloque){
	
	FILE * fp = get_or_create_file(get_path_block_file(bloque), "r");
	if (fp == NULL){
		log_error(logger, "[Save Block] No se obtuvo el archivo bloque");
		return EXIT_FAILURE;
	}
	int resultado = fseek(fp, 0, SEEK_END);
	if (resultado == -1){
		log_error(logger, "[Get Siguiente Bloque] No se pudo mover al final del archivo");
		return EXIT_FAILURE;
	}
	if (ftell( fp ) == 0 ){
		return -1;
	}

	resultado = fseek(fp, -sizeof(uint32_t), SEEK_END);

	if (resultado == -1){
		log_error(logger, "[Get Siguiente Bloque] No se pudo mover a la posicion del siguiente");
		return EXIT_FAILURE;
	}

	uint32_t siguiente = -1;
	resultado = fread(&siguiente, sizeof(uint32_t), 1, fp);

	if (resultado == -1){
		log_error(logger, "[Get Siguiente Bloque] No se pudo mover a la posicion del siguiente");
		return EXIT_FAILURE;
	}

	return siguiente;

}


bool create_afip_file(char* content, char* path){
	int number_of_blocks = calculate_blocks_required(content);

	t_afip_file* afip_file = malloc(sizeof(t_afip_file));
	afip_file->size = string_length(content) + number_of_blocks * sizeof(uint32_t);
	afip_file->initial_block = get_available_block();
	char log3[200];
	sprintf(log3, "[Create Afip File] Se asignó el bloque inicial %d al archivo %s\n", afip_file->initial_block, path);
	log_info(logger, log3);
	if(!save_afip_file(path, afip_file)){
		log_error(logger, "[Create Afip File] No se guardo el archivo afip");
		return false;
	}
	if(!save_in_blocks(afip_file->initial_block, content, number_of_blocks)){
		log_error(logger, "[Create Afip File] No se guardaron los bloques");
		return false;
	}

	char log1[200];
	sprintf(log1, "[Create Afip File] Se creó el archivo %s\n", path);
	log_info(logger, log1);

	return true;
}

bool update_afip_file(char* content, char* path){
	int number_of_blocks = calculate_blocks_required(content);

	t_config* config = config_create(path);
	int bloque_actual = config_get_int_value(config, "INITIAL_BLOCK");

	int size = string_length(content) + number_of_blocks * sizeof(uint32_t);
	config_set_value(config, "SIZE", string_itoa(size));
	config_save_in_file(config, path);

	if(!save_in_blocks(bloque_actual, content, number_of_blocks)){
		log_error(logger, "[Create Afip File] No se guardaron los bloques");
		return false;
	}
	return true;
}

char* afip_file_to_char(char* path){
	return read_blocks(read_afip_file(path));
}

char* get_receta_data(char* nombre_receta){
	return afip_file_to_char(get_path_receta_file(nombre_receta));
}

char* get_restaurante_data(char* restaurante){
	return afip_file_to_char(get_path_info_file(restaurante));
}

char* get_pedido_data(char* restaurante, char* pedido){
	return afip_file_to_char(get_path_pedido_file(restaurante, pedido));
}

char* data_to_char(char* data){
	char** data_split = string_split(data, "\n");
	int i = 0;
	char* data_values = string_new();
	while(data_split[i]!=NULL){
		char** key_and_value = string_split(data_split[i], "=");
		if (i==0){
			string_append(&data_values, key_and_value[1]);
		} else {
			string_append_with_format(&data_values, " %s", key_and_value[1]);
		}
		i++;
	}
	return data_values;
}