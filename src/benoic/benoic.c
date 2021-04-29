/**
 *
 * Benoic House Automation service
 *
 * Command house automation devices via an HTTP REST interface
 *
 * Initialization and http callback functions
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

#include <string.h>
#include "benoic.h"

/**
 * init_benoic
 * 
 * Initialize Benoic webservice with prefix and database connection parameters
 * 
 */
int init_benoic(struct _u_instance * instance, const char * url_prefix, struct _benoic_config * config) {
  pthread_t thread_monitor;
  int thread_ret_monitor = 0, thread_detach_monitor = 0;

  if (instance != NULL && url_prefix != NULL && config != NULL) {
    
    // Devices management
    ulfius_add_endpoint_by_val(instance, "GET", url_prefix, "/deviceTypes/", 2, &callback_benoic_device_get_types, (void*)config);
    ulfius_add_endpoint_by_val(instance, "PUT", url_prefix, "/deviceTypes/reload", 2, &callback_benoic_device_reload_types, (void*)config);
    ulfius_add_endpoint_by_val(instance, "GET", url_prefix, "/device/", 2, &callback_benoic_device_get_list, (void*)config);
    ulfius_add_endpoint_by_val(instance, "GET", url_prefix, "/device/@device_name", 2, &callback_benoic_device_get, (void*)config);
    ulfius_add_endpoint_by_val(instance, "POST", url_prefix, "/device/", 2, &callback_benoic_device_add, (void*)config);
    ulfius_add_endpoint_by_val(instance, "PUT", url_prefix, "/device/@device_name", 2, &callback_benoic_device_modify, (void*)config);
    ulfius_add_endpoint_by_val(instance, "DELETE", url_prefix, "/device/@device_name", 2, &callback_benoic_device_delete, (void*)config);
    ulfius_add_endpoint_by_val(instance, "GET", url_prefix, "/device/@device_name/connect", 2, &callback_benoic_device_connect, (void*)config);
    ulfius_add_endpoint_by_val(instance, "GET", url_prefix, "/device/@device_name/disconnect", 2, &callback_benoic_device_disconnect, (void*)config);
    ulfius_add_endpoint_by_val(instance, "GET", url_prefix, "/device/@device_name/ping", 2, &callback_benoic_device_ping, (void*)config);
    
    // Device elements overview
    ulfius_add_endpoint_by_val(instance, "GET", url_prefix, "/device/@device_name/overview", 2, &callback_benoic_device_overview, (void*)config);
    
    // Elements get, set and put
    ulfius_add_endpoint_by_val(instance, "GET", url_prefix, "/device/@device_name/@element_type/@element_name", 2, &callback_benoic_device_element_get, (void*)config);
    ulfius_add_endpoint_by_val(instance, "GET", url_prefix, "/device/@device_name/@element_type/@element_name/@command", 2, &callback_benoic_device_element_set, (void*)config);
    ulfius_add_endpoint_by_val(instance, "PUT", url_prefix, "/device/@device_name/@element_type/@element_name", 2, &callback_benoic_device_element_put, (void*)config);
    ulfius_add_endpoint_by_val(instance, "PUT", url_prefix, "/device/@device_name/@element_type/@element_name/@tag", 2, &callback_benoic_device_element_add_tag, (void*)config);
    ulfius_add_endpoint_by_val(instance, "DELETE", url_prefix, "/device/@device_name/@element_type/@element_name/@tag", 2, &callback_benoic_device_element_remove_tag, (void*)config);
    ulfius_add_endpoint_by_val(instance, "GET", url_prefix, "/monitor/@device_name/@element_type/@element_name/", 2, &callback_benoic_device_element_monitor, (void*)config);
    
    // Get differents types available for devices by loading library files in module_path
    if (init_device_type_list(config) != B_OK) {
      y_log_message(Y_LOG_LEVEL_ERROR, "init_benoic - Error loading device types list");
      return B_ERROR_IO;
    } else if (connect_enabled_devices(config) != B_OK) {
      y_log_message(Y_LOG_LEVEL_ERROR, "init_benoic - Error connecting devices");
      return B_ERROR_IO;
    }
    
    // Start monitor thread
    config->benoic_status = BENOIC_STATUS_RUN;
    thread_ret_monitor = pthread_create(&thread_monitor, NULL, thread_monitor_run, (void *)config);
    thread_detach_monitor = pthread_detach(thread_monitor);
    if (thread_ret_monitor || thread_detach_monitor) {
      y_log_message(Y_LOG_LEVEL_ERROR, "Error creating or detaching monitor thread, return code: %d, detach code: %d",
                  thread_ret_monitor, thread_detach_monitor);
      return B_ERROR_IO;
    }
    
    y_log_message(Y_LOG_LEVEL_INFO, "benoic is available on prefix %s", url_prefix);
    return B_OK;
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "init_benoic - Error input parameters");
    return B_ERROR_PARAM;
  }
}

/**
 * stop_benoic
 * 
 * Send a stop signal to thread_monitor and disconnect all devices
 */
