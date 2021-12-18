/**
 *
 * Angharad system
 *
 * Manage all subsystems and handles the house automation server:
 * - Benoic
 * - Gareth
 * - Carleon
 * 
 * Script management functions
 *
 * Copyright 2016-2021 Nicolas Mora <mail@babelouest.org>
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

#include "angharad.h"

/**
 * Get all scripts or the specified script
 */
json_t * script_get(struct config_elements * config, const char * script_name) {
  json_t * j_query, * j_result, * j_script, * j_actions, * j_options, * to_return;
  int res;
  size_t index;
  
  j_query = json_pack("{sss[ssss]}", "table", ANGHARAD_TABLE_SCRIPT, "columns", "asc_name AS name", "asc_description AS description", "asc_actions", "asc_options");
  
  if (j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "script_get - Error allocating resources for j_query");
    return json_pack("{si}", "result", A_ERROR_MEMORY);
  }
  
  if (script_name != NULL) {
    json_object_set_new(j_query, "where", json_pack("{ss}", "asc_name", script_name));
  }
  
  res = h_select(config->conn, j_query, &j_result, NULL);
  json_decref(j_query);
  if (res == H_OK) {
    if (script_name == NULL) {
      to_return = json_array();
      if (to_return == NULL) {
        y_log_message(Y_LOG_LEVEL_ERROR, "script_get - Error allocating resources for to_return");
        return json_pack("{si}", "result", A_ERROR_MEMORY);
      }
      json_array_foreach(j_result, index, j_script) {
        j_actions = json_loads(json_string_value(json_object_get(j_script, "asc_actions")), JSON_DECODE_ANY, NULL);
        json_object_del(j_script, "asc_actions");
        json_object_set_new(j_script, "actions", j_actions);
        j_options = json_loads(json_string_value(json_object_get(j_script, "asc_options")), JSON_DECODE_ANY, NULL);
        json_object_del(j_script, "asc_options");
        json_object_set_new(j_script, "options", j_options);
        json_array_append_new(to_return, json_copy(j_script));
      }
      json_decref(j_result);
      return json_pack("{siso}", "result", A_OK, "scripts", to_return);
    } else {
      if (json_array_size(j_result) == 0) {
        json_decref(j_result);
        return json_pack("{si}", "result", A_ERROR_NOT_FOUND);
      } else {
        j_script = json_copy(json_array_get(j_result, 0));
        json_decref(j_result);
        if (j_script == NULL) {
          y_log_message(Y_LOG_LEVEL_ERROR, "script_get - Error allocating resources for j_script");
          return json_pack("{si}", "result", A_ERROR_MEMORY);
        }
        j_actions = json_loads(json_string_value(json_object_get(j_script, "asc_actions")), JSON_DECODE_ANY, NULL);
        json_object_del(j_script, "asc_actions");
        json_object_set_new(j_script, "actions", j_actions);
        j_options = json_loads(json_string_value(json_object_get(j_script, "asc_options")), JSON_DECODE_ANY, NULL);
        json_object_del(j_script, "asc_options");
        json_object_set_new(j_script, "options", j_options);
        return json_pack("{siso}", "result", A_OK, "script", j_script);
      }
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "script_get - Error allocating resources for j_query");
    return json_pack("{si}", "result", A_ERROR_MEMORY);
  }
}

/**
 * Add a new script
 */
int script_add(struct config_elements * config, json_t * j_script) {
  json_t * j_query;
  int res;
  char * str_actions, * str_options;
  
  if (j_script == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "script_add - Error j_script is NULL");
    return A_ERROR_MEMORY;
  }
  
  str_actions = json_dumps(json_object_get(j_script, "actions"), JSON_COMPACT);
  str_options = json_dumps(json_object_get(j_script, "options"), JSON_COMPACT);
  j_query = json_pack("{sss[{ssssssss}]}",
                      "table", ANGHARAD_TABLE_SCRIPT,
                      "values",
                        "asc_name", json_string_value(json_object_get(j_script, "name")),
                        "asc_description", (json_object_get(j_script, "description")!=NULL?json_string_value(json_object_get(j_script, "description")):""),
                        "asc_actions", str_actions,
                        "asc_options", str_options);
  o_free(str_actions);
  o_free(str_options);
  
  if (j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "script_add - Error Allocating resources for j_query");
    return A_ERROR_MEMORY;
  }
  
  res = h_insert(config->conn, j_query, NULL);
  json_decref(j_query);
  if (res == H_OK) {
    return A_OK;
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "script_add - Error executing db query");
    return A_ERROR_DB;
  }
}

