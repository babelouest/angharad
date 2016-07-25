/**
 *
 * Angharad system
 *
 * Manage all subsystems and handles the house automation server:
 * - Benoic
 * - Gareth
 * - Carleon
 * 
 * Triggers management functions
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

int alert_received(struct config_elements * config, const char * submodule_name, const char * source, const char * element, const char * message) {
  json_t * j_query, * j_result, * cur_trigger, * j_result_trigger, * j_trigger, * script;
  int res, message_match, message_match_check = A_ERROR_FALSE;
  size_t index, index_sc;
  char * str_message_text;
  json_t * j_message;

  
  j_query = json_pack("{sss[s]s{sissssss}}",
                      "table",
                      ANGHARAD_TABLE_TRIGGER,
                      "columns",
                        "at_name",
                      "where",
                        "at_enabled",
                        1,
                        "at_submodule",
                        submodule_name,
                        "at_source",
                        source,
                        "at_element",
                        element);
  if (j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "alert_received - Error allocating resources for j_query");
    return A_ERROR_MEMORY;
  }
  
  res = h_select(config->conn, j_query, &j_result, NULL);
  json_decref(j_query);
  if (res == H_OK) {
    // Send a message via gareth submodule
    str_message_text = msprintf("Receiving trigger %s - %s - %s - %s", submodule_name, source, element, message);
    j_message = json_pack("{sssssss[s]}", "priority", "MEDIUM", "source", source, "text", str_message_text, "tags", "trigger");
    add_message(config->conn, j_message);
    json_decref(j_message);
    free(str_message_text);
    
    json_array_foreach (j_result, index, cur_trigger) {
      j_result_trigger = trigger_get(config, json_string_value(json_object_get(cur_trigger, "at_name")));
      
      if (j_result_trigger != NULL && json_integer_value(json_object_get(j_result_trigger, "result")) == A_OK) {
        j_trigger = json_object_get(j_result_trigger, "trigger");
        
        if (j_trigger != NULL) {
          // First, check message match
          message_match = json_integer_value(json_object_get(j_trigger, "at_message_match"));
          
          switch (message_match) {
            case TRIGGER_MESSAGE_MATCH_NONE:
              message_match_check = A_ERROR_TRUE;
              break;
            case TRIGGER_MESSAGE_MATCH_EQUAL:
              message_match_check = (0==nstrcmp(message, json_string_value(json_object_get(j_trigger, "message"))))?A_ERROR_TRUE:A_ERROR_FALSE;
              break;
            case TRIGGER_MESSAGE_MATCH_DIFFERENT:
              message_match_check = (0!=nstrcmp(message, json_string_value(json_object_get(j_trigger, "message"))))?A_ERROR_TRUE:A_ERROR_FALSE;
              break;
            case TRIGGER_MESSAGE_MATCH_CONTAINS:
              message_match_check = (NULL!=nstrstr(message, json_string_value(json_object_get(j_trigger, "message"))))?A_ERROR_TRUE:A_ERROR_FALSE;
              break;
            case TRIGGER_MESSAGE_MATCH_NOT_CONTAINS:
              message_match_check = (NULL==nstrstr(message, json_string_value(json_object_get(j_trigger, "message"))))?A_ERROR_TRUE:A_ERROR_FALSE;
              break;
            case TRIGGER_MESSAGE_MATCH_EMPTY:
              message_match_check = (nstrlen(message)==0)?A_ERROR_TRUE:A_ERROR_FALSE;
              break;
            case TRIGGER_MESSAGE_MATCH_NOT_EMPTY:
              message_match_check = (nstrlen(message)>0)?A_ERROR_TRUE:A_ERROR_FALSE;
              break;
            default:
              message_match_check = A_ERROR_FALSE;
              break;
          }
          
          // If there's a message match, check conditions
          if (message_match_check == A_ERROR_TRUE) {
            if (condition_list_check(config, json_object_get(j_trigger, "conditions"))) {
              // If all conditions are checked, run scripts
              json_array_foreach(json_object_get(j_trigger, "scripts"), index_sc, script) {
                if (json_object_get(script, "enabled") == json_true() && json_is_string(json_object_get(script, "name"))) {
                  y_log_message(Y_LOG_LEVEL_INFO, "alert_received - run script %s", json_string_value(json_object_get(script, "name")));
                  script_run(config, json_string_value(json_object_get(script, "name")));
                }
              }
            }
          }
        }
      }
      json_decref(j_result_trigger);
    }
    json_decref(j_result);
    
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "alert_received - Error executing j_query");
    return A_ERROR_DB;
  }
  return A_OK;
}

json_t * trigger_get(struct config_elements * config, const char * trigger_name) {
  json_t * j_query, * j_result, * j_trigger, * j_options, * j_conditions, * j_bool, * to_return, * script_list;
  int res;
  size_t index;
  
  j_query = json_pack("{sss[ssssssssss]}",
                        "table",
                          ANGHARAD_TABLE_TRIGGER,
                        "columns",
                          "at_name AS name",
                          "at_description AS description",
                          "at_enabled",
                          "at_options",
                          "at_conditions",
                          "at_submodule AS submodule",
                          "at_source AS source",
                          "at_element AS element",
                          "at_message AS message",
                          "at_message_match AS message_match");
  
  if (j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "trigger_get - Error allocating resources for j_query");
    return json_pack("{si}", "result", A_ERROR_MEMORY);
  }
  
  if (trigger_name != NULL) {
    json_object_set_new(j_query, "where", json_pack("{ss}", "at_name", trigger_name));
  }
  
  res = h_select(config->conn, j_query, &j_result, NULL);
  json_decref(j_query);
  if (res == H_OK) {
    if (trigger_name == NULL) {
      to_return = json_array();
      if (to_return == NULL) {
        y_log_message(Y_LOG_LEVEL_ERROR, "trigger_get - Error allocating resources for to_return");
        return json_pack("{si}", "result", A_ERROR_MEMORY);
      }
      json_array_foreach(j_result, index, j_trigger) {
        j_bool = json_object_get(j_trigger, "at_enabled");
        json_object_set_new(j_trigger, "enabled", json_integer_value(j_bool)==1?json_true():json_false());
        json_object_del(j_trigger, "at_enabled");
        j_options = json_loads(json_string_value(json_object_get(j_trigger, "at_options")), JSON_DECODE_ANY, NULL);
        json_object_del(j_trigger, "at_options");
        json_object_set_new(j_trigger, "options", j_options);
        j_conditions = json_loads(json_string_value(json_object_get(j_trigger, "at_conditions")), JSON_DECODE_ANY, NULL);
        json_object_del(j_trigger, "at_conditions");
        json_object_set_new(j_trigger, "conditions", j_conditions);
        
        script_list = trigger_get_script_list(config, json_string_value(json_object_get(j_trigger, "name")));
        if (json_integer_value(json_object_get(script_list, "result")) == A_OK) {
          json_object_set_new(j_trigger, "scripts", json_copy(json_object_get(script_list, "scripts")));
        } else {
          json_object_set_new(j_trigger, "scripts", json_array());
        }
        json_decref(script_list);
        
        json_array_append_new(to_return, json_copy(j_trigger));
      }
      json_decref(j_result);
      return json_pack("{siso}", "result", A_OK, "triggers", to_return);
    } else {
      if (json_array_size(j_result) == 0) {
        json_decref(j_result);
        return json_pack("{si}", "result", A_ERROR_NOT_FOUND);
      } else {
        j_trigger = json_copy(json_array_get(j_result, 0));
        json_decref(j_result);
        if (j_trigger == NULL) {
          y_log_message(Y_LOG_LEVEL_ERROR, "trigger_get - Error allocating resources for j_trigger");
          return json_pack("{si}", "result", A_ERROR_MEMORY);
        }
        j_bool = json_object_get(j_trigger, "at_enabled");
        json_object_set_new(j_trigger, "enabled", json_integer_value(j_bool)?json_true():json_false());
        json_object_del(j_trigger, "at_enabled");
        j_options = json_loads(json_string_value(json_object_get(j_trigger, "at_options")), JSON_DECODE_ANY, NULL);
        json_object_del(j_trigger, "at_options");
        json_object_set_new(j_trigger, "options", j_options);
        j_conditions = json_loads(json_string_value(json_object_get(j_trigger, "at_conditions")), JSON_DECODE_ANY, NULL);
        json_object_del(j_trigger, "at_conditions");
        json_object_set_new(j_trigger, "conditions", j_conditions);
        
        script_list = trigger_get_script_list(config, json_string_value(json_object_get(j_trigger, "name")));
        if (json_integer_value(json_object_get(script_list, "result")) == A_OK) {
          json_object_set_new(j_trigger, "scripts", json_copy(json_object_get(script_list, "scripts")));
        } else {
          json_object_set_new(j_trigger, "scripts", json_array());
        }
        json_decref(script_list);
        
        return json_pack("{siso}", "result", A_OK, "trigger", j_trigger);
      }
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "trigger_get - Error allocating resources for j_query");
    return json_pack("{si}", "result", A_ERROR_MEMORY);
  }
}

int trigger_enable(struct config_elements * config, json_t * j_trigger, int enabled) {
	if (j_trigger != NULL) {
		json_object_set(j_trigger, "enabled", enabled?json_true():json_false());
		return trigger_modify(config, json_string_value(json_object_get(j_trigger, "name")), j_trigger);
	} else {
		return A_ERROR_PARAM;
	}
}

int trigger_add(struct config_elements * config, json_t * j_trigger) {
  json_t * j_query;
  int res;
  char * str_options, * str_conditions;
  
  if (j_trigger == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "trigger_add - Error j_trigger is NULL");
    return A_ERROR_PARAM;
  }
  
  if (json_object_get(j_trigger, "options") != NULL) {
		str_options = json_dumps(json_object_get(j_trigger, "options"), JSON_COMPACT);
	} else {
		str_options = strdup("");
	}
	if (json_object_get(j_trigger, "conditions") != NULL) {
		str_conditions = json_dumps(json_object_get(j_trigger, "conditions"), JSON_COMPACT);
	} else {
		str_conditions = strdup("");
	}
  j_query = json_pack("{sss[{sssssisssssssssssssI}]}",
                      "table", ANGHARAD_TABLE_TRIGGER,
                      "values",
                        "at_name", json_string_value(json_object_get(j_trigger, "name")),
                        "at_description", json_string_value(json_object_get(j_trigger, "description")),
                        "at_enabled", json_object_get(j_trigger, "enabled")==json_false()?0:1,
                        "at_options", str_options, 
                        "at_conditions", str_conditions, 
                        "at_submodule", json_string_value(json_object_get(j_trigger, "submodule")),
                        "at_source", json_string_value(json_object_get(j_trigger, "source")),
                        "at_element", json_string_value(json_object_get(j_trigger, "element")),
                        "at_message", json_object_get(j_trigger, "message")!=NULL?json_string_value(json_object_get(j_trigger, "message")):"",
                        "at_message_match", json_object_get(j_trigger, "message_match")!=NULL?json_integer_value(json_object_get(j_trigger, "message_match")):0);
  free(str_options);
  free(str_conditions);
  
  if (j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "trigger_add - Error Allocating resources for j_query");
    return A_ERROR_MEMORY;
  }
  
  res = h_insert(config->conn, j_query, NULL);
  json_decref(j_query);
  if (res == H_OK) {
    return trigger_set_script_list(config, json_string_value(json_object_get(j_trigger, "name")), json_object_get(j_trigger, "scripts"));
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "trigger_add - Error executing db query");
    return A_ERROR_DB;
  }
}

/**
 * 
 */
