/**
 *
 * Benoic House Automation service
 *
 * Command house automation devices via an HTTP REST interface
 *
 * Device manipulation functions
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
#include <dirent.h>
#include <dlfcn.h>

#include "benoic.h"

/**
 * Close its dl handles and free a struct _device_type
 */
void close_device_type(struct _device_type device_type) {
  o_free(device_type.uid);
  o_free(device_type.name);
  o_free(device_type.description);
#ifndef DEBUG
  dlclose(device_type.dl_handle);
#endif
  json_decref(device_type.options);
  device_type.uid = NULL;
  device_type.name = NULL;
  device_type.description = NULL;
  device_type.dl_handle = NULL;
  device_type.options = NULL;
}

/**
 * Close a list of struct _device_type
 */
int close_device_type_list(struct _device_type * device_type_list) {
  if (device_type_list == NULL) {
    return B_ERROR_PARAM;
  } else {
    int i;
    for (i=0; device_type_list[i].uid != NULL; i++) {
      close_device_type(device_type_list[i]);
    }
    return B_OK;
  }
}

/**
 * Initializes the struct _device_type according to all the devices types libraries present in config->modules_path
 */
int init_device_type_list(struct _benoic_config * config) {
  json_t * j_query, * j_result, * device_handshake;
  DIR * modules_directory;
  struct dirent * in_file;
  char * file_path;
  void * file_handle;
  int res;
  size_t nb_device_types = 0;
  
  config->device_type_list = o_malloc(sizeof(struct _device_type));
  
  if (config->device_type_list == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "init_device_type_list - Error getting device_type_list");
    return B_ERROR_DB;
  }
  
  if (config->conn != NULL && config->modules_path != NULL) {
    config->device_type_list->uid = NULL;
    config->device_type_list->dl_handle = NULL;
    config->device_type_list->name = NULL;
    config->device_type_list->description = NULL;
    
    // Disable all types in the database
    j_query = json_object();
    if (j_query == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "init_device_type_list - Error allocating resources for j_query");
      return B_ERROR_MEMORY;
    }
    json_object_set_new(j_query, "table", json_string(BENOIC_TABLE_DEVICE_TYPE));
    json_object_set_new(j_query, "set", json_pack("{ss}", "bdt_enabled", "0"));
    res = h_update(config->conn, j_query, NULL);
    if (res != H_OK) {
      y_log_message(Y_LOG_LEVEL_ERROR, "init_device_type_list - Error updating table %s", BENOIC_TABLE_DEVICE_TYPE);
      return B_ERROR_DB;
    }
    json_decref(j_query);
    
    // read module_path and load modules
    if (NULL == (modules_directory = opendir (config->modules_path))) {
      y_log_message(Y_LOG_LEVEL_ERROR, "init_device_type_list - Error reading libraries folder %s", config->modules_path);
      return B_ERROR_IO;
    }
    
    while ((in_file = readdir(modules_directory))) {
      if (!o_strcmp (in_file->d_name, ".")) {
        continue;
      }
      if (!o_strcmp (in_file->d_name, "..")) {
        continue;
      }
      
      file_path = msprintf("%s/%s", config->modules_path, in_file->d_name);
      
      if (file_path == NULL) {
        y_log_message(Y_LOG_LEVEL_ERROR, "init_device_type_list - Error allocating resources for file_path");
        return B_ERROR_MEMORY;
      }
      
      file_handle = dlopen(file_path, RTLD_LAZY);
      
      if (file_handle != NULL) {
        y_log_message(Y_LOG_LEVEL_INFO, "Open module from file %s", file_path);
        struct _device_type cur_device = {0};
        
        dlerror();
        cur_device.dl_handle = file_handle;
        *(void **) (&cur_device.b_device_type_init) = dlsym(cur_device.dl_handle, "b_device_type_init");
        *(void **) (&cur_device.b_device_connect) = dlsym(cur_device.dl_handle, "b_device_connect");
        *(void **) (&cur_device.b_device_disconnect) = dlsym(cur_device.dl_handle, "b_device_disconnect");
        *(void **) (&cur_device.b_device_ping) = dlsym(cur_device.dl_handle, "b_device_ping");
        *(void **) (&cur_device.b_device_overview) = dlsym(cur_device.dl_handle, "b_device_overview");
        *(void **) (&cur_device.b_device_get_sensor) = dlsym(cur_device.dl_handle, "b_device_get_sensor");
        *(void **) (&cur_device.b_device_get_switch) = dlsym(cur_device.dl_handle, "b_device_get_switch");
        *(void **) (&cur_device.b_device_set_switch) = dlsym(cur_device.dl_handle, "b_device_set_switch");
        *(void **) (&cur_device.b_device_get_dimmer) = dlsym(cur_device.dl_handle, "b_device_get_dimmer");
        *(void **) (&cur_device.b_device_set_dimmer) = dlsym(cur_device.dl_handle, "b_device_set_dimmer");
        *(void **) (&cur_device.b_device_get_heater) = dlsym(cur_device.dl_handle, "b_device_get_heater");
        *(void **) (&cur_device.b_device_set_heater) = dlsym(cur_device.dl_handle, "b_device_set_heater");
        *(void **) (&cur_device.b_device_get_blind) = dlsym(cur_device.dl_handle, "b_device_get_blind");
        *(void **) (&cur_device.b_device_set_blind) = dlsym(cur_device.dl_handle, "b_device_set_blind");
        *(void **) (&cur_device.b_device_has_element) = dlsym(cur_device.dl_handle, "b_device_has_element");
        
        if ((cur_device.b_device_type_init != NULL) &&
            (cur_device.b_device_connect != NULL) &&
            (cur_device.b_device_disconnect != NULL) &&
            (cur_device.b_device_ping != NULL) &&
            (cur_device.b_device_overview != NULL) &&
            (cur_device.b_device_get_sensor != NULL) &&
            (cur_device.b_device_get_switch != NULL) &&
            (cur_device.b_device_set_switch != NULL) &&
            (cur_device.b_device_get_dimmer != NULL) &&
            (cur_device.b_device_set_dimmer != NULL) &&
            (cur_device.b_device_get_heater != NULL) && 
            (cur_device.b_device_set_heater != NULL) &&
            (cur_device.b_device_get_blind != NULL) &&
            (cur_device.b_device_set_blind != NULL) &&
            (cur_device.b_device_has_element != NULL)) {
          device_handshake = (*cur_device.b_device_type_init)();
          cur_device.uid = o_strdup(json_string_value(json_object_get(device_handshake, "uid")));
          cur_device.name = o_strdup(json_string_value(json_object_get(device_handshake, "name")));
          cur_device.description = o_strdup(json_string_value(json_object_get(device_handshake, "description")));
          cur_device.options = json_copy(json_object_get(device_handshake, "options"));
          json_decref(device_handshake);
          device_handshake = NULL;
          
          if (cur_device.uid != NULL && cur_device.name != NULL && cur_device.description != NULL) {
            nb_device_types++;
            config->device_type_list = o_realloc(config->device_type_list, (nb_device_types+1)*sizeof(struct _device_type));
            if (config->device_type_list == NULL) {
              y_log_message(Y_LOG_LEVEL_ERROR, "init_device_type_list - Error allocating resources for device_type_list");
              close_device_type_list(config->device_type_list);
              o_free(config->device_type_list);
              config->device_type_list = NULL;
              return B_ERROR_MEMORY;
            }
            config->device_type_list[nb_device_types - 1].uid = cur_device.uid;
            config->device_type_list[nb_device_types - 1].name = cur_device.name;
            config->device_type_list[nb_device_types - 1].description = cur_device.description;
            config->device_type_list[nb_device_types - 1].options = cur_device.options;
            
            config->device_type_list[nb_device_types - 1].dl_handle = cur_device.dl_handle;
            config->device_type_list[nb_device_types - 1].b_device_type_init = cur_device.b_device_type_init;
            config->device_type_list[nb_device_types - 1].b_device_connect = cur_device.b_device_connect;
            config->device_type_list[nb_device_types - 1].b_device_disconnect = cur_device.b_device_disconnect;
            config->device_type_list[nb_device_types - 1].b_device_ping = cur_device.b_device_ping;
            config->device_type_list[nb_device_types - 1].b_device_overview = cur_device.b_device_overview;
            config->device_type_list[nb_device_types - 1].b_device_get_sensor = cur_device.b_device_get_sensor;
            config->device_type_list[nb_device_types - 1].b_device_get_switch = cur_device.b_device_get_switch;
            config->device_type_list[nb_device_types - 1].b_device_set_switch = cur_device.b_device_set_switch;
            config->device_type_list[nb_device_types - 1].b_device_get_dimmer = cur_device.b_device_get_dimmer;
            config->device_type_list[nb_device_types - 1].b_device_set_dimmer = cur_device.b_device_set_dimmer;
            config->device_type_list[nb_device_types - 1].b_device_get_heater = cur_device.b_device_get_heater;
            config->device_type_list[nb_device_types - 1].b_device_set_heater = cur_device.b_device_set_heater;
            config->device_type_list[nb_device_types - 1].b_device_get_blind = cur_device.b_device_get_blind;
            config->device_type_list[nb_device_types - 1].b_device_set_blind = cur_device.b_device_set_blind;
            config->device_type_list[nb_device_types - 1].b_device_has_element = cur_device.b_device_has_element;

            config->device_type_list[nb_device_types].uid = NULL;
            config->device_type_list[nb_device_types].name = NULL;
            config->device_type_list[nb_device_types].description = NULL;
            config->device_type_list[nb_device_types].dl_handle = NULL;
            
            // Insert or update device type in database
            j_query = json_object();
            if (j_query == NULL) {
              y_log_message(Y_LOG_LEVEL_ERROR, "init_device_type_list - Error allocating resources for j_query");
              return B_ERROR_DB;
            }
            json_object_set_new(j_query, "table", json_string(BENOIC_TABLE_DEVICE_TYPE));
            json_object_set_new(j_query, "where", json_pack("{ss}", "bdt_uid", cur_device.uid));
            res = h_select(config->conn, j_query, &j_result, NULL);
            json_decref(j_query);
            if (res != H_OK) {
              y_log_message(Y_LOG_LEVEL_ERROR, "init_device_type_list - Error getting type");
            } else {
              j_query = json_object();
              char * s_options = json_dumps(cur_device.options, JSON_COMPACT);
              if (j_query == NULL) {
                y_log_message(Y_LOG_LEVEL_ERROR, "init_device_type_list - Error allocating resources for j_query");
                return B_ERROR_MEMORY;
              }
              json_object_set_new(j_query, "table", json_string(BENOIC_TABLE_DEVICE_TYPE));
              if (json_array_size(j_result) == 0) {
                // Insert new device_type
                json_object_set_new(j_query, "values", json_pack("{sssssssiss}", 
                                                        "bdt_uid", cur_device.uid, 
                                                        "bdt_name", cur_device.name, 
                                                        "bdt_description", cur_device.description, 
                                                        "bdt_enabled", 1, 
                                                        "bdt_options", s_options
                                                      ));
                res = h_insert(config->conn, j_query, NULL);
              } else {
                // Update existing device type
                json_object_set_new(j_query, "set", json_pack("{sssssiss}", 
                                                              "bdt_name", cur_device.name, 
                                                              "bdt_description", cur_device.description, 
                                                              "bdt_enabled", 1, 
                                                              "bdt_options", s_options
                                                            ));
                json_object_set_new(j_query, "where", json_pack("{ss}", "bdt_uid", cur_device.uid));
                res = h_update(config->conn, j_query, NULL);
              }
              free(s_options);
              json_decref(j_query);
              json_decref(j_result);
              if (res != H_OK) {
                y_log_message(Y_LOG_LEVEL_ERROR, "init_device_type_list - Error setting device types in database");
              }
            }
          } else {
            close_device_type(cur_device);
            y_log_message(Y_LOG_LEVEL_ERROR, "init_device_type_list - Error handshake for module %s", file_path);
          }
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "init_device_type_list - Error getting all function handles for module %s: b_device_type_init %p, b_device_connect %p, b_device_disconnect %p, b_device_ping %p, b_device_overview %p, b_device_get_sensor_value %p, b_device_get_switch_value %p, b_device_set_switch_value %p, b_device_get_dimmer_value %p, b_device_set_dimmer_value %p, b_device_get_heater_value %p, b_device_set_heater_value %p, b_device_has_element %p", file_path,
          cur_device.b_device_type_init, cur_device.b_device_connect, cur_device.b_device_disconnect, cur_device.b_device_ping, cur_device.b_device_overview, cur_device.b_device_get_sensor,
          cur_device.b_device_get_switch, cur_device.b_device_set_switch, cur_device.b_device_get_dimmer, cur_device.b_device_set_dimmer,
          cur_device.b_device_get_heater, cur_device.b_device_set_heater, cur_device.b_device_has_element);
        }
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "Error opening benoic module file %s, reason: %s", file_path, dlerror());
      }
      o_free(file_path);
    }
    closedir(modules_directory);
    
    if (nb_device_types == 0) {
      y_log_message(Y_LOG_LEVEL_WARNING, "No device type found for benoic subsystem. If not needed, you can disable it");
    }
    return B_OK;
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "init_device_type_list - Error input parameters");
    return B_ERROR_PARAM;
  }
}