int close_benoic(struct _u_instance * instance, const char * url_prefix, struct _benoic_config * config) {
  int res;
  
  if (instance != NULL && url_prefix != NULL && config != NULL) {
    ulfius_remove_endpoint_by_val(instance, "GET", url_prefix, "/deviceTypes/");
    ulfius_remove_endpoint_by_val(instance, "PUT", url_prefix, "/deviceTypes/reload");
    ulfius_remove_endpoint_by_val(instance, "GET", url_prefix, "/device/");
    ulfius_remove_endpoint_by_val(instance, "GET", url_prefix, "/device/@device_name");
    ulfius_remove_endpoint_by_val(instance, "POST", url_prefix, "/device/");
    ulfius_remove_endpoint_by_val(instance, "PUT", url_prefix, "/device/@device_name");
    ulfius_remove_endpoint_by_val(instance, "DELETE", url_prefix, "/device/@device_name");
    ulfius_remove_endpoint_by_val(instance, "GET", url_prefix, "/device/@device_name/connect");
    ulfius_remove_endpoint_by_val(instance, "GET", url_prefix, "/device/@device_name/disconnect");
    ulfius_remove_endpoint_by_val(instance, "GET", url_prefix, "/device/@device_name/ping");
    ulfius_remove_endpoint_by_val(instance, "GET", url_prefix, "/device/@device_name/overview");
    ulfius_remove_endpoint_by_val(instance, "GET", url_prefix, "/device/@device_name/@element_type/@element_name");
    ulfius_remove_endpoint_by_val(instance, "GET", url_prefix, "/device/@device_name/@element_type/@element_name/@command");
    ulfius_remove_endpoint_by_val(instance, "PUT", url_prefix, "/device/@device_name/@element_type/@element_name");
    ulfius_remove_endpoint_by_val(instance, "PUT", url_prefix, "/device/@device_name/@element_type/@element_name/@tag");
    ulfius_remove_endpoint_by_val(instance, "DELETE", url_prefix, "/device/@device_name/@element_type/@element_name/@tag");
    ulfius_remove_endpoint_by_val(instance, "GET", url_prefix, "/monitor/@device_name/@element_type/@element_name/");
    
    if (config->benoic_status == BENOIC_STATUS_RUN) {
      config->benoic_status = BENOIC_STATUS_STOPPING;
      while (config->benoic_status != BENOIC_STATUS_STOP) {
        sleep(1);
      }
    }
    
    res = disconnect_all_devices(config);
    if (res != B_OK) {
      y_log_message(Y_LOG_LEVEL_ERROR, "close_benoic - Error disconnecting all devices");
      return res;
    }
    res = close_device_type_list(config->device_type_list);
    o_free(config->device_type_list);
    config->device_type_list = NULL;
    if (res != B_OK) {
      y_log_message(Y_LOG_LEVEL_ERROR, "close_benoic - Error closing device type list");
      return res;
    } else {
      y_log_message(Y_LOG_LEVEL_INFO, "closing benoic");
      return B_OK;
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "close_benoic - Error input parameters");
    return B_ERROR_PARAM;
  }
}

/**
 * clean_benoic
 * 
 * clean configuration structure
 */
void clean_benoic(struct _benoic_config * config) {
  o_free(config->modules_path);
  o_free(config);
}

/**
 * thread_monitor_run
 * 
 * thread for monitoring data
 * loop every minutes and get data to monitor (sensor values, switches, dimmers and heaters commands)
 * end when benoic_status is different than BENOIC_STATUS_RUN
 * 
 */
