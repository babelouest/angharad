/**
 *
 * Carleon House Automation side services
 *
 * Command house automation side services via an HTTP REST interface
 *
 * Declarations for constants and prototypes
 *
 * Copyright 2016 Nicolas Mora <mail@babelouest.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU GENERAL PUBLIC LICENSE
 * License as published by the Free Software Foundation;
 * version 3 of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU GENERAL PUBLIC LICENSE for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __CARLEON_H_
#define __CARLEON_H_

/** Macro to avoid compiler warning when some parameters are unused and that's ok **/
#define UNUSED(x) (void)(x)

#define _CARLEON_VERSION 1.0

#include <jansson.h>

/** Angharad libraries **/
#include <ulfius.h>
#include <yder.h>

#define _HOEL_MARIADB
#define _HOEL_SQLITE
#include <hoel.h>

#define C_OK              0
#define C_ERROR           1
#define C_ERROR_MEMORY    2
#define C_ERROR_PARAM     3
#define C_ERROR_DB        4
#define C_ERROR_IO        5
#define C_ERROR_NOT_FOUND 6

#define WEBSERVICE_RESULT_ERROR     0
#define WEBSERVICE_RESULT_OK        1
#define WEBSERVICE_RESULT_NOT_FOUND 2
#define WEBSERVICE_RESULT_TIMEOUT   3
#define WEBSERVICE_RESULT_PARAM     4

#define CARLEON_TABLE_SERVICE "c_service"
#define CARLEON_TABLE_ELEMENT "c_element"

/** Macro to avoid compiler warning when some parameters are unused and that's ok **/
#define UNUSED(x) (void)(x)

struct _carleon_config {
  char                    * services_path;
  struct _h_connection    * conn;
  struct _carleon_service * service_list;
  char                    * alert_url;
  char                    * url_prefix;
  struct _u_instance      * instance;
};

/**
 * Structure for a service type
 * contains the handle to the library and handles for all the functions
 */
struct _carleon_service {
  char   * name;
  void   * dl_handle;
  int      enabled;
  char   * description;
  
  // dl files functions available
  json_t * (* c_service_init) (struct _carleon_config * config);
  json_t * (* c_service_close) (struct _carleon_config * config);
  json_t * (* c_service_command_get_list) (struct _carleon_config * config);
  json_t * (* c_service_element_get_list) (struct _carleon_config * config);
  json_t * (* c_service_exec) (struct _carleon_config * config, const char * command, const char * element, json_t * parameters);
};

// carleon core functions
int init_carleon(struct _carleon_config * config);
int close_carleon(struct _carleon_config * config);
void clean_carleon(struct _carleon_config * config);
int init_service_list(struct _carleon_config * config);
int close_service_list(struct _carleon_config * config);
void close_service(struct _carleon_service * service, struct _carleon_config * config);
int connect_enabled_services(struct _carleon_config * config);
int disconnect_all_services(struct _carleon_config * config);
int set_response_json_body_and_clean(struct _u_response * response, uint status, json_t * json_body);

// services core functions
json_t * service_get(struct _carleon_config * config, const char * service);
json_t * parse_service_from_db(json_t * service);
int service_enable(struct _carleon_config * config, const char * service, const int status);
struct _carleon_service * get_service_from_name(struct _carleon_config * config, const char * name);
json_t * service_exec(struct _carleon_config * config, struct _carleon_service * service, const char * command, const char * element, json_t * parameters);

// elements core functions
json_t * service_element_get_tag(struct _carleon_config * config, const char * service, const char * element);
int service_element_cleanup(struct _carleon_config * config, const char * service, const char * element);
int service_element_add_tag(struct _carleon_config * config, const char * service, const char * element, const char * tag);
int service_element_remove_tag(struct _carleon_config * config, const char * service, const char * element, const char * tag);

// Services callback functions
int callback_carleon_service_get (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_carleon_service_enable (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_carleon_service_element_cleanup (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_carleon_service_element_add_tag (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_carleon_service_element_remove_tag (const struct _u_request * request, struct _u_response * response, void * user_data);

#endif
