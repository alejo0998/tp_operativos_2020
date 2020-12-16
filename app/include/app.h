#ifndef APP_H
#define APP_H
#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdlib.h>
#include "shared_utils.h"
#include "server.h"
#include "list.h"
#include "api.h"
#include "tests.h"
#include "app_messages.h"
//#include "config_app.h"
//#include "planificador.h"

// FUNCIONES
void app_init(t_app_config** app_config, t_log** logger);
void crear_lista_platos_default();
void app_finally(t_app_config* app_config, t_log* logger);

#endif