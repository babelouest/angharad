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
#include <string.h>
#include <math.h>

#include "angharad.h"

struct _scheduler_runner {
  struct config_elements * config;
  json_t * scheduler;
};

static void * thread_scheduler_element_run(void * args) {
  struct _scheduler_runner * scheduler_runner = (struct _scheduler_runner *)args;
  time_t now;
  struct tm ts;
  size_t index = 0;
  json_t * script = NULL;

  time(&now);
  localtime_r(&now, &ts);
  // Execute scheduler scripts
  y_log_message(Y_LOG_LEVEL_DEBUG, "run scheduler %s", json_string_value(json_object_get(scheduler_runner->scheduler, "name")));
  if ((time_t)json_integer_value(json_object_get(scheduler_runner->scheduler, "next_time")) >= (now - 60)) {

    json_array_foreach(json_object_get(scheduler_runner->scheduler, "scripts"), index, script) {
      if (json_object_get(script, "enabled") == json_true() && json_is_string(json_object_get(script, "name"))) {
        y_log_message(Y_LOG_LEVEL_INFO, "thread_scheduler_run - run script %s", json_string_value(json_object_get(script, "name")));
        script_run(scheduler_runner->config, json_string_value(json_object_get(script, "name")));
        y_log_message(Y_LOG_LEVEL_DEBUG, "scheduler %s runned", json_string_value(json_object_get(scheduler_runner->scheduler, "name")));
      }
    }
  }
  json_decref(scheduler_runner->scheduler);
  o_free(scheduler_runner);
  return NULL;
}

/**
 * thread that loop forever annd check every minute if there are schedulers to execute
 */
