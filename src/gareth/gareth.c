/**
 *
 * Gareth messenger service
 *
 * Get messages from a REST Webservice
 * Send messages or digested data when previously parametered filters are triggered
 * Send protocols available: http, smtp
 *
 * main functions definitions
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
#include <pthread.h>

#include "gareth.h"

/**
 * init_gareth
 * 
 * Initialize Gareth webservice with prefix and database connection parameters
 * 
 */
int init_gareth(struct _u_instance * instance, const char * url_prefix, struct _h_connection * conn) {

  if (instance != NULL && url_prefix != NULL && conn != NULL) {
    // Set the endpoint list
    
    // Alert management callback functions
    ulfius_add_endpoint_by_val(instance, "GET", url_prefix, "/alert/", 2, &callback_gareth_get_alert_list, (void*)conn);
    ulfius_add_endpoint_by_val(instance, "GET", url_prefix, "/alert/@type/@alert_name", 2, &callback_gareth_get_alert, (void*)conn);
    ulfius_add_endpoint_by_val(instance, "POST", url_prefix, "/alert/@type/", 2, &callback_gareth_add_alert, (void*)conn);
    ulfius_add_endpoint_by_val(instance, "PUT", url_prefix, "/alert/@type/@alert_name", 2, &callback_gareth_modify_alert, (void*)conn);
    ulfius_add_endpoint_by_val(instance, "DELETE", url_prefix, "/alert/@type/@alert_name", 2, &callback_gareth_delete_alert, (void*)conn);
    
    // Filter management callback functions
    ulfius_add_endpoint_by_val(instance, "GET", url_prefix, "/filter/", 2, &callback_gareth_get_filter_list, (void*)conn);
    ulfius_add_endpoint_by_val(instance, "GET", url_prefix, "/filter/@filter_name", 2, &callback_gareth_get_filter, (void*)conn);
    ulfius_add_endpoint_by_val(instance, "POST", url_prefix, "/filter/", 2, &callback_gareth_add_filter, (void*)conn);
    ulfius_add_endpoint_by_val(instance, "PUT", url_prefix, "/filter/@filter_name", 2, &callback_gareth_modify_filter, (void*)conn);
    ulfius_add_endpoint_by_val(instance, "DELETE", url_prefix, "/filter/@filter_name", 2, &callback_gareth_delete_filter, (void*)conn);
    
    // Get all messages
    ulfius_add_endpoint_by_val(instance, "GET", url_prefix, "/message/", 2, &callback_gareth_get_messages, (void*)conn);
    
    // Get messages corresponding to filter_name
    ulfius_add_endpoint_by_val(instance, "GET", url_prefix, "/message/@filter_name", 2, &callback_gareth_get_messages, (void*)conn);
    
    // Add a new message
    ulfius_add_endpoint_by_val(instance, "POST", url_prefix, "/message", 2, &callback_gareth_add_messages, (void*)conn);
    
    y_log_message(Y_LOG_LEVEL_INFO, "gareth is available on prefix %s", url_prefix);
    return 1;
  } else {
    y_log_message(Y_LOG_LEVEL_INFO, "Error initializing gareth, error input parameters");
    return 0;
  }
}

/**
 * close_gareth
 * 
 * Close Gareth webservice
 * 
 */
int close_gareth(struct _u_instance * instance, const char * url_prefix) {
  if (instance != NULL && url_prefix != NULL) {
    ulfius_remove_endpoint_by_val(instance, "GET", url_prefix, "/alert/");
    ulfius_remove_endpoint_by_val(instance, "GET", url_prefix, "/alert/@type/@alert_name");
    ulfius_remove_endpoint_by_val(instance, "POST", url_prefix, "/alert/@type/");
    ulfius_remove_endpoint_by_val(instance, "PUT", url_prefix, "/alert/@type/@alert_name");
    ulfius_remove_endpoint_by_val(instance, "DELETE", url_prefix, "/alert/@type/@alert_name");
    ulfius_remove_endpoint_by_val(instance, "GET", url_prefix, "/filter/");
    ulfius_remove_endpoint_by_val(instance, "GET", url_prefix, "/filter/@filter_name");
    ulfius_remove_endpoint_by_val(instance, "POST", url_prefix, "/filter/");
    ulfius_remove_endpoint_by_val(instance, "PUT", url_prefix, "/filter/@filter_name");
    ulfius_remove_endpoint_by_val(instance, "DELETE", url_prefix, "/filter/@filter_name");
    ulfius_remove_endpoint_by_val(instance, "GET", url_prefix, "/message/");
    ulfius_remove_endpoint_by_val(instance, "GET", url_prefix, "/message/@filter_name");
    ulfius_remove_endpoint_by_val(instance, "POST", url_prefix, "/message");
    
    return 1;
  } else {
    y_log_message(Y_LOG_LEVEL_INFO, "Error closing gareth, error input parameters");
    return 0;
  }
}

