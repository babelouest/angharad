/**
 *
 * Angharad system
 *
 * Manage all subsystems and handles the house automation server:
 * - Benoic
 * - Gareth
 * - Carleon
 * 
 * Events (schedulers and alerts) management functions
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

json_t * event_get(struct config_elements * config, const char * event_name) {
  json_t * j_query, * j_result, * j_event, * j_options, * j_enabled, * to_return, * j_scheduler_or_trigger;
  int res;
  size_t index;
  
  j_query = json_pack("{sss[ssss]}",
                      "table",
                        ANGHARAD_TABLE_EVENT,
                      "columns",
                        "ae_name AS name",
                        "ae_description AS description",
                        "ae_enabled",
                        "ae_options");
  
  if (j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "event_get - Error allocating resources for j_query");
    return json_pack("{si}", "result", ANGHARAD_RESULT_ERROR);
  }
  
  if (event_name != NULL) {
    json_object_set_new(j_query, "where", json_pack("{ss}", "ae_name", event_name));
  }
  
  res = h_select(config->conn, j_query, &j_result, NULL);
  json_decref(j_query);
  if (res == H_OK) {
    if (event_name == NULL) {
      to_return = json_array();
      if (to_return == NULL) {
        y_log_message(Y_LOG_LEVEL_ERROR, "event_get - Error allocating resources for to_return");
        return json_pack("{si}", "result", ANGHARAD_RESULT_ERROR);
      }
      json_array_foreach(j_result, index, j_event) {
        j_scheduler_or_trigger = event_get_scheduler_or_trigger(config, json_string_value(json_object_get(j_event, "name")));
        if (j_scheduler_or_trigger != NULL && json_integer_value(json_object_get(j_scheduler_or_trigger, "result")) == ANGHARAD_RESULT_OK) {
          if (json_object_get(j_scheduler_or_trigger, "scheduler") != NULL) {
            json_object_set_new(j_event, "scheduler", json_copy(json_object_get(j_scheduler_or_trigger, "scheduler")));
          } else if (json_object_get(j_scheduler_or_trigger, "trigger") != NULL) {
            json_object_set_new(j_event, "trigger", json_copy(json_object_get(j_scheduler_or_trigger, "trigger")));
          }
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "event_get - Error getting scheduler or trigger");
          json_decref(j_result);
          json_decref(to_return);
          return json_pack("{si}", "result", ANGHARAD_RESULT_ERROR);
        }
        j_enabled = json_object_get(j_event, "ae_enabled");
        json_object_set_new(j_event, "enabled", json_integer_value(j_enabled)==1?json_true():json_false());
        json_object_del(j_event, "ae_enabled");
        j_options = json_loads(json_string_value(json_object_get(j_event, "ae_options")), JSON_DECODE_ANY, NULL);
        json_object_del(j_event, "ae_options");
        json_object_set_new(j_event, "options", j_options);
        json_array_append_new(to_return, json_copy(j_event));
      }
      json_decref(j_result);
      return json_pack("{siso}", "result", ANGHARAD_RESULT_OK, "events", to_return);
    } else {
      if (json_array_size(j_result) == 0) {
        json_decref(j_result);
        return json_pack("{si}", "result", ANGHARAD_RESULT_NOT_FOUND);
      } else {
        j_event = json_copy(json_array_get(j_result, 0));
        json_decref(j_result);
        if (j_event == NULL) {
          y_log_message(Y_LOG_LEVEL_ERROR, "event_get - Error allocating resources for j_event");
          return json_pack("{si}", "result", ANGHARAD_RESULT_ERROR);
        }
        
        j_scheduler_or_trigger = event_get_scheduler_or_trigger(config, json_string_value(json_object_get(j_event, "name")));
        if (j_scheduler_or_trigger != NULL && json_integer_value(json_object_get(j_scheduler_or_trigger, "result")) == ANGHARAD_RESULT_OK) {
          if (json_object_get(j_scheduler_or_trigger, "scheduler") != NULL) {
            json_object_set_new(j_event, "scheduler", json_copy(json_object_get(j_scheduler_or_trigger, "scheduler")));
          } else if (json_object_get(j_scheduler_or_trigger, "trigger") != NULL) {
            json_object_set_new(j_event, "trigger", json_copy(json_object_get(j_scheduler_or_trigger, "trigger")));
          }
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "event_get - Error getting scheduler or trigger");
          return json_pack("{si}", "result", ANGHARAD_RESULT_ERROR);
        }
        
        j_enabled = json_object_get(j_event, "ae_enabled");
        json_object_set_new(j_event, "enabled", json_integer_value(j_enabled)?json_true():json_false());
        json_object_del(j_event, "ae_enabled");
        j_options = json_loads(json_string_value(json_object_get(j_event, "ae_options")), JSON_DECODE_ANY, NULL);
        json_object_del(j_event, "ae_options");
        json_object_set_new(j_event, "options", j_options);
        return json_pack("{siso}", "result", ANGHARAD_RESULT_OK, "event", j_event);
      }
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "event_get - Error allocating resources for j_query");
    return json_pack("{si}", "result", ANGHARAD_RESULT_ERROR);
  }
}

int event_add(struct config_elements * config, json_t * j_event) {
  json_t * j_query;
  int res;
  char * str_options, * escaped;
  
  if (j_event == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "event_add - Error j_event is NULL");
    return ANGHARAD_RESULT_ERROR;
  }
  
  str_options = json_dumps(json_object_get(j_event, "options"), JSON_COMPACT);
  j_query = json_pack("{sss[{sssssiss}]}",
                      "table", ANGHARAD_TABLE_EVENT,
                      "values",
                        "ae_name", json_string_value(json_object_get(j_event, "name")),
                        "ae_description", json_string_value(json_object_get(j_event, "description")),
                        "ae_enabled", json_object_get(j_event, "enabled")==json_true()?1:0,
                        "ae_options", str_options);
  free(str_options);
  
  if (j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "event_add - Error Allocating resources for j_query");
    return ANGHARAD_RESULT_ERROR;
  }
  
  if (json_object_get(j_event, "scheduler") != NULL) {
    escaped = h_escape_string(config->conn, json_string_value(json_object_get(j_event, "scheduler")));
    str_options = msprintf("(SELECT ash_id FROM %s WHERE ash_name='%s')", ANGHARAD_TABLE_SCHEDULER, escaped);
    json_object_set_new(json_object_get(j_query, "values"), "ash_id", json_pack("{ss}", "raw", str_options));
    free(escaped);
    free(str_options);
  } else if (json_object_get(j_event, "trigger") != NULL) {
    escaped = h_escape_string(config->conn, json_string_value(json_object_get(j_event, "scheduler")));
    str_options = msprintf("(SELECT at_id FROM %s WHERE at_name='%s')", ANGHARAD_TABLE_TRIGGER, escaped);
    json_object_set_new(json_object_get(j_query, "values"), "ash_id", json_pack("{ss}", "raw", str_options));
    free(escaped);
    free(str_options);
  }
  
  res = h_insert(config->conn, j_query, NULL);
  json_decref(j_query);
  if (res == H_OK) {
    return ANGHARAD_RESULT_OK;
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "event_add - Error executing db query");
    return ANGHARAD_RESULT_ERROR;
  }
}

/**
 * 
 * event format must be:
 * {
 *   "name": string max 64 chars
 *   "description": string max 128 chars
 *   "enabled": boolean
 *   "options": object with a tag object in it
 * }
 */
