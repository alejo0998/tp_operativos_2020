#include "planificador.h"

void iniciar_planificador(){
    
    initlist(&suscriptores_cpu);
    initlist(&repartidores_libres);
    initlist(&pcb_new);
    initlist(&pcb_ready);

    sem_entrenador_libre = malloc(sizeof(sem_t));
    sem_init(sem_entrenador_libre, 0, 0);

    sem_pcb_new =  malloc(sizeof(sem_t));
    sem_init(sem_pcb_new, 0, 0);

    sem_pcb_ready =  malloc(sizeof(sem_t));
    sem_init(sem_pcb_ready, 0, 0);

    sem_ciclo_espera_HRRN = malloc(sizeof(sem_t));
    sem_init(sem_ciclo_espera_HRRN, 0, 0);

    sem_grado_multiprocesamiento = malloc(sizeof(sem_t));
    sem_init(sem_grado_multiprocesamiento, 0, app_config->grado_multiprocesamiento);

    iniciar_repartidores();

    //pcb_prueba();

    iniciar_planificador_largo_plazo();

    iniciar_clock();

    iniciar_planificador_corto_plazo();

}

void iniciar_clock(){
    pthread_t thread;
    pthread_create(&thread,NULL,(void*)clock_cpu, NULL);
	pthread_detach(thread);
}

void clock_cpu(){
    
    while(true){
        sleep(app_config->retardo_ciclo_cpu);

        //log_info(logger, "-----------CICLO-------------");

        for (IteratorList il = beginlist(suscriptores_cpu); il != NULL; il = nextlist(il)){
            sem_t* suscriptor = (sem_t*) dataiterlist(il);
            sem_post(suscriptor);
        }
    }
}

void iniciar_repartidores(){
    
    for (int i = 0; app_config->repartidores[i] != NULL; i++){
        t_repartidor* repartidor_actual = malloc(sizeof(t_repartidor));

        char** posiciones_spliteadas = string_split(app_config->repartidores[i], "|");

        if (app_config->frecuencia_descanso[i] == NULL || app_config->tiempo_descanso[i] == NULL){
            printf("Tiempo o frecuencia de descanso mal inicializada\n");
            return;
        }

        repartidor_actual->id = i;
        repartidor_actual->frecuencia_de_descanso = atoi(app_config->frecuencia_descanso[i]);
        repartidor_actual->tiempo_de_descanso = atoi(app_config->tiempo_descanso[i]);
        repartidor_actual->posicion.posx = atoi(posiciones_spliteadas[0]);
        repartidor_actual->posicion.posy = atoi(posiciones_spliteadas[1]);
        repartidor_actual->cansancio = 0;
        repartidor_actual->nuevo_pedido = malloc(sizeof(sem_t));
        sem_init(repartidor_actual->nuevo_pedido, 0, 0);
        repartidor_actual->ciclo_cpu = malloc(sizeof(sem_t));
        sem_init(repartidor_actual->ciclo_cpu, 0, 0);

        pthread_t thread;
        pthread_create(&thread,NULL,(void*)repartir_pedidos, repartidor_actual);
        pthread_detach(thread);

        pushbacklist(&repartidores_libres, repartidor_actual);
        sem_post(sem_entrenador_libre);

        char string_log[100];
        sprintf(string_log, "Se crea un repartidor %d", repartidor_actual->id);
        log_info(logger, string_log);
        
    }
    
}

void iniciar_planificador_largo_plazo(){
    pthread_t thread;
    pthread_create(&thread,NULL,(void*)planificar_largo_plazo, NULL);
	pthread_detach(thread);
}

void iniciar_planificador_corto_plazo(){
    if (!strcmp(app_config->algoritmo_planificacion, "FIFO")){
        planificar_corto_plazo_FIFO();
    } else if (!strcmp(app_config->algoritmo_planificacion, "SJF")){
        planificar_corto_plazo_SJF();
    } else if (!strcmp(app_config->algoritmo_planificacion, "HRRN")){
        pushbacklist(&suscriptores_cpu, sem_ciclo_espera_HRRN);
        pthread_t thread;
        pthread_create(&thread,NULL,(void*)aumentar_espera_HRRN, NULL);
	    pthread_detach(thread);
        planificar_corto_plazo_HRRN();
    }
}

void planificar_largo_plazo(){
    while (true){
        sem_wait(sem_pcb_new);
        sem_wait(sem_entrenador_libre);

        t_pcb* pcb = popfrontlist(&pcb_new);

        t_repartidor* repartidor = obtener_repartidor_mas_cercano(pcb);

        repartidor->pcb_actual = pcb;
        pcb->repartidor_actual = repartidor;

        t_restaurante *restaurante = buscar_restaurante_lista(pcb->restaurante);

        char string_log[200];
        sprintf(string_log, "Se asigna PCB: x=%d - y=%d | REPARTIDOR: x=%d - y=%d\n", restaurante->posicion.posx, restaurante->posicion.posy, repartidor->posicion.posx, repartidor->posicion.posy);
        log_info(logger, string_log);

        sem_post(repartidor->nuevo_pedido);

        pushbacklist(&pcb_ready, pcb);
        sem_post(sem_pcb_ready);
    }
}