int connect_enabled_devices(struct _benoic_config * config) {
  // Connect all devices that are marked connected in the database
  json_t * device_list = get_device(config, NULL), * device;
  size_t index;
  
  if (device_list != NULL) {
    json_array_foreach(device_list, index, device) {
      if (json_object_get(device, "connected") == json_true()) {
        int res = connect_device(config, device);
        if (res == B_OK) {
          y_log_message(Y_LOG_LEVEL_INFO, "Device %s connected", json_string_value(json_object_get(device, "name")));
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "Error connecting device %s, reason: %d", json_string_value(json_object_get(device, "name")), res);
        }
      }
    }
    json_decref(device_list);
    return B_OK;
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "init_device_type_list - Error getting device list");
    return B_ERROR_DB;
  }
}

/**
 * return the device_type available in the database
 * returned value must be free'd after use
 */
json_t * get_device_types_list(struct _benoic_config * config) {
  json_t * j_query = json_object(), * j_result, * j_device_types_list = json_array(), * value, * cur_type;
  size_t index;
  int res;
  
  if (j_query == NULL || j_device_types_list == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "get_device_types_list - Error allocating resources for j_query or j_device_types_list");
    json_decref(j_query);
    json_decref(j_device_types_list);
    return NULL;
  }
  json_object_set_new(j_query, "table", json_string(BENOIC_TABLE_DEVICE_TYPE));
  res = h_select(config->conn, j_query, &j_result, NULL);
  json_decref(j_query);
  if (res == H_OK) {
    json_array_foreach(j_result, index, value) {
      cur_type = json_object();
      if (cur_type == NULL) {
        y_log_message(Y_LOG_LEVEL_ERROR, "get_device_types_list - Error allocating resources for cur_type");
        json_decref(j_query);
        json_decref(j_device_types_list);
        json_decref(j_result);
        return NULL;
      }
      json_object_set_new(cur_type, "uid", json_copy(json_object_get(value, "bdt_uid")));
      json_object_set_new(cur_type, "enabled", (json_integer_value(json_object_get(value, "bdt_enabled"))==1)?json_true():json_false());
      json_object_set_new(cur_type, "name", json_copy(json_object_get(value, "bdt_name")));
      json_object_set_new(cur_type, "description", json_copy(json_object_get(value, "bdt_description")));
      json_object_set_new(cur_type, "options", json_loads(json_string_value(json_object_get(value, "bdt_options")), JSON_DECODE_ANY, NULL));
      json_array_append_new(j_device_types_list, cur_type);
    }
    json_decref(j_result);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "get_device_types_list - Error getting list in database");
    json_decref(j_query);
    json_decref(j_device_types_list);
    json_decref(j_result);
    return NULL;
  }
  return j_device_types_list;
}

