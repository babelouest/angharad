/**
 *
 * Gareth messenger service
 *
 * Get messages from a REST Webservice
 * Send messages or digested data when previously parametered filters are triggered
 * Send protocols available: http, smtp
 *
 * alert related functions definitions
 *
 * Copyright 2015-2016 Nicolas Mora <mail@babelouest.org>
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
#include "gareth.h"

/*************************
 * SMTP Alert management *
 *************************/

/**
 * get_smtp_alert
 * Get an smtp alert specified by its name, or all alerts if name is NULL
 * return a json_t pointer containing an object or an array of objects
 * returned value must be free'd after use
 */
json_t * get_smtp_alert(struct _h_connection * conn, const char * name) {
  int res;
  json_t * where_clause, * j_result = NULL, * j_return = NULL, * j_smtp_alert, * j_query;
  size_t index;
  
  if (name == NULL) {
    where_clause = NULL;
  } else {
    where_clause = json_object();
    if (where_clause ==NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "get_smtp_alert - Error allocating resource where_clause");
      return NULL;
    }
    json_object_set_new(where_clause, COLUMN_ALERT_SMTP_NAME, json_string(name));
  }
  
  j_query = json_object();
  if (j_query ==NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "get_smtp_alert - Error allocating resource j_query");
    json_decref(where_clause);
    return NULL;
  }
  
  json_object_set_new(j_query, "table", json_string(TABLE_ALERT_SMTP));
  json_object_set_new(j_query, "where", where_clause);
  
  res = h_select(conn, j_query, &j_result, NULL);
  
  json_decref(j_query);
  if (res == H_OK) {
    if (name != NULL) {
      // Extracting the first result (there should be only one) and return it as an object
      j_smtp_alert = json_array_get(j_result, 0);
      if (j_smtp_alert != NULL) {
        j_return = parse_smtp_alert_from_db(j_smtp_alert);
      } else {
        json_decref(j_result);
        return NULL;
      }
    } else {
      // Extracting all results and return them as an array of objects
      j_return = json_array();
      if (j_return != NULL) {
        json_array_foreach(j_result, index, j_smtp_alert) {
          json_array_append_new(j_return, parse_smtp_alert_from_db(j_smtp_alert));
        }
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "get_smtp_alert - Error allocating j_return");
        return NULL;
      }
    }
    json_decref(j_result);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "get_smtp_alert - Error getting smtp alert from database");
  }
  return j_return;
}

/**
 * parse_smtp_alert_from_db
 * converts an smtp alert from the database to a valid smtp alert for http output
 * returned value must be free'd after use
 */
json_t * parse_smtp_alert_from_db(const json_t * j_result) {
  json_t * json_smtp = json_object(), * j_tls, * j_check_ca;
  
  if (j_result == NULL || !json_is_object(j_result) || json_smtp == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "parse_smtp_alert_from_db - Error allocating resources or inpur parameters");
    json_decref(json_smtp);
    return NULL;
  } else {
    json_object_set_new(json_smtp, "name", json_copy(json_object_get(j_result, COLUMN_ALERT_SMTP_NAME)));
    json_object_set_new(json_smtp, "host", json_copy(json_object_get(j_result, COLUMN_ALERT_SMTP_HOST)));
    json_object_set_new(json_smtp, "port", json_copy(json_object_get(j_result, COLUMN_ALERT_SMTP_PORT)));
    json_object_set_new(json_smtp, "from", json_copy(json_object_get(j_result, COLUMN_ALERT_SMTP_FROM)));
    json_object_set_new(json_smtp, "to", json_copy(json_object_get(j_result, COLUMN_ALERT_SMTP_TO)));
    json_object_set_new(json_smtp, "subject", json_copy(json_object_get(j_result, COLUMN_ALERT_SMTP_SUBJECT)));
    json_object_set_new(json_smtp, "body", json_copy(json_object_get(j_result, COLUMN_ALERT_SMTP_BODY)));
    json_object_set_new(json_smtp, "user", json_copy(json_object_get(j_result, COLUMN_ALERT_SMTP_USER)));
    json_object_set_new(json_smtp, "password", json_copy(json_object_get(j_result, COLUMN_ALERT_SMTP_PASSWORD)));
    json_object_set_new(json_smtp, "cc", json_copy(json_object_get(j_result, COLUMN_ALERT_SMTP_CC)));
    json_object_set_new(json_smtp, "bcc", json_copy(json_object_get(j_result, COLUMN_ALERT_SMTP_BCC)));
    
    j_tls = json_object_get(j_result, COLUMN_ALERT_SMTP_TLS);
    json_object_set_new(json_smtp, "tls", json_integer_value(j_tls)==1?json_true():json_false());
    
    j_check_ca = json_object_get(j_result, COLUMN_ALERT_SMTP_CHECK_CA);
    json_object_set_new(json_smtp, "check_ca", json_integer_value(j_check_ca)==1?json_true():json_false());
    
    return json_smtp;
  }
}

