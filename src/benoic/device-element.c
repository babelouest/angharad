/**
 *
 * Benoic House Automation service
 *
 * Command house automation devices via an HTTP REST interface
 *
 * Device elements manipulation functions
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

#include "benoic.h"

/**
 * Get the lists of elements of the specified device stored in the database
 */
json_t * element_get_lists(struct _benoic_config * config, json_t * device) {
  json_t * j_query, * j_result, * j_return = NULL, * element;
  int res;
  size_t index;
  
  j_query = json_pack("{sss[ss]s{ss}}", 
                      "table", 
                      BENOIC_TABLE_ELEMENT,
                      "columns",
                        "be_name AS name",
                        "be_type AS type",
                      "where",
                        "bd_name",
                        json_string_value(json_object_get(device, "name")));
  if (j_query != NULL) {
    res = h_select(config->conn, j_query, &j_result, NULL);
    json_decref(j_query);
    if (res == H_OK || !json_is_array(j_result)) {
      j_return = json_pack("{s[]s[]s[]s[]}", "switches", "dimmers", "sensors", "heaters");
      if (j_return != NULL) {
        json_array_foreach(j_result, index, element) {
          switch (json_integer_value(json_object_get(element, "type"))) {
            case BENOIC_ELEMENT_TYPE_SENSOR:
              json_array_append_new(json_object_get(j_return, "sensors"), json_copy(json_object_get(element, "name")));
              break;
            case BENOIC_ELEMENT_TYPE_SWITCH:
              json_array_append_new(json_object_get(j_return, "switches"), json_copy(json_object_get(element, "name")));
              break;
            case BENOIC_ELEMENT_TYPE_DIMMER:
              json_array_append_new(json_object_get(j_return, "dimmers"), json_copy(json_object_get(element, "name")));
              break;
            case BENOIC_ELEMENT_TYPE_BLIND:
              json_array_append_new(json_object_get(j_return, "blinds"), json_copy(json_object_get(element, "name")));
              break;
            case BENOIC_ELEMENT_TYPE_HEATER:
              json_array_append_new(json_object_get(j_return, "heaters"), json_copy(json_object_get(element, "name")));
              break;
          }
        }
        json_decref(j_result);
      } else {
        y_log_message(Y_LOG_LEVEL_DEBUG, "element_get_lists - Error allocating resources for j_list");
      }
    } else {
      y_log_message(Y_LOG_LEVEL_DEBUG, "element_get_lists - Error executing j_query");
    }
  } else {
    y_log_message(Y_LOG_LEVEL_DEBUG, "element_get_lists - Error allocating resources for j_query");
  }
  return j_return;
}

/**
 * Check if the device has the specified element of the specified type
 * return true or false
 */
int has_element(struct _benoic_config * config, json_t * device, const int element_type, const char * element_name) {
  struct _device_type * device_type;
  
  if (device != NULL && json_object_get(device, "connected") == json_true()) {
    device_type = get_device_type(config, device);
    if (device_type == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "has_element - Error getting device_type");
      return 0;
    } else {
      return device_type->b_device_has_element(device, element_type, element_name, get_device_ptr(config, json_string_value(json_object_get(device, "name"))));
    }
  } else {
    return 0;
  }
}

/**
 * get the sensor value and data
 * return a json_t * containing the data, or NULL on error
 * returned value must be free'd after use
 */
json_t * get_sensor(struct _benoic_config * config, json_t * device, const char * sensor_name) {
  struct _device_type * device_type = get_device_type(config, device);
  json_t * sensor_value, * sensor_data, * to_return, * value;
  const char * key;
  
  if (device_type == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "get_sensor - Error getting device_type");
    return NULL;
  }
  
  sensor_value = device_type->b_device_get_sensor(device, sensor_name, get_device_ptr(config, json_string_value(json_object_get(device, "name"))));
  
  // Look for the device type
  if (sensor_value != NULL && json_integer_value(json_object_get(sensor_value, "result")) == DEVICE_RESULT_OK) {
    update_last_seen_device(config, device);
    sensor_data = get_element_data(config, device, BENOIC_ELEMENT_TYPE_SENSOR, sensor_name, 1);
    to_return = json_copy(sensor_data);
    if (to_return == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "get_sensor - Error allocating resources");
      json_decref(sensor_value);
      json_decref(sensor_data);
      return NULL;
    }
    json_object_del(sensor_value, "result");
    json_object_foreach(sensor_value, key, value) {
      json_object_set_new(to_return, key, json_copy(value));
    }
    json_decref(sensor_value);
    json_decref(sensor_data);
    return to_return;
  } else if (sensor_value != NULL) {
    json_decref(sensor_value);
  }
  return NULL;
}

