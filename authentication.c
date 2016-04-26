/**
 *
 * Angharad system
 *
 * Manage all subsystems and handles the house automation server:
 * - Benoic
 * - Gareth
 * - Carleon
 * 
 * authentication management functions
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
 * 
 * Get authentication:
 * 
 *  if token valid then
 *   return token and validity
 *  else if no authorization
 *   generate new token with validity of 10 years and no login
 *   store new session with login and validity
 *   return new token session with validity
 *  else
 *   return unauthorized
 *  end
 * end
 * 
 */
json_t * auth_get(struct config_elements * config, const char * session_id) {
  json_t * j_query, * j_result = NULL, * j_return;
  int res, res_size;
  time_t now;
  struct tm* tm;
  
  time(&now);
  
  if (session_id != NULL) {
    j_query = json_pack("{sss[sss]s{sssis{ssss}}}",
                        "table",
                        ANGHARAD_TABLE_SESSION,
                        "columns",
                          "UNIX_TIMESTAMP(ass_validity) AS validity",
                          "UNIX_TIMESTAMP(ass_lastseen) AS lastseen",
                          "ass_login AS login",
                        "where",
                          "ass_session_token",
                          session_id,
                          "ass_enabled",
                          1,
                          "ass_validity",
                            "operator",
                            "raw",
                            "value",
                            ">= NOW()");
    res = h_select(config->conn, j_query, &j_result, NULL);
    json_decref(j_query);
  } else {
    res = H_OK;
  }
  if (res == H_OK) {
    if (session_id != NULL) {
      res_size = json_array_size(j_result);
    } else {
      res_size = 0;
    }
    if (res_size == 0) {
      // No session or wrong session
      if (config->auth_type == ANGHARAD_AUTH_TYPE_NONE) {
        // Generate new session
        tm = localtime(&now);
        tm->tm_year += 10;
        j_return = auth_generate_new_token(config, NULL, mktime(tm));
      } else {
        // Return unauthorized
        j_return = json_pack("{si}", "result", A_ERROR_UNAUTHORIZED);
      }
    } else if (res_size > 1) {
      // Obviously an error
      y_log_message(Y_LOG_LEVEL_ERROR, "auth_get - Error in j_query result");
      j_return = json_pack("{si}", "result", A_ERROR_DB);
    } else {
      // Session found, return it
      j_return = json_pack("{sis{sssi}}", "result", A_OK, "session", "token", session_id, "validity", json_integer_value(json_object_get(json_array_get(j_result, 0), "validity")));
    }
    json_decref(j_result);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "auth_get - Error executing j_query");
    j_return = json_pack("{si}", "result", A_ERROR_DB);
  }
  return j_return;
}

/**
 * 
 * Check authentication:
 * 
 * if no authentication then
 *  generate new token with validity of 10 years and no login
 *  store new session with login and validity
* *  return new token session with validity
 * else
 *  if credentials valid then
 *   generate new token with validity specified (default 0)
 *   store new session with login and validity
 *   return new token session with validity
 *  else
 *   return unauthorized
 *  end
 * end
 * 
 */
json_t * auth_check(struct config_elements * config, const char * login, const char * password, const int validity) {
  json_t * j_return;
  time_t now;
  struct tm* tm;
  
  if (config->auth_type == ANGHARAD_AUTH_TYPE_NONE) {
    // Generate new session
    time(&now);
    tm = localtime(&now);
    tm->tm_year += 10;
    j_return = auth_generate_new_token(config, NULL, mktime(tm));
  } else {
    if (auth_check_credentials(config, login, password) == A_OK) {
      j_return = auth_generate_new_token(config, login, validity);
    } else {
      // Return unauthorized
      j_return = json_pack("{si}", "result", A_ERROR_UNAUTHORIZED);
    }
  }
  return j_return;
}

int auth_invalidate(struct config_elements * config, const char * session_id) {
  json_t * j_query;
  int res;
  
  if (session_id != NULL) {
    j_query = json_pack("{sss{si}s{ss}}", 
                        "table", 
                        ANGHARAD_TABLE_SESSION, 
                        "set", 
                          "ass_enabled",
                          0,
                        "where",
                          "ass_session_token",
                          session_id);
    res = h_update(config->conn, j_query, NULL);
    json_decref(j_query);
    if (res == H_OK) {
      return A_OK;
    } else {
      return A_ERROR_DB;
    }
  } else {
    return A_ERROR_PARAM;
  }
}