/**
 * is_smtp_alert_valid
 * return 1 if the specified smtp alert has valid parameters, 0 otherwise
 */
json_t * is_smtp_alert_valid(const json_t * input_smtp, int update) {
  json_t * j_name, * j_host, * j_port, * j_from, * j_to, * j_subject, * j_body, * j_user, * j_password, * j_cc, * j_bcc, * j_tls, * j_check_ca, * j_message = json_array();
  if (input_smtp != NULL && j_message != NULL) {
    if (!update) {
      j_name = json_object_get(input_smtp, "name");
      if (j_name == NULL || !json_is_string(j_name) || json_string_length(j_name) > 64 || json_string_length(j_name) <= 0) {
          json_array_append_new(j_message, json_pack("{ss}", "name", "name attribute is mandatory and must be a string value between 1 and 64 characters"));
      }
    }
    
    j_host = json_object_get(input_smtp, "host");
    if (j_host == NULL || !json_is_string(j_host) || json_string_length(j_host) > 128 || json_string_length(j_host) <= 0) {
        json_array_append_new(j_message, json_pack("{ss}", "host", "host attribute is mandatory and must be a string value between 1 and 128 characters"));
    }
    
    j_port = json_object_get(input_smtp, "port");
    if (j_port != NULL && !json_is_null(j_port) && (!json_is_integer(j_port) || json_integer_value(j_port) > 65535 || json_integer_value(j_port) < 0)) {
        json_array_append_new(j_message, json_pack("{ss}", "port", "port attribute must be an integer value between 1 and 65535"));
    }
    
    j_tls = json_object_get(input_smtp, "tls");
    if (j_tls != NULL && !json_is_null(j_tls) && !json_is_boolean(j_tls)) {
        json_array_append_new(j_message, json_pack("{ss}", "tls", "tls attribute must be a boolean"));
    }

    j_check_ca = json_object_get(input_smtp, "check_ca");
    if (j_check_ca != NULL && !json_is_null(j_check_ca) && !json_is_boolean(j_check_ca)) {
        json_array_append_new(j_message, json_pack("{ss}", "check_ca", "check_ca attribute must be a boolean"));
    }

    j_user = json_object_get(input_smtp, "user");
    if (j_user != NULL  && (!json_is_string(j_user) || json_string_length(j_user) > 128)) {
        json_array_append_new(j_message, json_pack("{ss}", "user", "user attribute must be a string value between 1 and 128 characters"));
    }

    j_password = json_object_get(input_smtp, "password");
    if (j_password != NULL && (!json_is_string(j_password) || json_string_length(j_password) > 128)) {
        json_array_append_new(j_message, json_pack("{ss}", "password", "password attribute must be a string value between 1 and 128 characters"));
    }

    j_from = json_object_get(input_smtp, "from");
    if (j_from == NULL || !json_is_string(j_from) || json_string_length(j_from) > 128 || json_string_length(j_from) <= 0) {
        json_array_append_new(j_message, json_pack("{ss}", "from", "from attribute is mandatory and must be a string value between 1 and 128 characters"));
    }

    j_to = json_object_get(input_smtp, "to");
    if (j_to == NULL || !json_is_string(j_to) || json_string_length(j_to) > 128 || json_string_length(j_to) <= 0) {
        json_array_append_new(j_message, json_pack("{ss}", "to", "to attribute is mandatory and must be a string value between 1 and 128 characters"));
    }

    j_cc = json_object_get(input_smtp, "cc");
    if (j_cc != NULL && (!json_is_string(j_cc) || json_string_length(j_cc) > 128)) {
        json_array_append_new(j_message, json_pack("{ss}", "cc", "cc attribute must be a string value between 1 and 128 characters"));
    }

    j_bcc = json_object_get(input_smtp, "bcc");
    if (j_bcc != NULL && (!json_is_string(j_bcc) || json_string_length(j_bcc) > 128)) {
        json_array_append_new(j_message, json_pack("{ss}", "bcc", "bcc attribute must be a string value between 1 and 128 characters"));
    }

    j_subject = json_object_get(input_smtp, "subject");
    if (j_subject == NULL || !json_is_string(j_subject) || json_string_length(j_subject) > 128 || json_string_length(j_subject) <= 0) {
        json_array_append_new(j_message, json_pack("{ss}", "subject", "subject attribute is mandatory and must be a string value between 1 and 128 characters"));
    }

    j_body = json_object_get(input_smtp, "body");
    if (j_body == NULL || !json_is_string(j_body) || json_string_length(j_body) > 512 || json_string_length(j_body) <= 0) {
        json_array_append_new(j_message, json_pack("{ss}", "body", "body attribute is mandatory and must be a string value between 1 and 512 characters"));
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "is_smtp_alert_valid - Error allocating resources or input parameters");
    json_decref(j_message);
    return NULL;
  }
  return j_message;
}

