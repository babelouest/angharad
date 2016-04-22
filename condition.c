/**
 *
 * Angharad system
 *
 * Manage all subsystems and handles the house automation server:
 * - Benoic
 * - Gareth
 * - Carleon
 * 
 * conditions (schedulers and alerts) management functions
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

json_t * is_condition_list_valid(struct config_elements * config, json_t * condition_list) {
  json_t * j_return = json_array(), * condition, * valid;
  size_t index;
  
  if (j_return == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "is_condition_list_valid - Error allocating resources for j_return");
    return NULL;
  }
  
  if (condition_list != NULL && !json_is_array(condition_list)) {
    json_array_append_new(j_return, json_pack("{ss}", "conditions", "conditions attribute must be an array of objects"));
  } else if (condition_list != NULL) {
    json_array_foreach(condition_list, index, condition) {
      valid = is_condition_valid(config, condition);
      
      if (valid != NULL) {
        json_array_append_new(j_return, valid);
      }
    }
  }
  return j_return;
}

/**
 * 
 * condition format:
 * action (cf script) and
 * {
 *   field: <string> optional
 *   condition: <string> mandatory ("==", "!=", "<", "<=", ">", ">=", "contains", "not contains")
 *   value: <string|number|boolean> mandatory
 * }
 * 
 */
json_t * is_condition_valid(struct config_elements * config, json_t * condition) {
  json_t * j_valid = is_action_valid(config, condition);
  
  if (j_valid == NULL) {
    j_valid = json_array();
  }
  
  if (j_valid == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "is_condition_list_valid - Error allocating resources for j_valid");
    return NULL;
  }
  
  if (json_object_get(condition, "field") != NULL && !json_is_string(json_object_get(condition, "field"))) {
    json_array_append_new(j_valid, json_pack("{ss}", "field", "field must be a string"));
  }
  
  if (json_object_get(condition, "condition") == NULL || !json_is_string(json_object_get(condition, "condition"))) {
    json_array_append_new(j_valid, json_pack("{ss}", "condition", "condition is mandatory and must be a string"));
  } else {
    if (0 != nstrcmp(json_string_value(json_object_get(condition, "condition")), "==")
        && 0 != nstrcmp(json_string_value(json_object_get(condition, "condition")), "!=")
        && 0 != nstrcmp(json_string_value(json_object_get(condition, "condition")), ">")
        && 0 != nstrcmp(json_string_value(json_object_get(condition, "condition")), ">=")
        && 0 != nstrcmp(json_string_value(json_object_get(condition, "condition")), "<")
        && 0 != nstrcmp(json_string_value(json_object_get(condition, "condition")), "<=")
        && 0 != nstrcmp(json_string_value(json_object_get(condition, "condition")), "contains")
        && 0 != nstrcmp(json_string_value(json_object_get(condition, "condition")), "not contains")) {
      json_array_append_new(j_valid, json_string("condition does not exist, please choose between '==', '!=', '<', '<=', '>', '>=', 'contains' and 'not contains'"));
    }
  }
  
  if (json_object_get(condition, "value") == NULL || (!json_is_string(json_object_get(condition, "value")) && !json_is_integer(json_object_get(condition, "value")) && !json_is_real(json_object_get(condition, "value")) && !json_is_boolean(json_object_get(condition, "value")))) {
    json_array_append_new(j_valid, json_pack("{ss}", "value", "value is mandatory and must be a string, a number or a boolean"));
  }
  
  if (json_array_size(j_valid) == 0) {
    json_decref(j_valid);
    return NULL;
  } else {
    return j_valid;
  }
}

/**
 * check if all conditions are verified, return true if so, false otherwise
 */
int condition_list_check(struct config_elements * config, json_t * condition_list) {
  json_t * condition;
  size_t index;
  if (condition_list == NULL || json_array_size(condition_list) == 0) {
    return 1;
  } else {
    json_array_foreach(condition_list, index, condition) {
      if (!condition_check(config, condition)) {
        char * str_condition = json_dumps(condition, JSON_ENCODE_ANY);
        y_log_message(Y_LOG_LEVEL_DEBUG, "condition not valid: %s", str_condition);
        free(str_condition);
        return 0;
      }
    }
    return 1;
  }
}

/**
 * 
 * action format:
 * 
 * condition format:
 * action (cf script) and
 * {
 *   field: <string> optional
 *   condition: <string> mandatory ("==", "!=", "<", "<=", ">", ">=", "contains", "not contains")
 *   value: <string|number|boolean> mandatory
 * }
 * 
 */