json_t * is_trigger_valid(struct config_elements * config, json_t * j_trigger, const int update) {
  json_t * j_result = json_array(), * j_element, * j_options_valid, * j_conditions_valid, * j_scripts_valid;
  size_t index;
  
  if (j_result == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "is_trigger_valid - Error allocating resources for j_result");
    return NULL;
  }
  
  if (j_trigger == NULL || !json_is_object(j_trigger)) {
    json_array_append_new(j_result, json_pack("{ss}", "trigger", "trigger object is mandatory"));
  } else {
    if (!update) {
      j_element = json_object_get(j_trigger, "name");
      if (j_element == NULL || !json_is_string(j_element) || json_string_length(j_element) > 64) {
        json_array_append_new(j_result, json_pack("{ss}", "name", "name parameter is mandatory must ba a string, maximum 64 characters"));
      }
    }

    j_element = json_object_get(j_trigger, "description");
    if (j_element != NULL && (!json_is_string(j_element) || json_string_length(j_element) > 128)) {
      json_array_append_new(j_result, json_pack("{ss}", "description", "description parameter must ba a string, maximum 128 characters"));
    }
    
    j_element = json_object_get(j_trigger, "enabled");
    if (j_element != NULL && !json_is_boolean(j_element)) {
      json_array_append_new(j_result, json_pack("{ss}", "enabled", "enabled parameter must ba a boolean"));
    }
    
    j_element = json_object_get(j_trigger, "submodule");
    if (j_element == NULL || !json_is_string(j_element) || json_string_length(j_element) > 64) {
      json_array_append_new(j_result, json_pack("{ss}", "submodule", "submodule parameter is mandatory must ba a string, maximum 64 characters"));
    }
    
    j_element = json_object_get(j_trigger, "source");
    if (j_element == NULL || !json_is_string(j_element) || json_string_length(j_element) > 64) {
      json_array_append_new(j_result, json_pack("{ss}", "source", "source parameter is mandatory must ba a string, maximum 64 characters"));
    }
    
    j_element = json_object_get(j_trigger, "element");
    if (j_element == NULL || !json_is_string(j_element) || json_string_length(j_element) > 64) {
      json_array_append_new(j_result, json_pack("{ss}", "element", "element parameter is mandatory must ba a string, maximum 64 characters"));
    }
    
    j_element = json_object_get(j_trigger, "message");
    if (j_element != NULL && (!json_is_string(j_element) || json_string_length(j_element) > 64)) {
      json_array_append_new(j_result, json_pack("{ss}", "message", "message parameter must ba a string, maximum 64 characters"));
    }
    
    j_element = json_object_get(j_trigger, "message_match");
    if (j_element != NULL && (!json_is_integer(j_element) || json_integer_value(j_element) < 0 || json_integer_value(j_element) > 5)) {
      json_array_append_new(j_result, json_pack("{ss}", "message_match", "message_match parameter must ba a string, maximum 64 characters"));
    }
    
    if (json_object_get(j_trigger, "options") != NULL) {
      j_options_valid = is_option_valid(json_object_get(j_trigger, "options"), BENOIC_ELEMENT_TYPE_NONE);
      if (j_options_valid != NULL && json_array_size(j_options_valid) > 0) {
        json_array_foreach(j_options_valid, index, j_element) {
          json_array_append_new(j_result, json_copy(j_element));
        }
        json_decref(j_options_valid);
      } else if(j_options_valid != NULL) {
        json_decref(j_options_valid);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "is_trigger_valid - Error, is_options_valid returned NULL");
        json_decref(j_result);
        return NULL;
      }
    }

    j_conditions_valid = is_condition_list_valid(config, json_object_get(j_trigger, "conditions"));
    if (j_conditions_valid != NULL && json_is_array(j_conditions_valid) && json_array_size(j_conditions_valid) > 0) {
      json_array_foreach(j_conditions_valid, index, j_element) {
        json_array_append_new(j_result, json_copy(j_element));
      }
      json_decref(j_conditions_valid);
    } else if(j_conditions_valid != NULL) {
      json_decref(j_conditions_valid);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "is_scheduler_valid - Error, is_condition_list_valid returned NULL");
      json_decref(j_result);
      return NULL;
    }
    
    j_scripts_valid = is_script_list_valid(config, json_object_get(j_trigger, "scripts"));
    if (j_scripts_valid != NULL && json_is_array(j_scripts_valid) && json_array_size(j_scripts_valid) > 0) {
      json_array_foreach(j_scripts_valid, index, j_element) {
        json_array_append_new(j_result, json_copy(j_element));
      }
      json_decref(j_scripts_valid);
    } else if(j_scripts_valid != NULL) {
      json_decref(j_scripts_valid);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "is_scheduler_valid - Error, is_script_list_valid returned NULL");
      json_decref(j_result);
      return NULL;
    }
  }
  return j_result;
}