void * thread_scheduler_run(void * args) {
  struct config_elements * config = (struct config_elements *)args;
  time_t now, next_time;
  struct tm ts;
  json_t * scheduler_list_names, * scheduler_name = NULL, * scheduler, * scheduler_result;
  size_t index = 0;
  struct _scheduler_runner * scheduler_runner;
  pthread_t thread_scheduler_element;
  int thread_ret_scheduler_element = 0, thread_detach_scheduler_element = 0;

  if (config != NULL) {
    while (config->angharad_status == ANGHARAD_STATUS_RUN) {
      // Run monitoring task every minute
      time(&now);
      localtime_r(&now, &ts);
      if (ts.tm_sec == 0) {
        // Check schedules and events
        scheduler_list_names = scheduler_get_next_schedules(config);
        if (scheduler_list_names != NULL) {
          json_array_foreach(scheduler_list_names, index, scheduler_name) {
            scheduler_result = scheduler_get(config, json_string_value(json_object_get(scheduler_name, "name")), 1);
            if (scheduler_result != NULL && json_integer_value(json_object_get(scheduler_result, "result")) == A_OK) {
              scheduler = json_object_get(scheduler_result, "scheduler");
              if ((scheduler_runner = o_malloc(sizeof(struct _scheduler_runner))) != NULL) {
                scheduler_runner->config = config;
                scheduler_runner->scheduler = json_incref(json_object_get(scheduler_result, "scheduler"));
                thread_ret_scheduler_element = pthread_create(&thread_scheduler_element, NULL, thread_scheduler_element_run, (void *)scheduler_runner);
                thread_detach_scheduler_element = pthread_detach(thread_scheduler_element);
                if (thread_ret_scheduler_element || thread_detach_scheduler_element) {
                  y_log_message(Y_LOG_LEVEL_ERROR, "Error creating or detaching scheduler element thread, return code: %d, detach code: %d",
                              thread_ret_scheduler_element, thread_detach_scheduler_element);
                  json_decref(scheduler_runner->scheduler);
                  o_free(scheduler_runner);
                }
              } else {
                y_log_message(Y_LOG_LEVEL_ERROR, "thread_scheduler_run - Error malloc for scheduler_runner");
              }
              next_time = (time_t)json_integer_value(json_object_get(scheduler, "next_time"));

              // Calculate next time or remove scheduler if needed or disable it if it's in the past
              if (json_object_get(scheduler, "remove_after") == json_true() && json_integer_value(json_object_get(scheduler, "repeat")) < 0) {
                y_log_message(Y_LOG_LEVEL_DEBUG, "Remove scheduler %s", json_string_value(json_object_get(scheduler, "name")));
                if (scheduler_delete(config, json_string_value(json_object_get(scheduler, "name"))) != A_OK) {
                  y_log_message(Y_LOG_LEVEL_ERROR, "Error removing scheduler %s", json_string_value(json_object_get(scheduler, "name")));
                }
              } else if (json_integer_value(json_object_get(scheduler, "repeat")) >= 0) {
                y_log_message(Y_LOG_LEVEL_DEBUG, "Calculate next_time for scheduler scheduler %s", json_string_value(json_object_get(scheduler, "name")));
                while (next_time <= now) {
                  next_time = scheduler_calculate_next_time(next_time, json_integer_value(json_object_get(scheduler, "repeat")), json_integer_value(json_object_get(scheduler, "repeat_value")));
                }

                if (next_time) {
                  json_object_set_new(scheduler, "next_time", json_integer(next_time));
                  if (scheduler_modify(config, json_string_value(json_object_get(scheduler, "name")), scheduler) != A_OK) {
                    y_log_message(Y_LOG_LEVEL_ERROR, "Error updating scheduler %s", json_string_value(json_object_get(scheduler, "name")));
                  }
                }
              } else {
                json_object_set_new(scheduler, "enabled", json_false());
                if (scheduler_modify(config, json_string_value(json_object_get(scheduler, "name")), scheduler) != A_OK) {
                  y_log_message(Y_LOG_LEVEL_ERROR, "Error updating scheduler %s", json_string_value(json_object_get(scheduler, "name")));
                }
              }
            } else {
              y_log_message(Y_LOG_LEVEL_ERROR, "thread_scheduler_run - Error getting scheduler '%s'", json_string_value(json_object_get(scheduler_name, "name")));
            }
            json_decref(scheduler_result);
          }
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "thread_scheduler_run - Error getting scheduler list");
        }
        json_decref(scheduler_list_names);
      }
      sleep(1);
    }
    config->angharad_status = ANGHARAD_STATUS_STOP;
  }
  return NULL;
}

/**
 * Return the schedulers that are due now
 */
json_t * scheduler_get_next_schedules(struct config_elements * config) {
  int res;
  json_t * j_result;
  json_t * j_query = json_pack("{sss[s]s{s{ssss}si}}",
                                "table",
                                  ANGHARAD_TABLE_SCHEDULER,
                                "columns",
                                  "ash_name AS name",
                                "where",
                                  "ash_next_time",
                                    "operator",
                                    "raw",
                                    "value",
                                    (config->conn->type == HOEL_DB_TYPE_MARIADB?"<= NOW()":"<= strftime('%s','now')"),
                                  "ash_enabled",
                                  1);

  if (j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "scheduler_get - Error allocating resources for j_query");
    return json_pack("{si}", "result", A_ERROR_MEMORY);
  }
  res = h_select(config->conn, j_query, &j_result, NULL);
  json_decref(j_query);
  if (res == H_OK) {
    return j_result;
  } else {
    return NULL;
  }
}

/**
 * Return the list of all schedulers or the sppecified scheduler
 */