json_t * is_event_valid(struct config_elements * config, json_t * j_event, const int update) {
  json_t * j_result = json_array(), * j_element, * j_options_valid, * j_tmp;
  size_t index;
  
  if (j_result == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "is_event_valid - Error allocating resources for j_result");
    return NULL;
  }
  
  if (j_event == NULL || !json_is_object(j_event)) {
    json_array_append_new(j_result, json_pack("{ss}", "event", "event object is mandatory"));
  } else {
    if (!update) {
      j_element = json_object_get(j_event, "name");
      if (j_element == NULL || !json_is_string(j_element) || json_string_length(j_element) > 64) {
        json_array_append_new(j_result, json_pack("{ss}", "name", "name parameter is mandatory must ba a string, maximum 64 characters"));
      }
    }

    j_element = json_object_get(j_event, "description");
    if (j_element != NULL && (!json_is_string(j_element) || json_string_length(j_element) > 128)) {
      json_array_append_new(j_result, json_pack("{ss}", "description", "description parameter must ba a string, maximum 128 characters"));
    }
    
    j_element = json_object_get(j_event, "enabled");
    if (j_element != NULL && !json_is_boolean(j_element)) {
      json_array_append_new(j_result, json_pack("{ss}", "enabled", "enabled parameter must ba a boolean"));
    }
    
    if (json_object_get(j_event, "scheduler") != NULL && json_object_get(j_event, "trigger") != NULL) {
      json_array_append_new(j_result, json_pack("{ss}", "event", "event must have only one scheduler or one trigger associated"));
    } else if (json_object_get(j_event, "scheduler") != NULL) {
      j_element = json_object_get(j_event, "scheduler");
      if (j_element != NULL && !json_is_string(j_element)) {
        json_array_append_new(j_result, json_pack("{ss}", "scheduler", "scheduler must be a string"));
      } else if (j_element != NULL && json_is_string(j_element)) {
        j_tmp = scheduler_get(config, json_string_value(j_element));
        if (j_tmp == NULL || json_integer_value(json_object_get(j_tmp, "result")) == ANGHARAD_RESULT_NOT_FOUND) {
          json_array_append_new(j_result, json_pack("{ss}", "scheduler", "scheduler does not exist"));
        }
      }
    } else if (json_object_get(j_event, "trigger") != NULL) {
      j_element = json_object_get(j_event, "trigger");
      if (j_element != NULL && !json_is_string(j_element)) {
        json_array_append_new(j_result, json_pack("{ss}", "trigger", "trigger must be a string"));
      } else if (j_element != NULL && json_is_string(j_element)) {
        j_tmp = trigger_get(config, json_string_value(j_element));
        if (j_tmp == NULL || json_integer_value(json_object_get(j_tmp, "result")) == ANGHARAD_RESULT_NOT_FOUND) {
          json_array_append_new(j_result, json_pack("{ss}", "trigger", "trigger does not exist"));
        }
      }
    }
    
    j_options_valid = is_option_valid(json_object_get(j_event, "options"), BENOIC_ELEMENT_TYPE_NONE);
    if (j_options_valid != NULL && json_array_size(j_options_valid) > 0) {
      json_array_foreach(j_options_valid, index, j_element) {
        json_array_append_new(j_result, json_copy(j_element));
      }
      json_decref(j_options_valid);
    } else if(j_options_valid != NULL) {
      json_decref(j_options_valid);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "is_event_valid - Error, is_options_valid returned NULL");
      json_decref(j_result);
      return NULL;
    }
  }
  return j_result;
}