int condition_check(struct config_elements * config, json_t * j_condition) {
  int res = ANGHARAD_RESULT_TRUE;
  json_t * parameters, * device, * element_type, * element, * command, * j_device, * is_valid, * j_result = NULL, * j_value;
  int i_element_type = BENOIC_ELEMENT_TYPE_NONE;
  struct _carleon_service * cur_service = NULL;
  
  is_valid = is_condition_valid(config, j_condition);
  if (is_valid != NULL && json_is_array(is_valid) && json_array_size(is_valid) > 0) {
    y_log_message(Y_LOG_LEVEL_DEBUG, "error in is_valid");
    res = ANGHARAD_RESULT_ERROR;
  } else if (0 == nstrcmp(json_string_value(json_object_get(j_condition, "submodule")), "benoic")) {
    parameters = json_object_get(j_condition, "parameters");
    element = json_object_get(j_condition, "element");
    command = json_object_get(j_condition, "command");
    device = json_object_get(parameters, "device");
    element_type = json_object_get(parameters, "element_type");
    j_device = get_device(config->b_config, json_string_value(device));
    if (j_device != NULL) {
      if (0 == nstrcmp("switch", json_string_value(element_type))) {
        i_element_type = BENOIC_ELEMENT_TYPE_SWITCH;
      } else if (0 == nstrcmp("dimmer", json_string_value(element_type))) {
        i_element_type = BENOIC_ELEMENT_TYPE_DIMMER;
      } else if (0 == nstrcmp("heater", json_string_value(element_type))) {
        i_element_type = BENOIC_ELEMENT_TYPE_HEATER;
      }
      
      if (i_element_type == BENOIC_ELEMENT_TYPE_SWITCH && json_is_integer(command)) {
        j_result = get_switch(config->b_config, j_device, json_string_value(element));
      } else if (i_element_type == BENOIC_ELEMENT_TYPE_DIMMER && json_is_integer(command)) {
        j_result = get_dimmer(config->b_config, j_device, json_string_value(element));
      } else if (i_element_type == BENOIC_ELEMENT_TYPE_HEATER && json_is_integer(command)) {
        j_result = get_heater(config->b_config, j_device, json_string_value(element));
      }
      
      if (j_result != NULL) {
        if (json_object_get(j_condition, "field") != NULL) {
          j_value = json_object_get(j_result, json_string_value(json_object_get(j_condition, "field")));
        } else {
          j_value = json_object_get(j_result, "value");
        }
        
        res = compare_values(j_value, json_object_get(j_condition, "value"), json_string_value(json_object_get(j_condition, "condition")));
      } else {
        y_log_message(Y_LOG_LEVEL_DEBUG, "error in getting benoic element value");
        res = ANGHARAD_RESULT_ERROR;
      }
      json_decref(j_result);
    } else {
      res = ANGHARAD_RESULT_ERROR;
    }
    json_decref(j_device);
  } else if (j_condition != NULL && 0 == nstrcmp(json_string_value(json_object_get(j_condition, "submodule")), "carleon")) {
    cur_service = get_service_from_uid(config->c_config, json_string_value(json_object_get(json_object_get(j_condition, "parameters"), "service")));
    if (cur_service != NULL) {
      j_result = service_exec(config->c_config, cur_service, json_string_value(json_object_get(j_condition, "command")), json_string_value(json_object_get(j_condition, "element")), json_object_get(j_condition, "parameters"));
      if (j_result == NULL || json_integer_value(json_object_get(j_result, "result")) != WEBSERVICE_RESULT_OK) {
        y_log_message(Y_LOG_LEVEL_DEBUG, "error in carleon command_res");
        res = ANGHARAD_RESULT_ERROR;
      } else {
        if (json_object_get(j_condition, "field") != NULL) {
          j_value = json_object_get(j_result, json_string_value(json_object_get(j_condition, "field")));
        } else {
          j_value = json_object_get(j_result, "value");
        }
        
        res = compare_values(j_value, json_object_get(j_condition, "value"), json_string_value(json_object_get(j_condition, "condition")));
      }
      json_decref(j_result);
    } else {
      y_log_message(Y_LOG_LEVEL_DEBUG, "error in cur_service");
      res = ANGHARAD_RESULT_ERROR;
    }
  } else {
    y_log_message(Y_LOG_LEVEL_DEBUG, "error in submodule");
    res = ANGHARAD_RESULT_ERROR;
  }
  json_decref(is_valid);
  return res;
}