/**
 * Alert management callback functions
 */

// Get all alerts
int callback_gareth_get_alert_list (const struct _u_request * request, struct _u_response * response, void * user_data) {
  UNUSED(request);
  struct _h_connection * conn;
  json_t * smtp_list, * http_list, * json_body;
  
  if (user_data == NULL) {
    return U_CALLBACK_ERROR;
  } else {
    conn = (struct _h_connection *)user_data;
    smtp_list = get_smtp_alert(conn, NULL);
    json_body = json_object();
    if (smtp_list != NULL && json_body != NULL) {
      response->status = 200;
      json_object_set_new(json_body, "smtp", smtp_list);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_gareth_get_alert_list - Error getting smtp_list, aborting");
      response->status = 500;
      json_decref(smtp_list);
      json_decref(json_body);
    }

    http_list = get_http_alert(conn, NULL);
    if (http_list != NULL) {
      response->status = 200;
      json_object_set_new(json_body, "http", http_list);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_gareth_get_alert_list - Error getting http_list, aborting");
      response->status = 500;
    }
    set_response_json_body_and_clean(response, (uint)response->status, json_body);
    return U_CALLBACK_CONTINUE;
  }
}

// Get one alert
int callback_gareth_get_alert (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct _h_connection * conn;
  const char * type, * alert_name;
  json_t * j_result;
  
  if (user_data == NULL) {
    return U_CALLBACK_ERROR;
  } else {
    conn = (struct _h_connection *)user_data;
    type = u_map_get(request->map_url, "type");
    alert_name = u_map_get(request->map_url, "alert_name");
    if (type != NULL && o_strcmp(type, "smtp") == 0) {
      j_result = get_smtp_alert(conn, alert_name);
      if (j_result != NULL) {
        set_response_json_body_and_clean(response, 200, j_result);
      } else {
        response->status = 404;
      }
    } else if (type != NULL && o_strcmp(type, "http") == 0) {
      j_result = get_http_alert(conn, alert_name);
      if (j_result != NULL) {
        set_response_json_body_and_clean(response, 200, j_result);
      } else {
        response->status = 404;
      }
    } else {
      set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "unknown type"));
    }
    return U_CALLBACK_CONTINUE;
  }
}

// Add a new alert
int callback_gareth_add_alert (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct _h_connection * conn;
  const char * type;
  json_t * j_name, * tmp, * j_is_valid;
  json_t * json_body = ulfius_get_json_body_request(request, NULL);

  if (json_body == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_gareth_add_alert - Error json input parameters callback_gareth_add_alert");
    json_object_set_new(json_body, "error", json_string("invalid input json format"));
    return U_CALLBACK_CONTINUE;
  } else if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_gareth_add_alert - Error, callback_gareth_add_alert user_data is NULL");
    json_decref(json_body);
    return U_CALLBACK_ERROR;
  } else {
    conn = (struct _h_connection *)user_data;
    type = u_map_get(request->map_url, "type");
    if (type != NULL && o_strcmp(type, "smtp") == 0) {
      j_name = json_object_get(json_body, "name");
      if (j_name != NULL && json_is_string(j_name)) {
        tmp = get_smtp_alert(conn, json_string_value(j_name));
        if (tmp == NULL) {
          json_decref(tmp);
          j_is_valid = is_smtp_alert_valid(json_body, 0);
          if (j_is_valid == NULL) {
            y_log_message(Y_LOG_LEVEL_ERROR, "callback_gareth_add_alert - Error is_smtp_alert_valid, aborting");
            response->status = 500;
          } else if (json_array_size(j_is_valid) > 0) {
            set_response_json_body_and_clean(response, 400, json_pack("{so}", "error", j_is_valid));
          } else {
            tmp = parse_smtp_alert_from_http(json_body);
            if (add_smtp_alert(conn, tmp)) {
              response->status = 200;
            } else {
              y_log_message(Y_LOG_LEVEL_ERROR, "callback_gareth_add_alert - Error inserting alert_smtp, aborting");
              response->status = 500;
            }
            json_decref(j_is_valid);
            json_decref(tmp);
          }
        } else {
          json_decref(tmp);
          set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "smtp alert invalid: name already exists in the database"));
        }
      } else {
        set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "smtp alert invalid: name must be a string"));
      }
    } else if (type != NULL && o_strcmp(type, "http") == 0) {
      j_name = json_object_get(json_body, "name");
      if (j_name != NULL && json_is_string(j_name)) {
        tmp = get_http_alert(conn, json_string_value(j_name));
        if (tmp == NULL) {
          json_decref(tmp);
          j_is_valid = is_http_alert_valid(json_body, 0);
          if (j_is_valid == NULL) {
            y_log_message(Y_LOG_LEVEL_ERROR, "callback_gareth_add_alert - Error is_http_alert_valid, aborting");
            response->status = 500;
          } else if (json_array_size(j_is_valid) > 0) {
            set_response_json_body_and_clean(response, 400, json_pack("{so}", "error", j_is_valid));
          } else {
            tmp = parse_http_alert_from_http(json_body);
            if (tmp != NULL) {
              if (add_http_alert(conn, tmp)) {
                response->status = 200;
              } else {
                y_log_message(Y_LOG_LEVEL_ERROR, "callback_gareth_add_alert - Error inserting alert_http, aborting");
                response->status = 500;
              }
              json_decref(j_is_valid);
              json_decref(tmp);
            } else {
              y_log_message(Y_LOG_LEVEL_ERROR, "callback_gareth_add_alert - Error getting alert_http, aborting");
              response->status = 500;
            }
          }
        } else {
          json_decref(tmp);
          set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "http alert invalid: name already exists in the database"));
        }
      } else {
        set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "http alert invalid: name must be a string"));
      }
    } else {
      set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "unknown type"));
    }
    json_decref(json_body);
    return U_CALLBACK_CONTINUE;
  }
}