int trigger_modify(struct config_elements * config, const char * trigger_name, json_t * j_trigger) {
  json_t * j_query, * cur_trigger;
  int res, res_cur_trigger;
  char * str_options, * str_conditions;
  
  if (j_trigger == NULL || trigger_name == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "trigger_modify - Error j_trigger or trigger_name is NULL");
    return A_ERROR_PARAM;
  }
  
  cur_trigger = trigger_get(config, trigger_name);
  res_cur_trigger = (cur_trigger != NULL?json_integer_value(json_object_get(cur_trigger, "result")):A_ERROR);
  json_decref(cur_trigger);
  if (res_cur_trigger == A_OK) {
		if (json_object_get(j_trigger, "options") != NULL) {
			str_options = json_dumps(json_object_get(j_trigger, "options"), JSON_COMPACT);
		} else {
			str_options = strdup("");
		}
		if (json_object_get(j_trigger, "conditions") != NULL) {
			str_conditions = json_dumps(json_object_get(j_trigger, "conditions"), JSON_COMPACT);
		} else {
			str_conditions = strdup("");
		}
    j_query = json_pack("{sss{sssisssssssssssssI}s{ss}}",
                        "table", ANGHARAD_TABLE_TRIGGER,
                        "set",
                          "at_description", json_string_value(json_object_get(j_trigger, "description")),
                          "at_enabled", json_object_get(j_trigger, "enabled")==json_false()?0:1,
                          "at_options", str_options,
                          "at_conditions", str_conditions,
                          "at_submodule", json_string_value(json_object_get(j_trigger, "submodule")),
                          "at_source", json_string_value(json_object_get(j_trigger, "source")),
                          "at_element", json_string_value(json_object_get(j_trigger, "element")),
                          "at_message", json_object_get(j_trigger, "message")!=NULL?json_string_value(json_object_get(j_trigger, "message")):"",
                          "at_message_match", json_object_get(j_trigger, "message_match")!=NULL?json_integer_value(json_object_get(j_trigger, "message_match")):0,
                        "where",
                          "at_name", trigger_name);
    free(str_options);
    free(str_conditions);
    
    if (j_query == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "trigger_modify - Error Allocating resources for j_query");
      return A_ERROR_MEMORY;
    }
    
    res = h_update(config->conn, j_query, NULL);
    json_decref(j_query);
    if (res == H_OK) {
      return trigger_set_script_list(config, json_string_value(json_object_get(j_trigger, "name")), json_object_get(j_trigger, "scripts"));
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "trigger_modify - Error executing db query");
      return A_ERROR_DB;
    }
  } else if (res_cur_trigger == A_ERROR_NOT_FOUND) {
    return A_ERROR_NOT_FOUND;
  } else {
    return A_ERROR;
  }
}

