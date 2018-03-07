#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <dlfcn.h>
#include <jansson.h>

/** Angharad libraries **/
#include <ulfius.h>
#include <yder.h>

#define _HOEL_SQLITE
#include <hoel.h>

#include "../carleon.h"

#define PORT 2473
#define PREFIX "carleon"
#define DATABASE "/tmp/database.db"
#define URL_ALERT "http://localhost:2473/alert/%s/%s/%s/%s/"

json_t * c_service_init(struct _carleon_config * config);
json_t * c_service_close(struct _carleon_config * config);
json_t * c_service_command_get_list(struct _carleon_config * config);
json_t * c_service_element_get_list(struct _carleon_config * config);
json_t * c_service_exec(struct _carleon_config * config, const char * command, const char * element, json_t * parameters);

int callback_test_alert (const struct _u_request * request, struct _u_response * response, void * user_data) {
  y_log_message(Y_LOG_LEVEL_DEBUG, "Calling alert callback with parameters submodule_name: %s, source: %s, element: %s, message: %s",
                u_map_get(request->map_url, "submodule_name"), u_map_get(request->map_url, "source"), 
                u_map_get(request->map_url, "element"), u_map_get(request->map_url, "message"));
  return U_OK;
}

int callback_test_commands (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct _carleon_config * config = (struct _carleon_config *)user_data;
  response->json_body = c_service_command_get_list(config);
  if (response->json_body == NULL) {
    response->status = 500;
  }
  return U_OK;
}

int callback_test_elements (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct _carleon_config * config = (struct _carleon_config *)user_data;
  response->json_body = c_service_element_get_list(config);
  if (response->json_body == NULL) {
    response->status = 500;
  }
  return U_OK;
}

int callback_test_exec (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct _carleon_config * config = (struct _carleon_config *)user_data;
  response->json_body = c_service_exec(config, u_map_get(request->map_url, "command"), u_map_get(request->map_url, "element"), request->json_body);  if (response->json_body == NULL) {
    response->status = 500;
  }
  return U_OK;
}

int callback_options (const struct _u_request * request, struct _u_response * response, void * user_data) {
  u_map_put(response->map_header, "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
  u_map_put(response->map_header, "Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept, ANGHARAD_SESSION_ID");
  u_map_put(response->map_header, "Access-Control-Max-Age", "1800");
  return U_OK;
}

int main(int argc, char ** argv) {
  // Initialize the instance
  struct _u_instance instance;
  struct _carleon_config config;
  json_t * j_res;
  char * c_res;
  
  y_init_logs("test-service", Y_LOG_MODE_CONSOLE, Y_LOG_LEVEL_DEBUG, NULL, "Starting test-service");
  
  config.conn = h_connect_sqlite(DATABASE);
  config.service_list = NULL;
  
  if (config.conn == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "Error opening database %s, abort", DATABASE);
    return(1);
  }
  
  if (ulfius_init_instance(&instance, PORT, NULL) != U_OK) {
    y_log_message(Y_LOG_LEVEL_ERROR, "Error ulfius_init_instance, abort");
    h_close_db(config.conn);
    h_clean_connection(config.conn);
    return(1);
  }
  config.instance = &instance;
  config.url_prefix = nstrdup(PREFIX);
  
  ulfius_add_endpoint_by_val(&instance, "GET", PREFIX, "/alert/@submodule_name/@source/@element/@message/", NULL, NULL, NULL, &callback_test_alert, NULL);
  ulfius_add_endpoint_by_val(&instance, "GET", PREFIX, "/commands/", NULL, NULL, NULL, &callback_test_commands, &config);
  ulfius_add_endpoint_by_val(&instance, "GET", PREFIX, "/elements/", NULL, NULL, NULL, &callback_test_elements, &config);
  ulfius_add_endpoint_by_val(&instance, "POST", PREFIX, "/exec/@element/@command", NULL, NULL, NULL, &callback_test_exec, &config);
  
  ulfius_add_endpoint_by_val(&instance, "OPTIONS", NULL, "*", NULL, NULL, NULL, &callback_options, &config);

  j_res = c_service_init(&config);
  
  if (j_res != NULL) {
    c_res = json_dumps(j_res, JSON_ENCODE_ANY);
    y_log_message(Y_LOG_LEVEL_INFO, "Result of c_service_init is %s", c_res);
    free(c_res);
    json_decref(j_res);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "Error, c_service_init returned NULL");
  }
  
  // Start the webservice
  y_log_message(Y_LOG_LEVEL_INFO, "Start test-service on port %d", instance.port);
  if (ulfius_start_framework(&instance) == U_OK) {
    y_log_message(Y_LOG_LEVEL_INFO, "Press enter to close the application");
    getchar();
  }
  
  j_res = c_service_close(&config);
  if (j_res != NULL) {
    c_res = json_dumps(j_res, JSON_ENCODE_ANY);
    y_log_message(Y_LOG_LEVEL_INFO, "Result of c_service_close is %s", c_res);
    free(c_res);
    json_decref(j_res);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "Error, c_service_close returned NULL");
  }

  ulfius_stop_framework(&instance);
  ulfius_clean_instance(&instance);
  h_close_db(config.conn);
  h_clean_connection(config.conn);
  free(config.url_prefix);
  y_close_logs();
  return 0;
}