int callback_gareth_modify_alert (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct _h_connection * conn;
  const char * type;
  json_t * tmp, * j_is_valid;
  const char * name;
  json_t * json_body = ulfius_get_json_body_request(request, NULL);

  if (json_body == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_gareth_modify_alert - Error json input parameters callback_gareth_modify_alert");
    json_object_set_new(json_body, "error", json_string("invalid input json format"));
    return U_CALLBACK_CONTINUE;
  } else if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "Error, callback_gareth_modify_alert user_data is NULL");
    json_decref(json_body);
    return U_CALLBACK_ERROR;
  } else {
    conn = (struct _h_connection *)user_data;
    type = u_map_get(request->map_url, "type");
    if (type != NULL && o_strcmp(type, "smtp") == 0) {
      name = u_map_get(request->map_url, "alert_name");
      tmp = get_smtp_alert(conn, name);
      if (tmp != NULL) {
        json_decref(tmp);
        j_is_valid = is_smtp_alert_valid(json_body, 1);
        if (j_is_valid == NULL) {
          y_log_message(Y_LOG_LEVEL_ERROR, "callback_gareth_modify_alert - Error is_smtp_alert_valid, aborting");
          response->status = 500;
        } else if (json_array_size(j_is_valid) > 0) {
          set_response_json_body_and_clean(response, 400, json_pack("{so}", "error", j_is_valid));
        } else {
          json_decref(j_is_valid);
          tmp = parse_smtp_alert_from_http(json_body);
          if (tmp != NULL && update_smtp_alert(conn, tmp, name)) {
            response->status = 200;
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "callback_gareth_modify_alert - Error updating alert_smtp, aborting");
            response->status = 500;
          }
          json_decref(tmp);
        }
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_gareth_modify_alert - Error smtp does not exist");
        json_decref(tmp);
        response->status = 404;
      }
    } else if (type != NULL && o_strcmp(type, "http") == 0) {
      name = u_map_get(request->map_url, "alert_name");
      tmp = get_http_alert(conn, name);
      if (tmp != NULL) {
        json_decref(tmp);
        j_is_valid = is_http_alert_valid(json_body, 1);
        if (j_is_valid == NULL) {
          y_log_message(Y_LOG_LEVEL_ERROR, "callback_gareth_modify_alert - Error is_http_alert_valid, aborting");
          response->status = 500;
        } else if (json_array_size(j_is_valid) > 0) {
          set_response_json_body_and_clean(response, 400, json_pack("{so}", "error", j_is_valid));
        } else {
          json_decref(j_is_valid);
          tmp = parse_http_alert_from_http(json_body);
          if (tmp != NULL) {
            if (update_http_alert(conn, tmp, name)) {
              response->status = 200;
            } else {
              y_log_message(Y_LOG_LEVEL_ERROR, "Error updating alert_http, aborting");
              response->status = 500;
            }
            json_decref(tmp);
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "Error getting alert_http, aborting");
            response->status = 500;
          }
        }
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "Error http does not exist");
        json_decref(tmp);
        response->status = 404;
      }
    } else {
      set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "unknown type"));
      y_log_message(Y_LOG_LEVEL_ERROR, "Error http invalid: unknown type");
    }
    json_decref(json_body);
    return U_CALLBACK_CONTINUE;
  }
}