void * thread_monitor_run(void * args) {
  struct _benoic_config * config = (struct _benoic_config *)args;
  time_t now;
  struct tm ts;
  int res;
  json_t * j_query, * j_result, * j_element, * device, * value;
  size_t index;
  char * s_value, * s_next_time;
  
  if (config != NULL) {
    while (config->benoic_status == BENOIC_STATUS_RUN) {
      // Run monitoring task every minute
      time(&now);
      ts = *localtime(&now);
      if (ts.tm_sec == 0) {
        j_query = json_pack("{sss{sis{ssss}}}", "table", BENOIC_TABLE_ELEMENT, "where", "be_monitored", 1, "be_monitored_next", "operator", "raw", "value", "<= CURRENT_TIMESTAMP");
        if (j_query != NULL) {
          res = h_select(config->conn, j_query, &j_result, NULL);
          json_decref(j_query);
          if (res == H_OK) {
            json_array_foreach(j_result, index, j_element) {
              device = get_device(config, json_string_value(json_object_get(j_element, "bd_name")));
              if (device != NULL) {
                if (json_object_get(device, "enabled") == json_true() && json_object_get(device, "connected") == json_true()) {
                  // Getting value
                  value = NULL;
                  if (has_element(config, device, json_integer_value(json_object_get(j_element, "be_type")), json_string_value(json_object_get(j_element, "be_name")))) {
                    switch (json_integer_value(json_object_get(j_element, "be_type"))) {
                      case BENOIC_ELEMENT_TYPE_SENSOR:
                        value = get_sensor(config, device, json_string_value(json_object_get(j_element, "be_name")));
                        break;
                      case BENOIC_ELEMENT_TYPE_SWITCH:
                        value = get_switch(config, device, json_string_value(json_object_get(j_element, "be_name")));
                        break;
                      case BENOIC_ELEMENT_TYPE_DIMMER:
                        value = get_dimmer(config, device, json_string_value(json_object_get(j_element, "be_name")));
                        break;
                      case BENOIC_ELEMENT_TYPE_HEATER:
                        value = get_heater(config, device, json_string_value(json_object_get(j_element, "be_name")));
                        break;
                    }
                  }
                  
                  // Inserting value in monitor table
                  if (value != NULL) {
                    s_value = NULL;
                    if (json_is_integer(json_object_get(value, "value"))) {
                      s_value = msprintf("%" JSON_INTEGER_FORMAT, json_integer_value(json_object_get(value, "value")));
                    } else if (json_is_number(json_object_get(value, "value"))) {
                      s_value = msprintf("%.2f", json_number_value(json_object_get(value, "value")));
                    } else if (json_is_string(json_object_get(value, "value"))) {
                      s_value = o_strdup(json_string_value(json_object_get(value, "value")));
                    }
                    if (s_value != NULL) {
                      j_query = json_pack("{sss{sIss}}", 
                                          "table", 
                                          BENOIC_TABLE_MONITOR, 
                                          "values", 
                                            "be_id", 
                                            json_integer_value(json_object_get(j_element, "be_id")), 
                                            "bm_value", 
                                            s_value);
                      res = h_insert(config->conn, j_query, NULL);
                      json_decref(j_query);
                      if (res != H_OK) {
                        y_log_message(Y_LOG_LEVEL_ERROR, "thread_monitor_run - Error inserting data for monitor %s/%s", json_string_value(json_object_get(j_element, "bd_name")), json_string_value(json_object_get(j_element, "be_name")));
                      }
                      o_free(s_value);
                    }
                    json_decref(value);
                  }
                  
                  // Updating next monitor time
                  if (config->conn->type == HOEL_DB_TYPE_MARIADB) {
                    s_next_time = msprintf("CURRENT_TIMESTAMP + INTERVAL %" JSON_INTEGER_FORMAT " SECOND", json_integer_value(json_object_get(j_element, "be_monitored_every")));
                  } else {
                    s_next_time = msprintf("strftime('%%s','now')+%" JSON_INTEGER_FORMAT, json_integer_value(json_object_get(j_element, "be_monitored_every")));
                  }
                  j_query = json_pack("{sss{s{ss}}s{sI}}", "table", BENOIC_TABLE_ELEMENT, "set", "be_monitored_next", "raw", s_next_time, "where", "be_id", json_integer_value(json_object_get(j_element, "be_id")));
                  res = h_update(config->conn, j_query, NULL);
                  json_decref(j_query);
                  o_free(s_next_time);
                  if (res != H_OK) {
                    y_log_message(Y_LOG_LEVEL_ERROR, "thread_monitor_run - Error updating next_time for monitor %s/%s", json_string_value(json_object_get(j_element, "bd_name")), json_string_value(json_object_get(j_element, "be_name")));
                  }
                }
              } else {
                y_log_message(Y_LOG_LEVEL_ERROR, "thread_monitor_run - device %s not found", json_string_value(json_object_get(j_element, "bd_name")));
              }
              json_decref(device);
            }
            json_decref(j_result);
          }
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "thread_monitor_run - Error allocating resources for j_query");
        }
      }
      sleep(1);
    }
    config->benoic_status = BENOIC_STATUS_STOP;
  }
  return NULL;
}

/**
 * Return a pointer to the device corresponding to the device_name
 * return NULL if not found
 */
void * get_device_ptr(struct _benoic_config * config, const char * device_name) {
  int i;
  
  if (config == NULL || device_name == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "get_device_ptr - Error input parameters");
    return NULL;
  }
  
  for (i=0; config->device_data_list != NULL && config->device_data_list[i].device_name != NULL; i++) {
    if (0 == o_strcmp(config->device_data_list[i].device_name, device_name)) {
      return config->device_data_list[i].device_ptr;
    }
  }
  
  return NULL;
}

/**
 * Store new device data in the database
 * return B_OK on success
 */
int set_device_data(struct _benoic_config * config, const char * device_name, void * device_ptr) {
  struct _benoic_device_data * tmp;
  size_t device_data_list_size;
  
  if (config == NULL || device_name == NULL) {
    return B_ERROR_PARAM;
  }
  
  // Append new device_ptr
  if (config->device_data_list == NULL) {
    config->device_data_list = o_malloc(2 * sizeof(struct _benoic_device_data));
    if (config->device_data_list == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "set_device_data - Error allocating resources for config->device_data_list");
      return B_ERROR_MEMORY;
    }
    config->device_data_list[0].device_name = o_strdup(device_name);
    if (config->device_data_list[0].device_name == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "set_device_data - Error allocating resources for config->device_data_list[0].device_name");
      return B_ERROR_MEMORY;
    }
    config->device_data_list[0].device_ptr = device_ptr;
    config->device_data_list[1].device_name = NULL;
    config->device_data_list[1].device_ptr = NULL;
  } else {
    for (device_data_list_size = 0; config->device_data_list[device_data_list_size].device_ptr != NULL; device_data_list_size++);
    tmp = o_realloc(config->device_data_list, (device_data_list_size + 2)*sizeof(struct _benoic_device_data));
    if (tmp == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "set_device_data - Error reallocating resources for config->device_data_list");
      return B_ERROR_MEMORY;
    }
    config->device_data_list = tmp;
    config->device_data_list[device_data_list_size].device_name = o_strdup(device_name);
    if (config->device_data_list[device_data_list_size].device_name == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "set_device_data - Error allocating resources for config->device_data_list[device_data_list_size].device_name");
      return B_ERROR_MEMORY;
    }
    config->device_data_list[device_data_list_size].device_ptr = device_ptr;
    config->device_data_list[device_data_list_size + 1].device_name = NULL;
    config->device_data_list[device_data_list_size + 1].device_ptr = NULL;
  }
  return B_OK;
}