/**
 * get the switch value and data
 * return a json_t * containing the data, or NULL on error
 * returned value must be free'd after use
 */
json_t * get_switch(struct _benoic_config * config, json_t * device, const char * switch_name) {
  struct _device_type * device_type = get_device_type(config, device);
  json_t * switch_value = device_type->b_device_get_switch(device, switch_name, get_device_ptr(config, json_string_value(json_object_get(device, "name")))), * switch_data, * to_return, * value;
  const char * key;
  
  // Look for the device type
  if (switch_value != NULL && json_integer_value(json_object_get(switch_value, "result")) == DEVICE_RESULT_OK) {
    json_object_del(switch_value, "result");
    update_last_seen_device(config, device);
    switch_data = get_element_data(config, device, BENOIC_ELEMENT_TYPE_SWITCH, switch_name, 1);
    to_return = json_copy(switch_data);
    if (to_return == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "get_switch - Error allocating resources");
      json_decref(switch_value);
      json_decref(switch_data);
      return NULL;
    }
    json_object_foreach(switch_value, key, value) {
      json_object_set_new(to_return, key, json_copy(value));
    }
    json_decref(switch_value);
    json_decref(switch_data);
    return to_return;
  } else if (switch_value != NULL) {
    json_decref(switch_value);
  }
  return NULL;
}

/**
 * set the switch command
 * return B_OK on success
 */
int set_switch(struct _benoic_config * config, json_t * device, const char * switch_name, const int command) {
  struct _device_type * device_type = get_device_type(config, device);
  json_t * result;
  int i_return;
  
  if (device_type != NULL) {
    result = device_type->b_device_set_switch(device, switch_name, command, get_device_ptr(config, json_string_value(json_object_get(device, "name"))));
    if (result != NULL) {
      if (json_integer_value(json_object_get(result, "result")) == DEVICE_RESULT_OK) {
        i_return =  B_OK;
      } else if (json_integer_value(json_object_get(result, "result")) == DEVICE_RESULT_NOT_FOUND) {
        i_return =  B_ERROR_NOT_FOUND;
      } else if (json_integer_value(json_object_get(result, "result")) != DEVICE_RESULT_OK) {
        i_return =  B_ERROR_IO;
      } else {
        i_return =  B_ERROR;
      }
      json_decref(result);
    } else {
      // Should not happen but I don't like gcc warnings
      i_return =  B_ERROR_IO;
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "set_switch - Device type not found");
    i_return =  B_ERROR_PARAM;
  }
  return i_return;
}

/**
 * get the dimmer value and data
 * return a json_t * containing the data, or NULL on error
 * returned value must be free'd after use
 */
json_t * get_dimmer(struct _benoic_config * config, json_t * device, const char * dimmer_name) {
  struct _device_type * device_type = get_device_type(config, device);
  json_t * dimmer_value = device_type->b_device_get_dimmer(device, dimmer_name, get_device_ptr(config, json_string_value(json_object_get(device, "name")))), * dimmer_data, * to_return, * value;
  const char * key;
  
  // Look for the device type
  if (dimmer_value != NULL && json_integer_value(json_object_get(dimmer_value, "result")) == DEVICE_RESULT_OK) {
    json_object_del(dimmer_value, "result");
    update_last_seen_device(config, device);
    dimmer_data = get_element_data(config, device, BENOIC_ELEMENT_TYPE_DIMMER, dimmer_name, 1);
    to_return = json_copy(dimmer_data);
    if (to_return == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "get_dimmer - Error allocating resources");
      json_decref(dimmer_value);
      json_decref(dimmer_data);
      return NULL;
    }
    json_object_foreach(dimmer_value, key, value) {
      json_object_set_new(to_return, key, json_copy(value));
    }
    json_decref(dimmer_value);
    json_decref(dimmer_data);
    return to_return;
  } else if (dimmer_value != NULL) {
    json_decref(dimmer_value);
  }
  return NULL;
}

/**
 * set the dimmer command
 * return B_OK on success
 */