json_t * scheduler_get(struct config_elements * config, const char * scheduler_name, const int runnable) {
  json_t * j_query, * j_result, * j_scheduler, * j_options, * j_bool, * to_return, * j_repeat, * script_list;
  int res;
  size_t index;

  j_query = json_pack("{sss[sssssss]}",
                        "table",
                          ANGHARAD_TABLE_SCHEDULER,
                        "columns",
                          "ash_name AS name",
                          "ash_description AS description",
                          "ash_enabled",
                          "ash_options",
                          "ash_repeat AS s_repeat",
                          "ash_repeat_value AS repeat_value",
                          "ash_remove_after");
  if (config->conn->type == HOEL_DB_TYPE_MARIADB) {
    json_array_append_new(json_object_get(j_query, "columns"), json_string("UNIX_TIMESTAMP(ash_next_time) AS next_time"));
  } else if (config->conn->type == HOEL_DB_TYPE_SQLITE) {
    json_array_append_new(json_object_get(j_query, "columns"), json_string("ash_next_time AS next_time"));
  }

  if (j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "scheduler_get - Error allocating resources for j_query");
    return json_pack("{si}", "result", A_ERROR_MEMORY);
  }

  if (scheduler_name != NULL || runnable) {
    json_object_set_new(j_query, "where", json_object());
  }

  if (scheduler_name != NULL) {
    json_object_set_new(json_object_get(j_query, "where"), "ash_name", json_string(scheduler_name));
  }

  if (runnable) {
    json_object_set_new(json_object_get(j_query, "where"), "ash_enabled", json_integer(1));
  }

  res = h_select(config->conn, j_query, &j_result, NULL);
  json_decref(j_query);
  if (res == H_OK) {
    if (scheduler_name == NULL) {
      to_return = json_array();
      if (to_return == NULL) {
        y_log_message(Y_LOG_LEVEL_ERROR, "scheduler_get - Error allocating resources for to_return");
        return json_pack("{si}", "result", A_ERROR_MEMORY);
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

        script_list = scheduler_get_script_list(config, json_string_value(json_object_get(j_scheduler, "name")));
        if (json_integer_value(json_object_get(script_list, "result")) == A_OK) {
          json_object_set_new(j_scheduler, "scripts", json_copy(json_object_get(script_list, "scripts")));
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "Error getting script list for scheduler %s", json_string_value(json_object_get(j_scheduler, "name")));
          json_object_set_new(j_scheduler, "scripts", json_array());
        }
        json_decref(script_list);

        json_array_append_new(to_return, json_copy(j_scheduler));
      }
      json_decref(j_result);
      return json_pack("{siso}", "result", A_OK, "schedulers", to_return);
    } else {
      if (json_array_size(j_result) == 0) {
        json_decref(j_result);
        return json_pack("{si}", "result", A_ERROR_NOT_FOUND);
      } else {
        j_scheduler = json_copy(json_array_get(j_result, 0));
        json_decref(j_result);
        if (j_scheduler == NULL) {
          y_log_message(Y_LOG_LEVEL_ERROR, "scheduler_get - Error allocating resources for j_scheduler");
          return json_pack("{si}", "result", A_ERROR_MEMORY);
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

        script_list = scheduler_get_script_list(config, json_string_value(json_object_get(j_scheduler, "name")));
        if (json_integer_value(json_object_get(script_list, "result")) == A_OK) {
          json_object_set_new(j_scheduler, "scripts", json_copy(json_object_get(script_list, "scripts")));
        } else {
          json_object_set_new(j_scheduler, "scripts", json_array());
        }
        json_decref(script_list);

        return json_pack("{siso}", "result", A_OK, "scheduler", j_scheduler);
      }
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "scheduler_get - Error allocating resources for j_query");
    return json_pack("{si}", "result", A_ERROR_MEMORY);
  }
}

/**
 * Enable or disable the specified scheduler
 */