/**
 * Clean all device_data
 * return B_OK on success
 */
int remove_device_data(struct _benoic_config * config, const char * device_name) {
  int i;

  if (config == NULL || device_name == NULL) {
    return B_ERROR_PARAM;
  }
  
  if (config->device_data_list != NULL) {
    for (i=0; config->device_data_list[i].device_name != NULL; i++) {
      if (0 == o_strcmp(config->device_data_list[i].device_name, device_name)) {
        // device_data found, remove it and move next device_data to previous index
        o_free(config->device_data_list[i].device_name);
        config->device_data_list[i].device_name = NULL;
        config->device_data_list[i].device_ptr = NULL;
        while (config->device_data_list[i+1].device_name != NULL) {
          config->device_data_list[i].device_name = config->device_data_list[i+1].device_name;
          config->device_data_list[i].device_ptr = config->device_data_list[i+1].device_ptr;
          i++;
        }
        config->device_data_list[i+1].device_name = NULL;
        config->device_data_list[i+1].device_ptr = NULL;
        break;
      }
    }
    return B_OK;
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "remove_device_data - Error input parameters");
    return B_ERROR_PARAM;
  }
}

/**
 * Disconnect all connected devices
 * return B_OK on success
 */
int disconnect_all_devices(struct _benoic_config * config) {
  size_t index;
  json_t * device, * device_list;
  
  if (config != NULL) {
    device_list = get_device(config, NULL);
    json_array_foreach(device_list, index, device) {
      if (device != NULL) {
        disconnect_device(config, device, 0);
      }
    }
    json_decref(device_list);
    o_free(config->device_data_list);
    config->device_data_list = NULL;
    return B_OK;
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "disconnect_all_devices - Error input parameters");
    return B_ERROR_PARAM;
  }
}

/**
 * Callback functions declaration
 */
int callback_benoic_device_get_types (const struct _u_request * request, struct _u_response * response, void * user_data) {
  UNUSED(request);
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_benoic_device_get_types - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    set_response_json_body_and_clean(response, 200, get_device_types_list((struct _benoic_config *)user_data));
    return U_CALLBACK_CONTINUE;
  }
}

int callback_benoic_device_reload_types (const struct _u_request * request, struct _u_response * response, void * user_data) {
  UNUSED(request);
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_benoic_device_get_types - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    if (close_device_type_list(((struct _benoic_config *)user_data)->device_type_list) == B_OK) {
      o_free(((struct _benoic_config *)user_data)->device_type_list);
      if (init_device_type_list((struct _benoic_config *)user_data) == B_OK) {
        set_response_json_body_and_clean(response, 200, get_device_types_list((struct _benoic_config *)user_data));
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_benoic_device_get_types - Error reopening device type list");
        response->status = 500;
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_benoic_device_get_types - Error closing device type list");
      response->status = 500;
    }
    return U_CALLBACK_CONTINUE;
  }
}

int callback_benoic_device_get_list (const struct _u_request * request, struct _u_response * response, void * user_data) {
  UNUSED(request);
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_benoic_device_get_list - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    set_response_json_body_and_clean(response, 200, get_device((struct _benoic_config *)user_data, NULL));
    return U_CALLBACK_CONTINUE;
  }
}

int callback_benoic_device_get (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * json_body;
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_benoic_device_get - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    json_body = get_device((struct _benoic_config *)user_data, u_map_get(request->map_url, "device_name"));
    if (json_body == NULL) {
      response->status = 404;
    } else {
      set_response_json_body_and_clean(response, 200, json_body);
    }
  }
  return U_CALLBACK_CONTINUE;
}

int callback_benoic_device_add (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * result, * device;
  json_t * json_body = ulfius_get_json_body_request(request, NULL);
  
  if (json_body == NULL) {
    set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "invalid input json format"));
    return U_CALLBACK_CONTINUE;
  } else if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_benoic_device_add - Error, callback_benoic_device_add user_data is NULL");
    json_decref(json_body);
    return U_CALLBACK_ERROR;
  } else {
    result = is_device_valid((struct _benoic_config *)user_data, json_body, 0);
    if (result == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_benoic_device_add - Error is_device_valid");
      response->status = 400;
    } else if (json_array_size(result) > 0) {
      set_response_json_body_and_clean(response, 400, json_copy(result));
    } else {
      device = get_device((struct _benoic_config *)user_data, json_string_value(json_object_get(json_body, "name")));
      if (device == NULL) {
        device = parse_device_to_db(json_body, 0);
        if (add_device((struct _benoic_config *)user_data, device) != B_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "callback_benoic_device_add - Error adding new device");
          response->status = 500;
        }
      } else {
        set_response_json_body_and_clean(response, 400, json_pack("{ss}", "message", "Device name already exist"));
      }
      json_decref(device);
    }
    json_decref(result);
    json_decref(json_body);
    return U_CALLBACK_CONTINUE;
  }
}

