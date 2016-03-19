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

#include "angharad.h"

json_t * script_get(struct config_elements * config, const char * script_id) {
  json_t * j_query, * j_result, * j_script, * j_actions, * j_options, * to_return;
  int res;
  size_t index;
  
  j_query = json_pack("{sss[ssss]}", "table", ANGHARAD_TABLE_SCRIPT, "columns", "asc_name AS name", "asc_description AS description", "asc_actions", "asc_options");
  
  if (j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "script_get - Error allocating resources for j_query");
    return json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  }
  
  if (script_id != NULL) {
    json_object_set_new(j_query, "where", json_pack("{ss}", "asc_id", script_id));
  }
  
  res = h_select(config->conn, j_query, &j_result, NULL);
  json_decref(j_query);
  if (res == H_OK) {
    to_return = json_array();
    if (to_return == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "script_get - Error allocating resources for to_return");
      return json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
    }
    if (script_id == NULL) {
      json_array_foreach(j_result, index, j_script) {
        j_actions = json_loads(json_string_value(json_object_get(j_script, "asc_actions")), JSON_DECODE_ANY, NULL);
        json_object_del(j_script, "asc_actions");
        json_object_set_new(j_script, "actions", j_actions);
        j_options = json_loads(json_string_value(json_object_get(j_script, "asc_optons")), JSON_DECODE_ANY, NULL);
        json_object_del(j_script, "asc_optons");
        json_object_set_new(j_script, "options", j_options);
        json_array_append_new(to_return, json_copy(j_script));
      }
      json_decref(j_result);
      return json_pack("{siso}", "result", WEBSERVICE_RESULT_OK, "scripts", to_return);
    } else {
      if (json_array_size(j_result) == 0) {
        return json_pack("{si}", "result", WEBSERVICE_RESULT_NOT_FOUND);
      } else {
        j_script = json_copy(json_array_get(j_result, 0));
        json_decref(j_result);
        if (j_script == NULL) {
          y_log_message(Y_LOG_LEVEL_ERROR, "script_get - Error allocating resources for j_script");
          return json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
        }
        j_actions = json_loads(json_string_value(json_object_get(j_script, "asc_actions")), JSON_DECODE_ANY, NULL);
        json_object_del(j_script, "asc_actions");
        json_object_set_new(j_script, "actions", j_actions);
        j_options = json_loads(json_string_value(json_object_get(j_script, "asc_optons")), JSON_DECODE_ANY, NULL);
        json_object_del(j_script, "asc_optons");
        json_object_set_new(j_script, "options", j_actions);
        return json_pack("{siso}", "result", WEBSERVICE_RESULT_OK, "script", j_script);
      }
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "script_get - Error allocating resources for j_query");
    return json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  }
}

int script_add(struct config_elements * config, json_t * j_script) {
  json_t * j_query;
  int res;
  char * str_actions, * str_options;
  
  if (j_script == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "script_add - Error j_script is NULL");
    return WEBSERVICE_RESULT_ERROR;
  }
  
  str_actions = json_dumps(json_object_get(j_script, "actions"), JSON_COMPACT);
  str_options = json_dumps(json_object_get(j_script, "options"), JSON_COMPACT);
  j_query = json_pack("{sss[{ss}{ss}{ss}{ss}]}",
                      "table", ANGHARAD_TABLE_SCRIPT,
                      "values",
                        "asc_name", json_string_value(json_object_get(j_script, "name")),
                        "asc_description", json_string_value(json_object_get(j_script, "description")),
                        "asc_actions", str_actions,
                        "asc_options", str_options);

  if (j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "script_add - Error Allocating resources for j_query");
    return WEBSERVICE_RESULT_ERROR;
  }
  
  res = h_insert(config->conn, j_query, NULL);
  json_decref(j_query);
  if (res == H_OK) {
    return WEBSERVICE_RESULT_OK;
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "script_add - Error executing db query");
    return WEBSERVICE_RESULT_ERROR;
  }
}