json_t * set_dimmer(struct _benoic_config * config, json_t * device, const char * dimmer_name, const int command) {
  struct _device_type * device_type = get_device_type(config, device);
  json_t * result, * j_return;

  if (device_type != NULL) {
    result = device_type->b_device_set_dimmer(device, dimmer_name, command, get_device_ptr(config, json_string_value(json_object_get(device, "name"))));
    if (result != NULL) {
      if (json_integer_value(json_object_get(result, "result")) == DEVICE_RESULT_OK) {
        j_return = json_pack("{sisI}", "result", B_OK, "value", json_integer_value(json_object_get(result, "value")));
      } else if (json_integer_value(json_object_get(result, "result")) == DEVICE_RESULT_NOT_FOUND) {
        j_return = json_pack("{si}", "result", B_ERROR_NOT_FOUND);
      } else {
        j_return = json_pack("{si}", "result", B_ERROR_IO);
      }
      json_decref(result);
    } else {
      j_return = json_pack("{si}", "result", B_ERROR_IO);
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "set_dimmer - Device type not found");
    j_return = json_pack("{si}", "result", B_ERROR_PARAM);
  }
  return j_return;
}

/**
 * get the sensor value and data
 * return a json_t * containing the data, or NULL on error
 * returned value must be free'd after use
 */
json_t * get_heater(struct _benoic_config * config, json_t * device, const char * heater_name) {
  struct _device_type * device_type = get_device_type(config, device);
  json_t * heater_value = device_type->b_device_get_heater(device, heater_name, get_device_ptr(config, json_string_value(json_object_get(device, "name")))), * heater_data, * to_return, * value;
  const char * key;
  
  // Look for the device type
  if (heater_value != NULL && json_integer_value(json_object_get(heater_value, "result")) == DEVICE_RESULT_OK) {
    json_object_del(heater_value, "result");
    update_last_seen_device(config, device);
    heater_data = get_element_data(config, device, BENOIC_ELEMENT_TYPE_HEATER, heater_name, 1);
    to_return = json_copy(heater_data);
    if (to_return == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "get_heater - Error allocating resources");
      json_decref(heater_value);
      json_decref(heater_data);
      return NULL;
    }
    json_object_foreach(heater_value, key, value) {
      json_object_set_new(to_return, key, json_copy(value));
    }
    json_decref(heater_value);
    json_decref(heater_data);
    return to_return;
  } else if (heater_value != NULL) {
    json_decref(heater_value);
  }
  return NULL;
}

/**
 * set the heater command
 * return B_OK on success
 */
int set_heater(struct _benoic_config * config, json_t * device, const char * heater_name, const char * mode, const float command) {
  struct _device_type * device_type = get_device_type(config, device);
  json_t * result;
  int i_return;
  
  if (device_type != NULL) {
    result = device_type->b_device_set_heater(device, heater_name, mode, command, get_device_ptr(config, json_string_value(json_object_get(device, "name"))));
    if (result != NULL) {
      if (json_integer_value(json_object_get(result, "result")) == DEVICE_RESULT_OK) {
        i_return = B_OK;
      } else if (json_integer_value(json_object_get(result, "result")) == DEVICE_RESULT_NOT_FOUND) {
        i_return = B_ERROR_NOT_FOUND;
      } else if (json_integer_value(json_object_get(result, "result")) == DEVICE_RESULT_PARAM) {
        i_return = B_ERROR_PARAM;
      } else {
        i_return = B_ERROR_IO;
      }
      json_decref(result);
    } else {
      i_return = B_ERROR_IO;
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "set_heater - Device type not found");
    i_return = B_ERROR_PARAM;
  }
  return i_return;
}

/**
 * get the blind value and data
 * return a json_t * containing the data, or NULL on error
 * returned value must be free'd after use
 */
json_t * get_blind(struct _benoic_config * config, json_t * device, const char * blind_name) {
  struct _device_type * device_type = get_device_type(config, device);
  json_t * blind_value = device_type->b_device_get_blind(device, blind_name, get_device_ptr(config, json_string_value(json_object_get(device, "name")))), * blind_data, * to_return, * value;
  const char * key;
  
  // Look for the device type
  if (blind_value != NULL && json_integer_value(json_object_get(blind_value, "result")) == DEVICE_RESULT_OK) {
    json_object_del(blind_value, "result");
    update_last_seen_device(config, device);
    blind_data = get_element_data(config, device, BENOIC_ELEMENT_TYPE_BLIND, blind_name, 1);
    to_return = json_copy(blind_data);
    if (to_return == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "get_blind - Error allocating resources");
      json_decref(blind_value);
      json_decref(blind_data);
      return NULL;
    }
    json_object_foreach(blind_value, key, value) {
      json_object_set_new(to_return, key, json_copy(value));
    }
    json_decref(blind_value);
    json_decref(blind_data);
    return to_return;
  } else if (blind_value != NULL) {
    json_decref(blind_value);
  }
  return NULL;
}