int callback_benoic_device_modify (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * result, * device;
  json_t * json_body = ulfius_get_json_body_request(request, NULL);
  
  if (json_body == NULL) {
    set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "invalid input json format"));
    return U_CALLBACK_CONTINUE;
  } else if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_benoic_device_modify - Error, callback_benoic_device_modify user_data is NULL");
    json_decref(json_body);
    return U_CALLBACK_ERROR;
  } else {
    result = is_device_valid((struct _benoic_config *)user_data, json_body, 1);
    if (result == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_benoic_device_modify - Error is_device_valid");
      response->status = 500;
    } else if (json_array_size(result) > 0) {
      set_response_json_body_and_clean(response, 400, result);
    } else {
      device = get_device((struct _benoic_config *)user_data, u_map_get(request->map_url, "device_name"));
      if (device == NULL) {
        json_t * j_body = json_pack("{ss}", "message", "Device not found");
        set_response_json_body_and_clean(response, 404, j_body);
      } else {
        json_decref(device);
        device = parse_device_to_db(json_body, 1);
        if (modify_device((struct _benoic_config *)user_data, device, u_map_get(request->map_url, "device_name")) != B_OK) {
          response->status = 500;
        }
        json_decref(device);
      }
      json_decref(result);
    }
    json_decref(json_body);
    return U_CALLBACK_CONTINUE;
  }
}

int callback_benoic_device_delete (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * device;
  int res;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_benoic_device_delete - Error, callback_benoic_device_delete user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    device = get_device((struct _benoic_config *)user_data, u_map_get(request->map_url, "device_name"));
    if (device == NULL) {
      set_response_json_body_and_clean(response, 404, json_pack("{ss}", "message", "Device not found"));
    } else {
      if (json_object_get(device, "connected") == json_true()) {
        res = disconnect_device((struct _benoic_config *)user_data, device, 1);
        if (res != B_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "callback_benoic_device_delete - Error disconnecting device");
        }
      }
      if (delete_device((struct _benoic_config *)user_data, u_map_get(request->map_url, "device_name")) != B_OK) {
        response->status = 500;
      }
    }
    json_decref(device);
    return U_CALLBACK_CONTINUE;
  }
}

int callback_benoic_device_connect (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * device;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_benoic_device_get - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    device = get_device((struct _benoic_config *)user_data, u_map_get(request->map_url, "device_name"));
    if (device == NULL) {
      response->status = 404;
    } else {
      response->status = connect_device((struct _benoic_config *)user_data, device)==B_OK?200:500;
    }
    json_decref(device);
  }
  return U_CALLBACK_CONTINUE;
}

int callback_benoic_device_disconnect (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * device;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_benoic_device_get - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    device = get_device((struct _benoic_config *)user_data, u_map_get(request->map_url, "device_name"));
    if (device == NULL) {
      response->status = 404;
    } else {
      response->status = disconnect_device((struct _benoic_config *)user_data, device, 1)==B_OK?200:500;
    }
    json_decref(device);
  }
  return U_CALLBACK_CONTINUE;
}

int callback_benoic_device_ping (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * device;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_benoic_device_get - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    device = get_device((struct _benoic_config *)user_data, u_map_get(request->map_url, "device_name"));
    if (device == NULL) {
      response->status = 404;
    } else if (json_object_get(device, "enabled") == json_false()) {
      json_decref(device);
      set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "device disabled"));
    } else if (json_object_get(device, "connected") == json_false()) {
      json_decref(device);
      set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "device disconnected"));
    } else {
      if (ping_device((struct _benoic_config *)user_data, device) == B_OK) {
        response->status = 200;
      } else {
        response->status = 503;
      }
      json_decref(device);
    }
  }
  return U_CALLBACK_CONTINUE;
}

int callback_benoic_device_overview (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * device, * overview;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_benoic_device_get - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    device = get_device((struct _benoic_config *)user_data, u_map_get(request->map_url, "device_name"));
    if (device == NULL) {
      response->status = 404;
    } else if (json_object_get(device, "enabled") == json_false()) {
      json_decref(device);
      set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "device disabled"));
    } else if (json_object_get(device, "connected") == json_false()) {
      json_decref(device);
      set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "device disconnected"));
    } else {
      overview = overview_device((struct _benoic_config *)user_data, device);
      if (overview != NULL) {
        set_response_json_body_and_clean(response, 200, overview);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_benoic_device_get - Error getting overview for device %s", u_map_get(request->map_url, "device_name"));
        response->status = 500;
      }
      json_decref(device);
    }
  }
  return U_CALLBACK_CONTINUE;
}