void planificar_corto_plazo_FIFO(){
    while (true){
        sem_wait(sem_pcb_ready);
        sem_wait(sem_grado_multiprocesamiento);

        t_pcb* pcb = popfrontlist(&pcb_ready);

        char string_log[100];
        sprintf(string_log, "PASA EXEC: Repartidor %d\n", pcb->repartidor_actual->id);
        log_info(logger, string_log);
        
        pushbacklist(&suscriptores_cpu, pcb->repartidor_actual->ciclo_cpu);

    }
}

void pcb_prueba(){
    
    pushbacklist(&pcb_new, crear_pcb("Resto Default", 1, NULL));
    sem_post(sem_pcb_new);
    pushbacklist(&pcb_new, crear_pcb("Resto Default", 2, NULL));
    sem_post(sem_pcb_new);
    pushbacklist(&pcb_new, crear_pcb("Resto Default", 3, NULL));
    sem_post(sem_pcb_new);
}

t_repartidor* obtener_repartidor_mas_cercano(t_pcb* pcb){

    t_repartidor* repartidorAux = NULL;
    int calculoAux;
    IteratorList iterAux;
    t_restaurante *restaurante = buscar_restaurante_lista(pcb->restaurante);
    
    for(IteratorList iter = beginlist(repartidores_libres); iter != NULL; iter = nextlist(iter)){

        t_repartidor* repartidor = (t_repartidor*) iter->data;
        
        int posix = repartidor->posicion.posx - restaurante->posicion.posx;

        int posiy = repartidor->posicion.posy - restaurante->posicion.posy;

        int calculo = abs(posix) + abs(posiy);

        if(repartidorAux == NULL){
            repartidorAux = repartidor;
            calculoAux = calculo;
            iterAux = iter;
        }
        if(calculo < calculoAux){
            repartidorAux = repartidor;
            calculoAux = calculo;
            iterAux = iter;
        }
    }
    popiterlist(&repartidores_libres, iterAux);
    return repartidorAux;
}

void planificar_corto_plazo_SJF(){
    while(1){
        sem_wait(sem_pcb_ready);
        sem_wait(sem_grado_multiprocesamiento);
        sem_wait(sem_mutex_sjf);

        double alpha = app_config->alpha;

        int mejor = 999999;
        IteratorList elMejor;
        t_pcb* pcb;

        for(IteratorList iter = beginlist(pcb_ready); iter != NULL; iter = nextlist(iter)){
            pcb = (t_pcb*) dataiterlist(iter);

            if(pcb->estimacion == -1){
                pcb->estimacion = alpha*pcb->rafaga_anterior + (1-alpha)*pcb->estimacion_anterior;
            }
            if(pcb->estimacion < mejor){
                mejor = pcb->estimacion;
                elMejor = iter;
            }

        }
        pcb = popiterlist(&pcb_ready, elMejor);

        pcb->estimacion_anterior = pcb->estimacion;
        pcb->estimacion = -1;

        sem_post(sem_mutex_sjf);

        char string_log[100];
        sprintf(string_log, "PASA EXEC: Repartidor %d\n", pcb->repartidor_actual->id);
        log_info(logger, string_log);
        
        pushbacklist(&suscriptores_cpu, pcb->repartidor_actual->ciclo_cpu);

    }

}

void planificar_corto_plazo_HRRN(){
    while(1){
        sem_wait(sem_pcb_ready);
        sem_wait(sem_grado_multiprocesamiento);
        sem_wait(sem_mutex_sjf);

        double alpha = app_config->alpha;

        int mejor = 999999;
        IteratorList elMejor;
        t_pcb* pcb;

        for(IteratorList iter = beginlist(pcb_ready); iter != NULL; iter = nextlist(iter)){
            pcb = (t_pcb*) dataiterlist(iter);

            if(pcb->estimacion == -1){
                pcb->estimacion = alpha*pcb->rafaga_anterior + (1-alpha)*pcb->estimacion_anterior;
            }
            pcb->valorHRRN = (pcb->estimacion+pcb->ciclos_espera)/pcb->estimacion;
            if(pcb->valorHRRN < mejor){
                mejor = pcb->valorHRRN;
                elMejor = iter;
            }

        }
        pcb = popiterlist(&pcb_ready, elMejor);

        pcb->rafaga_anterior = 0;
        pcb->estimacion_anterior = pcb->estimacion;
        pcb->estimacion = -1;
        pcb->ciclos_espera = 0;

        sem_post(sem_mutex_sjf);

        char string_log[100];
        sprintf(string_log, "PASA EXEC: Repartidor %d\n", pcb->repartidor_actual->id);
        log_info(logger, string_log);
        
        pushbacklist(&suscriptores_cpu, pcb->repartidor_actual->ciclo_cpu);

    }
}

void aumentar_espera_HRRN(){

    while(true){
        sem_wait(sem_ciclo_espera_HRRN);
        for(IteratorList iter = beginlist(pcb_ready); iter != NULL; iter = nextlist(iter)){
            t_pcb* pcb = (t_pcb*) dataiterlist(iter);

            pcb->ciclos_espera += 1;
        }
    }
}