int scheduler_enable(struct config_elements * config, json_t * j_scheduler, int enabled) {
  time_t now, next_time;

  if (j_scheduler != NULL) {
    json_object_set(j_scheduler, "enabled", enabled?json_true():json_false());
    time(&now);
    if (json_integer_value(json_object_get(j_scheduler, "repeat")) >= 0 && enabled && json_integer_value(json_object_get(j_scheduler, "next_time")) < now) {
      next_time = scheduler_calculate_next_time((time_t)json_integer_value(json_object_get(j_scheduler, "next_time")), json_integer_value(json_object_get(j_scheduler, "repeat")), json_integer_value(json_object_get(j_scheduler, "repeat_value")));
      while (next_time <= now) {
        next_time = scheduler_calculate_next_time(next_time, json_integer_value(json_object_get(j_scheduler, "repeat")), json_integer_value(json_object_get(j_scheduler, "repeat_value")));
      }

      if (next_time > 0) {
        json_object_set_new(j_scheduler, "next_time", json_integer(next_time));
      } else {
        return A_ERROR_IO;
      }
    }
    return scheduler_modify(config, json_string_value(json_object_get(j_scheduler, "name")), j_scheduler);
  } else {
    return A_ERROR_PARAM;
  }
}

/**
 * Creates a new scheduler
 */
int scheduler_add(struct config_elements * config, json_t * j_scheduler) {
  json_t * j_query;
  int res;
  char * str_options, * str_next_time;
  json_error_t error;

  if (j_scheduler == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "scheduler_add - Error j_scheduler is NULL");
    return A_ERROR_MEMORY;
  }

  if (config->conn->type == HOEL_DB_TYPE_MARIADB) {
    str_next_time = msprintf("FROM_UNIXTIME(%" JSON_INTEGER_FORMAT ")", json_integer_value(json_object_get(j_scheduler, "next_time")));
  } else if (config->conn->type == HOEL_DB_TYPE_SQLITE) {
    str_next_time = msprintf("%" JSON_INTEGER_FORMAT, json_integer_value(json_object_get(j_scheduler, "next_time")));
  } else {
    // Should not happen, but I don't like gcc warnings
    str_next_time = o_strdup("");
  }

  if (json_object_get(j_scheduler, "options") != NULL) {
    str_options = json_dumps(json_object_get(j_scheduler, "options"), JSON_COMPACT);
  } else {
    str_options = o_strdup("");
  }
  j_query = json_pack("{sss[{sssssisss{ss}sIsIsi}]}",
                      "table", ANGHARAD_TABLE_SCHEDULER,
                      "values",
                        "ash_name", json_string_value(json_object_get(j_scheduler, "name")),
                        "ash_description", json_object_get(j_scheduler, "description")!=NULL?json_string_value(json_object_get(j_scheduler, "description")):"",
                        "ash_enabled", json_object_get(j_scheduler, "enabled")==json_false()?0:1,
                        "ash_options", str_options,
                        "ash_next_time", "raw", str_next_time,
                        "ash_repeat", json_integer_value(json_object_get(j_scheduler, "repeat")),
                        "ash_repeat_value", json_integer_value(json_object_get(j_scheduler, "repeat_value")),
                        "ash_remove_after", json_object_get(j_scheduler, "remove_after")==json_false()?0:1);
  o_free(str_options);
  o_free(str_next_time);

  if (j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "scheduler_add - Error Allocating resources for j_query: %s", error.text);
    return A_ERROR_MEMORY;
  }

  res = h_insert(config->conn, j_query, NULL);
  json_decref(j_query);
  if (res == H_OK) {
    return scheduler_set_script_list(config, json_string_value(json_object_get(j_scheduler, "name")), json_object_get(j_scheduler, "scripts"));
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "scheduler_add - Error executing db query");
    return A_ERROR_DB;
  }
}

/**
 * Check if the spceified scheduler is valid
 */