int auth_update_last_seen(struct config_elements * config, const char * session_id) {
  json_t * j_query;
  int res;
  
  if (session_id != NULL) {
    j_query = json_pack("{sss{s{ss}s{ss}}s{ss}}", 
                        "table", 
                        ANGHARAD_TABLE_SESSION, 
                        "set", 
                          "ass_lastseen",
                            "raw",
                            "NOW()",
                          "ass_validity",
                            "raw",
                            "ass_validity", // This is to avoid mysql changing the value
                        "where",
                          "ass_session_token",
                          session_id);
    res = h_update(config->conn, j_query, NULL);
    json_decref(j_query);
    if (res == H_OK) {
      return A_OK;
    } else {
      return A_ERROR_DB;
    }
  } else {
    return A_ERROR_PARAM;
  }
}

int auth_check_credentials(struct config_elements * config, const char * login, const char * password) {
  if (config->auth_type == ANGHARAD_AUTH_TYPE_NONE) {
    return A_OK;
  } else if (config->auth_type == ANGHARAD_AUTH_TYPE_DATABASE) {
    return auth_check_credentials_database(config, login, password);
  } else if (config->auth_type == ANGHARAD_AUTH_TYPE_LDAP) {
    return auth_check_credentials_ldap(config, login, password);
  } else {
    return A_ERROR_PARAM;
  }
}

json_t * auth_generate_new_token(struct config_elements * config, const char * login, int validity) {
  json_t * j_query, * j_login = login==NULL?json_null():json_string(login);
  uuid_t uuid;
  char uuid_str[37], * str_validity;
  int res;
  
  uuid_generate_random(uuid);
  uuid_unparse_lower(uuid, uuid_str);
  str_validity = msprintf("FROM_UNIXTIME(%d)", validity);
  
  j_query = json_pack("{sss[{sssos{ss}s{ss}}]}",
                      "table",
                      ANGHARAD_TABLE_SESSION,
                      "values",
                        "ass_session_token",
                        uuid_str,
                        "ass_login",
                        j_login,
                        "ass_validity",
                          "raw",
                          str_validity,
                        "ass_lastseen",
                          "raw",
                          "NOW()");
  res = h_insert(config->conn, j_query, NULL);
  json_decref(j_query);
  free(str_validity);
  if (res == H_OK) {
    return json_pack("{sis{sssi}}", "result", A_OK, "session", "token", uuid_str, "validity", validity);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "auth_generate_new_token - Error database");
    return json_pack("{si}", "result", A_ERROR_DB);
  }
}

int auth_check_credentials_database(struct config_elements * config, const char * login, const char * password) {
  json_t * j_query, * j_result;
  char * escaped, * str_password;
  int res, res_size;
  
  if (nstrlen(login) <= 0 || nstrlen(password) <= 0) {
    return A_ERROR_UNAUTHORIZED;
  } else {
    escaped = h_escape_string(config->conn, password);
    str_password = msprintf("= PASSWORD('%s')", escaped);
    free(escaped);
    j_query = json_pack("{sss{sss{ssss}si}}",
                        "table",
                        ANGHARAD_TABLE_USER,
                        "where",
                          "au_login",
                          login,
                          "au_password",
                            "operator",
                            "raw",
                            "value",
                            str_password,
                          "au_enabled",
                          1);
    
    res = h_select(config->conn, j_query, &j_result, NULL);
    json_decref(j_query);
    free(str_password);
    if (res == H_OK) {
      res_size = json_array_size(j_result);
      json_decref(j_result);
      if (res_size == 0) {
        return A_ERROR_UNAUTHORIZED;
      } else if (res_size == 1) {
        return A_OK;
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "Error in database while getting credentials (obviously)");
        return A_ERROR_DB;
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "Error in database while executing query");
      return A_ERROR_DB;
    }
  }
}

int auth_check_credentials_ldap(struct config_elements * config, const char * login, const char * password) {
  return A_ERROR_UNAUTHORIZED;
}