/**
 * set the blind command
 * return B_OK on success
 */
json_t * set_blind(struct _benoic_config * config, json_t * device, const char * blind_name, const int command) {
  struct _device_type * device_type = get_device_type(config, device);
  json_t * result, * j_return;

  if (device_type != NULL) {
    result = device_type->b_device_set_blind(device, blind_name, command, get_device_ptr(config, json_string_value(json_object_get(device, "name"))));
    if (result != NULL) {
      if (json_integer_value(json_object_get(result, "result")) == DEVICE_RESULT_OK) {
        j_return = json_pack("{sisI}", "result", B_OK, "value", json_integer_value(json_object_get(result, "value")));
      } else if (json_integer_value(json_object_get(result, "result")) == DEVICE_RESULT_NOT_FOUND) {
        j_return = json_pack("{si}", "result", B_ERROR_NOT_FOUND);
      } else {
        j_return = json_pack("{si}", "result", B_ERROR_IO);
      }
      json_decref(result);
    } else {
      j_return = json_pack("{si}", "result", B_ERROR_IO);
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "set_blind - Device type not found");
    j_return = json_pack("{si}", "result", B_ERROR_PARAM);
  }
  return j_return;
}

/**
 * Element data functions
 */

/**
 * Get the element data
 * return a json_t * containing the element data, NULL on error
 * returned value must be free'd after use
 */
json_t * get_element_data(struct _benoic_config * config, json_t * device, const int element_type, const char * element_name, int create) {
  json_t * j_query = json_object(), * j_result, * j_return, * default_data;
  int res;
  
  if (config == NULL || device == NULL || element_name == NULL || j_query == NULL) {
    json_decref(j_query);
    y_log_message(Y_LOG_LEVEL_ERROR, "get_element_data - Error input parameters");
    return NULL;
  }
  
  json_object_set_new(j_query, "table", json_string(BENOIC_TABLE_ELEMENT));
  json_object_set_new(j_query, "where", json_pack("{sssiss}", "be_name", element_name, "be_type", element_type, "bd_name", json_string_value(json_object_get(device, "name"))));
  res = h_select(config->conn, j_query, &j_result, NULL);
  json_decref(j_query);
  if (res != H_OK) {
    json_decref(j_query);
    y_log_message(Y_LOG_LEVEL_ERROR, "get_element_data - Error query select");
    return NULL;
  }
  
  if (json_array_size(j_result) == 0 && create) {
    // Element missing in the database, create it with default values
    json_decref(j_result);
    default_data = json_object();
    if (default_data == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "get_element_data - Error allocating resources for default_data");
      return NULL;
    }
    json_object_set_new(default_data, "display", json_string(element_name));
    json_object_set_new(default_data, "description", json_string(""));
    json_object_set_new(default_data, "enabled", json_true());
    json_object_set_new(default_data, "monitored", json_false());
    json_object_set_new(default_data, "monitored_every", json_integer(0));
    switch (element_type) {
      case BENOIC_ELEMENT_TYPE_SENSOR:
      case BENOIC_ELEMENT_TYPE_HEATER:
        json_object_set_new(default_data, "options", json_pack("{ss}", "unit", ""));
        break;
      default:
        json_object_set_new(default_data, "options", json_object());
        break;
    }
    if (set_element_data(config, device, element_name, element_type, default_data, 0) == B_OK) {
      return default_data;
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "get_element_data - Error saving default_data");
      return NULL;
    }
  } else if (json_array_size(j_result) > 0) {
    j_return = parse_element_from_db(json_array_get(j_result, 0));
    json_decref(j_result);
    return j_return;
  } else {
    json_decref(j_result);
    return NULL;
  }
}

/**
 * Set the element data
 * return B_OK on success
 */
