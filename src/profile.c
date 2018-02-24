/**
 *
 * Angharad system
 *
 * Manage all subsystems and handles the house automation server:
 * - Benoic
 * - Gareth
 * - Carleon
 *
 * Profile managament functions
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

/**
 * Get the list of all profiles
 */
json_t * profile_list(struct config_elements * config) {
  json_t * j_query, * j_result, * profile, * to_return;
  int res;
  size_t index;
  
  if (config == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "profile_list - Error no config");
    return json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  }
  
  j_query = json_pack("{sss[s]}", "table", ANGHARAD_TABLE_PROFILE, "columns", "ap_name");
  
  if (j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "profile_list - Error allocating resources for j_query");
    return NULL;
  }
  
  res = h_select(config->conn, j_query, &j_result, NULL);
  json_decref(j_query);
  if (res == H_OK) {
    to_return = json_array();
    if (to_return == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "profile_list - Error allocating resources for to_return");
      json_decref(j_result);
      return json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
    }
    json_array_foreach(j_result, index, profile) {
	  json_t * j_profile = profile_get(config, json_string_value(json_object_get(profile, "ap_name")));
	  if (j_profile != NULL && json_integer_value(json_object_get(j_profile, "result")) == WEBSERVICE_RESULT_OK) {
		json_array_append_new(to_return, json_copy(json_object_get(j_profile, "profile")));
      }
      json_decref(j_profile);
    }
    json_decref(j_result);
    return json_pack("{siso}", "result", WEBSERVICE_RESULT_OK, "list", to_return);
  } else {
    return json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  }
}

/**
 * Get the specified profile
 */
json_t * profile_get(struct config_elements * config, const char * profile_id) {
  json_t * j_query, * j_result, * profile;
  int res;
  
  if (config == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "profile_get - Error no config");
    return json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  }
  
  j_query = json_pack("{sss{ss}}", "table", ANGHARAD_TABLE_PROFILE, "where", "ap_name", profile_id);
  
  if (j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "profile_get - Error allocating resources for j_query");
    return json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  }
  
  res = h_select(config->conn, j_query, &j_result, NULL);
  json_decref(j_query);
  if (res == H_OK) {
    if (json_array_size(j_result) == 0) {
      json_decref(j_result);
      return json_pack("{si}", "result", WEBSERVICE_RESULT_NOT_FOUND);
    } else {
      profile = json_loads(json_string_value(json_object_get(json_array_get(j_result, 0), "ap_data")), JSON_DECODE_ANY, NULL);
      json_decref(j_result);
      if (profile == NULL) {
        profile = json_object();
        if (profile == NULL) {
          y_log_message(Y_LOG_LEVEL_ERROR, "profile_get - Error allocating resources for profile");
          return json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
        }
      }
      return json_pack("{siso}", "result", WEBSERVICE_RESULT_OK, "profile", profile);
    }
  } else {
    return NULL;
  }
}

/**
 * Update the specified profile
 */
int profile_modify(struct config_elements * config, const char * profile_id, json_t * profile_data) {
  json_t * profile = profile_get(config, profile_id), * j_query;
  char * str_data;
  int res;
  
  if (profile_data == NULL || profile_id == NULL) {
    json_decref(profile);
    return C_ERROR_PARAM;
  }
  
  if (profile != NULL && json_integer_value(json_object_get(profile, "result")) == WEBSERVICE_RESULT_NOT_FOUND) {
    json_decref(profile);
    str_data = json_dumps(profile_data, JSON_COMPACT);
    j_query = json_pack("{sss{ssss}}", "table", ANGHARAD_TABLE_PROFILE, "values", "ap_data", str_data, "ap_name", profile_id);
    free(str_data);
    res = h_insert(config->conn, j_query, NULL);
    json_decref(j_query);
    if (res == H_OK) {
      return C_OK;
    } else {
      return C_ERROR_DB;
    }
  } else if (profile != NULL && json_integer_value(json_object_get(profile, "result")) == WEBSERVICE_RESULT_OK) {
    json_decref(profile);
    str_data = json_dumps(profile_data, JSON_COMPACT);
    j_query = json_pack("{sss{ss}s{ss}}", "table", ANGHARAD_TABLE_PROFILE, "set", "ap_data", str_data, "where", "ap_name", profile_id);
    free(str_data);
    res = h_update(config->conn, j_query, NULL);
    json_decref(j_query);
    if (res == H_OK) {
      return C_OK;
    } else {
      return C_ERROR_DB;
    }
  } else {
    json_decref(profile);
    return WEBSERVICE_RESULT_ERROR;
  }
}

/**
 * Delete the specified profile
 */
int profile_delete(struct config_elements * config, const char * profile_id) {
  json_t * profile = profile_get(config, profile_id), * j_query;
  int res;
  
  if (profile_id == NULL) {
    json_decref(profile);
    return C_ERROR_PARAM;
  }
  
  if (profile != NULL && json_integer_value(json_object_get(profile, "result")) == WEBSERVICE_RESULT_NOT_FOUND) {
    json_decref(profile);
    return C_ERROR_NOT_FOUND;
  } else if (profile != NULL && json_integer_value(json_object_get(profile, "result")) == WEBSERVICE_RESULT_OK) {
    json_decref(profile);
    j_query = json_pack("{sss{ss}}", "table", ANGHARAD_TABLE_PROFILE, "where", "ap_name", profile_id);
    res = h_delete(config->conn, j_query, NULL);
    json_decref(j_query);
    if (res == H_OK) {
      return C_OK;
    } else {
      return C_ERROR_DB;
    }
  } else {
    json_decref(profile);
    return WEBSERVICE_RESULT_ERROR;
  }
}