/**
 * return all the devices or a specific device using its name
 * returned value must be free'd after use
 */
json_t * get_device(struct _benoic_config * config, const char * name) {
  json_t * j_query, * j_result, * j_to_return = NULL, * value, * cur_device;
  size_t index;
  int res;
  struct _device_type * cur_type;
  
  j_query = json_pack("{sss[sssssss]}",
                      "table", BENOIC_TABLE_DEVICE,
                      "columns",
                        "bd_name",
                        "bd_description",
                        "bd_enabled",
                        "bd_connected",
                        "bdt_uid",
                        "UNIX_TIMESTAMP(bd_last_seen) AS bd_last_seen",
                        "bd_options");
  if (name != NULL) {
    json_object_set_new(j_query, "where", json_pack("{ss}", "bd_name", name));
  }
  
  res = h_select(config->conn, j_query, &j_result, NULL);
  json_decref(j_query);
  if (res == H_OK) {
    if (name == NULL) {
      j_to_return = json_array();
      if (j_to_return == NULL) {
        y_log_message(Y_LOG_LEVEL_ERROR, "get_device - Error allocating resources for j_to_return");
        json_decref(j_result);
        return NULL;
      }
      json_array_foreach(j_result, index, value) {
        cur_device = parse_device_from_db(value);
        cur_type = get_device_type(config, cur_device);
        if (cur_type != NULL) {
          json_array_append_new(j_to_return, cur_device);
        } else {
          json_decref(cur_device);
        }
      }
      json_decref(j_result);
    } else {
      if (json_array_size(j_result) == 0) {
        json_decref(j_result);
        return NULL;
      } else {
        j_to_return = parse_device_from_db(json_array_get(j_result, 0));
        json_decref(j_result);
        cur_type = get_device_type(config, j_to_return);
        if (cur_type != NULL) {
          return(j_to_return);
        } else {
          json_decref(j_to_return);
          return NULL;
        }
      }
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "get_device - Error getting list in database");
    return NULL;
  }
  return j_to_return;
}