int callback_gareth_delete_alert (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct _h_connection * conn;
  const char * type, * alert_name;
  json_t * j_result;
  
  if (user_data == NULL) {
    return U_CALLBACK_ERROR;
  } else {
    conn = (struct _h_connection *)user_data;
    type = u_map_get(request->map_url, "type");
    alert_name = u_map_get(request->map_url, "alert_name");
    if (type != NULL && o_strcmp(type, "smtp") == 0) {
      j_result = get_smtp_alert(conn, alert_name);
      if (j_result != NULL) {
        if (remove_smtp_alert(conn, alert_name)) {
          response->status = 200;
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "callback_gareth_delete_alert - Error deleting smtp alert, aborting");
          response->status = 500;
        }
        json_decref(j_result);
      } else {
        response->status = 404;
      }
    } else if (type != NULL && o_strcmp(type, "http") == 0) {
      j_result = get_http_alert(conn, alert_name);
      if (j_result != NULL) {
        if (remove_http_alert(conn, alert_name)) {
          response->status = 200;
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "callback_gareth_delete_alert - Error deleting http alert, aborting");
          response->status = 500;
        }
        json_decref(j_result);
      } else {
        response->status = 404;
      }
    } else {
      set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "unknown type"));
    }
    return U_CALLBACK_CONTINUE;
  }
}

// Filter management callback functions
int callback_gareth_get_filter_list (const struct _u_request * request, struct _u_response * response, void * user_data) {
  UNUSED(request);
  struct _h_connection * conn;
  
  if (user_data == NULL) {
    return U_CALLBACK_ERROR;
  } else {
    conn = (struct _h_connection *)user_data;
    set_response_json_body_and_clean(response, 200, get_filter(conn, NULL));
    return U_CALLBACK_CONTINUE;
  }
}

int callback_gareth_get_filter (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct _h_connection * conn;
  json_t * json_body;
  
  if (user_data == NULL) {
    return U_CALLBACK_ERROR;
  } else {
    conn = (struct _h_connection *)user_data;
    json_body = get_filter(conn, u_map_get(request->map_url, "filter_name"));
    if (json_body == NULL) {
      response->status = 404;
    } else {
      set_response_json_body_and_clean(response, 200, json_body);
    }
  }
  return U_CALLBACK_CONTINUE;
}

int callback_gareth_add_filter (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct _h_connection * conn;
  json_t * j_name, * tmp, * j_is_valid;
  json_t * json_body = ulfius_get_json_body_request(request, NULL);

  if (json_body == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_gareth_add_filter - Error json input parameters callback_gareth_add_filter");
    set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "invalid input json format"));
    return U_CALLBACK_CONTINUE;
  } else if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_gareth_add_filter - Error, callback_gareth_add_filter user_data is NULL");
    json_decref(json_body);
    return U_CALLBACK_ERROR;
  } else {
    conn = (struct _h_connection *)user_data;
    j_name = json_object_get(json_body, "name");
    if (j_name != NULL && json_is_string(j_name)) {
      tmp = get_filter(conn, json_string_value(j_name));
      if (tmp == NULL) {
        json_decref(tmp);
        j_is_valid = is_filter_valid(conn, json_body, 0);
        if (j_is_valid == NULL) {
          y_log_message(Y_LOG_LEVEL_ERROR, "callback_gareth_add_filter - Error is_filter_valid, aborting");
          response->status = 500;
        } else if (json_array_size(j_is_valid) > 0) {
          set_response_json_body_and_clean(response, 400, json_pack("{so}", "error", j_is_valid));
        } else {
          tmp = parse_filter_from_http(json_body);
          if (tmp != NULL) {
            if (add_filter(conn, tmp)) {
              response->status = 200;
            } else {
              y_log_message(Y_LOG_LEVEL_ERROR, "callback_gareth_add_filter - Error inserting filter, aborting");
              response->status = 500;
            }
            json_decref(tmp);
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "callback_gareth_add_filter - Error parsing filter, aborting");
            response->status = 500;
          }
          json_decref(j_is_valid);
        }
      } else {
        json_decref(tmp);
        set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "filter invalid: name already exists in the database"));
      }
    } else {
      set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "filter invalid: name must be a string"));
    }
    json_decref(json_body);
    return U_CALLBACK_CONTINUE;
  }
}