int callback_benoic_device_element_get (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * device, * result = NULL;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_benoic_device_get - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    device = get_device((struct _benoic_config *)user_data, u_map_get(request->map_url, "device_name"));
    if (device == NULL) {
      response->status = 404;
    } else if (json_object_get(device, "enabled") == json_false()) {
      json_decref(device);
      set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "device disabled"));
    } else if (json_object_get(device, "connected") == json_false()) {
      json_decref(device);
      set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "device disconnected"));
    } else {
      if (0 == o_strcmp(u_map_get(request->map_url, "element_type"), "sensor")) {
        result = get_sensor((struct _benoic_config *)user_data, device, u_map_get(request->map_url, "element_name"));
      } else if (0 == o_strcmp(u_map_get(request->map_url, "element_type"), "switch")) {
        result = get_switch((struct _benoic_config *)user_data, device, u_map_get(request->map_url, "element_name"));
      } else if (0 == o_strcmp(u_map_get(request->map_url, "element_type"), "dimmer")) {
        result = get_dimmer((struct _benoic_config *)user_data, device, u_map_get(request->map_url, "element_name"));
      } else if (0 == o_strcmp(u_map_get(request->map_url, "element_type"), "heater")) {
        result = get_heater((struct _benoic_config *)user_data, device, u_map_get(request->map_url, "element_name"));
      } else {
        set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "element type incorrect"));
      }
      if (result != NULL) {
        set_response_json_body_and_clean(response, 200, result);
      } else {
        response->status = 404;
      }
      json_decref(device);
    }
  }
  return U_CALLBACK_CONTINUE;
}

int callback_benoic_device_element_put (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * device, * element = NULL, * valid;
  int element_type = BENOIC_ELEMENT_TYPE_NONE;
  json_t * json_body = ulfius_get_json_body_request(request, NULL);
  
  if (json_body == NULL) {
    set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "invalid input json format"));
    return U_CALLBACK_CONTINUE;
  } else if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_benoic_device_element_put - Error, user_data is NULL");
    json_decref(json_body);
    return U_CALLBACK_ERROR;
  } else {
    device = get_device((struct _benoic_config *)user_data, u_map_get(request->map_url, "device_name"));
    if (device == NULL) {
      response->status = 404;
    } else {
      if (0 == o_strcmp(u_map_get(request->map_url, "element_type"), "sensor")) {
        element_type = BENOIC_ELEMENT_TYPE_SENSOR;
      } else if (0 == o_strcmp(u_map_get(request->map_url, "element_type"), "switch")) {
        element_type = BENOIC_ELEMENT_TYPE_SWITCH;
      } else if (0 == o_strcmp(u_map_get(request->map_url, "element_type"), "dimmer")) {
        element_type = BENOIC_ELEMENT_TYPE_DIMMER;
      } else if (0 == o_strcmp(u_map_get(request->map_url, "element_type"), "heater")) {
        element_type = BENOIC_ELEMENT_TYPE_HEATER;
      } else {
        set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "element type incorrect"));
      }
      if (has_element((struct _benoic_config *)user_data, device, element_type, u_map_get(request->map_url, "element_name"))) {
        element = get_element_data((struct _benoic_config *)user_data, device, element_type, u_map_get(request->map_url, "element_name"), 1);
        if (element != NULL) {
          valid = is_element_valid(json_body, element_type);
          if (valid == NULL) {
            y_log_message(Y_LOG_LEVEL_ERROR, "callback_benoic_device_element_put - Error is_element_valid");
            response->status = 500;
          } else if (json_array_size(valid) > 0) {
            set_response_json_body_and_clean(response, 400, valid);
          } else {
            if (set_element_data((struct _benoic_config *)user_data, device, u_map_get(request->map_url, "element_name"), element_type, json_body, 1) != B_OK) {
              y_log_message(Y_LOG_LEVEL_ERROR, "callback_benoic_device_element_put - Error setting element data");
              response->status = 500;
            }
            json_decref(valid);
          }
          json_decref(element);
        } else {
          // Error
          y_log_message(Y_LOG_LEVEL_ERROR, "callback_benoic_device_element_put - Error get_element_data");
          response->status = 500;
        }
      } else {
        response->status = 404;
      }
      json_decref(device);
    }
    json_decref(json_body);
    return U_CALLBACK_CONTINUE;
  }
}

