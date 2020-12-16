#include "app.h"
int main(void){

    app_init(&app_config, &logger);

    //INSERTO LISTA DE PLATOS DEFAULT
    crear_lista_platos_default();

    //INSERTO RESTAURANTE DEFAULT
    t_restaurante* restaurante = nuevo_restaurante(0, "Resto Default", app_config->posicion_rest_default_x, app_config->posicion_rest_default_y, "", "");
    pushbacklist(&lista_restaurantes, restaurante);

    pthread_t thread;
    pthread_create(&thread,NULL,(void*)iniciar_planificador, NULL);
	pthread_detach(thread);

    iniciar_servidor("127.0.0.1", "5004", handle_client);

    app_finally(app_config, logger);
    return 0;
}

void app_init(t_app_config** app_config, t_log** logger){

    //INICIALIZO LISTAS DE RESTAURANTES Y CLIENTES
    initlist(&lista_clientes);
    initlist(&lista_restaurantes);
    initlist(&listaPlatosDefault);
    initlist(&lista_semaforos_pedidos);

    id_pedido_default = 0;

    sem_pedido_default = malloc(sizeof(sem_t));
    sem_init(sem_pedido_default, 0, 1);
    sem_id_pedido = malloc(sizeof(sem_t));
    sem_init(sem_id_pedido, 0, 1);
    sem_mutex_sjf = malloc(sizeof(sem_t));
    sem_init(sem_mutex_sjf, 0, 1);
    

    *app_config = app_config_loader("./cfg/app.config", logger);

}

void app_finally(t_app_config* app_config, t_log* logger) {
    app_destroy(app_config);
    log_destroy(logger);
}

void crear_lista_platos_default(){

    for(int i = 0; app_config->platos_default[i] != NULL ; i++){

        pushfrontlist(&listaPlatosDefault, app_config->platos_default[i]);
    }

}