/**
 * converts a device from the db format to the web format
 * returned value must be free'd after use
 */
json_t * parse_device_from_db(json_t * result) {
  json_t * to_return;
  if (result == NULL || !json_is_object(result)) {
    return NULL;
  } else {
    to_return = json_object();
    if (to_return == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "parse_device_from_db - Error allocating resources for to_return");
      return NULL;
    }
    json_object_set_new(to_return, "name", json_copy(json_object_get(result, "bd_name")));
    json_object_set_new(to_return, "description", json_copy(json_object_get(result, "bd_description")));
    json_object_set_new(to_return, "enabled", json_integer_value(json_object_get(result, "bd_enabled"))==1?json_true():json_false());
    json_object_set_new(to_return, "connected", json_integer_value(json_object_get(result, "bd_connected"))==1?json_true():json_false());
    json_object_set_new(to_return, "type_uid", json_copy(json_object_get(result, "bdt_uid")));
    json_object_set_new(to_return, "last_seen", json_copy(json_object_get(result, "bd_last_seen")));
    json_object_set_new(to_return, "options", json_loads(json_string_value(json_object_get(result, "bd_options")), JSON_DECODE_ANY, NULL));
    return to_return;
  }
}

/**
 * converts a device from the web format to the db format
 * returned value must be free'd after use
 */
json_t * parse_device_to_db(json_t * device, const int update) {
  json_t * result = json_object();
  char * tmp;
  
  if (result == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "parse_device_to_db - Error allocating resources for result");
    return NULL;
  }
  
  if (device == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "parse_device_to_db - Error input parameters");
    json_decref(result);
    return NULL;
  }
  
  if (!update) {
    json_object_set_new(result, "bd_name", json_copy(json_object_get(device, "name")));
    json_object_set_new(result, "bdt_uid", json_copy(json_object_get(device, "type_uid")));
  }
  json_object_set_new(result, "bd_description", json_copy(json_object_get(device, "description")));
  json_object_set_new(result, "bd_enabled", json_object_get(device, "enabled")==json_true()?json_integer(1):json_integer(2));
  tmp = json_dumps(json_object_get(device, "options"), JSON_COMPACT);
  json_object_set_new(result, "bd_options", json_string(tmp));
  o_free(tmp);
  
  return result;
}

/**
 * Check if a device send via http is valid
 * return a json array containing errors, or an empty array if no error
 * returned value must be free'd after use
 */
json_t * is_device_valid(struct _benoic_config * config, json_t * device, const int update) {
  json_t * result = json_array(), * j_element, * option_valid;
  if (device == NULL || config == NULL || result == NULL || !json_is_object(device)) {
    y_log_message(Y_LOG_LEVEL_ERROR, "is_device_valid - Error input parameters");
    json_decref(result);
    return NULL;
  }
  
  // Check device parameters
  j_element = json_object_get(device, "name");
  if (!update && (j_element == NULL || !json_is_string(j_element) || json_string_length(j_element) > 64 || json_string_length(j_element) <= 0)) {
    json_array_append_new(result, json_pack("{ss}", "name", "name is mandatory and must be unique, and a string between 1 and 64 characters"));
  }

  j_element = json_object_get(device, "description");
  if (j_element != NULL && (!json_is_string(j_element) || json_string_length(j_element) > 128)) {
    json_array_append_new(result, json_pack("{ss}", "description", "description is optional but must be a string between 1 and 128 characters"));
  }

  j_element = json_object_get(device, "enabled");
  if (j_element != NULL && !json_is_boolean(j_element)) {
    json_array_append_new(result, json_pack("{ss}", "enabled", "enabled is optional but must be a boolean"));
  }
  
  if (!update) {
    j_element = json_object_get(device, "type_uid");
    if (j_element == NULL || !json_is_string(j_element)) {
      json_array_append_new(result, json_pack("{ss}", "type_uid", "type_uid is mandatory and must be a valid device type"));
    }
  }
  
  option_valid = is_device_option_list_valid(config, device);
  if (option_valid != NULL) {
    json_array_extend(result, option_valid);
    json_decref(option_valid);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "is_device_valid - Error is_device_option_valid");
    json_decref(result);
    return NULL;
  }
  
  return result;
}

/**
 * Check if the option field of a device send via http is valid
 * return a json array containing errors, or an empty array if no error
 * returned value must be free'd after use
 */
json_t * is_device_option_list_valid(struct _benoic_config * config, json_t * device) {
  json_t * result = json_array(), * j_option_valid;
  int i, found = 0;
  
  if (result == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "is_device_option_list_valid - Error allocating resources for result");
    return NULL;
  }
  
  if (config->device_type_list == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "is_device_option_list_valid - Error config");
    json_decref(result);
    return NULL;
  }
  
  for (i=0; config->device_type_list[i].uid != NULL; i++) {
    if (0 == o_strcmp(config->device_type_list[i].uid, json_string_value(json_object_get(device, "type_uid")))) {
      found = 1;
      
      // Loop in all options and check each of them
      j_option_valid = is_device_option_valid(config->device_type_list[i].options, json_object_get(device, "options"));
      if (j_option_valid != NULL && json_array_size(j_option_valid) > 0) {
        json_array_extend(result, j_option_valid);
      }
      json_decref(j_option_valid);
    }
  }
  
  if (!found) {
    json_array_append_new(result, json_pack("{ss}", "type", "type not found"));
  }
  
  return result;
}