json_t * is_scheduler_valid(struct config_elements * config, json_t * j_scheduler, const int update) {
  json_t * j_result = json_array(), * j_element, * j_options_valid, * j_scripts_valid;
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
        json_array_append_new(j_result, json_pack("{ss}", "name", "name parameter is mandatory must be a string, maximum 64 characters"));
      }
    }

    j_element = json_object_get(j_scheduler, "description");
    if (j_element != NULL && (!json_is_string(j_element) || json_string_length(j_element) > 128)) {
      json_array_append_new(j_result, json_pack("{ss}", "description", "description parameter must be a string, maximum 128 characters"));
    }

    j_element = json_object_get(j_scheduler, "enabled");
    if (j_element != NULL && !json_is_boolean(j_element)) {
      json_array_append_new(j_result, json_pack("{ss}", "enabled", "enabled parameter must be a boolean"));
    }

    j_element = json_object_get(j_scheduler, "next_time");
    if (j_element == NULL || !json_is_integer(j_element) || json_integer_value(j_element) < 0) {
      json_array_append_new(j_result, json_pack("{ss}", "next_time", "next_time parameter must be a valid epoch time"));
    }

    j_element = json_object_get(j_scheduler, "repeat");
    if (j_element != NULL && (!json_is_integer(j_element) || json_integer_value(j_element) < -1 || json_integer_value(j_element) > 5)) {
      json_array_append_new(j_result, json_pack("{ss}", "repeat", "repeat parameter must be an integer between -1 and 5"));
    }

    j_element = json_object_get(j_scheduler, "repeat_value");
    if (j_element != NULL && (!json_is_integer(j_element) || json_integer_value(j_element) < 0)) {
      json_array_append_new(j_result, json_pack("{ss}", "repeat_value", "repeat_value parameter must be a positive integer"));
    }

    j_element = json_object_get(j_scheduler, "remove_after");
    if (j_element != NULL && !json_is_boolean(j_element)) {
      json_array_append_new(j_result, json_pack("{ss}", "remove_after", "remove_after parameter must be a boolean"));
    }

    if (json_object_get(j_scheduler, "options") != NULL) {
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

    j_scripts_valid = is_script_list_valid(config, json_object_get(j_scheduler, "scripts"));
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

/**
 * Updates the specfied scheduler
 */
int scheduler_modify(struct config_elements * config, const char * scheduler_name, json_t * j_scheduler) {
  json_t * j_query, * cur_scheduler;
  int res, res_cur_scheduler;
  char * str_options, * str_next_time;

  if (j_scheduler == NULL || scheduler_name == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "scheduler_modify - Error j_scheduler or scheduler_name is NULL");
    return A_ERROR_MEMORY;
  }

  cur_scheduler = scheduler_get(config, scheduler_name, 0);
  res_cur_scheduler = (cur_scheduler != NULL?(int)json_integer_value(json_object_get(cur_scheduler, "result")):A_ERROR);
  json_decref(cur_scheduler);
  if (res_cur_scheduler == A_OK) {
    if (json_object_get(j_scheduler, "options") != NULL) {
      str_options = json_dumps(json_object_get(j_scheduler, "options"), JSON_COMPACT);
    } else {
      str_options = o_strdup("");
    }
    if (config->conn->type == HOEL_DB_TYPE_MARIADB) {
      str_next_time = msprintf("FROM_UNIXTIME(%" JSON_INTEGER_FORMAT ")", json_integer_value(json_object_get(j_scheduler, "next_time")));
    } else if (config->conn->type == HOEL_DB_TYPE_SQLITE) {
      str_next_time = msprintf("%" JSON_INTEGER_FORMAT, json_integer_value(json_object_get(j_scheduler, "next_time")));
    } else {
      // Should not happen
      str_next_time = o_strdup("");
    }
    j_query = json_pack("{sss{sssisss{ss}sIsIsi}s{ss}}",
                        "table", ANGHARAD_TABLE_SCHEDULER,
                        "set",
                          "ash_description", json_object_get(j_scheduler, "description")!=NULL?json_string_value(json_object_get(j_scheduler, "description")):"",
                          "ash_enabled", json_object_get(j_scheduler, "enabled")==json_false()?0:1,
                          "ash_options", str_options,
                          "ash_next_time", "raw", str_next_time,
                          "ash_repeat", json_integer_value(json_object_get(j_scheduler, "repeat")),
                          "ash_repeat_value", json_integer_value(json_object_get(j_scheduler, "repeat_value")),
                          "ash_remove_after", json_object_get(j_scheduler, "remove_after")==json_false()?0:1,
                        "where",
                          "ash_name", scheduler_name);
    o_free(str_options);
    o_free(str_next_time);

    if (j_query == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "scheduler_modify - Error Allocating resources for j_query");
      return A_ERROR_MEMORY;
    }

    res = h_update(config->conn, j_query, NULL);
    json_decref(j_query);
    if (res == H_OK) {
      return scheduler_set_script_list(config, json_string_value(json_object_get(j_scheduler, "name")), json_object_get(j_scheduler, "scripts"));
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "scheduler_modify - Error executing db query");
      return A_ERROR_DB;
    }
  } else if (res_cur_scheduler == A_ERROR_NOT_FOUND) {
    return A_ERROR_NOT_FOUND;
  } else {
    return A_ERROR;
  }
}

