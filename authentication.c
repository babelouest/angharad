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

json_t * auth_get(struct config_elements * config, const char * session_id) {
  json_t * j_query, * j_result;
  int res;
  time_t now;
  char * query = NULL;
  
  j_query = json_pack("{sss[ss]s{ss}}",
                      "table",
                      ANGHARAD_TABLE_SESSION,
                      "columns",
                        "UNIX_TIMESTAMP(ass_validity) AS validity",
                        "UNIX_TIMESTAMP(ass_lastseen) AS lastseen",
                      "where",
                        "ass_session_token",
                        session_id);
  res = h_select(config->conn, j_query, &j_result, &query);
  y_log_message(Y_LOG_LEVEL_DEBUG, "query is %s", query);
  json_decref(j_query);
  if (res == H_OK) {
    if (json_array_size(j_result) == 0) {
      return json_pack("{si}", "result", A_ERROR_UNAUTHORIZED);
    } else if (json_array_size(j_result) > 1) {
      y_log_message(Y_LOG_LEVEL_ERROR, "auth_get - Error in j_query result");
      return json_pack("{si}", "result", A_ERROR_DB);
    } else {
      time(&now);
      if (json_object_get(j_result, "validity") != NULL && json_object_get(j_result, "validity") != json_null()) {
        // Check if session hasn't expired
        if (json_integer_value(json_object_get(j_result, "validity")) > now) {
          return json_pack("{sis{sssi}}", "result", A_OK, "session", "token", session_id, "validity", json_integer_value(json_object_get(j_result, "validity")));
        } else {
          return json_pack("{si}", "result", A_ERROR_EXPIRED);
        }
      } else if (json_object_get(j_result, "validity") == json_null()) {
        // Check if temp session is still valid
        if (json_integer_value(json_object_get(j_result, "lastseen")) > (now - (60 * 5))) {
          return json_pack("{sis{sssi}}", "result", A_OK, "session", "token", session_id, "validity", 0);
        } else {
          return json_pack("{si}", "result", A_ERROR_EXPIRED);
        }
      } else {
        return json_pack("{si}", "result", A_ERROR_EXPIRED);
      }
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "auth_get - Error executing j_query");
    return json_pack("{si}", "result", A_ERROR_DB);
  }
}

json_t * auth_check(struct config_elements * config, const char * user, const char * password, const int validity) {
  uuid_t uuid;
  char uuid_str[37];
  int res;
  
  res = auth_check_credentials(config, user, password);
  if (res == A_OK) {
    uuid_generate_random(uuid);
    uuid_unparse_lower(uuid, uuid_str);
    y_log_message(Y_LOG_LEVEL_DEBUG, "uuid generated is %s", uuid_str);
    return json_pack("{sissssi}}", "result", A_OK, "session", "token", uuid_str, "validity", validity);
  } else if (res == A_ERROR_UNAUTHORIZED) {
    return json_pack("{si}", "result", A_ERROR_UNAUTHORIZED);
  } else {
    return json_pack("{si}", "result", A_ERROR);
  }
}

int auth_check_credentials(struct config_elements * config, const char * user, const char * password) {
  if (config->auth_type == ANGHARAD_AUTH_TYPE_NONE) {
    return A_OK;
  } else if (config->auth_type == ANGHARAD_AUTH_TYPE_DATABASE) {
    return auth_check_credentials_database(config, user, password);
  } else if (config->auth_type == ANGHARAD_AUTH_TYPE_LDAP) {
    return auth_check_credentials_ldap(config, user, password);
  } else {
    return A_ERROR_PARAM;
  }
}

int auth_check_credentials_database(struct config_elements * config, const char * user, const char * password) {
  return A_ERROR_UNAUTHORIZED;
}

int auth_check_credentials_ldap(struct config_elements * config, const char * user, const char * password) {
  return A_ERROR_UNAUTHORIZED;
}