/**
 * Update the specified script
 */
int script_modify(struct config_elements * config, const char * script_name, json_t * j_script) {
  json_t * j_query, * cur_script;
  int res, res_cur_script;
  char * str_actions, * str_options;
  
  if (j_script == NULL || script_name == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "script_modify - Error j_script or script_name is NULL");
    return A_ERROR_MEMORY;
  }
  
  cur_script = script_get(config, script_name);
  res_cur_script = (cur_script != NULL?json_integer_value(json_object_get(cur_script, "result")):A_ERROR);
  json_decref(cur_script);
  if (res_cur_script == A_OK) {
    str_actions = json_dumps(json_object_get(j_script, "actions"), JSON_COMPACT);
    str_options = json_dumps(json_object_get(j_script, "options"), JSON_COMPACT);
    j_query = json_pack("{sss{ssssssss}s{ss}}",
                        "table", ANGHARAD_TABLE_SCRIPT,
                        "set",
                          "asc_name", script_name,
                          "asc_description", json_object_get(j_script, "description")!=NULL?json_string_value(json_object_get(j_script, "description")):"",
                          "asc_actions", str_actions,
                          "asc_options", str_options,
                        "where",
                          "asc_name", script_name);
    o_free(str_actions);
    o_free(str_options);
    
    if (j_query == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "script_modify - Error Allocating resources for j_query");
      return A_ERROR_MEMORY;
    }
    
    res = h_update(config->conn, j_query, NULL);
    json_decref(j_query);
    if (res == H_OK) {
      return A_OK;
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "script_add - Error executing db query");
      return A_ERROR_DB;
    }
  } else if (res_cur_script == A_ERROR_NOT_FOUND) {
    return A_ERROR_NOT_FOUND;
  } else {
    return A_ERROR;
  }
}

/**
 * Remove the specified script
 */
int script_delete(struct config_elements * config, const char * script_name) {
  json_t * j_query, * cur_script;
  int res, res_cur_script;
  
  if (script_name == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "script_modify - Error script_name is NULL");
    return A_ERROR_MEMORY;
  }
  
  cur_script = script_get(config, script_name);
  res_cur_script = (cur_script != NULL?json_integer_value(json_object_get(cur_script, "result")):A_ERROR);
  json_decref(cur_script);
  if (res_cur_script == A_OK) {
    j_query = json_pack("{sss{ss}}",
                        "table", ANGHARAD_TABLE_SCRIPT,
                        "where",
                          "asc_name", script_name);
    
    if (j_query == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "script_delete - Error Allocating resources for j_query");
      return A_ERROR_MEMORY;
    }
    
    res = h_delete(config->conn, j_query, NULL);
    json_decref(j_query);
    if (res == H_OK) {
      return A_OK;
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "script_delete - Error executing db query");
      return A_ERROR_DB;
    }
  } else if (res_cur_script == A_ERROR_NOT_FOUND) {
    return A_ERROR_NOT_FOUND;
  } else {
    return A_ERROR;
  }
}

/**
 * 
 * Check if the script is valid
 * 
 * script format must be:
 * {
 *   "name": string max 64 chars
 *   "description": string max 128 chars
 *   "options": object with a tag object in it
 *   "actions": array with valid actions in it
 * }
 */