/**
 * parse_smtp_alert_from_http
 * converts an smtp alert from the http input to a valid smtp alert for database
 * returned value must be free'd after use
 */
json_t * parse_smtp_alert_from_http(const json_t * input_smtp) {
  json_t * db_smtp = json_object();
  
  if (input_smtp == NULL || !json_is_object(input_smtp) || db_smtp == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "add_smtp_alert - Error allocating resources or inpur parameters");
    json_decref(db_smtp);
    return NULL;
  } else {
    json_object_set_new(db_smtp, COLUMN_ALERT_SMTP_NAME, json_copy(json_object_get(input_smtp, "name")));
    json_object_set_new(db_smtp, COLUMN_ALERT_SMTP_HOST, json_copy(json_object_get(input_smtp, "host")));
    if (json_object_get(input_smtp, "port") != NULL) {
      json_object_set_new(db_smtp, COLUMN_ALERT_SMTP_PORT, json_integer(0));
    } else {
      json_object_set_new(db_smtp, COLUMN_ALERT_SMTP_PORT, json_copy(json_object_get(input_smtp, "port")));
    }
    json_object_set_new(db_smtp, COLUMN_ALERT_SMTP_FROM, json_copy(json_object_get(input_smtp, "from")));
    json_object_set_new(db_smtp, COLUMN_ALERT_SMTP_TO, json_copy(json_object_get(input_smtp, "to")));
    json_object_set_new(db_smtp, COLUMN_ALERT_SMTP_SUBJECT, json_copy(json_object_get(input_smtp, "subject")));
    json_object_set_new(db_smtp, COLUMN_ALERT_SMTP_BODY, json_copy(json_object_get(input_smtp, "body")));
    if (json_object_get(input_smtp, "user") != NULL && json_string_length(json_object_get(input_smtp, "user")) > 0) {
      json_object_set_new(db_smtp, COLUMN_ALERT_SMTP_USER, json_copy(json_object_get(input_smtp, "user")));
    }
    if (json_object_get(input_smtp, "password") != NULL && json_string_length(json_object_get(input_smtp, "password")) > 0) {
      json_object_set_new(db_smtp, COLUMN_ALERT_SMTP_PASSWORD, json_copy(json_object_get(input_smtp, "password")));
    }
    if (json_object_get(input_smtp, "cc") != NULL && json_string_length(json_object_get(input_smtp, "cc")) > 0) {
      json_object_set_new(db_smtp, COLUMN_ALERT_SMTP_CC, json_copy(json_object_get(input_smtp, "cc")));
    }
    if (json_object_get(input_smtp, "bcc") != NULL && json_string_length(json_object_get(input_smtp, "bcc")) > 0) {
      json_object_set_new(db_smtp, COLUMN_ALERT_SMTP_BCC, json_copy(json_object_get(input_smtp, "bcc")));
    }
    
    json_object_set_new(db_smtp,COLUMN_ALERT_SMTP_TLS, json_object_get(input_smtp, "tls")==json_true()?json_integer(1):json_integer(0));
    
    json_object_set_new(db_smtp, COLUMN_ALERT_SMTP_CHECK_CA, json_object_get(input_smtp, "check_ca")==json_true()?json_integer(1):json_integer(0));
    
    return db_smtp;
  }
}

