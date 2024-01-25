/**
 *
 * Benoic House Automation service
 *
 * Command house automation devices via an HTTP REST interface
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

#ifndef __BENOIC_H_
#define __BENOIC_H_

#define _BENOIC_VERSION 1.1

/** Macro to avoid compiler warning when some parameters are unused and that's ok **/
#define UNUSED(x) (void)(x)

#include <jansson.h>

/** Angharad libraries **/
#include <ulfius.h>
#include <yder.h>

#define _HOEL_MARIADB
#define _HOEL_SQLITE
#include <hoel.h>

#define B_OK              0
#define B_ERROR           1
#define B_ERROR_MEMORY    2
#define B_ERROR_PARAM     3
#define B_ERROR_DB        4
#define B_ERROR_IO        5
#define B_ERROR_NOT_FOUND 6

#define DEVICE_RESULT_ERROR     0
#define DEVICE_RESULT_OK        1
#define DEVICE_RESULT_NOT_FOUND 2
#define DEVICE_RESULT_TIMEOUT   3
#define DEVICE_RESULT_PARAM     4

#define BENOIC_TABLE_DEVICE_TYPE "b_device_type"
#define BENOIC_TABLE_DEVICE      "b_device"
#define BENOIC_TABLE_ELEMENT     "b_element"
#define BENOIC_TABLE_MONITOR     "b_monitor"

#define BENOIC_ELEMENT_TYPE_NONE   0
#define BENOIC_ELEMENT_TYPE_SENSOR 1
#define BENOIC_ELEMENT_TYPE_SWITCH 2
#define BENOIC_ELEMENT_TYPE_DIMMER 3
#define BENOIC_ELEMENT_TYPE_HEATER 4
#define BENOIC_ELEMENT_TYPE_BLIND  5

#define BENOIC_STATUS_RUN      0
#define BENOIC_STATUS_STOPPING 1
#define BENOIC_STATUS_STOP     2

/**
 * Structure for a device type
 * contains the handle to the library and handles for all the functions
 */
struct _device_type {
  char   * uid;
  void   * dl_handle;
  char   * name;
  char   * description;
  json_t * options;
  
  // dl files functions available
  json_t * (* b_device_type_init) ();
  json_t * (* b_device_connect) (json_t * device, void ** device_ptr);
  json_t * (* b_device_disconnect) (json_t * device, void * device_ptr);
  json_t * (* b_device_ping) (json_t * device, void * device_ptr);
  json_t * (* b_device_overview) (json_t * device, void * device_ptr);
  json_t * (* b_device_get_sensor) (json_t * device, const char * sensor_name, void * device_ptr);
  json_t * (* b_device_get_switch) (json_t * device, const char * switch_name, void * device_ptr);
  json_t * (* b_device_set_switch) (json_t * device, const char * switch_name, const int command, void * device_ptr);
  json_t * (* b_device_get_dimmer) (json_t * device, const char * dimmer_name, void * device_ptr);
  json_t * (* b_device_set_dimmer) (json_t * device, const char * dimmer_name, const int command, void * device_ptr);
  json_t * (* b_device_get_heater) (json_t * device, const char * heater_name, void * device_ptr);
  json_t * (* b_device_set_heater) (json_t * device, const char * heater_name, const char * mode, const float command, void * device_ptr);
  json_t * (* b_device_get_blind) (json_t * device, const char * blind_name, void * device_ptr);
  json_t * (* b_device_set_blind) (json_t * device, const char * blind_name, int command, void * device_ptr);
  int      (* b_device_has_element) (json_t * device, int element_type, const char * element_name, void * device_ptr);
};

struct _benoic_device_data {
  char * device_name;
  void * device_ptr;
};

struct _benoic_config {
  char                       * modules_path;
  struct _h_connection       * conn;
  struct _device_type        * device_type_list;
  struct _benoic_device_data * device_data_list;
  int                          benoic_status;
  char                       * alert_url;
};

struct _device_type * get_device_type(struct _benoic_config * config, json_t * device);
int set_response_json_body_and_clean(struct _u_response * response, uint status, json_t * json_body);