int set_element_data(struct _benoic_config * config, json_t * device, const char * element_name, const int element_type, json_t * element_data, const int update) {
  json_t * db_data, * j_query;
  int res;
  
  if (config == NULL || device == NULL || element_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "set_element_data - Error input parameter");
    return B_ERROR_PARAM;
  }
  
  db_data = parse_element_to_db(element_data, json_string_value(json_object_get(device, "name")), element_name, element_type, update);
  
  if (db_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "set_element_data - Error generating db_data");
    return B_ERROR;
  }
  
  j_query = json_object();
  
  if (j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "set_element_data - Error allocating resources for j_query");
    json_decref(db_data);
    return B_ERROR;
  }
  
  json_object_set_new(j_query, "table", json_string(BENOIC_TABLE_ELEMENT));
  if (update) {
    json_object_set_new(j_query, "set", db_data);
    json_object_set_new(j_query, "where", json_pack("{ssss}", "be_name", element_name, "bd_name", json_string_value(json_object_get(device, "name"))));
    res = h_update(config->conn, j_query, NULL);
  } else {
    json_object_set_new(j_query, "values", db_data);
    res = h_insert(config->conn, j_query, NULL);
  }
  json_decref(j_query);
  if (res == H_OK) {
    return B_OK;
  } else {
    return B_ERROR_DB;
  }
}

/**
 * Converts an element from the db format to the web format
 * return a json_t * containing the element data, NULL on error
 * returned value must be free'd after use
 */
json_t * parse_element_from_db(json_t * element) {
  json_t * to_return;
  
  if (element == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "parse_element_from_db - Error input parameter");
    return NULL;
  }
  
  to_return = json_object();
  
  if (to_return == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "parse_element_from_db - Error allocating resources for to_return");
    return NULL;
  }
  json_object_set_new(to_return, "display", json_copy(json_object_get(element, "be_display")));
  json_object_set_new(to_return, "description", json_copy(json_object_get(element, "be_description")));
  json_object_set_new(to_return, "enabled", json_integer_value(json_object_get(element, "be_enabled"))==1?json_true():json_false());
  json_object_set_new(to_return, "options", json_loads(json_string_value(json_object_get(element, "be_options")), JSON_DECODE_ANY, NULL));
  json_object_set_new(to_return, "monitored", json_integer_value(json_object_get(element, "be_monitored"))==1?json_true():json_false());
  json_object_set_new(to_return, "monitored_every", json_copy(json_object_get(element, "be_monitored_every")));
  
  return to_return;
}

/**
 * Converts an element from the web format to the db format
 * return a json_t * containing the element data, NULL on error
 * returned value must be free'd after use
 */
json_t * parse_element_to_db(json_t * element, const char * device, const char * element_name, const int element_type, const int update) {
  json_t * to_return;
  char * dump;
  
  if (element == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "parse_element_to_db - Error input parameter");
    return NULL;
  }
  
  to_return = json_object();
  
  if (to_return == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "parse_element_to_db - Error allocating resources for to_return");
    return NULL;
  }
  
  if (!update) {
    json_object_set_new(to_return, "be_name", json_string(element_name));
    json_object_set_new(to_return, "bd_name", json_string(device));
    json_object_set_new(to_return, "be_type", json_integer(element_type));
  }
  json_object_set_new(to_return, "be_display", json_copy(json_object_get(element, "display")));
  json_object_set_new(to_return, "be_description", json_copy(json_object_get(element, "description")));
  json_object_set_new(to_return, "be_enabled", json_object_get(element, "enabled")==json_true()?json_integer(1):json_integer(0));
  dump = json_dumps(json_object_get(element, "options"), JSON_COMPACT);
  json_object_set_new(to_return, "be_options", json_string(dump));
  o_free(dump);
  json_object_set_new(to_return, "be_monitored", json_object_get(element, "monitored")==json_true()?json_integer(1):json_integer(0));
  json_object_set_new(to_return, "be_monitored_every", json_copy(json_object_get(element, "monitored_every")));
  
  return to_return;
}

/**
 * check if an element is valid
 * return a json array containing the errors, or an empty array if no error
 * returned value must be free'd after use
 */