json_t * is_script_valid(struct config_elements * config, json_t * j_script, const int update) {
  json_t * j_result = json_array(), * j_element, * j_actions_valid, * j_options_valid;
  size_t index;
  
  if (j_result == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "is_script_valid - Error allocating resources for j_result");
    return NULL;
  }
  if (j_script == NULL || !json_is_object(j_script)) {
    json_array_append_new(j_result, json_pack("{ss}", "script", "script object is mandatory"));
  } else {
    if (!update) {
      j_element = json_object_get(j_script, "name");
      if (j_element == NULL || !json_is_string(j_element) || json_string_length(j_element) > 64) {
        json_array_append_new(j_result, json_pack("{ss}", "name", "name parameter is mandatory must ba a string, maximum 64 characters"));
      }
    }

    j_element = json_object_get(j_script, "description");
    if (j_element != NULL && (!json_is_string(j_element) || json_string_length(j_element) > 128)) {
      json_array_append_new(j_result, json_pack("{ss}", "description", "description parameter must ba a string, maximum 128 characters"));
    }
    
    j_actions_valid = is_actions_valid(config, json_object_get(j_script, "actions"));
    if (j_actions_valid != NULL && json_array_size(j_actions_valid) > 0) {
      json_array_foreach(j_actions_valid, index, j_element) {
        json_array_append_new(j_result, json_copy(j_element));
      }
      json_decref(j_actions_valid);
    } else if (j_actions_valid != NULL) {
      json_decref(j_actions_valid);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "is_script_valid - Error, is_actions_valid returned NULL");
      json_decref(j_result);
      return NULL;
    }
    
    if (json_object_get(j_script, "options") != NULL) {
      j_options_valid = is_option_valid(json_object_get(j_script, "options"), BENOIC_ELEMENT_TYPE_NONE);
      if (j_options_valid != NULL && json_array_size(j_options_valid) > 0) {
        json_array_foreach(j_options_valid, index, j_element) {
          json_array_append_new(j_result, json_copy(j_element));
        }
        json_decref(j_options_valid);
      } else if(j_options_valid != NULL) {
        json_decref(j_options_valid);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "is_script_valid - Error, is_options_valid returned NULL");
        json_decref(j_result);
        return NULL;
      }
    }
  }
  return j_result;
}

/**
 * Check if all actions of the script are valid
 */
json_t * is_actions_valid(struct config_elements * config, json_t * j_action_list) {
  json_t * j_result = json_array(), * j_action, * is_valid;
  size_t index;
  
  if (j_result == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "is_actions_valid - Error allocating resources for j_result");
    return NULL;
  }
  
  if (json_is_array(j_action_list) && json_array_size(j_action_list) > 0) {
    json_array_foreach(j_action_list, index, j_action) {
      is_valid = is_action_valid(config, j_action, 0);
      if (is_valid != NULL) {
        json_array_append_new(j_result, is_valid);
      }
    }
  } else {
    json_array_append_new(j_result, json_pack("{ss}", "actions", "actions is mandatory and must be a non empty array of valid actions"));
  }
  
  return j_result;
}

/**
 * 
 * Check if an action is valid
 * 
 * action format:
 * 
 * {
 *   "submodule": "submodule_name",   // Required
 *   "element": "element_name"        // Required
 *   "command": "command"             // Required
 *   "parameters": {                  // Required for submodule benoic, depends on the element for submodule carleon
 *     "device": "device_name"        // Required for submodule benoic
 *     "element_type": "element_type" // Required for submodule benoic
 *     "service": "service_name"      // Required for carleon
 *     "param1": "value1",            // for a string value
 *     "param2": 2,                   // for an integer value
 *     "param3": 3.3                  // for a real value
 *     "param4": ["x","y","z"]        // for an array value
 *   }
 * 
 */
