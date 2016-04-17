/**
 *
 * Angharad system
 *
 * Manage all subsystems and handles the house automation server:
 * - Benoic
 * - Gareth
 * - Carleon
 * 
 * Schedulers management functions
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

void * thread_scheduler_run(void * args) {
  struct config_elements * config = (struct config_elements *)args;
  time_t now;
  struct tm ts;
  if (config != NULL) {
    while (config->angharad_status == ANGHARAD_STATUS_RUN) {
      // Run monitoring task every minute
      time(&now);
      ts = *localtime(&now);
      if (ts.tm_sec == 0) {
        // Check schedules and events
        y_log_message(Y_LOG_LEVEL_DEBUG, "thread_scheduler_run - Check schedules and events");
      }
      
      sleep(1);
    }
    config->angharad_status = ANGHARAD_STATUS_STOP;
  }
  
  return NULL;
}

json_t * scheduler_get(struct config_elements * config, const char * scheduler_name) {
  json_t * j_query, * j_result, * j_scheduler, * j_options, * j_bool, * to_return, * j_repeat;
  int res;
  size_t index;
  
  j_query = json_pack("{sss[ssssssss]}", 
                        "table", 
                          ANGHARAD_TABLE_SCHEDULER, 
                        "columns", 
                          "ash_name AS name", 
                          "ash_description AS description", 
                          "ash_enabled", 
                          "ash_options", 
                          "UNIX_TIMESTAMP(ash_next_time) AS next_time", 
                          "ash_repeat AS s_repeat", 
                          "ash_repeat_value AS repeat_value", 
                          "ash_remove_after");
  
  if (j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "scheduler_get - Error allocating resources for j_query");
    return json_pack("{si}", "result", ANGHARAD_RESULT_ERROR);
  }
  
  if (scheduler_name != NULL) {
    json_object_set_new(j_query, "where", json_pack("{ss}", "ash_name", scheduler_name));
  }
  
  res = h_select(config->conn, j_query, &j_result, NULL);
  json_decref(j_query);
  if (res == H_OK) {
    if (scheduler_name == NULL) {
      to_return = json_array();
      if (to_return == NULL) {
        y_log_message(Y_LOG_LEVEL_ERROR, "scheduler_get - Error allocating resources for to_return");
        return json_pack("{si}", "result", ANGHARAD_RESULT_ERROR);
      }
      json_array_foreach(j_result, index, j_scheduler) {
        j_bool = json_object_get(j_scheduler, "ash_enabled");
        json_object_set_new(j_scheduler, "enabled", json_integer_value(j_bool)==1?json_true():json_false());
        json_object_del(j_scheduler, "ash_enabled");
        j_options = json_loads(json_string_value(json_object_get(j_scheduler, "ash_options")), JSON_DECODE_ANY, NULL);
        json_object_del(j_scheduler, "ash_options");
        json_object_set_new(j_scheduler, "options", j_options);
        j_bool = json_object_get(j_scheduler, "ash_remove_after");
        json_object_set_new(j_scheduler, "remove_after", json_integer_value(j_bool)==1?json_true():json_false());
        json_object_del(j_scheduler, "ash_remove_after");
        j_repeat = json_copy(json_object_get(j_scheduler, "s_repeat"));
        json_object_del(j_scheduler, "s_repeat");
        json_object_set_new(j_scheduler, "repeat", j_repeat);
        
        json_array_append_new(to_return, json_copy(j_scheduler));
      }
      json_decref(j_result);
      return json_pack("{siso}", "result", ANGHARAD_RESULT_OK, "schedulers", to_return);
    } else {
      if (json_array_size(j_result) == 0) {
        json_decref(j_result);
        return json_pack("{si}", "result", ANGHARAD_RESULT_NOT_FOUND);
      } else {
        j_scheduler = json_copy(json_array_get(j_result, 0));
        json_decref(j_result);
        if (j_scheduler == NULL) {
          y_log_message(Y_LOG_LEVEL_ERROR, "scheduler_get - Error allocating resources for j_scheduler");
          return json_pack("{si}", "result", ANGHARAD_RESULT_ERROR);
        }
        j_bool = json_object_get(j_scheduler, "ash_enabled");
        json_object_set_new(j_scheduler, "enabled", json_integer_value(j_bool)?json_true():json_false());
        json_object_del(j_scheduler, "ash_enabled");
        j_options = json_loads(json_string_value(json_object_get(j_scheduler, "ash_options")), JSON_DECODE_ANY, NULL);
        json_object_del(j_scheduler, "ash_options");
        json_object_set_new(j_scheduler, "options", j_options);
        j_bool = json_object_get(j_scheduler, "ash_remove_after");
        json_object_set_new(j_scheduler, "remove_after", json_integer_value(j_bool)==1?json_true():json_false());
        json_object_del(j_scheduler, "ash_remove_after");
        j_repeat = json_copy(json_object_get(j_scheduler, "s_repeat"));
        json_object_del(j_scheduler, "s_repeat");
        json_object_set_new(j_scheduler, "repeat", j_repeat);

        return json_pack("{siso}", "result", ANGHARAD_RESULT_OK, "scheduler", j_scheduler);
      }
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "scheduler_get - Error allocating resources for j_query");
    return json_pack("{si}", "result", ANGHARAD_RESULT_ERROR);
  }
}

int scheduler_add(struct config_elements * config, json_t * j_scheduler) {
  json_t * j_query;
  int res;
  char * str_options, * str_next_time;
  
  if (j_scheduler == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "scheduler_add - Error j_scheduler is NULL");
    return ANGHARAD_RESULT_ERROR;
  }
  
  str_next_time = msprintf("FROM_UNIXTIME(%d)", json_integer_value(json_object_get(j_scheduler, "next_time")));
  str_options = json_dumps(json_object_get(j_scheduler, "options"), JSON_COMPACT);
  j_query = json_pack("{sss[{sssssisss{ss}sisisi}]}",
                      "table", ANGHARAD_TABLE_SCHEDULER,
                      "values",
                        "ash_name", json_string_value(json_object_get(j_scheduler, "name")),
                        "ash_description", json_string_value(json_object_get(j_scheduler, "description")),
                        "ash_enabled", json_object_get(j_scheduler, "enabled")==json_false()?0:1,
                        "ash_options", str_options,
                        "ash_next_time", "raw", str_next_time, 
                        "ash_repeat", json_integer_value(json_object_get(j_scheduler, "repeat")), 
                        "ash_repeat_value", json_integer_value(json_object_get(j_scheduler, "repeat_value")), 
                        "ash_remove_after", json_object_get(j_scheduler, "remove_after")==json_false()?0:1);
  free(str_options);
  free(str_next_time);
  
  if (j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "scheduler_add - Error Allocating resources for j_query");
    return ANGHARAD_RESULT_ERROR;
  }
  
  res = h_insert(config->conn, j_query, NULL);
  json_decref(j_query);
  if (res == H_OK) {
    return ANGHARAD_RESULT_OK;
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "scheduler_add - Error executing db query");
    return ANGHARAD_RESULT_ERROR;
  }
}

/**
 * 
 */