int callback_gareth_modify_filter (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct _h_connection * conn;
  json_t * tmp, * j_is_valid;
  const char * name;
  json_t * json_body = ulfius_get_json_body_request(request, NULL);

  if (json_body == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_gareth_modify_filter - Error json input parameters callback_gareth_modify_filter");
    set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "invalid input json format"));
    return U_CALLBACK_CONTINUE;
  } else if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_gareth_modify_filter - Error, callback_gareth_modify_filter user_data is NULL");
    json_decref(json_body);
    return U_CALLBACK_ERROR;
  } else {
    conn = (struct _h_connection *)user_data;
    name = u_map_get(request->map_url, "filter_name");
    tmp = get_filter(conn, name);
    if (tmp != NULL) {
      json_decref(tmp);
      j_is_valid = is_filter_valid(conn, json_body, 1);
      if (j_is_valid == NULL) {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_gareth_modify_filter - Error is_filter_valid, aborting");
        response->status = 500;
      } else if (json_array_size(j_is_valid) > 0) {
        set_response_json_body_and_clean(response, 400, json_pack("{so}", "error", j_is_valid));
      } else {
        json_decref(j_is_valid);
        tmp = parse_filter_from_http(json_body);
        if (tmp != NULL && update_filter(conn, tmp, name)) {
          response->status = 200;
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "callback_gareth_modify_filter - Error updating filter, aborting");
          response->status = 500;
        }
        json_decref(tmp);
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_gareth_modify_filter - Error filter does not exist");
      json_decref(tmp);
      response->status = 404;
    }
    json_decref(json_body);
    return U_CALLBACK_CONTINUE;
  }
}

int callback_gareth_delete_filter (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct _h_connection * conn;
  const char * name;
  json_t * j_result;
  
  if (user_data == NULL) {
    return U_CALLBACK_ERROR;
  } else {
    conn = (struct _h_connection *)user_data;
    name = u_map_get(request->map_url, "filter_name");
    j_result = get_filter(conn, name);
    if (j_result != NULL) {
      if (remove_filter(conn, name)) {
        response->status = 200;
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_gareth_delete_filter - Error deleting filter, aborting");
        response->status = 500;
      }
      json_decref(j_result);
    } else {
      response->status = 404;
    }
    return U_CALLBACK_CONTINUE;
  }
}

/**
 * Messages management callback functions
 */
int callback_gareth_get_messages (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct _h_connection * conn;
  char * endptr;
  const char * filter_name = u_map_get(request->map_url, "filter_name");
  long int limit = (u_map_has_key_case(request->map_url, "limit")?strtol(u_map_get_case(request->map_url, "limit"), &endptr, 10):0), 
           offset = (u_map_has_key_case(request->map_url, "offset")?strtol(u_map_get_case(request->map_url, "offset"), &endptr, 10):0);
  json_t * j_filter, * json_body;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_gareth_get_messages - Error, callback_gareth_get_messages user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    conn = (struct _h_connection *)user_data;
    if (filter_name != NULL) {
      j_filter = get_filter(conn, filter_name);
      if (j_filter == NULL) {
        response->status = 404;
        return U_CALLBACK_CONTINUE;
      }
      json_decref(j_filter);
    }
    json_body = get_message_list(conn, filter_name, request->map_url, limit, offset);
    if (json_body == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_gareth_get_messages - Error getting message list");
      response->status = 500;
    } else {
      set_response_json_body_and_clean(response, 200, json_body);
    }
    return U_CALLBACK_CONTINUE;
  }
}

// Add a new message in the database
int callback_gareth_add_messages (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct _h_connection * conn;
  json_t * j_is_valid;
  json_t * json_body = ulfius_get_json_body_request(request, NULL);

  if (json_body == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_gareth_add_messages - Error json input parameters callback_gareth_add_messages");
    set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "invalid input json format"));
    return U_CALLBACK_CONTINUE;
  } else if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_gareth_add_messages - Error, callback_gareth_add_messages user_data is NULL");
    json_decref(json_body);
    return U_CALLBACK_ERROR;
  } else {
    conn = (struct _h_connection *)user_data;
    j_is_valid = is_message_valid(json_body);
    if (j_is_valid == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_gareth_add_messages - Error is_message_valid, aborting");
      response->status = 500;
    } else if (json_array_size(j_is_valid) > 0) {
      set_response_json_body_and_clean(response, 400, json_pack("{so}", "error", j_is_valid));
    } else {
      if (add_message(conn, json_body)) {
        response->status = 200;
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_gareth_add_messages - Error adding message, aborting");
        response->status = 500;
      }
      json_decref(j_is_valid);
    }
    json_decref(json_body);
    return U_CALLBACK_CONTINUE;
  }
}