/**
 * add_smtp_alert
 * insert a new smtp alert in the database
 * return 1 on success
 */
int add_smtp_alert(struct _h_connection * conn, const json_t * alert_smtp) {
  json_t * j_query = json_object();
  int res;
  
  if (conn == NULL || alert_smtp == NULL || j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "add_smtp_alert - Error allocating resources or input parameters");
    json_decref(j_query);
    return 0;
  } else {
    json_object_set_new(j_query, "table", json_string(TABLE_ALERT_SMTP));
    json_object_set_new(j_query, "values", json_copy((json_t *)alert_smtp));
    res = h_insert(conn, j_query, NULL);
    json_decref(j_query);
    return (res == H_OK);
  }
}

/**
 * add_smtp_alert
 * update an existing smtp alert in the database
 * return 1 on success
 */
int update_smtp_alert(struct _h_connection * conn, const json_t * alert_smtp, const char * name) {
  json_t * where_clause = json_object(), * alert_smtp_copy = json_copy((json_t *)alert_smtp), * j_query = json_object();
  int res;
  
  if (conn == NULL || alert_smtp == NULL || alert_smtp_copy == NULL || name == NULL || where_clause ==NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "update_smtp_alert - Error allocating resources or inpur parameters");
    json_decref(where_clause);
    json_decref(alert_smtp_copy);
    json_decref(j_query);
    return 0;
  } else {
    json_object_set_new(where_clause, COLUMN_ALERT_SMTP_NAME, json_string(name));
    json_object_del(alert_smtp_copy, COLUMN_ALERT_SMTP_NAME);
    json_object_set_new(j_query, "table", json_string(TABLE_ALERT_SMTP));
    json_object_set_new(j_query, "set", alert_smtp_copy);
    json_object_set_new(j_query, "where", where_clause);
    res = h_update(conn, j_query, NULL);
    json_decref(j_query);
    return (res == H_OK);
  }
}

/**
 * add_smtp_alert
 * remove an existing smtp alert in the database
 * return 1 on success
 */
int remove_smtp_alert(struct _h_connection * conn, const char * name) {
  json_t * where_clause = json_object(), * j_query = json_object();
  int res;
  if (conn == NULL || name == NULL || j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "remove_smtp_alert - Error allocating resources or inpur parameters");
    json_decref(where_clause);
    json_decref(j_query);
    return 0;
  } else {
    json_object_set_new(where_clause, COLUMN_ALERT_SMTP_NAME, json_string(name));
    json_object_set_new(j_query, "table", json_string(TABLE_ALERT_SMTP));
    json_object_set_new(j_query, "where", where_clause);
    res = h_delete(conn, j_query, NULL);
    json_decref(j_query);
    return (res == H_OK);
  }
}

/********************************
 * HTTP Header alert management *
 ********************************/

/**
 * get_http_header_alert
 * Get an http alert specified by its name, or all alerts if name is NULL
 * return a json_t pointer containing an object or an array of objects
 * returned value must be free'd after use
 */
json_t * get_http_header_alert(struct _h_connection * conn, json_t * http_alert_id) {
  json_t * where_clause = json_object(), * j_result, * j_query = json_object();
  int res;
  
  if (conn == NULL || http_alert_id == NULL || j_query == NULL || where_clause == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "get_http_header_alert - Error allocating resources or inpur parameters");
    json_decref(where_clause);
    json_decref(j_query);
    return NULL;
  } else {
    json_object_set_new(where_clause, COLUMN_ALERT_HTTP_ID, json_copy(http_alert_id));
    json_object_set_new(j_query, "table", json_string(TABLE_ALERT_HTTP_HEADER));
    json_object_set_new(j_query, "where", where_clause);
    
    res = h_select(conn, j_query, &j_result, NULL);
    json_decref(j_query);
    
    if (res == H_OK) {
      return j_result;
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "get_http_header_alert - Error getting http headers alert from database");
      return NULL;
    }
  }
}

/**
 * is_http_header_alert_valid
 * return 1 if input_http_header has valid input parameters
 */