int callback_benoic_device_element_set (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * device, * element = NULL;
  int element_type = BENOIC_ELEMENT_TYPE_NONE, i_command, res = 0;
  float f_command;
  char * endptr;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_benoic_device_element_set - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    device = get_device((struct _benoic_config *)user_data, u_map_get(request->map_url, "device_name"));
    if (device == NULL) {
      set_response_json_body_and_clean(response, 404, json_pack("{ss}", "error", "device not found"));
    } else if (json_object_get(device, "enabled") == json_false()) {
      set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "device disabled"));
    } else if (json_object_get(device, "connected") == json_false()) {
      set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "device disconnected"));
    } else {
      if (0 == o_strcmp(u_map_get(request->map_url, "element_type"), "sensor")) {
        element_type = BENOIC_ELEMENT_TYPE_SENSOR;
      } else if (0 == o_strcmp(u_map_get(request->map_url, "element_type"), "switch")) {
        element_type = BENOIC_ELEMENT_TYPE_SWITCH;
      } else if (0 == o_strcmp(u_map_get(request->map_url, "element_type"), "dimmer")) {
        element_type = BENOIC_ELEMENT_TYPE_DIMMER;
      } else if (0 == o_strcmp(u_map_get(request->map_url, "element_type"), "heater")) {
        element_type = BENOIC_ELEMENT_TYPE_HEATER;
      }
      if (element_type != BENOIC_ELEMENT_TYPE_NONE) {
        element = get_element_data((struct _benoic_config *)user_data, device, element_type, u_map_get(request->map_url, "element_name"), 0);
        if (element != NULL) {
          switch (element_type) {
            case BENOIC_ELEMENT_TYPE_SWITCH:
              i_command = strtol(u_map_get(request->map_url, "command"), &endptr, 10);
              if (*endptr == '\0' && i_command >= -1 && i_command <= 1) {
                if (set_switch((struct _benoic_config *)user_data, device, u_map_get(request->map_url, "element_name"), i_command) != B_OK) {
                  response->status = 500;
                }
              } else {
                set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "incorrect command, must be -1 (toggle), 0 (off) or 1 (on)"));
              }
              break;
            case BENOIC_ELEMENT_TYPE_DIMMER:
              i_command = strtol(u_map_get(request->map_url, "command"), &endptr, 10);
              if (*endptr == '\0' && i_command >= 0 && i_command <= 101) {
                json_t * j_result = set_dimmer((struct _benoic_config *)user_data, device, u_map_get(request->map_url, "element_name"), i_command);
                if (json_integer_value(json_object_get(j_result, "result")) != B_OK) {
                  response->status = 500;
                } else {
                  json_object_del(j_result, "result");
                  set_response_json_body_and_clean(response, 200, json_copy(j_result));
                }
                json_decref(j_result);
              } else {
                set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "incorrect command, must be between 0 and 101"));
              }
              break;
            case BENOIC_ELEMENT_TYPE_HEATER:
              f_command = strtof(u_map_get(request->map_url, "command"), &endptr);
              if (*endptr == '\0') {
                res = set_heater((struct _benoic_config *)user_data, device, u_map_get(request->map_url, "element_name"), u_map_get(request->map_url, "mode"), f_command);
                if (res == B_ERROR_PARAM) {
                  response->status = 400;
                } else if (res != B_OK) {
                  response->status = 500;
                }
              } else {
                set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "mode (optional) must be off, manual or auto, command must be a numeric value"));
              }
              break;
            default:
            set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "element type incorrect"));
              break;
          }
          json_decref(element);
        } else {
          set_response_json_body_and_clean(response, 404, json_pack("{ss}", "error", "element not found"));
        }
      } else {
        set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "element type incorrect"));
      }
      json_decref(device);
    }
    return U_CALLBACK_CONTINUE;
  }
}

int callback_benoic_device_element_add_tag (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * device, * element = NULL;
  int element_type = BENOIC_ELEMENT_TYPE_NONE;
  int res;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_benoic_device_element_set - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    device = get_device((struct _benoic_config *)user_data, u_map_get(request->map_url, "device_name"));
    if (device == NULL) {
      set_response_json_body_and_clean(response, 404, json_pack("{ss}", "error", "device not found"));
    } else if (json_object_get(device, "enabled") == json_false()) {
      set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "device disabled"));
    } else if (json_object_get(device, "connected") == json_false()) {
      set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "device disconnected"));
    } else {
      if (0 == o_strcmp(u_map_get(request->map_url, "element_type"), "sensor")) {
        element_type = BENOIC_ELEMENT_TYPE_SENSOR;
      } else if (0 == o_strcmp(u_map_get(request->map_url, "element_type"), "switch")) {
        element_type = BENOIC_ELEMENT_TYPE_SWITCH;
      } else if (0 == o_strcmp(u_map_get(request->map_url, "element_type"), "dimmer")) {
        element_type = BENOIC_ELEMENT_TYPE_DIMMER;
      } else if (0 == o_strcmp(u_map_get(request->map_url, "element_type"), "heater")) {
        element_type = BENOIC_ELEMENT_TYPE_HEATER;
      }
      if (element_type != BENOIC_ELEMENT_TYPE_NONE && has_element((struct _benoic_config *)user_data, device, element_type, u_map_get(request->map_url, "element_name"))) {
        element = get_element_data((struct _benoic_config *)user_data, device, element_type, u_map_get(request->map_url, "element_name"), 1);
        if (element != NULL) {
          if (strlen(u_map_get(request->map_url, "tag")) > 64) {
            set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "tag must be max 64 characters"));
          } else {
            res = element_add_tag((struct _benoic_config *)user_data, device, element_type, u_map_get(request->map_url, "element_name"), u_map_get(request->map_url, "tag"));
            if (res == B_ERROR_PARAM) {
              response->status = 400;
            } else if (res != B_OK) {
              response->status = 500;
              y_log_message(Y_LOG_LEVEL_ERROR, "Error saving tag %s", u_map_get(request->map_url, "tag"));
            }
          }
          json_decref(element);
        } else {
          response->status = 500;
        }
      } else {
        set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "element type incorrect"));
      }
      json_decref(device);
    }
    return U_CALLBACK_CONTINUE;
  }
}