int event_modify(struct config_elements * config, const char * event_name, json_t * j_event) {
  json_t * j_query, * cur_event;
  int res, res_cur_event;
  char * str_options, * escaped;
  
  if (j_event == NULL || event_name == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "event_modify - Error j_event or event_name is NULL");
    return ANGHARAD_RESULT_ERROR;
  }
  
  cur_event = event_get(config, event_name);
  res_cur_event = (cur_event != NULL?json_integer_value(json_object_get(cur_event, "result")):ANGHARAD_RESULT_ERROR);
  json_decref(cur_event);
  if (res_cur_event == ANGHARAD_RESULT_OK) {
    str_options = json_dumps(json_object_get(j_event, "options"), JSON_COMPACT);
    j_query = json_pack("{sss{sssiss}s{ss}}",
                        "table", ANGHARAD_TABLE_EVENT,
                        "set",
                          "ae_description", json_string_value(json_object_get(j_event, "description")),
                          "ae_enabled", json_object_get(j_event, "enabled")==json_true()?1:0,
                          "ae_options", str_options,
                        "where",
                          "ae_name", event_name);
    free(str_options);
    
    if (j_query == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "event_modify - Error Allocating resources for j_query");
      return ANGHARAD_RESULT_ERROR;
    }
    
  if (json_object_get(j_event, "scheduler") != NULL) {
    escaped = h_escape_string(config->conn, json_string_value(json_object_get(j_event, "scheduler")));
    str_options = msprintf("(SELECT ash_id FROM %s WHERE ash_name='%s')", ANGHARAD_TABLE_SCHEDULER, escaped);
    json_object_set_new(json_object_get(j_query, "set"), "ash_id", json_pack("{ss}", "raw", str_options));
    free(escaped);
    free(str_options);
  } else if (json_object_get(j_event, "trigger") != NULL) {
    escaped = h_escape_string(config->conn, json_string_value(json_object_get(j_event, "scheduler")));
    str_options = msprintf("(SELECT at_id FROM %s WHERE at_name='%s')", ANGHARAD_TABLE_TRIGGER, escaped);
    json_object_set_new(json_object_get(j_query, "set"), "ash_id", json_pack("{ss}", "raw", str_options));
    free(escaped);
    free(str_options);
  }
  
    res = h_update(config->conn, j_query, NULL);
    json_decref(j_query);
    if (res == H_OK) {
      return ANGHARAD_RESULT_OK;
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "event_modify - Error executing db query");
      return ANGHARAD_RESULT_ERROR;
    }
  } else if (res_cur_event == ANGHARAD_RESULT_NOT_FOUND) {
    return ANGHARAD_RESULT_NOT_FOUND;
  } else {
    return ANGHARAD_RESULT_ERROR;
  }
}