int compare_values(json_t * j_value1, json_t * j_value2, const char * operator) {
  if (j_value1 == NULL || j_value2 == NULL || operator == NULL) {
    y_log_message(Y_LOG_LEVEL_DEBUG, "compare_values - error input parameters");
    return ANGHARAD_RESULT_ERROR;
  } else if (0 == nstrcmp(operator, "==")) {
    return json_equal(j_value1, j_value2)?ANGHARAD_RESULT_TRUE:ANGHARAD_RESULT_FALSE;
  } else if (0 == nstrcmp(operator, "==")) {
    return json_equal(j_value1, j_value2)?ANGHARAD_RESULT_FALSE:ANGHARAD_RESULT_TRUE;
  } else if (0 == nstrcmp(operator, ">")) {
    if (json_is_integer(j_value1) && json_is_integer(j_value2)) {
      return json_integer_value(j_value1)>json_integer_value(j_value2)?ANGHARAD_RESULT_TRUE:ANGHARAD_RESULT_FALSE;
    } else if (json_is_real(j_value1) && json_is_real(j_value2)) {
      return json_real_value(j_value1)>json_real_value(j_value2)?ANGHARAD_RESULT_TRUE:ANGHARAD_RESULT_FALSE;
    } else {
      y_log_message(Y_LOG_LEVEL_DEBUG, "compare_values - error values not the same type");
      return ANGHARAD_RESULT_ERROR;
    }
  } else if (0 == nstrcmp(operator, ">=")) {
    if (json_is_integer(j_value1) && json_is_integer(j_value2)) {
      return json_integer_value(j_value1)>=json_integer_value(j_value2)?ANGHARAD_RESULT_TRUE:ANGHARAD_RESULT_FALSE;
    } else if (json_is_real(j_value1) && json_is_real(j_value2)) {
      return json_real_value(j_value1)>json_real_value(j_value2)?ANGHARAD_RESULT_TRUE:ANGHARAD_RESULT_FALSE;
    } else {
      y_log_message(Y_LOG_LEVEL_DEBUG, "compare_values - error values not the same type");
      return ANGHARAD_RESULT_ERROR;
    }
  } else if (0 == nstrcmp(operator, "<")) {
    if (json_is_integer(j_value1) && json_is_integer(j_value2)) {
      return json_integer_value(j_value1)<json_integer_value(j_value2)?ANGHARAD_RESULT_TRUE:ANGHARAD_RESULT_FALSE;
    } else if (json_is_real(j_value1) && json_is_real(j_value2)) {
      return json_real_value(j_value1)>json_real_value(j_value2)?ANGHARAD_RESULT_TRUE:ANGHARAD_RESULT_FALSE;
    } else {
      y_log_message(Y_LOG_LEVEL_DEBUG, "compare_values - error values not the same type");
      return ANGHARAD_RESULT_ERROR;
    }
  } else if (0 == nstrcmp(operator, "<=")) {
    if (json_is_integer(j_value1) && json_is_integer(j_value2)) {
      return json_integer_value(j_value1)<=json_integer_value(j_value2)?ANGHARAD_RESULT_TRUE:ANGHARAD_RESULT_FALSE;
    } else if (json_is_real(j_value1) && json_is_real(j_value2)) {
      return json_real_value(j_value1)>json_real_value(j_value2)?ANGHARAD_RESULT_TRUE:ANGHARAD_RESULT_FALSE;
    } else {
      y_log_message(Y_LOG_LEVEL_DEBUG, "compare_values - error values not the same type");
      return ANGHARAD_RESULT_ERROR;
    }
  } else if (0 == nstrcmp(operator, "contains")) {
    if (!json_is_string(j_value1) || !json_is_string(j_value2)) {
      y_log_message(Y_LOG_LEVEL_DEBUG, "compare_values - error values not string type with a contains operator");
      return ANGHARAD_RESULT_ERROR;
    } else {
      return strstr(json_string_value(j_value1), json_string_value(j_value2))!=NULL?ANGHARAD_RESULT_TRUE:ANGHARAD_RESULT_FALSE;
    }
  } else if (0 == nstrcmp(operator, "not contains")) {
    if (!json_is_string(j_value1) || !json_is_string(j_value2)) {
      y_log_message(Y_LOG_LEVEL_DEBUG, "compare_values - error values not string type with a not contains operator");
      return ANGHARAD_RESULT_ERROR;
    } else {
      return strstr(json_string_value(j_value1), json_string_value(j_value2))==NULL?ANGHARAD_RESULT_TRUE:ANGHARAD_RESULT_FALSE;
    }
  } else {
    y_log_message(Y_LOG_LEVEL_DEBUG, "compare_values - error operator not found");
    return ANGHARAD_RESULT_ERROR;
  }
}

json_t * is_script_list_valid(struct config_elements * config, json_t * script_list) {
  json_t * j_return = json_array(), * script, * j_script;
  size_t index;
  
  if(j_return == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "is_script_list_valid - Error allocating resources for j_return");
    return NULL;
  }
  
  if (script_list == NULL || !json_is_array(script_list)) {
    json_array_append_new(j_return, json_pack("{ss}", "script_list", "script list is mandatory and must be an array"));
  } else if (json_array_size(script_list) == 0) {
    json_array_append_new(j_return, json_pack("{ss}", "script_list", "script list must have at least one script"));
  } else {
    json_array_foreach(script_list, index, script) {
      if (!json_is_object(script)) {
        json_array_append_new(j_return, json_pack("{ss}", "script", "script must be an object"));
      } else {
        if (json_object_get(script, "name") == NULL || !json_is_string(json_object_get(script, "name"))) {
          json_array_append_new(j_return, json_pack("{ss}", "script", "name attribute is mandatory and must be a string"));
        } else {
          j_script = script_get(config, json_string_value(json_object_get(script, "name")));
          if (json_integer_value(json_object_get(j_script, "result")) != ANGHARAD_RESULT_OK) {
            json_array_append_new(j_return, json_pack("{ss}", "script", "script does not exist"));
          }
          json_decref(j_script);
        }
        
        if (json_object_get(script, "enabled") == NULL || !json_is_boolean(json_object_get(script, "enabled"))) {
          json_array_append_new(j_return, json_pack("{ss}", "script", "enabled attribute is mandatory and must be a boolean"));
        }
      }
    }
  }
  
  return j_return;
}