int script_modify(struct config_elements * config, const char * script_id, json_t * j_script) {
  json_t * j_query, * cur_script;
  int res, res_cur_script;
  char * str_actions, * str_options;
  
  if (j_script == NULL || script_id == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "script_modify - Error j_script or script_id is NULL");
    return WEBSERVICE_RESULT_ERROR;
  }
  
  cur_script = script_get(config, script_id);
  res_cur_script = (cur_script != NULL?json_integer_value(json_object_get(cur_script, "result")):WEBSERVICE_RESULT_ERROR);
  json_decref(cur_script);
  if (res_cur_script == WEBSERVICE_RESULT_OK) {
    
    str_actions = json_dumps(json_object_get(j_script, "actions"), JSON_COMPACT);
    str_options = json_dumps(json_object_get(j_script, "options"), JSON_COMPACT);
    j_query = json_pack("{sss[{ss}{ss}{ss}{ss}]s{ss}}",
                        "table", ANGHARAD_TABLE_SCRIPT,
                        "set",
                          "asc_name", json_string_value(json_object_get(j_script, "name")),
                          "asc_description", json_string_value(json_object_get(j_script, "description")),
                          "asc_actions", str_actions,
                          "asc_options", str_options,
                        "where",
                          "asc_id", script_id);

    if (j_query == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "script_add - Error Allocating resources for j_query");
      return WEBSERVICE_RESULT_ERROR;
    }
    
    res = h_update(config->conn, j_query, NULL);
    json_decref(j_query);
    if (res == H_OK) {
      return WEBSERVICE_RESULT_OK;
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "script_add - Error executing db query");
      return WEBSERVICE_RESULT_ERROR;
    }
  } else if (res_cur_script == WEBSERVICE_RESULT_NOT_FOUND) {
    return WEBSERVICE_RESULT_NOT_FOUND;
  } else {
    return WEBSERVICE_RESULT_ERROR;
  }
}

int script_delete(struct config_elements * config, const char * script_id) {
  json_t * j_query, * cur_script;
  int res, res_cur_script;
  
  if (script_id == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "script_modify - Error script_id is NULL");
    return WEBSERVICE_RESULT_ERROR;
  }
  
  cur_script = script_get(config, script_id);
  res_cur_script = (cur_script != NULL?json_integer_value(json_object_get(cur_script, "result")):WEBSERVICE_RESULT_ERROR);
  json_decref(cur_script);
  if (res_cur_script == WEBSERVICE_RESULT_OK) {
    j_query = json_pack("{sss{ss}}",
                        "table", ANGHARAD_TABLE_SCRIPT,
                        "where",
                          "asc_id", script_id);
                        
    if (j_query == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "script_add - Error Allocating resources for j_query");
      return WEBSERVICE_RESULT_ERROR;
    }
    
    res = h_delete(config->conn, j_query, NULL);
    json_decref(j_query);
    if (res == H_OK) {
      return WEBSERVICE_RESULT_OK;
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "script_add - Error executing db query");
      return WEBSERVICE_RESULT_ERROR;
    }
  } else if (res_cur_script == WEBSERVICE_RESULT_NOT_FOUND) {
    return WEBSERVICE_RESULT_NOT_FOUND;
  } else {
    return WEBSERVICE_RESULT_ERROR;
  }
}