json_t * is_action_valid(struct config_elements * config, json_t * j_action, const int is_condition) {
  json_t * j_result = json_array(), * j_submodule, * j_parameters, * j_command, * j_element,     // Common parameters
          * j_device, * j_element_type, * j_mode,                                                // Benoic parameters
          * j_service, * j_command_list, * j_cur_command, * j_element_list, 										 // Carleon parameters
          * j_command_param, * j_cur_param, * j_cur_element;
  int i_element_type = BENOIC_ELEMENT_TYPE_NONE, found_element;
  struct _carleon_service * cur_service;
  const char * str_param;
  size_t index;

  if (j_result == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "is_action_valid - Error allocating resources for j_result");
    return NULL;
  }
  
  j_submodule = json_object_get(j_action, "submodule");
  if (j_submodule == NULL || !json_is_string(j_submodule)) {
    json_array_append_new(j_result, json_pack("{ss}", "action", "a valid submodule is mandatory"));
  } else {
    if (0 == o_strcmp("benoic", json_string_value(j_submodule))) {
      j_parameters = json_object_get(j_action, "parameters");
      if (j_parameters == NULL || !json_is_object(j_parameters)) {
        json_array_append_new(j_result, json_pack("{ss}", "action", "parameters is missing or not a json object"));
      } else {
        j_device = get_device(config->b_config, json_string_value(json_object_get(j_parameters, "device")));
        if (j_device == NULL) {
          json_array_append_new(j_result, json_pack("{ss}", "action", "device is missing or not valid"));
        } else {
          j_element_type = json_object_get(j_parameters, "element_type");
          if (j_element_type == NULL || !json_is_string(j_element_type)) {
            json_array_append_new(j_result, json_pack("{ss}", "action", "element_type is mandatory and must be a string"));
          } else {
            if (0 == o_strcmp("switch", json_string_value(j_element_type))) {
              i_element_type = BENOIC_ELEMENT_TYPE_SWITCH;
            } else if (0 == o_strcmp("dimmer", json_string_value(j_element_type))) {
              i_element_type = BENOIC_ELEMENT_TYPE_DIMMER;
            } else if (0 == o_strcmp("heater", json_string_value(j_element_type))) {
              i_element_type = BENOIC_ELEMENT_TYPE_HEATER;
            } else if (is_condition && 0 == o_strcmp("sensor", json_string_value(j_element_type))) {
              i_element_type = BENOIC_ELEMENT_TYPE_SENSOR;
            }
            
            if (i_element_type == BENOIC_ELEMENT_TYPE_NONE) {
              json_array_append_new(j_result, json_pack("{ss}", "action", "element_type invalid"));
            } else {
              if (!has_element(config->b_config, j_device, i_element_type, json_string_value(json_object_get(j_action, "element")))) {
                json_array_append_new(j_result, json_pack("{ss}", "action", "element invalid"));
              } else if (!is_condition) {
                j_command = json_object_get(j_action, "command");
                if (j_command == NULL) {
                  json_array_append_new(j_result, json_pack("{ss}", "action", "command is mandatory"));
                } else {
                  if (i_element_type == BENOIC_ELEMENT_TYPE_SWITCH) {
                    if (!json_is_integer(j_command) || (json_integer_value(j_command) != 0 && json_integer_value(j_command) != 1)) {
                      json_array_append_new(j_result, json_pack("{ss}", "action", "command is invalid, must be a 1 or a 0"));
                    }
                  } else if (i_element_type == BENOIC_ELEMENT_TYPE_DIMMER) {
                    if (!json_is_integer(j_command) || (json_integer_value(j_command) < 0 && json_integer_value(j_command) > 100)) {
                      json_array_append_new(j_result, json_pack("{ss}", "action", "command is invalid, must be between 0 and 100"));
                    }
                  } else if (i_element_type == BENOIC_ELEMENT_TYPE_HEATER) {
                    j_mode = json_object_get(j_parameters, "mode");
                    if (j_mode != NULL && !json_is_string(j_mode) && 0 != o_strcmp("off", json_string_value(j_mode)) && 0 != o_strcmp("auto", json_string_value(j_mode)) && 0 != o_strcmp("manual", json_string_value(j_mode))) {
                      json_array_append_new(j_result, json_pack("{ss}", "action", "mode is invalid, must be 'off', 'auto', 'manual' or none"));
                    }
                    
                    if (!json_is_number(j_command)) {
                      json_array_append_new(j_result, json_pack("{ss}", "action", "command is invalid, must be a real number"));
                    }
                  }
                }
              }
            }
          }
        }
        json_decref(j_device);
      }
    } else if (0 == o_strcmp("carleon", json_string_value(j_submodule))) {
      j_command = json_object_get(j_action, "command");
      j_element = json_object_get(j_action, "element");
      j_parameters = json_object_get(j_action, "parameters");
      
      if (j_command == NULL || !json_is_string(j_command)) {
        json_array_append_new(j_result, json_pack("{ss}", "action", "command is invalid"));
      } else if (j_element == NULL || !json_is_string(j_element)) {
        json_array_append_new(j_result, json_pack("{ss}", "action", "element is invalid"));
      } else if (j_parameters == NULL || !json_is_object(j_parameters)) {
        json_array_append_new(j_result, json_pack("{ss}", "action", "parameters is invalid"));
      } else {
        j_service = json_object_get(j_parameters, "service");
        if (j_service == NULL || !json_is_string(j_service)) {
          json_array_append_new(j_result, json_pack("{ss}", "action", "service parameter is invalid"));
        } else {
          cur_service = get_service_from_name(config->c_config, json_string_value(j_service));
          if (cur_service == NULL) {
            json_array_append_new(j_result, json_pack("{ss}", "action", "service parameter is invalid"));
          } else {
            j_command_list = cur_service->c_service_command_get_list(config->c_config);
            j_element_list = cur_service->c_service_element_get_list(config->c_config);
            
            if (j_command_list == NULL || json_integer_value(json_object_get(j_command_list, "result")) != MODULE_RESULT_OK) {
              json_array_append_new(j_result, json_pack("{ss}", "action", "service command list is invalid"));
            } else if (j_element_list == NULL || json_integer_value(json_object_get(j_element_list, "result")) != MODULE_RESULT_OK) {
              json_array_append_new(j_result, json_pack("{ss}", "action", "service element list is invalid"));
            } else {
              j_cur_command = json_object_get(json_object_get(j_command_list, "commands"), json_string_value(j_command));
              if (j_cur_command == NULL) {
                json_array_append_new(j_result, json_pack("{ss}", "action", "command is invalid"));
              } else {
                found_element = 0;
                json_array_foreach(json_object_get(j_element_list, "element"), index, j_cur_element) {
                  if (0 == o_strcmp(json_string_value(json_object_get(j_cur_element, "name")), json_string_value(j_element))) {
                    found_element = 1;
                  }
                }
                if (!found_element) {
                  json_array_append_new(j_result, json_pack("{ss}", "action", "element not found"));
                }
                
                json_object_foreach(json_object_get(j_cur_command, "parameters"), str_param, j_cur_param) {
                  j_command_param = json_object_get(j_parameters, str_param);
                  if (j_command_param == NULL && json_object_get(j_cur_param, "required") == json_true()) {
                    json_array_append_new(j_result, json_pack("{ss}", "action", "parameter is required"));
                  }
                  if (j_command_param != NULL) {
                    if (0 == o_strcmp("string", json_string_value(json_object_get(j_cur_param, "type"))) && !json_is_string(j_command_param)) {
                      json_array_append_new(j_result, json_pack("{ss}", "action", "parameter must be a string"));
                    } else if (0 == o_strcmp("integer", json_string_value(json_object_get(j_cur_param, "type"))) && !json_is_integer(j_command_param)) {
                      json_array_append_new(j_result, json_pack("{ss}", "action", "parameter must be an integer"));
                    } else if (0 == o_strcmp("real", json_string_value(json_object_get(j_cur_param, "type"))) && !json_is_number(j_command_param)) {
                      json_array_append_new(j_result, json_pack("{ss}", "action", "parameter must be an real"));
                    } else if (0 == o_strcmp("array", json_string_value(json_object_get(j_cur_param, "type"))) && !json_is_array(j_command_param)) {
                      json_array_append_new(j_result, json_pack("{ss}", "action", "parameter must be an array"));
                    }
                  }
                }
              }
            }
            json_decref(j_command_list);
            json_decref(j_element_list);
          }
        }
      }
    } else {
      json_array_append_new(j_result, json_pack("{ss}", "action", "a valid submodule is mandatory"));
    }
  }
  
  if (json_array_size(j_result) == 0) {
    json_decref(j_result);
    return NULL;
  } else {
    return j_result;
  }
}