json_t * is_scheduler_valid(struct config_elements * config, json_t * j_scheduler, const int update) {
  json_t * j_result = json_array(), * j_element, * j_options_valid;
  size_t index;
  
  if (j_result == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "is_scheduler_valid - Error allocating resources for j_result");
    return NULL;
  }
  
  if (j_scheduler == NULL || !json_is_object(j_scheduler)) {
    json_array_append_new(j_result, json_pack("{ss}", "scheduler", "scheduler object is mandatory"));
  } else {
    if (!update) {
      j_element = json_object_get(j_scheduler, "name");
      if (j_element == NULL || !json_is_string(j_element) || json_string_length(j_element) > 64) {
        json_array_append_new(j_result, json_pack("{ss}", "name", "name parameter is mandatory must ba a string, maximum 64 characters"));
      }
    }

    j_element = json_object_get(j_scheduler, "description");
    if (j_element != NULL && (!json_is_string(j_element) || json_string_length(j_element) > 128)) {
      json_array_append_new(j_result, json_pack("{ss}", "description", "description parameter must ba a string, maximum 128 characters"));
    }
    
    j_element = json_object_get(j_scheduler, "enabled");
    if (j_element != NULL && !json_is_boolean(j_element)) {
      json_array_append_new(j_result, json_pack("{ss}", "enabled", "enabled parameter must ba a boolean"));
    }
    
    j_element = json_object_get(j_scheduler, "next_time");
    if (j_element == NULL || !json_is_integer(j_element) || json_integer_value(j_element) < 0) {
      json_array_append_new(j_result, json_pack("{ss}", "next_time", "next_time parameter must ba a valid epoch time"));
    }
    
    j_element = json_object_get(j_scheduler, "repeat");
    if (j_element != NULL && (!json_is_integer(j_element) || json_integer_value(j_element) < -1 || json_integer_value(j_element) > 5)) {
      json_array_append_new(j_result, json_pack("{ss}", "repeat", "repeat parameter must ba an integer between -1 and 5"));
    }
    
    j_element = json_object_get(j_scheduler, "repeat_value");
    if (j_element != NULL && (!json_is_integer(j_element) || json_integer_value(j_element) < 0)) {
      json_array_append_new(j_result, json_pack("{ss}", "repeat_value", "repeat_value parameter must ba a positive integer"));
    }
    
    j_element = json_object_get(j_scheduler, "remove_after");
    if (j_element != NULL && !json_is_boolean(j_element)) {
      json_array_append_new(j_result, json_pack("{ss}", "remove_after", "remove_after parameter must ba a boolean"));
    }
    
    j_options_valid = is_option_valid(json_object_get(j_scheduler, "options"), BENOIC_ELEMENT_TYPE_NONE);
    if (j_options_valid != NULL && json_array_size(j_options_valid) > 0) {
      json_array_foreach(j_options_valid, index, j_element) {
        json_array_append_new(j_result, json_copy(j_element));
      }
      json_decref(j_options_valid);
    } else if(j_options_valid != NULL) {
      json_decref(j_options_valid);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "is_scheduler_valid - Error, is_options_valid returned NULL");
      json_decref(j_result);
      return NULL;
    }
  }
  return j_result;
}