json_t * is_element_valid(json_t * element, const int element_type) {
  json_t * result, * value, * option_result;
  
  if (element == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "is_element_valid - Error input parameters");
    return NULL;
  }
  
  result = json_array();
  if (result == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "is_element_valid - Error allocating resources for result");
    return NULL;
  }
  
  if (!json_is_object(element)) {
    json_array_append_new(result, json_pack("{ss}", "element", "Element must be an object"));
  } else {
    value = json_object_get(element, "display");
    if (value != NULL && (!json_is_string(value) || json_string_length(value) > 128)) {
      json_array_append_new(result, json_pack("{ss}", "display", "display must be a string between 1 and 128 characters"));
    }
    
    value = json_object_get(element, "description");
    if (value != NULL && (!json_is_string(value) || json_string_length(value) > 128)) {
      json_array_append_new(result, json_pack("{ss}", "description", "description must be a string between 1 and 128 characters"));
    }
    
    value = json_object_get(element, "enabled");
    if (value != NULL && !json_is_boolean(value)) {
      json_array_append_new(result, json_pack("{ss}", "enabled", "enabled must be a boolean"));
    }
    
    value = json_object_get(element, "monitored");
    if (value != NULL && !json_is_boolean(value)) {
      json_array_append_new(result, json_pack("{ss}", "monitored", "monitored must be a boolean"));
    }
    
    value = json_object_get(element, "monitored_every");
    if (value != NULL && !(json_is_integer(value) || json_integer_value(value) < 0)) {
      json_array_append_new(result, json_pack("{ss}", "monitored_every", "monitored_every must be a positive integer"));
    }
    
    value = json_object_get(element, "options");
    if (value != NULL) {
      option_result = is_option_valid(value, element_type);
      if (option_result != NULL) {
        json_array_extend(result, option_result);
        json_decref(option_result);
      }
    }
  }
  return result;
}

/**
 * check if an element option is valid
 * return a json array containing the errors, or an empty array if no error
 * returned value must be free'd after use
 */
json_t * is_option_valid(json_t * options, int element_type) {
  json_t * result;
  
  result = json_array();
  
  if (result == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "is_option_valid - Error allocating resources for result");
    return NULL;
  }
  
  if (options == NULL) {
    return result;
  }
  
  if (!json_is_object(options)) {
    json_array_append_new(result, json_pack("{ss}", "options", "options must be an object"));
  } else {
    if (!is_tag_list_valid(json_object_get(options, "tags"))) {
      json_array_append_new(result, json_pack("{ss}", "options", "tags must be an array of maximum 128 strings which are maximum 128 characters length"));
    }
    
    if (element_type == BENOIC_ELEMENT_TYPE_HEATER || element_type == BENOIC_ELEMENT_TYPE_SENSOR) {
      if (json_object_get(options, "unit") != NULL && (!json_is_string(json_object_get(options, "unit")) || json_string_length(json_object_get(options, "unit")) > 64)) {
        json_array_append_new(result, json_pack("{ss}", "options", "unit must be a string of maximum 64 characters"));
      }
    }
  }
  return result;
}

/**
 * check if an element option tag list is valid
 * return a json array containing the errors, or an empty array if no error
 */
int is_tag_list_valid(json_t * tags) {
  json_t * value;
  size_t index;
  
  if (tags == NULL) {
    return 1;
  } else if (!json_is_array(tags)) {
    return 0;
  } else if (json_array_size(tags) > 128) {
    return 0;
  } else {
    json_array_foreach(tags, index, value) {
      if (!json_is_string(value) || json_string_length(value) > 64) {
        return 0;
      }
    }
    return 1;
  }
}

/**
 * add a new tag to the element
 * return B_OK on success
 */
int element_add_tag(struct _benoic_config * config, json_t * device, const int element_type, const char * element_name, const char * tag) {
  json_t * element_data = get_element_data(config, device, element_type, element_name, 0), * tag_list;
  int res;
  
  if (element_data != NULL) {
    tag_list = json_object_get(json_object_get(element_data, "options"), "tags");
    if (tag_list == NULL) {
      json_object_set_new(json_object_get(element_data, "options"), "tags", json_array());
    }
    if (json_is_array(tag_list) && json_array_size(tag_list) < 128) {
      json_array_append_new(json_object_get(json_object_get(element_data, "options"), "tags"), json_string(tag));
      res = set_element_data(config, device, element_name, element_type, element_data, 1);
    } else {
      res = B_ERROR_PARAM;
    }
  } else {
    res = B_ERROR_NOT_FOUND;
  }
  json_decref(element_data);
  return res;
}

/**
 * remove en existing tag from the element if it exists
 * return B_OK on success
 */