/**
 * Check if an option value of a device send via http is valid
 * return a json array containing errors, or an empty array if no error
 * returned value must be free'd after use
 */
json_t * is_device_option_valid(json_t * option_format_list, json_t * options) {
  json_t * option_format, * cur_option, * j_result = json_array();
  json_t * j_string = json_string("string"), * j_integer = json_string("integer"), * j_double = json_string("double"), * j_boolean = json_string("boolean");
  size_t index;
  
  if (j_result == NULL || j_string == NULL || j_integer == NULL || j_double == NULL || j_boolean == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "is_device_option_valid - Error allocating resources for result");
    json_decref(j_result);
    json_decref(j_string);
    json_decref(j_integer);
    json_decref(j_double);
    json_decref(j_boolean);
    return NULL;
  }
  
  json_array_foreach(option_format_list, index, option_format) {
    cur_option = json_object_get(options, json_string_value(json_object_get(option_format, "name")));
    
    if (cur_option == NULL && json_object_get(option_format, "optional") != json_true()) {
      json_array_append_new(j_result, json_pack("{ss}", json_string_value(json_object_get(option_format, "name")), "option is mandatory"));
    } else if (cur_option != NULL) {
      if (json_equal(json_object_get(option_format, "type"), j_string) && !json_is_string(cur_option)) {
        json_array_append_new(j_result, json_pack("{ss}", json_string_value(json_object_get(option_format, "name")), "option must be a string"));
      } else if (json_equal(json_object_get(option_format, "type"), j_integer) && !json_is_integer(cur_option)) {
        json_array_append_new(j_result, json_pack("{ss}", json_string_value(json_object_get(option_format, "name")), "option must be an integer"));
      } else if (json_equal(json_object_get(option_format, "type"), j_double) && !json_is_number(cur_option)) {
        json_array_append_new(j_result, json_pack("{ss}", json_string_value(json_object_get(option_format, "name")), "option must be a double"));
      } else if (json_equal(json_object_get(option_format, "type"), j_boolean) && !json_is_boolean(cur_option)) {
        json_array_append_new(j_result, json_pack("{ss}", json_string_value(json_object_get(option_format, "name")), "option must be a boolean"));
      }
    }
  }
  json_decref(j_string);
  json_decref(j_integer);
  json_decref(j_double);
  json_decref(j_boolean);
  return j_result;
}

/**
 * Add a new device
 * return B_OK on success
 */
int add_device(struct _benoic_config * config, const json_t * device) {
  json_t * j_query = json_object();
  int res;
  
  if (config == NULL || device == NULL || j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "add_device - Error allocating resources or input parameters");
    json_decref(j_query);
    return B_ERROR_PARAM;
  } else {
    json_object_set_new(j_query, "table", json_string(BENOIC_TABLE_DEVICE));
    json_object_set_new(j_query, "values", json_copy((json_t *)device));
    res = h_insert(config->conn, j_query, NULL);
    json_decref(j_query);
    if (res == H_OK) {
      return B_OK;
    } else {
      return B_ERROR_DB;
    }
  }
}

/**
 * Modify an existing device
 * return B_OK on success
 */
int modify_device(struct _benoic_config * config, const json_t * device, const char * name) {
  json_t * j_query = json_object();
  int res;
  
  if (config == NULL || device == NULL || j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "modify_device - Error allocating resources or input parameters");
    json_decref(j_query);
    return B_ERROR_PARAM;
  } else {
    json_object_set_new(j_query, "table", json_string(BENOIC_TABLE_DEVICE));
    json_object_set_new(j_query, "set", json_copy((json_t *)device));
    json_object_set_new(j_query, "where", json_pack("{ss}", "bd_name", name));
    res = h_update(config->conn, j_query, NULL);
    json_decref(j_query);
    if (res == H_OK) {
      return B_OK;
    } else {
      return B_ERROR_DB;
    }
  }
}

/**
 * update the connected flag of the specified device
 * return B_OK on success
 */
int set_device_connection(struct _benoic_config * config, const json_t * device, const int connected) {
  json_t * j_query = json_object();
  int res;
  
  if (config == NULL || device == NULL || j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "set_device_connection - Error allocating resources or input parameters");
    json_decref(j_query);
    return 0;
  } else {
    json_object_set_new(j_query, "table", json_string(BENOIC_TABLE_DEVICE));
    json_object_set_new(j_query, "set", json_pack("{so}", "bd_connected", connected?json_integer(1):json_integer(0)));
    json_object_set_new(j_query, "where", json_pack("{ss}", "bd_name", json_object_get(device, "name")));
    res = h_update(config->conn, j_query, NULL);
    json_decref(j_query);
    return ((res == H_OK)?B_OK:B_ERROR_DB);
  }
}

/**
 * delete the specified device
 * return B_OK on success
 */
int delete_device(struct _benoic_config * config, const char * name) {
  json_t * j_query = json_object();
  int res;
  
  if (config == NULL || j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "delete_device - Error allocating resources or input parameters");
    json_decref(j_query);
    return 0;
  } else {
    json_object_set_new(j_query, "table", json_string(BENOIC_TABLE_DEVICE));
    json_object_set_new(j_query, "where", json_pack("{ss}", "bd_name", name));
    res = h_delete(config->conn, j_query, NULL);
    json_decref(j_query);
    return ((res == H_OK)?B_OK:B_ERROR_DB);
  }
}