/**
 * Deletes the specified scheduler
 */
int scheduler_delete(struct config_elements * config, const char * scheduler_name) {
  json_t * j_query, * cur_scheduler;
  int res, res_cur_scheduler;

  if (scheduler_name == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "scheduler_modify - Error scheduler_name is NULL");
    return A_ERROR_PARAM;
  }

  cur_scheduler = scheduler_get(config, scheduler_name, 0);
  res_cur_scheduler = (cur_scheduler != NULL?(int)json_integer_value(json_object_get(cur_scheduler, "result")):A_ERROR);
  json_decref(cur_scheduler);
  if (res_cur_scheduler == A_OK) {
    j_query = json_pack("{sss{ss}}",
                        "table", ANGHARAD_TABLE_SCHEDULER,
                        "where",
                          "ash_name", scheduler_name);

    if (j_query == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "scheduler_delete - Error Allocating resources for j_query");
      return A_ERROR_MEMORY;
    }

    res = h_delete(config->conn, j_query, NULL);
    json_decref(j_query);
    if (res == H_OK) {
      return A_OK;
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "scheduler_delete - Error executing db query");
      return A_ERROR_DB;
    }
  } else if (res_cur_scheduler == A_ERROR_NOT_FOUND) {
    return A_ERROR_NOT_FOUND;
  } else {
    return A_ERROR;
  }
}

/**
 * add a new tag to the sspecified scheduler
 */
int scheduler_add_tag(struct config_elements * config, const char * scheduler_name, const char * tag) {
  json_t * j_result = scheduler_get(config, scheduler_name, 0), * j_scheduler, * j_tags;
  int res;

  if (j_result == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "scheduler_add_tag - Error getting scheduler");
    return A_ERROR_PARAM;
  } else {
    if (json_integer_value(json_object_get(j_result, "result")) == A_ERROR_NOT_FOUND) {
      json_decref(j_result);
      return A_ERROR_NOT_FOUND;
    } else if (json_integer_value(json_object_get(j_result, "result")) == A_OK) {
      j_scheduler = json_object_get(j_result, "scheduler");
      j_tags = json_object_get(json_object_get(j_scheduler, "options"), "tags");
      if (j_tags == NULL) {
        json_object_set_new(json_object_get(j_scheduler, "options"), "tags", json_pack("[s]", tag));
      } else if (json_is_array(j_tags)) {
        if (json_array_size(j_tags) < 128) {
          json_array_append_new(json_object_get(json_object_get(j_scheduler, "options"), "tags"), json_string(tag));
        } else {
          json_decref(j_result);
          return A_ERROR;
        }
      } else {
        json_decref(j_result);
        return A_ERROR;
      }
      res = scheduler_modify(config, scheduler_name, j_scheduler);
      json_decref(j_result);
      return res;
    } else {
      json_decref(j_result);
      return A_ERROR;
    }
  }
}

/**
 * Remove the specified tag from the specified scheduler
 */