int trigger_delete(struct config_elements * config, const char * trigger_name) {
  json_t * j_query, * cur_trigger;
  int res, res_cur_trigger;
  
  if (trigger_name == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "trigger_modify - Error trigger_name is NULL");
    return A_ERROR_PARAM;
  }
  
  cur_trigger = trigger_get(config, trigger_name);
  res_cur_trigger = (cur_trigger != NULL?json_integer_value(json_object_get(cur_trigger, "result")):A_ERROR);
  json_decref(cur_trigger);
  if (res_cur_trigger == A_OK) {
    j_query = json_pack("{sss{ss}}",
                        "table", ANGHARAD_TABLE_TRIGGER,
                        "where",
                          "at_name", trigger_name);
    
    if (j_query == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "trigger_delete - Error Allocating resources for j_query");
      return A_ERROR_MEMORY;
    }
    
    res = h_delete(config->conn, j_query, NULL);
    json_decref(j_query);
    if (res == H_OK) {
      return A_OK;
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "trigger_delete - Error executing db query");
      return A_ERROR_DB;
    }
  } else if (res_cur_trigger == A_ERROR_NOT_FOUND) {
    return A_ERROR_NOT_FOUND;
  } else {
    return A_ERROR;
  }
}

int trigger_add_tag(struct config_elements * config, const char * trigger_name, const char * tag) {
  json_t * j_result = trigger_get(config, trigger_name), * j_trigger, * j_tags;
  int res;
  
  if (j_result == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "trigger_add_tag - Error getting trigger");
    return A_ERROR;
  } else {
    if (json_integer_value(json_object_get(j_result, "result")) == A_ERROR_NOT_FOUND) {
      json_decref(j_result);
      return A_ERROR_NOT_FOUND;
    } else if (json_integer_value(json_object_get(j_result, "result")) == A_OK) {
      j_trigger = json_object_get(j_result, "trigger");
      j_tags = json_object_get(json_object_get(j_trigger, "options"), "tags");
      if (j_tags == NULL) {
        json_object_set_new(json_object_get(j_trigger, "options"), "tags", json_pack("[s]", tag));
      } else if (json_is_array(j_tags)) {
        json_array_append_new(json_object_get(json_object_get(j_trigger, "options"), "tags"), json_string(tag));
      } else {
        json_decref(j_result);
        return A_ERROR;
      }
      res = trigger_modify(config, trigger_name, j_trigger);
      json_decref(j_result);
      return res;
    } else {
      json_decref(j_result);
      return A_ERROR;
    }
  }
}