/**
 * return the struct _device_type corresponding to the specified device
 */
struct _device_type * get_device_type(struct _benoic_config * config, json_t * device) {
  const char * type_uid;
  int i;
  
  if (config == NULL || config->device_type_list == NULL || device == NULL) {
    return NULL;
  }
  
  type_uid = json_string_value(json_object_get(device, "type_uid"));
  
  if (type_uid == NULL) {
    return NULL;
  }
  
  for (i=0; config->device_type_list[i].uid != NULL; i++) {
    if (0 == o_strcmp(config->device_type_list[i].uid, type_uid)) {
      return (config->device_type_list + i);
    }
  }
  
  return NULL;
}

/**
 * Device modules management functions
 */

/**
 * Connect the device
 * Update the device options attribute if the module sends new data
 * return B_OK on success
 */
int connect_device(struct _benoic_config * config, json_t * device) {
  json_t * result, * result_options, * value, * j_db_device, * j_element_lists;
  struct _device_type * device_type = NULL;
  const char * key;
  char * device_name;
  int res;
  void * device_ptr = NULL;
  int to_return = B_OK;
  
  if (json_object_get(device, "enabled") != json_true()) {
    y_log_message(Y_LOG_LEVEL_ERROR, "Device is disabled");
    return B_ERROR_PARAM;
  }
  device_type = get_device_type(config, device);
  
  // Look for the device type
  if (device_type != NULL) {
    if (config->alert_url != NULL) {
      json_object_set_new(json_object_get(device, "options"), "alert_url", json_string(config->alert_url));
    }
    
    j_element_lists = element_get_lists(config, device);
    if (j_element_lists != NULL) {
      json_object_set_new(json_object_get(device, "options"), "element", json_copy(j_element_lists));
    } else {
      json_object_set_new(json_object_get(device, "options"), "element", json_pack("{s[]s[]s[]s[]}", "switches", "dimmers", "sensors", "heaters"));
    }
    json_decref(j_element_lists);
    
    result = device_type->b_device_connect(device, &device_ptr);
    
    // Remove element list
    json_object_del(json_object_get(device, "options"), "element");
    
    if (result != NULL && json_integer_value(json_object_get(result, "result")) == DEVICE_RESULT_OK) {
      y_log_message(Y_LOG_LEVEL_INFO, "Connect device %s: success", json_string_value(json_object_get(device, "name")));
      // Update device_data array
      if (device_ptr != NULL) {
        if (set_device_data(config, json_string_value(json_object_get(device, "name")), device_ptr) != B_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "Error setting device_data for device %s", json_string_value(json_object_get(device, "name")));
        }
      }
      // update database with options sent back if exist
      result_options = json_object_get(result, "options");
      if (result_options != NULL) {
        json_object_foreach(result_options, key, value) {
          json_object_del(json_object_get(device, "options"), key);
          json_object_set_new(json_object_get(device, "options"), key, json_copy(value));
        }
      }
      device_name = o_strdup(json_string_value(json_object_get(device, "name")));
      j_db_device = parse_device_to_db(device, 1);
      json_object_set_new(j_db_device, "bd_connected", json_integer(1));
      res = modify_device(config, j_db_device, device_name);
      o_free(device_name);
      json_decref(j_db_device);
      update_last_seen_device(config, device);
      to_return = res;
    } else if (result != NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "Error connecting device %s, result code is %" JSON_INTEGER_FORMAT, json_string_value(json_object_get(device, "name")), json_integer_value(json_object_get(result, "result")));
      device_name = o_strdup(json_string_value(json_object_get(device, "name")));
      j_db_device = parse_device_to_db(device, 1);
      json_object_set_new(j_db_device, "bd_connected", json_integer(0));
      modify_device(config, j_db_device, device_name);
      o_free(device_name);
      json_decref(j_db_device);
      to_return = B_ERROR_IO;
    } else {
      to_return = B_ERROR_IO;
    }
    json_decref(result);
  } else {
    j_db_device = parse_device_to_db(device, 1);
    json_object_set_new(j_db_device, "bd_connected", json_integer(0));
    modify_device(config, j_db_device, json_string_value(json_object_get(device, "name")));
    json_decref(j_db_device);
    y_log_message(Y_LOG_LEVEL_ERROR, "Error, No type found for this device");
    to_return = B_ERROR_PARAM;
  }
  return to_return;
}

/**
 * disconnect the device
 * return B_OK on success
 */
int disconnect_device(struct _benoic_config * config, json_t * device, int update_db_status) {
  json_t * result, * result_options, * value, * j_db_device;
  struct _device_type * device_type;
  const char * key;
  char * device_name;
  int res;
  
  if (json_object_get(device, "enabled") != json_true() || json_object_get(device, "connected") != json_true()) {
    y_log_message(Y_LOG_LEVEL_ERROR, "Device disabled or disconnected");
    return B_ERROR_PARAM;
  }
  device_type = get_device_type(config, device);
  
  // Look for the device type
  if (device_type != NULL) {
    result = device_type->b_device_disconnect(device, get_device_ptr(config, json_string_value(json_object_get(device, "name"))));
    if (get_device_ptr(config, json_string_value(json_object_get(device, "name"))) != NULL && remove_device_data(config, json_string_value(json_object_get(device, "name"))) != B_OK) {
      y_log_message(Y_LOG_LEVEL_ERROR, "Error removing device_data for device %s", json_string_value(json_object_get(device, "name")));
      return B_ERROR_MEMORY;
    }
    if (result != NULL && json_integer_value(json_object_get(result, "result")) == DEVICE_RESULT_OK) {
      y_log_message(Y_LOG_LEVEL_INFO, "Disconnect device %s: success", json_string_value(json_object_get(device, "name")));
      // update database with options sent back if exist
      result_options = json_object_get(result, "options");
      if (result_options != NULL) {
        json_object_foreach(result_options, key, value) {
          json_object_del(json_object_get(device, "options"), key);
          json_object_set_new(json_object_get(device, "options"), key, json_copy(value));
        }
      }
      device_name = o_strdup(json_string_value(json_object_get(device, "name")));
      j_db_device = parse_device_to_db(device, 1);
      if (update_db_status) {
        json_object_set_new(j_db_device, "bd_connected", json_integer(0));
      }
      res = modify_device(config, j_db_device, device_name);
      o_free(device_name);
      json_decref(result);
      json_decref(j_db_device);
      update_last_seen_device(config, device);
      return res;
    } else if (result != NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "Error disconnect device %s, result code is %" JSON_INTEGER_FORMAT, json_string_value(json_object_get(device, "name")), json_integer_value(json_object_get(result, "result")));
      json_decref(result);
      device_name = o_strdup(json_string_value(json_object_get(device, "name")));
      j_db_device = parse_device_to_db(device, 1);
      if (update_db_status) {
        json_object_set_new(j_db_device, "bd_connected", json_integer(0));
      }
      modify_device(config, j_db_device, device_name);
      o_free(device_name);
      json_decref(result);
      json_decref(j_db_device);
      return B_ERROR_IO;
    } else {
      return B_ERROR_IO;
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "No type found for this device");
    return B_ERROR_PARAM;
  }
}