int scheduler_remove_tag(struct config_elements * config, const char * scheduler_name, const char * tag) {
  json_t * j_result = scheduler_get(config, scheduler_name, 0), * j_scheduler, * j_tags;
  int i, res;

  if (j_result == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "scheduler_remove_tag - Error getting scheduler");
    return A_ERROR;
  } else {
    if (json_integer_value(json_object_get(j_result, "result")) == A_ERROR_NOT_FOUND) {
      json_decref(j_result);
      return A_ERROR_NOT_FOUND;
    } else if (json_integer_value(json_object_get(j_result, "result")) == A_OK) {
      j_scheduler = json_object_get(j_result, "scheduler");
      j_tags = json_object_get(json_object_get(j_scheduler, "options"), "tags");
      if (j_tags == NULL) {
        json_decref(j_result);
        return A_OK;
      } else if (json_is_array(j_tags)) {
        for (i = (int)json_array_size(json_object_get(json_object_get(j_scheduler, "options"), "tags"))-1; i >= 0; i--) {
          if (0 == o_strcmp(json_string_value(json_array_get(json_object_get(json_object_get(j_scheduler, "options"), "tags"), (size_t)i)), tag)) {
            json_array_remove(json_object_get(json_object_get(j_scheduler, "options"), "tags"), (size_t)i);
          }
        }
        res = scheduler_modify(config, scheduler_name, j_scheduler);
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
 * Calculate the next time for a scheduler or a monitor
 */
time_t scheduler_calculate_next_time(time_t from, json_int_t schedule_type, json_int_t schedule_value) {
  struct tm ts;
  time_t to_return;
  int isdst_from, isdst_to;

  localtime_r(&from, &ts);
  isdst_from = ts.tm_isdst;

  switch (schedule_type) {
    case SCHEDULER_REPEAT_MINUTE:
      ts.tm_min += (int)(schedule_value);
      to_return = mktime(&ts);
      isdst_to = ts.tm_isdst;
      break;
    case SCHEDULER_REPEAT_HOUR:
      ts.tm_hour += (int)(schedule_value);
      to_return = mktime(&ts);
      isdst_to = ts.tm_isdst;
      break;
    case SCHEDULER_REPEAT_DAY:
      ts.tm_mday += (int)(schedule_value);
      to_return = mktime(&ts);
      isdst_to = ts.tm_isdst;
      break;
    case SCHEDULER_REPEAT_DAY_OF_WEEK:
      if (schedule_value != 0) {
        do {
          ts.tm_wday++;
          ts.tm_mday++;
          ts.tm_wday%=7;
        } while (!((int)pow(2, (ts.tm_wday)) & schedule_value));
        to_return = mktime(&ts);
        isdst_to = ts.tm_isdst;
      } else {
        to_return = 0;
        isdst_to = isdst_from;
      }
      break;
    case SCHEDULER_REPEAT_MONTH:
      ts.tm_mon += (int)schedule_value;
      to_return = mktime(&ts);
      isdst_to = ts.tm_isdst;
      break;
    case SCHEDULER_REPEAT_YEAR:
      ts.tm_year += (int)schedule_value;
      to_return = mktime(&ts);
      isdst_to = ts.tm_isdst;
      break;
    default:
      to_return = 0;
      isdst_to = ts.tm_isdst;
      break;
  }

  // Adjusting next time with Daylight saving time if changed
  // when schedule_type is REPEAT_DAY, REPEAT_DAY_OF_WEEK, REPEAT_MONTH, REPEAT_YEAR
  if (schedule_type & (SCHEDULER_REPEAT_DAY|SCHEDULER_REPEAT_DAY_OF_WEEK|SCHEDULER_REPEAT_MONTH|SCHEDULER_REPEAT_YEAR)) {
    if (isdst_from < isdst_to) {
      ts.tm_hour--;
      to_return = mktime(&ts);
    } else if (isdst_from > isdst_to) {
      ts.tm_hour++;
      to_return = mktime(&ts);
    }
  }
  return (to_return);
}

/**
 * Get the list of script a scheduler has to run
 */
json_t * scheduler_get_script_list(struct config_elements * config, const char * scheduler_name) {
  char * escaped = h_escape_string(config->conn, scheduler_name),
        * query = msprintf("SELECT a_script.asc_name AS name, a_scheduler_script.ass_enabled AS i_enabled FROM a_script, a_scheduler_script WHERE a_scheduler_script.ash_id = (SELECT ash_id FROM a_scheduler WHERE ash_name = '%s') AND a_script.asc_id = a_scheduler_script.asc_id", escaped);
  int res;
  json_t * j_result, * j_script;
  size_t index;

  res = h_query_select_json(config->conn, query, &j_result);
  o_free(escaped);
  o_free(query);

  if (res != H_OK) {
    y_log_message(Y_LOG_LEVEL_ERROR, "scheduler_get_script_list - Error getting scheduler");
    return json_pack("{si}", "result", A_ERROR);
  } else {
    json_array_foreach(j_result, index, j_script) {
      if (json_integer_value(json_object_get(j_script, "i_enabled")) == 0) {
        json_object_set_new(j_script, "enabled", json_false());
      } else {
        json_object_set_new(j_script, "enabled", json_true());
      }
      json_object_del(j_script, "i_enabled");
    }
    return json_pack("{siso}", "result", A_OK, "scripts", j_result);
  }
}

/**
 * Updates the list of script a scheduler has to run
 */
int scheduler_set_script_list(struct config_elements * config, const char * scheduler_name, json_t * script_list) {
  json_t * j_query, * script;
  int res;
  size_t index;
  char * sch_escaped = h_escape_string(config->conn, scheduler_name), * sch_clause = msprintf("(SELECT ash_id FROM a_scheduler WHERE ash_name = '%s')", sch_escaped), * tmp = msprintf("= %s", sch_clause), * sc_escaped, * sc_clause;

  j_query = json_pack("{sss{s{ssss}}}",
                      "table",
                      ANGHARAD_TABLE_SCHEDULER_SCRIPT,
                      "where",
                        "ash_id",
                          "operator",
                          "raw",
                          "value",
                          tmp);
  o_free(tmp);

  if (j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "scheduler_set_script_list - Error allocating resources for j_query");
    return A_ERROR_MEMORY;
  }

  res = h_delete(config->conn, j_query, NULL);
  json_decref(j_query);
  if (res != H_OK) {
    y_log_message(Y_LOG_LEVEL_ERROR, "scheduler_set_script_list - Error deleting script list");
    o_free(sch_escaped);
    o_free(sch_clause);
    return A_ERROR_DB;
  } else {
    j_query = json_pack("{ssso}",
                        "table",
                          ANGHARAD_TABLE_SCHEDULER_SCRIPT,
                        "values",
                          json_array());
    if (j_query == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "scheduler_set_script_list - Error allocating resources for j_query (2)");
      o_free(sch_escaped);
      o_free(sch_clause);
      return A_ERROR_MEMORY;
    }
    json_array_foreach(script_list, index, script) {
      sc_escaped = h_escape_string(config->conn, json_string_value(json_object_get(script, "name")));
      sc_clause = msprintf("(SELECT asc_id FROM a_script WHERE asc_name = '%s')", sc_escaped);
      json_array_append_new(json_object_get(j_query, "values"),
        json_pack("{s{ss}s{ss}so}",
                  "ash_id",
                    "raw",
                    sch_clause,
                  "asc_id",
                    "raw",
                    sc_clause,
                  "ass_enabled",
                    json_object_get(script, "enabled")));
      o_free(sc_escaped);
      o_free(sc_clause);
    }

    res = h_insert(config->conn, j_query, NULL);
    json_decref(j_query);
    o_free(sch_escaped);
    o_free(sch_clause);
    if (res == H_OK) {
      return A_OK;
    } else {
      return A_ERROR_DB;
    }
  }
}
