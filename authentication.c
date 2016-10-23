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

#include <uuid/uuid.h>
#include <ldap.h>

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
                          config->conn->type==HOEL_DB_TYPE_MARIADB?"UNIX_TIMESTAMP(ass_validity) AS validity":"ass_validity AS validity",
                          config->conn->type==HOEL_DB_TYPE_MARIADB?"UNIX_TIMESTAMP(ass_lastseen) AS lastseen":"ass_validity AS validity",
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
                            config->conn->type==HOEL_DB_TYPE_MARIADB?">= NOW()":">= strftime('%s','now')");
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
      if (!config->has_auth_database && !config->has_auth_ldap) {
        // Generate fake session
        tm = localtime(&now);
        tm->tm_year += 10;
        j_return = json_pack("{sis{sssi}}", "result", A_OK, "session", "token", "00000000-0000-0000-0000-000000000000", "validity", mktime(tm));
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
      j_return = json_pack("{sis{sssI}}", "result", A_OK, "session", "token", session_id, "validity", json_integer_value(json_object_get(json_array_get(j_result, 0), "validity")));
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
  
  if (!config->has_auth_database && !config->has_auth_ldap) {
    // Generate new session
    time(&now);
    tm = localtime(&now);
    tm->tm_year += 10;
    j_return = auth_generate_new_token(config, NULL, mktime(tm));
  } else {
    if (auth_check_credentials(config, login, password) == A_OK) {
      // if validity is 0 (session only), set it to 1 day, but the cookie will be session only
      j_return = auth_generate_new_token(config, login, (validity?validity:(24*60*60)));
    } else {
      // Return unauthorized
      j_return = json_pack("{si}", "result", A_ERROR_UNAUTHORIZED);
    }
  }
  return j_return;
}

/**
 * Inalidates a session_id by setting its enabled flag to false
 */
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

/**
 * Updates the last_seen value of the specified session
 */
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
                            config->conn->type==HOEL_DB_TYPE_MARIADB?"NOW()":"strftime('%s','now')",
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
      y_log_message(Y_LOG_LEVEL_ERROR, "Error setting last_seen date in the database");
      return A_ERROR_DB;
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "Error, no session provided");
    return A_ERROR_PARAM;
  }
}

/**
 * Authentication check
 * 
 */
int auth_check_credentials(struct config_elements * config, const char * login, const char * password) {
  int res = A_ERROR_UNAUTHORIZED;
  
  if (!config->has_auth_database && !config->has_auth_ldap) {
    return A_OK;
  } else {
    if (config->has_auth_ldap) {
      res = auth_check_credentials_ldap(config, login, password);
    }
    if (config->has_auth_database && res != A_OK) {
      res = auth_check_credentials_database(config, login, password);
    }
  }
  return res;
}

/**
 * Generates a new token for the specified login and the specified validity
 */
json_t * auth_generate_new_token(struct config_elements * config, const char * login, int validity) {
  json_t * j_query, * j_login = login==NULL?json_null():json_string(login);
  uuid_t uuid;
  char uuid_str[37], * str_validity;
  int res;
  
  uuid_generate_random(uuid);
  uuid_unparse_lower(uuid, uuid_str);
  str_validity = msprintf(config->conn->type==HOEL_DB_TYPE_MARIADB?"FROM_UNIXTIME(%d)":"%d", validity);
  
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
                          config->conn->type==HOEL_DB_TYPE_MARIADB?"NOW()":"strftime('%s','now')");
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

/**
 * Check if the login and password specified are valid as a database user
 */
int auth_check_credentials_database(struct config_elements * config, const char * login, const char * password) {
  json_t * j_query, * j_result;
  char * escaped, * str_password;
  int res, res_size;
  
  if (nstrlen(login) <= 0 || nstrlen(password) <= 0) {
    return A_ERROR_UNAUTHORIZED;
  } else {
    escaped = h_escape_string(config->conn, password);
    if (config->conn->type == HOEL_DB_TYPE_SQLITE) {
      str_password = msprintf("= HEX('%s')", escaped);
    } else {
      str_password = msprintf("= PASSWORD('%s')", escaped);
    }
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

/**
 * Check if the login and password specified are valid as a LDAP user
 */
int auth_check_credentials_ldap(struct config_elements * config, const char * login, const char * password) {
  LDAP * ldap;
  LDAPMessage * answer, * entry;
  
  int  result, result_login;
  int  ldap_version   = LDAP_VERSION3;
  int  scope          = LDAP_SCOPE_SUBTREE;
  char * filter       = NULL;
  char *attrs[]       = {"memberOf", NULL};
  int  attrsonly      = 0;
  char * user_dn      = NULL;
  int res = A_ERROR;

  if (ldap_initialize(&ldap, config->auth_ldap->uri) != LDAP_SUCCESS) {
    y_log_message(Y_LOG_LEVEL_ERROR, "Error initializing ldap");
    res = A_ERROR_PARAM;
  } else if (ldap_set_option(ldap, LDAP_OPT_PROTOCOL_VERSION, &ldap_version) != LDAP_OPT_SUCCESS) {
    y_log_message(Y_LOG_LEVEL_ERROR, "Error setting ldap protocol version");
    res = A_ERROR_PARAM;
  } else if ((result = ldap_simple_bind_s(ldap, config->auth_ldap->bind_dn, config->auth_ldap->bind_passwd)) != LDAP_OPT_SUCCESS) {
    y_log_message(Y_LOG_LEVEL_ERROR, "Error binding to ldap server: %s", ldap_err2string(result));
    res = A_ERROR_PARAM;
  } else {
    // Connection successful, doing ldap search
    filter = msprintf("(&(%s)(%s=%s))", config->auth_ldap->filter, config->auth_ldap->login_property, login);
    
    if (filter != NULL && (result = ldap_search_s(ldap, config->auth_ldap->base_search, scope, filter, attrs, attrsonly, &answer)) != LDAP_SUCCESS) {
      y_log_message(Y_LOG_LEVEL_ERROR, "Error ldap search: %s", ldap_err2string(result));
      res = A_ERROR_PARAM;
    } else if (ldap_count_entries(ldap, answer) == 0) {
      // No result found for login
      res = A_ERROR_UNAUTHORIZED;
    } else {
      // ldap found some results, getting the first one
      entry = ldap_first_entry(ldap, answer);
      
      if (entry == NULL) {
        y_log_message(Y_LOG_LEVEL_ERROR, "ldap search: error getting first result");
        res = A_ERROR;
      } else {
        // Testing the first result to login with the given password
        user_dn = ldap_get_dn(ldap, entry);
        result_login = ldap_simple_bind_s(ldap, user_dn, password);
        ldap_memfree(user_dn);
        if (result_login == LDAP_SUCCESS) {
          res = A_OK;
        } else {
          res = A_ERROR_UNAUTHORIZED;
        }
      }
    }
    free(filter);
    ldap_msgfree(answer);
  }
  ldap_unbind(ldap);
  return res;
}