/**
 * Add a tag to the specified script
 */
int script_add_tag(struct config_elements * config, const char * script_name, const char * tag) {
  json_t * j_result = script_get(config, script_name), * j_script, * j_tags;
  int res;
  
  if (j_result == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "script_add_tag - Error getting script");
    return A_ERROR;
  } else {
    if (json_integer_value(json_object_get(j_result, "result")) == A_ERROR_NOT_FOUND) {
      json_decref(j_result);
      return A_ERROR_NOT_FOUND;
    } else if (json_integer_value(json_object_get(j_result, "result")) == A_OK) {
      j_script = json_object_get(j_result, "script");
      j_tags = json_object_get(json_object_get(j_script, "options"), "tags");
      if (j_tags == NULL) {
        json_object_set_new(json_object_get(j_script, "options"), "tags", json_pack("[s]", tag));
      } else if (json_is_array(j_tags)) {
        if (json_array_size(j_tags) < 128) {
          json_array_append_new(json_object_get(json_object_get(j_script, "options"), "tags"), json_string(tag));
        } else {
          json_decref(j_result);
          return A_ERROR;
        }
      } else {
      }
      res = script_modify(config, script_name, j_script);
      json_decref(j_result);
      return res;
    } else {
      json_decref(j_result);
      return A_ERROR;
    }
  }
}