int trigger_remove_tag(struct config_elements * config, const char * trigger_name, const char * tag) {
  json_t * j_result = trigger_get(config, trigger_name), * j_trigger, * j_tags;
  int i, res;
  
  if (j_result == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "trigger_remove_tag - Error getting trigger");
    return A_ERROR;
  } else {
    if (json_integer_value(json_object_get(j_result, "result")) == A_ERROR_NOT_FOUND) {
      json_decref(j_result);
      return A_ERROR_NOT_FOUND;
    } else if (json_integer_value(json_object_get(j_result, "result")) == A_OK) {
      j_trigger = json_object_get(j_result, "trigger");
      j_tags = json_object_get(json_object_get(j_trigger, "options"), "tags");
      if (j_tags == NULL) {
        json_decref(j_result);
        return A_OK;
      } else if (json_is_array(j_tags)) {
        for (i = json_array_size(json_object_get(json_object_get(j_trigger, "options"), "tags"))-1; i >= 0; i--) {
          if (0 == nstrcmp(json_string_value(json_array_get(json_object_get(json_object_get(j_trigger, "options"), "tags"), i)), tag)) {
            json_array_remove(json_object_get(json_object_get(j_trigger, "options"), "tags"), i);
          }
        }
        res = trigger_modify(config, trigger_name, j_trigger);
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

json_t * trigger_get_script_list(struct config_elements * config, const char * trigger_name) {
  
  char * escaped = h_escape_string(config->conn, trigger_name),
        * query = msprintf("SELECT a_script.asc_name AS name, a_trigger_script.ats_enabled AS i_enabled FROM a_script, a_trigger_script WHERE a_trigger_script.at_id = (SELECT at_id FROM a_trigger WHERE at_name = '%s') AND a_script.asc_id = a_trigger_script.asc_id", trigger_name);
  int res;
  json_t * j_result, * j_trigger;
  size_t index;
  
  res = h_query_select_json(config->conn, query, &j_result);
  free(escaped);
  free(query);
  
  if (res != H_OK) {
    y_log_message(Y_LOG_LEVEL_ERROR, "trigger_get_script_list - Error getting trigger");
    return json_pack("{si}", "result", A_ERROR);
  } else {
    json_array_foreach(j_result, index, j_trigger) {
      if (json_integer_value(json_object_get(j_trigger, "i_enabled")) == 0) {
        json_object_set_new(j_trigger, "enabled", json_false());
      } else {
        json_object_set_new(j_trigger, "enabled", json_true());
      }
      json_object_del(j_trigger, "i_enabled");
    }
    return json_pack("{siso}", "result", A_OK, "scripts", j_result);
  }
}

int trigger_set_script_list(struct config_elements * config, const char * trigger_name, json_t * script_list) {
  json_t * j_query, * script;
  int res;
  size_t index;
  char * t_escaped = h_escape_string(config->conn, trigger_name), * t_clause = msprintf("(SELECT at_id FROM a_trigger WHERE at_name = '%s')", t_escaped), * tmp = msprintf("= %s", t_clause), * sc_escaped, * sc_clause;
  
  j_query = json_pack("{sss{s{ssss}}}",
                      "table",
                        ANGHARAD_TABLE_TRIGGER_SCRIPT,
                      "where",
                        "at_id",
                          "operator",
                          "raw",
                          "value",
                          tmp);
  free(tmp);
  
  if (j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "trigger_set_script_list - Error allocating resources for j_query");
    return A_ERROR_MEMORY;
  }
  
  res = h_delete(config->conn, j_query, NULL);
  json_decref(j_query);
  if (res != H_OK) {
    y_log_message(Y_LOG_LEVEL_ERROR, "trigger_set_script_list - Error deleting script list");
    free(t_escaped);
    free(t_clause);
    return A_ERROR_DB;
  } else {
    j_query = json_pack("{ssso}",
                        "table",
                          ANGHARAD_TABLE_TRIGGER_SCRIPT,
                        "values",
                          json_array());
    if (j_query == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "trigger_set_script_list - Error allocating resources for j_query");
      free(t_escaped);
      free(t_clause);
      return A_ERROR_MEMORY;
    }
    json_array_foreach(script_list, index, script) {
      sc_escaped = h_escape_string(config->conn, json_string_value(json_object_get(script, "name")));
      sc_clause = msprintf("(SELECT asc_id FROM a_script WHERE asc_name = '%s')", sc_escaped);
      json_array_append_new(json_object_get(j_query, "values"), 
        json_pack("{s{ss}s{ss}so}", 
                  "at_id",
                    "raw",
                    t_clause,
                  "asc_id",
                    "raw",
                    sc_clause,
                  "ats_enabled",
                    json_object_get(script, "enabled")));
      free(sc_escaped);
      free(sc_clause);
    }
    
    res = h_insert(config->conn, j_query, NULL);
    json_decref(j_query);
    free(t_escaped);
    free(t_clause);
    if (res == H_OK) {
      return A_OK;
    } else {
      return A_ERROR_DB;
    }
  }
}