int scheduler_modify(struct config_elements * config, const char * scheduler_name, json_t * j_scheduler) {
  json_t * j_query, * cur_scheduler;
  int res, res_cur_scheduler;
  char * str_options, * str_next_time;
  
  if (j_scheduler == NULL || scheduler_name == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "scheduler_modify - Error j_scheduler or scheduler_name is NULL");
    return ANGHARAD_RESULT_ERROR;
  }
  
  cur_scheduler = scheduler_get(config, scheduler_name);
  res_cur_scheduler = (cur_scheduler != NULL?json_integer_value(json_object_get(cur_scheduler, "result")):ANGHARAD_RESULT_ERROR);
  json_decref(cur_scheduler);
  if (res_cur_scheduler == ANGHARAD_RESULT_OK) {
    str_options = json_dumps(json_object_get(j_scheduler, "options"), JSON_COMPACT);
    str_next_time = msprintf("FROM_UNIXTIME(%d)", json_integer_value(json_object_get(j_scheduler, "next_time")));
    j_query = json_pack("{sss{sssisss{ss}sisisi}s{ss}}",
                        "table", ANGHARAD_TABLE_SCHEDULER,
                        "set",
                          "ash_description", json_string_value(json_object_get(j_scheduler, "description")),
                          "ash_enabled", json_object_get(j_scheduler, "enabled")==json_false()?0:1,
                          "ash_options", str_options,
                          "ash_next_time", "raw", str_next_time, 
                          "ash_repeat", json_integer_value(json_object_get(j_scheduler, "repeat")), 
                          "ash_repeat_value", json_integer_value(json_object_get(j_scheduler, "repeat_value")), 
                          "ash_remove_after", json_object_get(j_scheduler, "remove_after")==json_false()?0:1,
                        "where",
                          "ash_name", scheduler_name);
    free(str_options);
    free(str_next_time);
    
    if (j_query == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "scheduler_modify - Error Allocating resources for j_query");
      return ANGHARAD_RESULT_ERROR;
    }
    
    res = h_update(config->conn, j_query, NULL);
    json_decref(j_query);
    if (res == H_OK) {
      return ANGHARAD_RESULT_OK;
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "scheduler_modify - Error executing db query");
      return ANGHARAD_RESULT_ERROR;
    }
  } else if (res_cur_scheduler == ANGHARAD_RESULT_NOT_FOUND) {
    return ANGHARAD_RESULT_NOT_FOUND;
  } else {
    return ANGHARAD_RESULT_ERROR;
  }
}