/**
 * Remove the specified tag from the specified script
 */
int script_remove_tag(struct config_elements * config, const char * script_name, const char * tag) {
  json_t * j_result = script_get(config, script_name), * j_script, * j_tags;
  int i, res;
  
  if (j_result == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "script_remove_tag - Error getting script");
    return A_ERROR;
  } else {
    if (json_integer_value(json_object_get(j_result, "result")) == A_ERROR_NOT_FOUND) {
      json_decref(j_result);
      return A_ERROR_NOT_FOUND;
    } else if (json_integer_value(json_object_get(j_result, "result")) == A_OK) {
      j_script = json_object_get(j_result, "script");
      j_tags = json_object_get(json_object_get(j_script, "options"), "tags");
      if (j_tags == NULL) {
        json_decref(j_result);
        return A_OK;
      } else if (json_is_array(j_tags)) {
        for (i = json_array_size(json_object_get(json_object_get(j_script, "options"), "tags"))-1; i >= 0; i--) {
          if (0 == o_strcmp(json_string_value(json_array_get(json_object_get(json_object_get(j_script, "options"), "tags"), i)), tag)) {
            json_array_remove(json_object_get(json_object_get(j_script, "options"), "tags"), i);
          }
        }
        res = script_modify(config, script_name, j_script);
        json_decref(j_result);
        return res;
      } else {
        json_decref(j_result);
        return A_ERROR;
      }
    } else {
      json_decref(j_result);
      return A_ERROR;
    }
  }
}

/**
 * Execute the specified script
 */
int script_run(struct config_elements * config, const char * script_name) {
  json_t * j_result, * j_script, * j_message, * j_action_list, * j_action;
  int res = A_OK;
  char * str_message_text;
  size_t index;
  
  if (config == NULL || script_name == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "script_run - Error input parameters");
    res = A_ERROR_PARAM;
  } else {
    j_result = script_get(config, script_name);
    if (j_result == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "script_run - Error getting script");
      res = A_ERROR_NOT_FOUND;
    } else {
      if (json_integer_value(json_object_get(j_result, "result")) == A_ERROR_NOT_FOUND) {
        json_decref(j_result);
        res = A_ERROR_NOT_FOUND;
      } else if (json_integer_value(json_object_get(j_result, "result")) == A_OK) {
        // Send a message via gareth submodule
        j_script = json_object_get(j_result, "script");
        str_message_text = msprintf("Running script %s", script_name);
        j_message = json_pack("{sssssss[s]}", "priority", "LOW", "source", "angharad", "text", str_message_text, "tags", "script");
        add_message(config->conn, j_message);
        json_decref(j_message);
        o_free(str_message_text);
        
        j_action_list = json_object_get(j_script, "actions");
        if (j_action_list != NULL && json_is_array(j_action_list)) {
          json_array_foreach(j_action_list, index, j_action) {
            if (action_run(config, j_action) != A_OK) {
              y_log_message(Y_LOG_LEVEL_ERROR, "script_run - Error executing action %d in script %s", index, script_name);
              res = A_ERROR;
            }
          }
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "script_run - Error getting action list");
          res = A_ERROR;
        }
      } else {
        res = A_ERROR;
      }
      json_decref(j_result);
    }
  }
  return res;
}