/**
 * Ping the device
 * return B_OK on success
 */
int ping_device(struct _benoic_config * config, json_t * device) {
  struct _device_type * device_type = NULL;
  json_t * result;
  json_int_t i_result;
  
  if (json_object_get(device, "enabled") != json_true()) {
    y_log_message(Y_LOG_LEVEL_ERROR, "Device disabled");
    return B_ERROR_PARAM;
  }
  device_type = get_device_type(config, device);
  
  // Look for the device type
  if (device_type != NULL) {
    result = device_type->b_device_ping(device, get_device_ptr(config, json_string_value(json_object_get(device, "name"))));
    if (result != NULL) {
      i_result = json_integer_value(json_object_get(result, "result"));
      json_decref(result);
      if (i_result == DEVICE_RESULT_OK) {
        update_last_seen_device(config, device);
        set_device_connection(config, device, 1);
        return B_OK;
      } else if (i_result == DEVICE_RESULT_NOT_FOUND) {
        set_device_connection(config, device, 1);
        return B_ERROR_NOT_FOUND;
      } else {
        set_device_connection(config, device, 1);
        return B_ERROR_IO;
      }
    } else {
      set_device_connection(config, device, 1);
      return B_ERROR_IO;
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "No type found for this device");
    return B_ERROR_PARAM;
  }
}

/**
 * get the device overview: return all the device elements and their status
 * return a json_t * pointer contianing the result
 * returned value must be free'd after use
 */