int event_delete(struct config_elements * config, const char * event_name) {
  json_t * j_query, * cur_event;
  int res, res_cur_event;
  
  if (event_name == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "event_modify - Error event_name is NULL");
    return ANGHARAD_RESULT_ERROR;
  }
  
  cur_event = event_get(config, event_name);
  res_cur_event = (cur_event != NULL?json_integer_value(json_object_get(cur_event, "result")):ANGHARAD_RESULT_ERROR);
  json_decref(cur_event);
  if (res_cur_event == ANGHARAD_RESULT_OK) {
    j_query = json_pack("{sss{ss}}",
                        "table", ANGHARAD_TABLE_EVENT,
                        "where",
                          "ae_name", event_name);
    
    if (j_query == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "event_delete - Error Allocating resources for j_query");
      return ANGHARAD_RESULT_ERROR;
    }
    
    res = h_delete(config->conn, j_query, NULL);
    json_decref(j_query);
    if (res == H_OK) {
      return ANGHARAD_RESULT_OK;
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "event_delete - Error executing db query");
      return ANGHARAD_RESULT_ERROR;
    }
  } else if (res_cur_event == ANGHARAD_RESULT_NOT_FOUND) {
    return ANGHARAD_RESULT_NOT_FOUND;
  } else {
    return ANGHARAD_RESULT_ERROR;
  }
}

int event_add_tag(struct config_elements * config, const char * event_name, const char * tag) {
  json_t * j_result = event_get(config, event_name), * j_event, * j_tags;
  int res;
  
  if (j_result == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "event_add_tag - Error getting event");
    return ANGHARAD_RESULT_ERROR;
  } else {
    if (json_integer_value(json_object_get(j_result, "result")) == ANGHARAD_RESULT_NOT_FOUND) {
      json_decref(j_result);
      return ANGHARAD_RESULT_NOT_FOUND;
    } else if (json_integer_value(json_object_get(j_result, "result")) == ANGHARAD_RESULT_OK) {
      j_event = json_object_get(j_result, "event");
      j_tags = json_object_get(json_object_get(j_event, "options"), "tags");
      if (j_tags == NULL) {
        json_object_set_new(json_object_get(j_event, "options"), "tags", json_pack("[s]", tag));
      } else if (json_is_array(j_tags)) {
        json_array_append_new(json_object_get(json_object_get(j_event, "options"), "tags"), json_string(tag));
      } else {
        json_decref(j_result);
        return ANGHARAD_RESULT_ERROR;
      }
      res = event_modify(config, event_name, j_event);
      json_decref(j_result);
      return res;
    } else {
      json_decref(j_result);
      return ANGHARAD_RESULT_ERROR;
    }
  }
}

int event_remove_tag(struct config_elements * config, const char * event_name, const char * tag) {
  json_t * j_result = event_get(config, event_name), * j_event, * j_tags;
  int i, res;
  
  if (j_result == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "event_remove_tag - Error getting event");
    return ANGHARAD_RESULT_ERROR;
  } else {
    if (json_integer_value(json_object_get(j_result, "result")) == ANGHARAD_RESULT_NOT_FOUND) {
      json_decref(j_result);
      return ANGHARAD_RESULT_NOT_FOUND;
    } else if (json_integer_value(json_object_get(j_result, "result")) == ANGHARAD_RESULT_OK) {
      j_event = json_object_get(j_result, "event");
      j_tags = json_object_get(json_object_get(j_event, "options"), "tags");
      if (j_tags == NULL) {
        json_decref(j_result);
        return ANGHARAD_RESULT_OK;
      } else if (json_is_array(j_tags)) {
        for (i = json_array_size(json_object_get(json_object_get(j_event, "options"), "tags"))-1; i >= 0; i--) {
          if (0 == strcmp(json_string_value(json_array_get(json_object_get(json_object_get(j_event, "options"), "tags"), i)), tag)) {
            json_array_remove(json_object_get(json_object_get(j_event, "options"), "tags"), i);
          }
        }
        res = event_modify(config, event_name, j_event);
        json_decref(j_result);
        return res;
      } else {
        json_decref(j_result);
        return ANGHARAD_RESULT_ERROR;
      }
    } else {
      json_decref(j_result);
      return ANGHARAD_RESULT_ERROR;
    }
  }
}

