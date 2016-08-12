/**
 *
 * Angharad system
 *
 * Manage all subsystems and handles the house automation server:
 * - Benoic
 * - Gareth
 * - Carleon
 * 
 * Database users management functions
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

json_t * user_get(struct config_elements * config, const char * user_name) {
  json_t * j_query, * j_result, * j_user, * to_return;
  int res;
  size_t index;
  
  j_query = json_pack("{sss[ss]}", "table", ANGHARAD_TABLE_USER, "columns", "au_login AS login", "au_enabled");
  
  if (j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "user_get - Error allocating resources for j_query");
    return json_pack("{si}", "result", A_ERROR_MEMORY);
  }
  
  if (user_name != NULL) {
    json_object_set_new(j_query, "where", json_pack("{ss}", "au_login", user_name));
  }
  
  res = h_select(config->conn, j_query, &j_result, NULL);
  json_decref(j_query);
  if (res == H_OK) {
    if (user_name == NULL) {
      to_return = json_array();
      if (to_return == NULL) {
        y_log_message(Y_LOG_LEVEL_ERROR, "user_get - Error allocating resources for to_return");
        return json_pack("{si}", "result", A_ERROR_MEMORY);
      }
      json_array_foreach(j_result, index, j_user) {
        json_object_set_new(j_user, "enabled", json_integer_value(json_object_get(j_user, "au_enabled"))==1?json_true():json_false());
        json_object_del(j_user, "au_enabled");
        json_array_append_new(to_return, json_copy(j_user));
      }
      json_decref(j_result);
      return json_pack("{siso}", "result", A_OK, "users", to_return);
    } else {
      if (json_array_size(j_result) == 0) {
        json_decref(j_result);
        return json_pack("{si}", "result", A_ERROR_NOT_FOUND);
      } else {
        j_user = json_copy(json_array_get(j_result, 0));
        json_decref(j_result);
        if (j_user == NULL) {
          y_log_message(Y_LOG_LEVEL_ERROR, "user_get - Error allocating resources for j_user");
          return json_pack("{si}", "result", A_ERROR_MEMORY);
        }
        json_object_set_new(j_user, "enabled", json_integer_value(json_object_get(j_user, "au_enabled"))==1?json_true():json_false());
        json_object_del(j_user, "au_enabled");
        return json_pack("{siso}", "user", A_OK, "user", j_user);
      }
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "user_get - Error executing j_query");
    return json_pack("{si}", "result", A_ERROR_MEMORY);
  }
}

json_t * is_user_valid(struct config_elements * config, json_t * j_user, const int update) {
  json_t * to_return = json_array(), * element;
  if (to_return == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "is_user_valid - Error allocating resources for to_return");
  } else {
    if (j_user == NULL || !json_is_object(j_user)) {
      json_array_append_new(to_return, json_pack("{ss}", "user", "user must be a json object"));
    } else {
      if (!update) {
        element = json_object_get(j_user, "login");
        if (element == NULL || !json_is_string(element) || json_string_length(element) > 128 || json_string_length(element) == 0) {
          json_array_append_new(to_return, json_pack("{ss}", "login", "login is mandatory and must be a string up to 128 characters"));
        }
      }
      
      element = json_object_get(j_user, "password");
      if (element == NULL || !json_is_string(element) || json_string_length(element) > 128 || json_string_length(element) == 0) {
        json_array_append_new(to_return, json_pack("{ss}", "password", "password is mandatory and must be a string up to 128 characters"));
      }
      
      element = json_object_get(j_user, "enabled");
      if (element != NULL && !json_is_boolean(element)) {
        json_array_append_new(to_return, json_pack("{ss}", "enabled", "enabled must be a boolean"));
      }
    }
  }
  return to_return;
}

int user_add(struct config_elements * config, json_t * j_user) {
  json_t * j_query;
  int res;
  char * escaped_password = h_escape_string(config->conn, json_string_value(json_object_get(j_user, "password"))), 
       * str_password = escaped_password!=NULL?msprintf("PASSWORD('%s')", escaped_password):NULL;
  
  if (j_user == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "user_add - Error j_user is NULL");
    return A_ERROR_MEMORY;
  }
  
  j_query = json_pack("{sss[{sss{ss}si}]}",
                      "table", ANGHARAD_TABLE_USER,
                      "values",
                        "au_login", json_string_value(json_object_get(j_user, "login")),
                        "au_password",
                          "raw",
                          str_password,
                        "au_enabled", json_object_get(j_user, "enabled")==json_false()?0:1);
  
  free(escaped_password);
  free(str_password);
  if (j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "user_add - Error Allocating resources for j_query");
    return A_ERROR_MEMORY;
  }
  
  res = h_insert(config->conn, j_query, NULL);
  json_decref(j_query);
  if (res == H_OK) {
    return A_OK;
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "user_add - Error executing db query");
    return A_ERROR_DB;
  }
}

int user_modify(struct config_elements * config, const char * user_name, json_t * j_user) {
  json_t * j_query;
  int res;
  char * escaped_password = h_escape_string(config->conn, json_string_value(json_object_get(j_user, "password"))), 
       * str_password = escaped_password!=NULL?msprintf("PASSWORD('%s')", escaped_password):NULL;
  
  if (j_user == NULL || user_name == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "user_modify - Error j_user or user_name is NULL");
    return A_ERROR_MEMORY;
  }
  
  j_query = json_pack("{sss{s{ss}si}s{ss}}",
                      "table", ANGHARAD_TABLE_USER,
                      "set",
                        "au_password",
                          "raw",
                          str_password,
                        "au_enabled", json_object_get(j_user, "enabled")==json_false()?0:1,
                      "where",
                        "au_login", user_name);
  free(escaped_password);
  free(str_password);
  
  if (j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "user_modify - Error Allocating resources for j_query");
    return A_ERROR_MEMORY;
  }
  
  res = h_update(config->conn, j_query, NULL);
  json_decref(j_query);
  if (res == H_OK) {
    return A_OK;
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "user_modify - Error executing db query");
    return A_ERROR_DB;
  }
}

int user_delete(struct config_elements * config, const char * user_name) {
  json_t * j_query;
  int res;
  
  if (user_name == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "user_delete - Error user_name is NULL");
    return A_ERROR_MEMORY;
  }
  
  j_query = json_pack("{sss{ss}}",
                      "table", ANGHARAD_TABLE_USER,
                      "where",
                        "au_login", user_name);
  
  if (j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "user_delete - Error Allocating resources for j_query");
    return A_ERROR_MEMORY;
  }
  
  res = h_delete(config->conn, j_query, NULL);
  json_decref(j_query);
  if (res == H_OK) {
    return A_OK;
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "user_delete - Error executing db query");
    return A_ERROR_DB;
  }
}

json_t * token_get_list(struct config_elements * config) {
  json_t * j_query, * j_result, * j_token;
  int res;
  size_t index;
  
  j_query = json_pack("{sss[sssss]}", "table", ANGHARAD_TABLE_SESSION, "columns", "ass_session_token AS token", "ass_enabled", "ass_login AS login", "ass_validity AS validity", "ass_lastseen");
  
  if (j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "token_get_list - Error allocating resources for j_query");
    return json_pack("{si}", "result", A_ERROR_MEMORY);
  }
  
  res = h_select(config->conn, j_query, &j_result, NULL);
  json_decref(j_query);
  if (res == H_OK) {
    json_array_foreach(j_result, index, j_token) {
      json_object_set_new(j_token, "enabled", json_integer_value(json_object_get(j_token, "ass_enabled"))==1?json_true():json_false());
      json_object_del(j_token, "ass_enabled");
    }
    return json_pack("{siso}", "result", A_OK, "tokens", j_result);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "token_get_list - Error executing j_query");
    return json_pack("{si}", "result", A_ERROR_MEMORY);
  }
}

int token_revoke(struct config_elements * config, json_t * token) {
  json_t * j_query;
  int res;
  
  if (token != NULL && json_is_object(token) && json_object_get(token, "token") != NULL && json_is_string(json_object_get(token, "token"))) {
    j_query = json_pack("{sss{si}s{ss}}", 
                        "table", 
                        ANGHARAD_TABLE_SESSION, 
                        "set",
                          "ass_enabled",
                          0,
                        "where",
                          "ass_session_token",
                          json_string_value(json_object_get(token, "token")));
    if (j_query == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "token_revoke - Error allocating resources for j_query");
      return A_ERROR_MEMORY;
    } else {
      res = h_update(config->conn, j_query, NULL);
      json_decref(j_query);
      return res==H_OK?A_OK:A_ERROR_DB;
    }
  } else {
    return A_ERROR_PARAM;
  }
}