// Device db management functions
json_t * get_device_types_list(struct _benoic_config * config);
json_t * reload_device_types_list(struct _benoic_config * config);
json_t * get_device(struct _benoic_config * config, const char * name);
int add_device(struct _benoic_config * config, const json_t * device);
int modify_device(struct _benoic_config * config, const json_t * device, const char * name);
int delete_device(struct _benoic_config * config, const char * name);
json_t * parse_device_from_db(json_t * result);
json_t * parse_device_to_db(json_t * device, const int update);
json_t * is_device_valid(struct _benoic_config * config, json_t * device, const int update);
json_t * is_device_option_list_valid(struct _benoic_config * config, json_t * device);
json_t * is_device_option_valid(json_t * option_format, json_t * options);
int update_last_seen_device(struct _benoic_config * config, json_t * device);

// Device hardware management functions
int connect_enabled_devices(struct _benoic_config * config);
int connect_device(struct _benoic_config * config, json_t * device);
int disconnect_device(struct _benoic_config * config, json_t * device, int update_db_status);
int ping_device(struct _benoic_config * config, json_t * device);
json_t * overview_device(struct _benoic_config * config, json_t * device);

// Elements hardware management functions
int has_element(struct _benoic_config * config, json_t * device, const int element_type, const char * element_name);
json_t * get_sensor(struct _benoic_config * config, json_t * device, const char * sensor_name);
json_t * get_switch(struct _benoic_config * config, json_t * device, const char * switch_name);
int set_switch(struct _benoic_config * config, json_t * device, const char * switch_name, const int command);
json_t * get_dimmer(struct _benoic_config * config, json_t * device, const char * dimmer_name);
json_t * set_dimmer(struct _benoic_config * config, json_t * device, const char * dimmer_name, const int command);
json_t * get_heater(struct _benoic_config * config, json_t * device, const char * heater_name);
json_t * get_blind(struct _benoic_config * config, json_t * device, const char * blind_name);
json_t * set_blind(struct _benoic_config * config, json_t * device, const char * blind_name, const int command);
int set_heater(struct _benoic_config * config, json_t * device, const char * heater_name, const char * mode, const float command);

// Elements data management functions
json_t * get_element_data(struct _benoic_config * config, json_t * device, const int element_type, const char * element_name, int create);
int set_element_data(struct _benoic_config * config, json_t * device, const char * element_name, const int element_type, json_t * element_data, const int update);
json_t * parse_element_from_db(json_t * element);
json_t * parse_element_to_db(json_t * element, const char * device, const char * element_name, const int element_type, const int update, int db_type);
json_t * is_element_valid(json_t * element, const int element_type);
json_t * is_option_valid(json_t * options, int element_type);
int is_tag_list_valid(json_t * tags);
int element_add_tag(struct _benoic_config * config, json_t * device, const int element_type, const char * element_name, const char * tag);
int element_remove_tag(struct _benoic_config * config, json_t * device, const int element_type, const char * element_name, const char * tag);
json_t * element_get_monitor(struct _benoic_config * config, json_t * device, const int element_type, const char * element_name, json_t * params);
json_t * element_get_lists(struct _benoic_config * config, json_t * device);

// benoic initialization function
int init_benoic(struct _u_instance * instance, const char * url_prefix, struct _benoic_config * config);
int close_benoic(struct _u_instance * instance, const char * url_prefix, struct _benoic_config * config);
void clean_benoic(struct _benoic_config * config);
int init_device_type_list(struct _benoic_config * config);
int close_device_type_list(struct _device_type * device_type_list);
void close_device_type(struct _device_type device_type);
void * get_device_ptr(struct _benoic_config * config, const char * device_name);
int set_device_data(struct _benoic_config * config, const char * device_name, void * device_ptr);
int remove_device_data(struct _benoic_config * config, const char * device_name);
int disconnect_all_devices(struct _benoic_config * config);
void * thread_monitor_run(void * args);

// endpoints callback functions
int callback_benoic_device_get_types (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_benoic_device_reload_types (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_benoic_device_get_list (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_benoic_device_get (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_benoic_device_add (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_benoic_device_modify (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_benoic_device_delete (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_benoic_device_connect (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_benoic_device_disconnect (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_benoic_device_ping (const struct _u_request * request, struct _u_response * response, void * user_data);

// Device overview callback function
int callback_benoic_device_overview (const struct _u_request * request, struct _u_response * response, void * user_data);

// Elements callback functions
int callback_benoic_device_element_get (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_benoic_device_element_put (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_benoic_device_element_set (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_benoic_device_element_add_tag (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_benoic_device_element_remove_tag (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_benoic_device_element_monitor(const struct _u_request * request, struct _u_response * response, void * user_data);

#endif