json_t * is_script_valid(struct config_elements * config, json_t * j_script) {
  json_t * j_result = json_array(), * j_element, * j_actions_valid, * j_options_valid;
  size_t index;
  
  if (j_result == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "is_script_valid - Error allocating resources for j_result");
    return NULL;
  }
  
  if (j_script == NULL || !json_is_object(j_script)) {
    json_array_append_new(j_result, json_pack("{ss}", "script", "script object is mandatory"));
  } else {
    j_element = json_object_get(j_script, "name");
    if (j_element == NULL || !json_is_string(j_element) || json_string_length(j_element) > 64) {
      json_array_append_new(j_result, json_pack("{ss}", "name", "name parameter is mandatory must ba a string, maximum 64 characters"));
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
  return j_script;
}

json_t * is_actions_valid(struct config_elements * config, json_t * j_action_list) {
  json_t * j_result = json_array(), * j_action, * is_valid;
  size_t index;
  
  if (j_result == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "is_actions_valid - Error allocating resources for j_result");
    return NULL;
  }
  
  if (json_is_array(j_action_list)) {
    json_array_foreach(j_action_list, index, j_action) {
      is_valid = is_action_valid(config, j_action);
      if (is_valid != NULL) {
        json_array_append_new(j_result, is_valid);
      }
    }
  } else {
    json_array_append_new(j_result, json_pack("{ss}", "actions", "actions is mandatory and must be an array of valid actions"));
  }
  
  return j_result;
}

/**
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
 *     "param1": "value1",
 *     "param2": 2,
 *     "param3", 3.3
 *   }
 * 
 */