int is_http_header_alert_valid(const json_t * input_http_header) {
  json_t * http_header_key, * http_header_value;
  if (input_http_header == NULL || !json_is_object(input_http_header)) {
    y_log_message(Y_LOG_LEVEL_ERROR, "is_http_header_alert_valid - Error input parameters");
    return 0;
  } else {
    http_header_key = json_object_get(input_http_header, "key");
    http_header_value = json_object_get(input_http_header, "value");
    if (http_header_key == NULL || !json_is_string(http_header_key) || json_string_length(http_header_key) <= 0 || json_string_length(http_header_key) > 64 ||
        http_header_value == NULL || !json_is_string(http_header_value) || json_string_length(http_header_value) <= 0 || json_string_length(http_header_value) > 128) {
      return 0;
    }
    return 1;
  }
}

/**
 * parse_http_header_alert_from_db
 * converts an http alert header from the database to a valid http alert header for http response
 * returned value must be free'd after use
 */
json_t * parse_http_header_alert_from_db(const json_t * j_result) {
  json_t * to_return = NULL;
  if (j_result != NULL && json_is_object(j_result)) {
    to_return = json_object();
    if (to_return ==NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "parse_http_header_alert_from_db - Error allocating resources");
      return NULL;
    }
    json_object_set_new(to_return, "key", json_copy(json_object_get(j_result, COLUMN_ALERT_HTTP_HEADER_KEY)));
    json_object_set_new(to_return, "value", json_copy(json_object_get(j_result, COLUMN_ALERT_HTTP_HEADER_VALUE)));
  }
  return to_return;
}

/**
 * parse_http_header_alert_from_http
 * converts an http alert header from the http request to a valid http alert header for the database
 * returned value must be free'd after use
 */
json_t * parse_http_header_alert_from_http(const json_t * http_header) {
  json_t * to_return = NULL;
  if (http_header != NULL && json_is_object(http_header)) {
    to_return = json_object();
    if (to_return ==NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "parse_http_header_alert_from_http - Error allocating resources");
      return NULL;
    }
    json_object_set_new(to_return, COLUMN_ALERT_HTTP_HEADER_KEY, json_copy(json_object_get(http_header, "key")));
    json_object_set_new(to_return, COLUMN_ALERT_HTTP_HEADER_VALUE, json_copy(json_object_get(http_header, "value")));
  }
  return to_return;
}

/**
 * add_http_header_alert
 * Insert a new http alert in the database
 * return 1 on success
 */
int add_http_header_alert(struct _h_connection * conn, const json_t * alert_http_header) {
  json_t * j_query = json_object();
  int res;
  
  if (conn == NULL || alert_http_header == NULL || j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "add_http_header_alert - Error allocating resources or input parameters");
    json_decref(j_query);
    return 0;
  } else {
    json_object_set_new(j_query, "table", json_string(TABLE_ALERT_HTTP_HEADER));
    json_object_set_new(j_query, "values", json_copy((json_t *)alert_http_header));
    res = h_insert(conn, j_query, NULL);
    json_decref(j_query);
    return (res == H_OK);
  }
}

/**
 * add_http_header_alert
 * Removes an http alert in the database
 * return 1 on success
 */
int remove_http_header_alert(struct _h_connection * conn, const json_t * alert_http_id) {
  json_t * j_query = json_object();
  int res;
  if (conn == NULL || alert_http_id == NULL || j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "remove_http_header_alert - Error allocating resources or input parameters");
    json_decref(j_query);
    return 0;
  } else {
    json_object_set_new(j_query, "table", json_string(TABLE_ALERT_HTTP_HEADER));
    json_object_set_new(j_query, "where", json_copy((json_t *)alert_http_id));
    res = h_delete(conn, j_query, NULL);
    json_decref(j_query);
    return (res == H_OK);
  }
}

/*************************
 * HTTP alert management *
 *************************/

/**
 * get_http_alert
 * Get an http alert specified by its name, or all alerts if name is NULL
 * return a json_t pointer containing an object or an array of objects
 * returned value must be free'd after use
 */