json_t * event_get_scheduler_or_trigger(struct config_elements * config, const char * event_name) {
  json_t * j_query, * j_result, * j_return;
  int res;
  
  j_query = json_pack("{sss[ss]s{ss}}",
                      "table",
                        ANGHARAD_TABLE_EVENT,
                      "columns",
                        "ash_id",
                        "at_id",
                      "where",
                        "ae_name",
                        event_name);

  if (j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "event_get_scheduler_or_trigger - Error allocating resources for j_query");
    return json_pack("{si}", "result", ANGHARAD_RESULT_ERROR);
  }
  
  res = h_select(config->conn, j_query, &j_result, NULL);
  json_decref(j_query);
  if (res == H_OK) {
    if (json_array_size(j_result) > 0) {
      if (json_object_get(json_array_get(j_result, 0), "ash_id") != json_null()) {
        j_query = json_pack("{sss[s]s{ss}}",
                            "table",
                              ANGHARAD_TABLE_SCHEDULER,
                            "columns",
                              "ash_name",
                            "where",
                              "ash_id",
                              json_string_value(json_object_get(json_array_get(j_result, 0), "ash_id")));
        json_decref(j_result);
        
        if (j_query == NULL) {
          y_log_message(Y_LOG_LEVEL_ERROR, "event_get_scheduler_or_trigger - Error allocating resources for j_query (2)");
          return json_pack("{si}", "result", ANGHARAD_RESULT_ERROR);
        }
        res = h_select(config->conn, j_query, &j_result, NULL);
        json_decref(j_query);
        if (res == H_OK && json_array_size(j_result) > 0) {
          j_return = json_pack("{ssss}", "result", ANGHARAD_RESULT_OK, "scheduler", json_string_value(json_object_get(json_array_get(j_result, 0), "ash_name")));
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "event_get_scheduler_or_trigger - Error getting scheduler name");
          j_return = json_pack("{si}", "result", ANGHARAD_RESULT_ERROR);
        }
        json_decref(j_result);
      } else if (json_object_get(json_array_get(j_result, 0), "at_id") != json_null()) {
        j_query = json_pack("{sss[s]s{ss}}",
                            "table",
                              ANGHARAD_TABLE_TRIGGER,
                            "columns",
                              "at_name",
                            "where",
                              "at_id",
                              json_string_value(json_object_get(json_array_get(j_result, 0), "at_id")));
        json_decref(j_result);
        
        if (j_query == NULL) {
          y_log_message(Y_LOG_LEVEL_ERROR, "event_get_scheduler_or_trigger - Error allocating resources for j_query (3)");
          return json_pack("{si}", "result", ANGHARAD_RESULT_ERROR);
        }
        res = h_select(config->conn, j_query, &j_result, NULL);
        json_decref(j_query);
        if (res == H_OK && json_array_size(j_result) > 0) {
          j_return = json_pack("{ssss}", "result", ANGHARAD_RESULT_OK, "trigger", json_string_value(json_object_get(json_array_get(j_result, 0), "at_name")));
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "event_get_scheduler_or_trigger - Error getting trigger name");
          j_return = json_pack("{si}", "result", ANGHARAD_RESULT_ERROR);
        }
        json_decref(j_result);
      } else {
        j_return = json_pack("{si}", "result", ANGHARAD_RESULT_NOT_FOUND);
      }
      return j_return;
    } else {
      json_decref(j_result);
      return json_pack("{si}", "result", ANGHARAD_RESULT_ERROR);
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "event_get_scheduler_or_trigger - Error getting event");
    return json_pack("{si}", "result", ANGHARAD_RESULT_ERROR);
  }
}