json_t * is_action_valid(struct config_elements * config, json_t * j_action) {
  json_t * j_result = json_array(), * j_submodule, * j_parameters, * j_command, * j_element,     // Common parameters
          * j_device, * j_element_type, * j_mode,                                                // Benoic parameters
          * j_service, * j_carleon_service, * j_command_list, * j_cur_command, * j_element_list, // Carleon parameters
          * j_cur_element, * j_command_param_list, * j_command_param, * j_cur_param; 
  int i_element_type = BENOIC_ELEMENT_TYPE_NONE, found, command_found;
  struct _carleon_service * cur_service;
  size_t index;
  char * str_command, * str_param;

  if (j_result == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "is_action_valid - Error allocating resources for j_result");
    return NULL;
  }
  
  j_submodule = json_object_get(j_action, "submodule");
  if (j_submodule == NULL || !json_is_string(j_submodule)) {
    json_array_append_new(j_result, json_pack("{ss}", "action", "a valid submodule is mandatory"));
  } else {
    if (0 == strcmp("benoic", json_string_value(j_submodule))) {
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
            if (0 == strcmp("switch", json_string_value(j_element_type))) {
              i_element_type = BENOIC_ELEMENT_TYPE_SWITCH;
            } else if (0 == strcmp("dimmer", json_string_value(j_element_type))) {
              i_element_type = BENOIC_ELEMENT_TYPE_DIMMER;
            } else if (0 == strcmp("heater", json_string_value(j_element_type))) {
              i_element_type = BENOIC_ELEMENT_TYPE_HEATER;
            }
            
            if (i_element_type == BENOIC_ELEMENT_TYPE_NONE) {
              json_array_append_new(j_result, json_pack("{ss}", "action", "element_type invalid"));
            } else {
              j_element = get_element_data(config->b_config, j_device, i_element_type, json_string_value(json_object_get(j_action, "element")), 0);
              if (j_element == NULL) {
                json_array_append_new(j_result, json_pack("{ss}", "action", "element invalid"));
              } else {
                j_command = json_object_get(j_action, "command");
                if (j_command == NULL || !json_is_string(j_command)) {
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
                    if (j_mode != NULL && !json_is_string(j_mode) && 0 != strcmp("off", json_string_value(j_mode)) && 0 != strcmp("off", json_string_value(j_mode)) && 0 != strcmp("off", json_string_value(j_mode))) {
                      json_array_append_new(j_result, json_pack("{ss}", "action", "mode is invalid, must be 'off', 'auto', 'manual' or none"));
                    }
                    
                    if (!json_is_integer(j_command) || (json_integer_value(j_command) < 0 && json_integer_value(j_command) > 100)) {
                      json_array_append_new(j_result, json_pack("{ss}", "action", "command is invalid, must be between 0 and 100"));
                    }
                  }
                }
              }
              json_decref(j_element);
            }
          }
        }
        json_decref(j_device);
      }
    } else if (0 == strcmp("carleon", json_string_value(j_submodule))) {
      j_parameters = json_object_get(j_action, "parameters");
      if (j_parameters == NULL || !json_is_object(j_parameters)) {
        json_array_append_new(j_result, json_pack("{ss}", "action", "parameters is missing or not a json object"));
      } else {
        j_service = json_object_get(j_parameters, "service");
        if (j_service == NULL || !json_is_string(j_service)) {
          json_array_append_new(j_result, json_pack("{ss}", "action", "service is mandatory and must ba a string"));
        } else {
          j_carleon_service = service_get(config->c_config, json_string_value(j_service));
          if (j_carleon_service == NULL) {
            json_array_append_new(j_result, json_pack("{ss}", "action", "service unknown"));
          } else {
            cur_service = get_service_from_uid(config->c_config, json_string_value(json_object_get(j_carleon_service, "uid")));
            if (cur_service == NULL) {
              json_array_append_new(j_result, json_pack("{ss}", "action", "service error"));
            } else {
              j_command_list = cur_service->c_service_command_get_list(config->c_config);
              j_element_list = cur_service->c_service_element_get_list(config->c_config);
              
              if (j_command_list == NULL || !json_is_object(j_command_list) || json_integer_value(json_object_get(j_command_list, "result")) != WEBSERVICE_RESULT_OK || 
                  j_element_list == NULL || !json_is_object(j_element_list) || json_integer_value(json_object_get(j_element_list, "result")) != WEBSERVICE_RESULT_OK) {
                json_array_append_new(j_result, json_pack("{ss}", "action", "error getting command or element list"));
              } else {
                found = 0;
                json_array_foreach(json_object_get(j_command_list, "elements"), index, j_cur_element) {
                  if (json_is_string(j_cur_element)) {
                    // FIXME, j_command is no good
                    if (0 == strcmp(json_string_value(j_command), json_string_value(j_cur_element))) {
                      found = 1;
                    }
                  }
                }
                if (!found) {
                  json_array_append_new(j_result, json_pack("{ss}", "action", "element not found"));
                } else {
                  j_command = json_object_get(j_action, "command");
                  if (j_command == NULL || !json_is_string(j_command)) {
                    json_array_append_new(j_result, json_pack("{ss}", "action", "command is mandatory"));
                  } else {
                    command_found = 0;
                    json_object_foreach(json_object_get(j_command_list, "commands"), str_command, j_cur_command) {
                      if (0 == strcmp(str_command, json_string_value(j_command))) {
                        command_found = 1;
                        j_command_param_list = json_object_get(j_cur_command, "parameters");
                        if (j_command_param_list == NULL || !json_is_object(j_command_param_list)) {
                          json_array_append_new(j_result, json_pack("{ss}", "action", "error in command pattern"));
                        } else {
                          json_object_foreach(j_command_param_list, str_param, j_command_param) {
                            j_cur_param = json_object_get(json_object_get(j_action, "parameters"), str_param);
                            if (j_cur_param == NULL && json_object_get(j_command_param, "required") == json_true()) {
                              json_array_append_new(j_result, json_pack("{ss}", "action", "parameter is required"));
                            } else if (j_cur_param != NULL) {
                              if (0 == strcmp("string", json_string_value(json_object_get(j_command_param, "type")))) {
                                if (!json_is_string(j_cur_param)) {
                                  json_array_append_new(j_result, json_pack("{ss}", "action", "parameter must be a string"));
                                }
                              } else if (0 == strcmp("integer", json_string_value(json_object_get(j_command_param, "type")))) {
                                if (!json_is_integer(j_cur_param)) {
                                  json_array_append_new(j_result, json_pack("{ss}", "action", "parameter must be an integer"));
                                }
                              } else if (0 == strcmp("real", json_string_value(json_object_get(j_command_param, "type")))) {
                                if (!json_is_real(j_cur_param)) {
                                  json_array_append_new(j_result, json_pack("{ss}", "action", "parameter must be a real"));
                                }
                              } else {
                                json_array_append_new(j_result, json_pack("{ss}", "action", "unknown parameter type"));
                              }
                            }
                          }
                        }
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