json_t * get_http_alert(struct _h_connection * conn, const char * name) {
  int res;
  json_t * where_clause, * j_result, * j_return = NULL, * j_tmp, * j_http_headers, * j_query = json_object();
  size_t index;
  
  if (j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "get_http_alert - Error allocating j_query");
    return NULL;
  }
  
  if (name == NULL) {
    where_clause = NULL;
  } else {
    where_clause = json_object();
    if (where_clause == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "get_http_alert - Error allocating where_clause");
      json_decref(j_query);
      return NULL;
    }
    json_object_set_new(where_clause, COLUMN_ALERT_HTTP_NAME, json_string(name));
  }
  
  json_object_set_new(j_query, "table", json_string(TABLE_ALERT_HTTP));
  json_object_set_new(j_query, "where", where_clause);
  
  res = h_select(conn, j_query, &j_result, NULL);
  json_decref(j_query);
  if (res == H_OK) {
    if (name != NULL) {
      // Extracting the first result (there should be only one), parse it, and return it as an object
      j_tmp = json_array_get(j_result, 0);
      if (j_tmp != NULL) {
        // Get http headers
        j_http_headers = get_http_header_alert(conn, json_object_get(j_tmp, COLUMN_ALERT_HTTP_ID));
        if (j_http_headers != NULL) {
          json_object_set_new(j_tmp, "http_headers", j_http_headers);
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "get_http_alert - Error getting http_header list");
        }
        j_return = parse_http_alert_from_db(j_tmp);
      }
      json_decref(j_result);
    } else {
      // Extracting all results, parse them, and return them as an array of objects
      j_return = json_array();
      if (j_return != NULL) {
        json_array_foreach(j_result, index, j_tmp) {
          // Get http headers
          j_http_headers = get_http_header_alert(conn, json_object_get(j_tmp, COLUMN_ALERT_HTTP_ID));
          if (j_http_headers != NULL) {
            json_object_set_new(j_tmp, "http_headers", j_http_headers);
            json_array_append_new(j_return, parse_http_alert_from_db(j_tmp));
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "get_http_alert - Error getting http_header list");
          }
        }
        json_decref(j_result);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "get_http_alert - Error allocating j_http_headers");
      }
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "get_http_alert - Error getting http alert from database");
  }
  return j_return;
}

/**
 * parse_http_alert_from_db
 * converts an http alert from the database to a valid http alert for http response
 * returned value must be free'd after use
 */
json_t * parse_http_alert_from_db(const json_t * j_result) {
  json_t * json_http = json_object(),  * j_http_header, * j_http_header_list = json_array();
  size_t index;
  
  if (j_result == NULL || !json_is_object(j_result) || json_http == NULL || j_http_header_list == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "parse_http_alert_from_db - Error allocating resources or input parameters");
    json_decref(json_http);
    json_decref(j_http_header_list);
    return NULL;
  } else {
    json_object_set_new(json_http, "name", json_copy(json_object_get(j_result, COLUMN_ALERT_HTTP_NAME)));
    json_object_set_new(json_http, "url", json_copy(json_object_get(j_result, COLUMN_ALERT_HTTP_URL)));
    json_object_set_new(json_http, "method", json_copy(json_object_get(j_result, COLUMN_ALERT_HTTP_METHOD)));
    json_object_set_new(json_http, "body", json_copy(json_object_get(j_result, COLUMN_ALERT_HTTP_BODY)));
    
    json_array_foreach(json_object_get(j_result, "http_headers"), index, j_http_header) {
      json_array_append_new(j_http_header_list, parse_http_header_alert_from_db(j_http_header));
    }
    json_object_set_new(json_http, "http_headers", j_http_header_list);
    
    return json_http;
  }
}

/**
 * parse_http_alert_from_http
 * converts an http alert from the http request to a valid http alert for the database
 * returned value must be free'd after use
 */