/**
 * 
 * Execute the specified action
 * 
 * action format:
 * 
 * {
 *   "submodule": "submodule_name",   // Required
 *   "element": "element_name"        // Required
 *   "command": "command"             // Required
 *   "parameters": {                  // Required for submodule benoic, depends on the element for submodule carleon
 *     "device": "device_name"        // Required for submodule benoic
 *     "element_type": "element_type" // Required for submodule benoic
 *     "service": "service_name"      // Required for carleon
 *     "param1": "value1",            // for a string value
 *     "param2": 2,                   // for an integer value
 *     "param3", 3.3                  // for a real value
 *   }
 * 
 */
int action_run(struct config_elements * config, json_t * j_action) {
  int res = A_OK;
  json_t * parameters, * device, * element_type, * element, * command, * j_device, * is_valid, * j_result;
  int i_element_type = BENOIC_ELEMENT_TYPE_NONE;
  struct _carleon_service * cur_service = NULL;
  char * str_heater_mode;
  json_int_t command_res;
  
  is_valid = is_action_valid(config, j_action, 0);
  if (is_valid != NULL && json_is_array(is_valid) && json_array_size(is_valid) > 0) {
    y_log_message(Y_LOG_LEVEL_ERROR, "error in action is_valid");
    res = A_ERROR;
  } else if (0 == o_strcmp(json_string_value(json_object_get(j_action, "submodule")), "benoic")) {
    parameters = json_object_get(j_action, "parameters");
    element = json_object_get(j_action, "element");
    command = json_object_get(j_action, "command");
    device = json_object_get(parameters, "device");
    element_type = json_object_get(parameters, "element_type");
    j_device = get_device(config->b_config, json_string_value(device));
    if (j_device != NULL) {
      if (0 == o_strcmp("switch", json_string_value(element_type))) {
        i_element_type = BENOIC_ELEMENT_TYPE_SWITCH;
      } else if (0 == o_strcmp("dimmer", json_string_value(element_type))) {
        i_element_type = BENOIC_ELEMENT_TYPE_DIMMER;
      } else if (0 == o_strcmp("heater", json_string_value(element_type))) {
        i_element_type = BENOIC_ELEMENT_TYPE_HEATER;
      }
      
      if (i_element_type == BENOIC_ELEMENT_TYPE_SWITCH && json_is_integer(command)) {
        command_res = set_switch(config->b_config, j_device, json_string_value(element), json_integer_value(command));
        if (command_res != B_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "error in benoic command_res");
          res = A_ERROR;
        }
      } else if (i_element_type == BENOIC_ELEMENT_TYPE_DIMMER && json_is_integer(command)) {
        j_result = set_dimmer(config->b_config, j_device, json_string_value(element), json_integer_value(command));
        command_res = json_integer_value(json_object_get(j_result, "result"));
        json_decref(j_result);
        if (command_res != B_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "error in benoic command_res");
          res = A_ERROR;
        }
      } else if (i_element_type == BENOIC_ELEMENT_TYPE_HEATER && json_is_integer(command)) {
        str_heater_mode = (char*)json_string_value(json_object_get(parameters, "mode"));
        command_res = set_heater(config->b_config, j_device, json_string_value(element), str_heater_mode, json_number_value(command));
        if (command_res != B_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "error in benoic command_res");
          res = A_ERROR;
        }
      }
    } else {
      res = A_ERROR;
    }
    json_decref(j_device);
  } else if (j_action != NULL && 0 == o_strcmp(json_string_value(json_object_get(j_action, "submodule")), "carleon")) {
    cur_service = get_service_from_name(config->c_config, json_string_value(json_object_get(json_object_get(j_action, "parameters"), "service")));
    if (cur_service != NULL) {
      j_result = service_exec(config->c_config, cur_service, json_string_value(json_object_get(j_action, "command")), json_string_value(json_object_get(j_action, "element")), json_object_get(j_action, "parameters"));
      if (j_result == NULL || json_integer_value(json_object_get(j_result, "result")) != WEBSERVICE_RESULT_OK) {
        y_log_message(Y_LOG_LEVEL_ERROR, "error in carleon command_res");
        res = A_ERROR;
      }
      json_decref(j_result);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "error in cur_service");
      res = A_ERROR;
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "error in submodule");
    res = A_ERROR;
  }
  json_decref(is_valid);
  return res;
}