int scheduler_delete(struct config_elements * config, const char * scheduler_name) {
  json_t * j_query, * cur_scheduler;
  int res, res_cur_scheduler;
  
  if (scheduler_name == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "scheduler_modify - Error scheduler_name is NULL");
    return ANGHARAD_RESULT_ERROR;
  }
  
  cur_scheduler = scheduler_get(config, scheduler_name);
  res_cur_scheduler = (cur_scheduler != NULL?json_integer_value(json_object_get(cur_scheduler, "result")):ANGHARAD_RESULT_ERROR);
  json_decref(cur_scheduler);
  if (res_cur_scheduler == ANGHARAD_RESULT_OK) {
    j_query = json_pack("{sss{ss}}",
                        "table", ANGHARAD_TABLE_SCHEDULER,
                        "where",
                          "ash_name", scheduler_name);
    
    if (j_query == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "scheduler_delete - Error Allocating resources for j_query");
      return ANGHARAD_RESULT_ERROR;
    }
    
    res = h_delete(config->conn, j_query, NULL);
    json_decref(j_query);
    if (res == H_OK) {
      return ANGHARAD_RESULT_OK;
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "scheduler_delete - Error executing db query");
      return ANGHARAD_RESULT_ERROR;
    }
  } else if (res_cur_scheduler == ANGHARAD_RESULT_NOT_FOUND) {
    return ANGHARAD_RESULT_NOT_FOUND;
  } else {
    return ANGHARAD_RESULT_ERROR;
  }
}

int scheduler_add_tag(struct config_elements * config, const char * scheduler_name, const char * tag) {
  json_t * j_result = scheduler_get(config, scheduler_name), * j_scheduler, * j_tags;
  int res;
  
  if (j_result == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "scheduler_add_tag - Error getting scheduler");
    return ANGHARAD_RESULT_ERROR;
  } else {
    if (json_integer_value(json_object_get(j_result, "result")) == ANGHARAD_RESULT_NOT_FOUND) {
      json_decref(j_result);
      return ANGHARAD_RESULT_NOT_FOUND;
    } else if (json_integer_value(json_object_get(j_result, "result")) == ANGHARAD_RESULT_OK) {
      j_scheduler = json_object_get(j_result, "scheduler");
      j_tags = json_object_get(json_object_get(j_scheduler, "options"), "tags");
      if (j_tags == NULL) {
        json_object_set_new(json_object_get(j_scheduler, "options"), "tags", json_pack("[s]", tag));
      } else if (json_is_array(j_tags)) {
        json_array_append_new(json_object_get(json_object_get(j_scheduler, "options"), "tags"), json_string(tag));
      } else {
        json_decref(j_result);
        return ANGHARAD_RESULT_ERROR;
      }
      res = scheduler_modify(config, scheduler_name, j_scheduler);
      json_decref(j_result);
      return res;
    } else {
      json_decref(j_result);
      return ANGHARAD_RESULT_ERROR;
    }
  }
}

int scheduler_remove_tag(struct config_elements * config, const char * scheduler_name, const char * tag) {
  json_t * j_result = scheduler_get(config, scheduler_name), * j_scheduler, * j_tags;
  int i, res;
  
  if (j_result == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "scheduler_remove_tag - Error getting scheduler");
    return ANGHARAD_RESULT_ERROR;
  } else {
    if (json_integer_value(json_object_get(j_result, "result")) == ANGHARAD_RESULT_NOT_FOUND) {
      json_decref(j_result);
      return ANGHARAD_RESULT_NOT_FOUND;
    } else if (json_integer_value(json_object_get(j_result, "result")) == ANGHARAD_RESULT_OK) {
      j_scheduler = json_object_get(j_result, "scheduler");
      j_tags = json_object_get(json_object_get(j_scheduler, "options"), "tags");
      if (j_tags == NULL) {
        json_decref(j_result);
        return ANGHARAD_RESULT_OK;
      } else if (json_is_array(j_tags)) {
        for (i = json_array_size(json_object_get(json_object_get(j_scheduler, "options"), "tags"))-1; i >= 0; i--) {
          if (0 == strcmp(json_string_value(json_array_get(json_object_get(json_object_get(j_scheduler, "options"), "tags"), i)), tag)) {
            json_array_remove(json_object_get(json_object_get(j_scheduler, "options"), "tags"), i);
          }
        }
        res = scheduler_modify(config, scheduler_name, j_scheduler);
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