json_t * overview_device(struct _benoic_config * config, json_t * device) {
  struct _device_type * device_type = NULL;
  json_t * overview, * element, * element_array, * to_return, * value;
  const char * key;
  
  if (json_object_get(device, "enabled") != json_true()) {
    y_log_message(Y_LOG_LEVEL_ERROR, "Device disabled");
    return NULL;
  }
  device_type = get_device_type(config, device);
  
  // Look for the device type
  if (device_type != NULL) {
    overview = device_type->b_device_overview(device, get_device_ptr(config, json_string_value(json_object_get(device, "name"))));
    if (overview != NULL && json_integer_value(json_object_get(overview, "result")) == DEVICE_RESULT_OK) {
      to_return = json_object();
      if (to_return == NULL) {
        y_log_message(Y_LOG_LEVEL_ERROR, "overview_device - Error allocating resources for to_return");
        json_decref(overview);
        return NULL;
      }
      update_last_seen_device(config, device);
      // Parse elements
      element_array = json_object_get(overview, "sensors");
      if (element_array != NULL) {
        json_object_set_new(to_return, "sensors", json_object());
        if (json_is_object(element_array)) {
          json_object_foreach(element_array, key, value) {
            element = get_element_data(config, device, BENOIC_ELEMENT_TYPE_SENSOR, key, 1);
            if (element != NULL) {
              if (json_is_object(value)) {
                if (json_object_get(value, "unit") != NULL) {
                  const char * elt_unit = json_string_value(json_object_get(json_object_get(element, "options"), "unit"));
                  if (elt_unit == NULL || strlen(elt_unit) == 0) {
                    json_object_set_new(json_object_get(element, "options"), "unit", json_copy(json_object_get(value, "unit")));
                  }
                }
                if (json_object_get(value, "trigger") != NULL) {
                  json_object_set_new(json_object_get(element, "options"), "trigger", json_copy(json_object_get(value, "trigger")));
                }
                json_object_set_new(element, "value", json_copy(json_object_get(value, "value")));
              } else {
                json_object_set_new(element, "value", json_copy(value));
              }
              json_object_set_new(json_object_get(to_return, "sensors"), key, element);
            } else {
              y_log_message(Y_LOG_LEVEL_ERROR, "overview_device - Error getting sensor %s from device %s", key, json_string_value(json_object_get(device, "name")));
            }
          }
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "overview_device - Error overview sensors");
        }
      }
      
      element_array = json_object_get(overview, "switches");
      if (element_array != NULL) {
        json_object_set_new(to_return, "switches", json_object());
        if (json_is_object(element_array)) {
          json_object_foreach(element_array, key, value) {
            element = get_element_data(config, device, BENOIC_ELEMENT_TYPE_SWITCH, key, 1);
            if (element != NULL) {
              if (json_is_object(value)) {
                if (json_object_get(value, "unit") != NULL) {
                  const char * elt_unit = json_string_value(json_object_get(json_object_get(element, "options"), "unit"));
                  if (elt_unit == NULL || strlen(elt_unit) == 0) {
                    json_object_set_new(json_object_get(element, "options"), "unit", json_copy(json_object_get(value, "unit")));
                  }
                }
                json_object_set_new(element, "value", json_copy(json_object_get(value, "value")));
              } else {
                json_object_set_new(element, "value", json_copy(value));
              }
              json_object_set_new(json_object_get(to_return, "switches"), key, element);
            } else {
              y_log_message(Y_LOG_LEVEL_ERROR, "overview_device - Error getting switch %s from device %s", key, json_string_value(json_object_get(device, "name")));
            }
          }
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "overview_device - Error overview switches");
        }
      }
      
      element_array = json_object_get(overview, "dimmers");
      if (element_array != NULL) {
        json_object_set_new(to_return, "dimmers", json_object());
        if (json_is_object(element_array)) {
          json_object_foreach(element_array, key, value) {
            element = get_element_data(config, device, BENOIC_ELEMENT_TYPE_DIMMER, key, 1);
            if (element != NULL) {
              if (json_is_object(value)) {
                if (json_object_get(value, "unit") != NULL) {
                  const char * elt_unit = json_string_value(json_object_get(json_object_get(element, "options"), "unit"));
                  if (elt_unit == NULL || strlen(elt_unit) == 0) {
                    json_object_set_new(json_object_get(element, "options"), "unit", json_copy(json_object_get(value, "unit")));
                  }
                }
                json_object_set_new(element, "value", json_copy(json_object_get(value, "value")));
              } else {
                json_object_set_new(element, "value", json_copy(value));
              }
              json_object_set_new(json_object_get(to_return, "dimmers"), key, element);
            } else {
              y_log_message(Y_LOG_LEVEL_ERROR, "overview_device - Error getting dimmer %s from device %s", key, json_string_value(json_object_get(device, "name")));
            }
          }
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "overview_device - Error overview dimmers");
        }
      }
      
      element_array = json_object_get(overview, "heaters");
      if (element_array != NULL) {
        json_object_set_new(to_return, "heaters", json_object());
        if (json_is_object(element_array)) {
          json_object_foreach(element_array, key, value) {
            element = get_element_data(config, device, BENOIC_ELEMENT_TYPE_HEATER, key, 1);
            if (element != NULL) {
              if (json_object_get(value, "unit") != NULL) {
                const char * elt_unit = json_string_value(json_object_get(json_object_get(element, "options"), "unit"));
                if (elt_unit == NULL || strlen(elt_unit) == 0) {
                  json_object_set_new(json_object_get(element, "options"), "unit", json_copy(json_object_get(value, "unit")));
                }
                json_object_del(value, "unit");
              }
              json_object_set_new(element, "value", json_copy(value));
              json_object_set_new(json_object_get(to_return, "heaters"), key, element);
            } else {
              y_log_message(Y_LOG_LEVEL_ERROR, "overview_device - Error getting heater %s from device %s", key, json_string_value(json_object_get(device, "name")));
            }
          }
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "overview_device - Error overview heaters");
        }
      }
      
      element_array = json_object_get(overview, "blinds");
      if (element_array != NULL) {
        json_object_set_new(to_return, "blinds", json_object());
        if (json_is_object(element_array)) {
          json_object_foreach(element_array, key, value) {
            element = get_element_data(config, device, BENOIC_ELEMENT_TYPE_BLIND, key, 1);
            if (element != NULL) {
              if (json_is_object(value)) {
                if (json_object_get(value, "unit") != NULL) {
                  const char * elt_unit = json_string_value(json_object_get(json_object_get(element, "options"), "unit"));
                  if (elt_unit == NULL || strlen(elt_unit) == 0) {
                    json_object_set_new(json_object_get(element, "options"), "unit", json_copy(json_object_get(value, "unit")));
                  }
                }
                json_object_set_new(element, "value", json_copy(json_object_get(value, "value")));
              } else {
                json_object_set_new(element, "value", json_copy(value));
              }
              json_object_set_new(json_object_get(to_return, "blinds"), key, element);
            } else {
              y_log_message(Y_LOG_LEVEL_ERROR, "overview_device - Error getting blind %s from device %s", key, json_string_value(json_object_get(device, "name")));
            }
          }
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "overview_device - Error overview blinds");
        }
      }
      
      json_decref(overview);
      return to_return;
    } else {
      json_decref(overview);
      y_log_message(Y_LOG_LEVEL_ERROR, "overview_device - Error getting overview");
      return NULL;
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "overview_device - No type found for this device");
    return NULL;
  }
}

/**
 * Update the last seen parameter for the specified device to the current date
 * return B_OK on success
 */
int update_last_seen_device(struct _benoic_config * config, json_t * device) {
  json_t * j_query;
  int res;
  
  if (json_object_get(device, "enabled") != json_true()) {
    y_log_message(Y_LOG_LEVEL_ERROR, "Device disabled");
    return B_ERROR_PARAM;
  }
  
  if (config == NULL || device == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "update_last_seen_device - Error input parameters");
    return B_ERROR_PARAM;
  }
  
  j_query = json_object();
  if (j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "update_last_seen_device - Error allocating resources for j_query");
    return B_ERROR_MEMORY;
  }
  
  json_object_set_new(j_query, "table", json_string(BENOIC_TABLE_DEVICE));
  json_object_set_new(j_query, "set", json_pack("{s{ss}}", "bd_last_seen", "raw", (config->conn->type == HOEL_DB_TYPE_MARIADB?"NOW()":"strftime('%s','now')")));
  json_object_set_new(j_query, "where", json_pack("{ss}", "bd_name", json_string_value(json_object_get(device, "name"))));
  
  res = h_update(config->conn, j_query, NULL);
  json_decref(j_query);
  
  if (res == H_OK) {
    return B_OK;
  } else {
    return B_ERROR_DB;
  }
}