json_t * parse_http_alert_from_http(const json_t * input_http) {
  json_t * db_smtp = json_object(), * j_http_header, * j_http_header_list = json_array();
  size_t index;
  
  if (input_http == NULL || !json_is_object(input_http) || db_smtp == NULL || j_http_header_list == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "parse_http_alert_from_http - Error allocating resources or input parameters");
    json_decref(db_smtp);
    json_decref(j_http_header_list);
    return NULL;
  } else {
    json_object_set_new(db_smtp, COLUMN_ALERT_HTTP_NAME, json_copy(json_object_get(input_http, "name")));
    json_object_set_new(db_smtp, COLUMN_ALERT_HTTP_URL, json_copy(json_object_get(input_http, "url")));
    json_object_set_new(db_smtp, COLUMN_ALERT_HTTP_METHOD, json_copy(json_object_get(input_http, "method")));
    json_object_set_new(db_smtp, COLUMN_ALERT_HTTP_BODY, json_copy(json_object_get(input_http, "body")));
    
    json_array_foreach(json_object_get(input_http, "http_headers"), index, j_http_header) {
      json_array_append_new(j_http_header_list, parse_http_header_alert_from_http(j_http_header));
    }
    json_object_set_new(db_smtp, "http_headers", j_http_header_list);

    return db_smtp;
  }
}

/**
 * is_http_alert_valid
 * return 1 if input_http is a valid http alert
 */
json_t * is_http_alert_valid(const json_t * input_http, int update) {
  json_t * j_name, * j_method, * j_url, * j_body, * j_message = json_array(), * j_http_header;
  size_t index;

  if (input_http != NULL && j_message != NULL) {
    
    if (!update) {
      j_name = json_object_get(input_http, "name");
      if (j_name == NULL || !json_is_string(j_name) || json_string_length(j_name) > 64 || json_string_length(j_name) <= 0) {
          json_array_append_new(j_message, json_pack("{ss}", "name", "name attribute is mandatory and must be a string value between 1 and 64 characters"));
      }
    }
    
    j_url = json_object_get(input_http, "url");
    if (j_url == NULL || !json_is_string(j_url) || json_string_length(j_url) > 128 || json_string_length(j_url) <= 0) {
        json_array_append_new(j_message, json_pack("{ss}", "url", "url attribute is mandatory and must be a string value between 1 and 64 characters"));
    }
    
    j_method = json_object_get(input_http, "method");
    if (j_method != NULL && !json_is_null(j_method) && (!json_is_string(j_method) || json_string_length(j_method) > 16 || json_string_length(j_method) <= 0)) {
        json_array_append_new(j_message, json_pack("{ss}", "method", "method attribute must be a string value between 1 and 128 characters"));
    }
    
    j_body = json_object_get(input_http, "body");
    if (j_body != NULL && !json_is_null(j_body) && (!json_is_string(j_body) || json_string_length(j_body) > 512)) {
        json_array_append_new(j_message, json_pack("{ss}", "body", "body attribute must be a string value up to 128 characters"));
    }
    
    json_array_foreach(json_object_get(input_http, "http_headers"), index, j_http_header) {
      if (!is_http_header_alert_valid(j_http_header)) {
        json_array_append_new(j_message, json_pack("{ss}", "http_header", "http header must be an object {key:string, value:string}"));
      }
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "is_http_alert_valid - Error allocating resources or input parameters");
    json_decref(j_message);
    return NULL;
  }
  return j_message;
}

/**
 * add_http_alert
 * insert a new http alert in the database
 * return 1 on success
 */
int add_http_alert(struct _h_connection * conn, const json_t * alert_http) {
  json_t * j_http_header_list, * j_last_id, * j_http_header, * alert_http_copy = json_copy((json_t *)alert_http), * j_query = json_object();
  int res;
  size_t index;
  
  if (conn == NULL || alert_http == NULL || alert_http_copy == NULL || j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "add_http_alert - Error allocating retources or input parameters");
    json_decref(alert_http_copy);
    json_decref(j_query);
    return 0;
  } else {
    j_http_header_list = json_copy(json_object_get(alert_http_copy, "http_headers"));
    json_object_del(alert_http_copy, "http_headers");
    
    json_object_set_new(j_query, "table", json_string(TABLE_ALERT_HTTP));
    json_object_set_new(j_query, "values", alert_http_copy);

    res = h_insert(conn, j_query, NULL);
    json_decref(j_query);
    if (res == H_OK) {
      j_last_id = h_last_insert_id(conn);
      
      json_array_foreach(j_http_header_list, index, j_http_header) {
        json_object_set_new(j_http_header, COLUMN_ALERT_HTTP_ID, json_copy(j_last_id));
        if (!add_http_header_alert(conn, j_http_header)) {
          y_log_message(Y_LOG_LEVEL_ERROR, "add_http_alert - Error inserting http_header");
          json_decref(j_http_header_list);
          json_decref(j_last_id);
          return 0;
        }
      }
      json_decref(j_http_header_list);
      json_decref(j_last_id);
      return 1;
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "add_http_alert - Error inserting new alert_http");
      json_decref(j_http_header_list);
      return 0;
    }
  }
}