int callback_benoic_device_element_remove_tag (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * device, * element = NULL;
  int element_type = BENOIC_ELEMENT_TYPE_NONE;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_benoic_device_element_set - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    device = get_device((struct _benoic_config *)user_data, u_map_get(request->map_url, "device_name"));
    if (device == NULL) {
      set_response_json_body_and_clean(response, 404, json_pack("{ss}", "error", "device not found"));
    } else if (json_object_get(device, "enabled") == json_false()) {
      set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "device disabled"));
    } else if (json_object_get(device, "connected") == json_false()) {
      set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "device disconnected"));
    } else {
      if (0 == o_strcmp(u_map_get(request->map_url, "element_type"), "sensor")) {
        element_type = BENOIC_ELEMENT_TYPE_SENSOR;
      } else if (0 == o_strcmp(u_map_get(request->map_url, "element_type"), "switch")) {
        element_type = BENOIC_ELEMENT_TYPE_SWITCH;
      } else if (0 == o_strcmp(u_map_get(request->map_url, "element_type"), "dimmer")) {
        element_type = BENOIC_ELEMENT_TYPE_DIMMER;
      } else if (0 == o_strcmp(u_map_get(request->map_url, "element_type"), "heater")) {
        element_type = BENOIC_ELEMENT_TYPE_HEATER;
      }
      if (element_type != BENOIC_ELEMENT_TYPE_NONE && has_element((struct _benoic_config *)user_data, device, element_type, u_map_get(request->map_url, "element_name"))) {
        element = get_element_data((struct _benoic_config *)user_data, device, element_type, u_map_get(request->map_url, "element_name"), 1);
        if (element != NULL) {
          if (element_remove_tag((struct _benoic_config *)user_data, device, element_type, u_map_get(request->map_url, "element_name"), u_map_get(request->map_url, "tag")) != B_OK) {
            response->status = 500;
            y_log_message(Y_LOG_LEVEL_ERROR, "Error saving tag %s", u_map_get(request->map_url, "tag"));
          }
          json_decref(element);
        } else {
          response->status = 500;
        }
      } else {
        set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "element type incorrect"));
      }
      json_decref(device);
    }
    return U_CALLBACK_CONTINUE;
  }
}

int callback_benoic_device_element_monitor(const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * device, * result, * params = json_object();
  int element_type = BENOIC_ELEMENT_TYPE_NONE, dt_param;
  char * endptr;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_benoic_device_element_monitor - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else if (params == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_benoic_device_element_monitor - Error allocating resources for params");
    return U_CALLBACK_ERROR;
  } else {
    device = get_device((struct _benoic_config *)user_data, u_map_get(request->map_url, "device_name"));
    if (device == NULL) {
      response->status = 404;
    } else if (json_object_get(device, "enabled") == json_false()) {
      json_decref(device);
      set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "device disabled"));
    } else if (json_object_get(device, "connected") == json_false()) {
      json_decref(device);
      set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "device disconnected"));
    } else {
      if (u_map_get(request->map_url, "from") != NULL) {
        dt_param = strtol(u_map_get(request->map_url, "from"), &endptr, 10);
        if (endptr != NULL && endptr[0] != '\0') {
          set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "from parameter must be an epoch timestamp value"));
          json_decref(params);
          return U_CALLBACK_CONTINUE;
        }
        json_object_set_new(params, "from", json_integer(dt_param));
      }
      if (u_map_get(request->map_url, "to") != NULL) {
        dt_param = strtol(u_map_get(request->map_url, "to"), &endptr, 10);
        if (endptr != NULL && endptr[0] != '\0') {
          set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "to parameter must be an epoch timestamp value"));
          json_decref(params);
          return U_CALLBACK_CONTINUE;
        }
        json_object_set_new(params, "to", json_integer(dt_param));
      }
      if (0 == o_strcmp(u_map_get(request->map_url, "element_type"), "sensor")) {
        element_type = BENOIC_ELEMENT_TYPE_SENSOR;
      } else if (0 == o_strcmp(u_map_get(request->map_url, "element_type"), "switch")) {
        element_type = BENOIC_ELEMENT_TYPE_SWITCH;
      } else if (0 == o_strcmp(u_map_get(request->map_url, "element_type"), "dimmer")) {
        element_type = BENOIC_ELEMENT_TYPE_DIMMER;
      } else if (0 == o_strcmp(u_map_get(request->map_url, "element_type"), "heater")) {
        element_type = BENOIC_ELEMENT_TYPE_HEATER;
      } else {
        set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "element type incorrect"));
      }
      result = element_get_monitor((struct _benoic_config *)user_data, device, element_type, u_map_get(request->map_url, "element_name"), params);
      if (result != NULL) {
        set_response_json_body_and_clean(response, 200, result);
      } else {
        response->status = 500;
      }
      json_decref(device);
      json_decref(params);
    }
  }
  return U_CALLBACK_CONTINUE;
}