int element_remove_tag(struct _benoic_config * config, json_t * device, const int element_type, const char * element_name, const char * tag) {
  json_t * element_data = get_element_data(config, device, element_type, element_name, 0), * cur_tag;
  int res, index;
  
  if (element_data != NULL) {
    for (index = (int)json_array_size(json_object_get(json_object_get(element_data, "options"), "tags"))-1; index>=0; index--) {
      cur_tag = json_array_get(json_object_get(json_object_get(element_data, "options"), "tags"), (size_t)index);
      if (json_is_string(cur_tag) && 0 == o_strcmp(tag, json_string_value(cur_tag))) {
        json_array_remove(json_object_get(json_object_get(element_data, "options"), "tags"), (size_t)index);
      }
    }
    res = set_element_data(config, device, element_name, element_type, element_data, 1);
  } else {
    res = B_ERROR_NOT_FOUND;
  }
  json_decref(element_data);
  return res;
}

/**
 * return a monitor array for the specified element in the specified time range
 * returned value must be free'd after use
 */
json_t * element_get_monitor(struct _benoic_config * config, json_t * device, const int element_type, const char * element_name, json_t * params) {
  json_t * j_query = json_object(), * j_result = NULL, * j_where = json_object();
  char * tmp = NULL, * escaped_device, * escaped_name;
  int res;
  
  if (j_query == NULL || j_where == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "element_get_monitor - Error allocating values for j_query or j_where");
    json_decref(j_query);
    json_decref(j_where);
    return NULL;
  }
  
  json_object_set_new(j_query, "table", json_string(BENOIC_TABLE_MONITOR));
  if (config->conn->type == HOEL_DB_TYPE_MARIADB) {
    json_object_set_new(j_query, "columns", json_pack("[ss]", "UNIX_TIMESTAMP(bm_date) AS timestamp", "bm_value AS value"));
  } else {
    json_object_set_new(j_query, "columns", json_pack("[ss]", "bm_date AS timestamp", "bm_value AS value"));
  }
  
  escaped_device = h_escape_string(config->conn, json_string_value(json_object_get(device, "name")));
  escaped_name = h_escape_string(config->conn, element_name);
  tmp = msprintf("= (SELECT `be_id` FROM `b_element` WHERE `be_name` = '%s' AND `be_type` = %d AND `bd_name` = '%s')", escaped_name, element_type, escaped_device);
  o_free(escaped_device);
  o_free(escaped_name);
  json_object_set_new(j_where, "be_id", json_pack("{ssss}", "operator", "raw", "value", tmp));
  o_free(tmp);
  
  if (json_object_get(params, "from") != NULL) {
    if (config->conn->type == HOEL_DB_TYPE_MARIADB) {
      tmp = msprintf("> FROM_UNIXTIME(%" JSON_INTEGER_FORMAT ")", json_integer_value(json_object_get(params, "from")));
    } else {
      tmp = msprintf("> '%" JSON_INTEGER_FORMAT "'", json_integer_value(json_object_get(params, "from")));
    }
    json_object_set_new(j_where, "bm_date", json_pack("{ssss}", "operator", "raw", "value", tmp));
    o_free(tmp);
  } else {
    if (config->conn->type == HOEL_DB_TYPE_MARIADB) {
      json_object_set_new(j_where, "bm_date", json_pack("{ssss}", "operator", "raw", "value", "> DATE_SUB(CURDATE(), INTERVAL 1 DAY)"));
    } else {
      json_object_set_new(j_where, "bm_date", json_pack("{ssss}", "operator", "raw", "value", "> strftime('%s','now', '-1 day')"));
    }
  }
  
  if (json_object_get(params, "to") != NULL) {
    if (config->conn->type == HOEL_DB_TYPE_MARIADB) {
      tmp = msprintf("< FROM_UNIXTIME(%" JSON_INTEGER_FORMAT ")", json_integer_value(json_object_get(params, "to")));
    } else {
      tmp = msprintf("< '%" JSON_INTEGER_FORMAT "'", json_integer_value(json_object_get(params, "from")));
    }
    json_object_set_new(j_where, "bm_date", json_pack("{ssss}", "operator", "raw", "value", tmp));
    o_free(tmp);
  }
  
  json_object_set_new(j_query, "where", j_where);
  
  res = h_select(config->conn, j_query, &j_result, NULL);
  json_decref(j_query);
  if (res != H_OK) {
    y_log_message(Y_LOG_LEVEL_ERROR, "element_get_monitor - Error getting monitor from element %s/%s", json_string_value(json_object_get(device, "name")), element_name);
    return NULL;
  } else {
    return j_result;
  }
}