/**
 * update_http_alert
 * updates an existing http alert in the database
 * return 1 on success
 */
int update_http_alert(struct _h_connection * conn, const json_t * alert_http, const char * name) {
  json_t * where_clause, * j_ah_id_a, * j_ah_id, * j_http_header_list, * j_http_header, * alert_http_copy = json_copy((json_t *)alert_http), * j_query = json_object();
  int res, to_return = 1;
  size_t index;
  
  if (conn == NULL || alert_http == NULL || alert_http_copy == NULL || name == NULL || j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "update_http_alert - Error allocating retources or input parameters");
    json_decref(alert_http_copy);
    json_decref(j_query);
    return 0;
  } else {
    j_http_header_list = json_copy(json_object_get(alert_http_copy, "http_headers"));
    where_clause = json_object();
    if (j_http_header_list != NULL && where_clause != NULL) {
      json_object_del(alert_http_copy, "http_headers");
      json_object_del(alert_http_copy, COLUMN_ALERT_HTTP_NAME);
      json_object_set_new(where_clause, COLUMN_ALERT_HTTP_NAME, json_string(name));
      
      json_object_set_new(j_query, "table", json_string(TABLE_ALERT_HTTP));
      json_object_set_new(j_query, "set", alert_http_copy);
      json_object_set_new(j_query, "where", where_clause);

      res = h_update(conn, j_query, NULL);
      json_decref(j_query);
      if (res == H_OK) {
        j_query = json_object();
        if (j_query != NULL) {
          json_object_set_new(j_query, "table", json_string(TABLE_ALERT_HTTP));
          json_object_set_new(j_query, "columns", json_pack("[s]", COLUMN_ALERT_HTTP_ID));
          json_object_set_new(j_query, "where", json_pack("{ss}", COLUMN_ALERT_HTTP_NAME, name));
          
          if (h_select(conn, j_query, &j_ah_id_a, NULL) == H_OK) {
            j_ah_id = json_array_get(j_ah_id_a, 0);
            if (remove_http_header_alert(conn, j_ah_id)) {
              json_array_foreach(j_http_header_list, index, j_http_header) {
                json_object_set_new(j_http_header, "ah_id", json_copy(json_object_get(j_ah_id, COLUMN_ALERT_HTTP_ID)));
                if (!add_http_header_alert(conn, j_http_header)) {
                  to_return = 0;
                  y_log_message(Y_LOG_LEVEL_ERROR, "update_http_alert - Error inserting http_header");
                }
              }
            } else {
              to_return = 0;
              y_log_message(Y_LOG_LEVEL_ERROR, "update_http_alert - Error removing old http_header");
            }
            json_decref(j_ah_id_a);
          } else {
            to_return = 0;
            y_log_message(Y_LOG_LEVEL_ERROR, "update_http_alert - Error getting %s", COLUMN_ALERT_HTTP_ID);
          }
          json_decref(j_query);
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "update_http_alert - Error allocating j_query");
          to_return = 0;
        }
      }
      json_decref(j_http_header_list);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "update_http_alert - Error allocating j_http_header_list or where_clause");
      to_return = 0;
    }
    return to_return;
  }
}

/**
 * remove_http_alert
 * removes an existing http alert in the database
 * return 1 on success
 */
int remove_http_alert(struct _h_connection * conn, const char * name) {
  json_t * where_clause = json_object(), * j_query = json_object();
  int res;
  if (conn == NULL || name == NULL || j_query == NULL || where_clause == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "remove_http_alert - Error allocating resource or input parameters");
    json_decref(where_clause);
    json_decref(j_query);
    return 0;
  } else {
    json_object_set_new(where_clause, COLUMN_ALERT_HTTP_NAME, json_string(name));
    json_object_set_new(j_query, "table", json_string(TABLE_ALERT_HTTP));
    json_object_set_new(j_query, "where", where_clause);

    res = h_delete(conn, j_query, NULL);
    json_decref(j_query);
    return (res == H_OK);
  }
}
