/**
 *
 * Gareth messenger service
 *
 * Get messages from a REST Webservice
 * Send messages or digested data when previously parametered filters are triggered
 * Send protocols available: http, smtp
 *
 * filter related functions definitions
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

json_t * get_filter(struct _h_connection * conn, const char * name) {
  int res;
  json_t * where_clause, * j_result, * j_return = NULL, * j_tmp, * j_filter_clause, * j_filter_alert, * j_query = json_object();
  size_t index;
  
  if (j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "get_filter - Error allocating j_query");
    return NULL;
  }
  
  if (name == NULL) {
    where_clause = NULL;
  } else {
    where_clause = json_object();
    if (where_clause == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "get_filter - Error allocating where_clause resource");
      json_decref(j_query);
      return NULL;
    }
    json_object_set_new(where_clause, COLUMN_FILTER_NAME, json_string(name));
  }
  json_object_set_new(j_query, "table", json_string(TABLE_FILTER));
  json_object_set_new(j_query, "where", where_clause);
  
  res = h_select(conn, j_query, &j_result, NULL);
  json_decref(j_query);
  if (res == H_OK) {
    if (name != NULL) {
      // Extracting the first result (there should be only one), parse it, and return it as an object
      j_tmp = json_array_get(j_result, 0);
      if (j_tmp != NULL) {
        // Get http headers
        j_filter_clause = get_filter_clause(conn, json_object_get(j_tmp, COLUMN_FILTER_ALERT_ID));
        if (j_filter_clause != NULL) {
          json_object_set_new(j_tmp, "filter_clauses", j_filter_clause);
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "get_filter - Error getting filter clause list");
        }
        j_filter_alert = get_filter_alert(conn, json_object_get(j_tmp, COLUMN_FILTER_ALERT_ID));
        if (j_filter_alert != NULL) {
          json_object_set_new(j_tmp, "filter_alerts", j_filter_alert);
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "get_filter - Error getting filter alert list");
        }
        j_return = parse_filter_from_db(j_tmp);
      }
      json_decref(j_result);
    } else {
      // Extracting all results, parse them, and return them as an array of objects
      j_return = json_array();
      if (j_return != NULL) {
        json_array_foreach(j_result, index, j_tmp) {
          // Get http headers
          j_filter_clause = get_filter_clause(conn, json_object_get(j_tmp, COLUMN_FILTER_ALERT_ID));
          if (j_filter_clause != NULL) {
            json_object_set_new(j_tmp, "filter_clauses", j_filter_clause);
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "get_filter - Error getting http_header list");
          }
          j_filter_alert = get_filter_alert(conn, json_object_get(j_tmp, COLUMN_FILTER_ALERT_ID));
          if (j_filter_alert != NULL) {
            json_object_set_new(j_tmp, "filter_alerts", j_filter_alert);
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "get_filter - Error getting filter alert list");
          }
          json_array_append_new(j_return, parse_filter_from_db(j_tmp));
        }
        json_decref(j_result);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "get_filter - Error allocating resources");
      }
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "get_filter - Error getting filter from database");
  }
  return j_return;
}

json_t * parse_filter_from_db(const json_t * j_result) {
  json_t * json_filter = json_object(), * j_filter_clause, * j_filter_clause_list = json_array();
  size_t index;
  
  if (j_result == NULL || !json_is_object(j_result) || json_filter == NULL || j_filter_clause_list == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "parse_filter_from_db - Error allocating resources or input parameters");
    json_decref(json_filter);
    json_decref(j_filter_clause_list);
    return NULL;
  } else {
    json_object_set_new(json_filter, "name", json_copy(json_object_get(j_result, COLUMN_FILTER_NAME)));
    json_object_set_new(json_filter, "description", json_copy(json_object_get(j_result, COLUMN_FILTER_DESCRIPTION)));
    
    json_array_foreach(json_object_get(j_result, "filter_clauses"), index, j_filter_clause) {
      json_array_append_new(j_filter_clause_list, parse_filter_clause_from_db(j_filter_clause));
    }
    json_object_set_new(json_filter, "filter_clauses", j_filter_clause_list);
    
    json_object_set_new(json_filter, "filter_alerts", parse_filter_alert_from_db(json_object_get(j_result, "filter_alerts")));
    
    return json_filter;
  }
}

json_t * is_filter_valid(struct _h_connection * conn, const json_t * input_filter, int update) {
  json_t * j_name, * j_description, * j_message = json_array(), * j_filter_clause;
  size_t index;

  if (input_filter != NULL && j_message != NULL) {
    
    if (!update) {
      j_name = json_object_get(input_filter, "name");
      if (j_name == NULL || !json_is_string(j_name) || json_string_length(j_name) > 64 || json_string_length(j_name) <= 0) {
          json_array_append_new(j_message, json_pack("{ss}", "name", "name attribute is mandatory and must be a string value between 1 and 64 characters"));
      }
    }
    
    j_description = json_object_get(input_filter, "description");
    if (j_description == NULL || !json_is_string(j_description) || json_string_length(j_description) > 128 || json_string_length(j_description) <= 0) {
        json_array_append_new(j_message, json_pack("{ss}", "description", "description attribute must be a string value between 1 and 128 characters"));
    }
    
    json_array_foreach(json_object_get(input_filter, "filter_clauses"), index, j_filter_clause) {
      if (!is_filter_clause_valid(j_filter_clause)) {
        json_array_append_new(j_message, json_pack("{ss}", "filter_clause", "filter_clause invalid"));
        break;
      }
    }
    
    if (json_object_get(input_filter, "filter_alerts") != NULL && !is_filter_alert_valid(conn, json_object_get(input_filter, "filter_alerts"))) {
      json_array_append_new(j_message, json_pack("{ss}", "filter_alerts", "filter_alert invalid"));
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "is_filter_valid - Error allocating resources or input parameters");
    json_decref(j_message);
    return NULL;
  }
  return j_message;
}

json_t * parse_filter_from_http(const json_t * input_filter) {
  json_t * db_smtp = json_object(), * j_filter_clause, * j_filter_clause_list = json_array();
  size_t index;
  
  if (input_filter == NULL || !json_is_object(input_filter) || db_smtp == NULL || j_filter_clause_list == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "parse_filter_from_http - Error allocating resources or input parameters");
    json_decref(db_smtp);
    json_decref(j_filter_clause_list);
    return NULL;
  } else {
    json_object_set_new(db_smtp, COLUMN_FILTER_NAME, json_copy(json_object_get(input_filter, "name")));
    json_object_set_new(db_smtp, COLUMN_FILTER_DESCRIPTION, json_copy(json_object_get(input_filter, "description")));
    
    json_array_foreach(json_object_get(input_filter, "filter_clauses"), index, j_filter_clause) {
      json_array_append_new(j_filter_clause_list, parse_filter_clause_from_http(j_filter_clause));
    }
    json_object_set_new(db_smtp, "filter_clauses", j_filter_clause_list);

    json_object_set_new(db_smtp, "filter_alerts", json_copy(json_object_get(input_filter, "filter_alerts")));

    return db_smtp;
  }
}

int add_filter(struct _h_connection * conn, const json_t * filter) {
  json_t * j_filter_clause_list, * j_last_id, * j_filter_clause, * filter_copy = json_copy((json_t *)filter), * filter_alert, * j_filter_alert_list, * j_query = json_object();
  int res;
  size_t index;
  
  if (conn == NULL || filter == NULL || filter_copy == NULL || j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "add_filter - Error allocating resources or input parameters");
    json_decref(filter_copy);
    json_decref(j_query);
    return 0;
  } else {
    j_filter_clause_list = json_copy(json_object_get(filter_copy, "filter_clauses"));
    j_filter_alert_list = json_copy(json_object_get(filter_copy, "filter_alerts"));
    json_object_del(filter_copy, "filter_clauses");
    json_object_del(filter_copy, "filter_alerts");
    
    json_object_set_new(j_query, "table", json_string(TABLE_FILTER));
    json_object_set_new(j_query, "values", filter_copy);
    
    res = h_insert(conn, j_query, NULL);
    json_decref(j_query);
    if (res == H_OK) {
      j_last_id = h_last_insert_id(conn);
      
      json_array_foreach(j_filter_clause_list, index, j_filter_clause) {
        json_object_set(j_filter_clause, COLUMN_FILTER_ALERT_ID, j_last_id);
        if (!add_filter_clause(conn, j_filter_clause)) {
          y_log_message(Y_LOG_LEVEL_ERROR, "add_filter - Error inserting filter_clause");
          json_decref(j_filter_clause_list);
          json_decref(j_filter_alert_list);
          json_decref(j_last_id);
          return 0;
        }
      }
      json_decref(j_filter_clause_list);
      
      filter_alert = parse_filter_alert_from_http(j_filter_alert_list, j_last_id);
      json_decref(j_filter_alert_list);
      json_decref(j_last_id);
      if (filter_alert != NULL && json_array_size(filter_alert) > 0) {
        if (!add_filter_alert(conn, filter_alert)) {
          y_log_message(Y_LOG_LEVEL_ERROR, "add_filter - Error inserting filter_alert");
          json_decref(filter_alert);
          json_decref(j_last_id);
          return 0;
        }
      }
      json_decref(filter_alert);
      return 1;
      
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "add_filter - Error inserting new filter");
      json_decref(j_filter_clause_list);
      json_decref(j_filter_alert_list);
      return 0;
    }
  }
}

int update_filter(struct _h_connection * conn, const json_t * filter, const char * name) {
  json_t * where_clause = json_object(), * j_f_id_a, * j_f_id, * j_filter_clause_list, * j_filter_clause, * j_filter_alert_list, * filter_list, * filter_copy = json_copy((json_t *)filter), * j_query = json_object();
  int res, to_return = 1;
  size_t index;
  
  if (conn == NULL || filter == NULL || filter_copy == NULL || name == NULL || j_query == NULL || where_clause == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "update_filter - Error allocating resources or input parameters");
    json_decref(where_clause);
    json_decref(filter_copy);
    json_decref(j_query);
    return 0;
  } else {
    j_filter_clause_list = json_copy(json_object_get(filter_copy, "filter_clauses"));
    j_filter_alert_list = json_copy(json_object_get(filter_copy, "filter_alerts"));
    json_object_del(filter_copy, "filter_clauses");
    json_object_del(filter_copy, "filter_alerts");
    json_object_del(filter_copy, COLUMN_FILTER_NAME);
    json_object_set_new(where_clause, COLUMN_FILTER_NAME, json_string(name));
    
    json_object_set_new(j_query, "table", json_string(TABLE_FILTER));
    json_object_set_new(j_query, "set", filter_copy);
    json_object_set_new(j_query, "where", where_clause);
    
    res = h_update(conn, j_query, NULL);
    json_decref(j_query);
    if (res == H_OK) {
      where_clause = json_object();
      j_query = json_object();
      if (where_clause != NULL && j_query != NULL) {
        json_object_set_new(where_clause, COLUMN_FILTER_NAME, json_string(name));
        json_object_set_new(j_query, "table", json_string(TABLE_FILTER));
        json_object_set_new(j_query, "columns", json_pack("[s]", COLUMN_FILTER_ALERT_ID));
        json_object_set_new(j_query, "where", where_clause);
        json_object_set_new(j_query, "limit", json_integer(1));
        if (h_select(conn, j_query, &j_f_id_a, NULL) == H_OK) {
          j_f_id = json_array_get(j_f_id_a, 0);
          if (remove_filter_clause(conn, j_f_id)) {
            json_array_foreach(j_filter_clause_list, index, j_filter_clause) {
              json_object_set(j_filter_clause, COLUMN_FILTER_ALERT_ID, json_object_get(j_f_id, COLUMN_FILTER_ALERT_ID));
              if (!add_filter_clause(conn, j_filter_clause)) {
                to_return = 0;
                y_log_message(Y_LOG_LEVEL_ERROR, "update_filter - Error inserting filter_clause");
              }
            }
          } else {
            to_return = 0;
            y_log_message(Y_LOG_LEVEL_ERROR, "update_filter - Error removing old filter_clause");
          }
          filter_list = parse_filter_alert_from_http(j_filter_alert_list, json_object_get(j_f_id, COLUMN_FILTER_ALERT_ID));
          if (!remove_filter_alert(conn, j_f_id)) {
            to_return = 0;
            y_log_message(Y_LOG_LEVEL_ERROR, "update_filter - Error removing old filter_alert");
          } else if (filter_list != NULL) {
            if (json_array_size(filter_list) > 0) {
              if (!add_filter_alert(conn, filter_list)) {
                to_return = 0;
                y_log_message(Y_LOG_LEVEL_ERROR, "update_filter - Error inserting filter_alert");
              }
            }
          }
          json_decref(filter_list);
          json_decref(j_f_id_a);
        } else {
          to_return = 0;
          y_log_message(Y_LOG_LEVEL_ERROR, "update_filter - Error getting %s", COLUMN_FILTER_ALERT_ID);
        }
        json_decref(j_query);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "update_filter - Error allocating resources");
        json_decref(where_clause);
        json_decref(j_query);
      }
    } else {
      to_return = 0;
      y_log_message(Y_LOG_LEVEL_ERROR, "update_filter - Error update filter query");
    }
    json_decref(j_filter_clause_list);
    json_decref(j_filter_alert_list);
    return to_return;
  }
}

int remove_filter(struct _h_connection * conn, const char * name) {
  json_t * where_clause = json_object(), * j_query = json_object();
  int res;
  if (conn == NULL || name == NULL || j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "remove_filter - Error input parameters");
    json_decref(where_clause);
    json_decref(j_query);
    return 0;
  } else {
    json_object_set_new(where_clause, COLUMN_FILTER_NAME, json_string(name));
    json_object_set_new(j_query, "table", json_string(TABLE_FILTER));
    json_object_set_new(j_query, "where", where_clause);
    
    res = h_delete(conn, j_query, NULL);
    json_decref(j_query);
    return (res == H_OK);
  }
}

/**
 * Filter clause alert management
 */
json_t * get_filter_clause(struct _h_connection * conn, json_t * filter_id) {
  json_t * where_clause = json_object(), * j_result, * j_query = json_object();
  int res;
  
  if (conn == NULL || filter_id == NULL || j_query == NULL || where_clause == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "get_filter_clause - Error input parameters");
    json_decref(where_clause);
    json_decref(j_query);
    return NULL;
  } else {
    json_object_set(where_clause, COLUMN_FILTER_ALERT_ID, filter_id);
    
    json_object_set_new(j_query, "table", json_string(TABLE_FILTER_CLAUSE));
    json_object_set_new(j_query, "where", where_clause);

    res = h_select(conn, j_query, &j_result, NULL);
    json_decref(j_query);
    
    if (res == H_OK) {
      return j_result;
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "get_filter_clause - Error getting filter clause from database");
      return NULL;
    }
  }
}

int is_filter_clause_valid(const json_t * input_filter_clause) {
  json_t * filter_clause_element, * filter_clause_operator, * filter_clause_value;
  const char * element_value = NULL;
  
  if (input_filter_clause == NULL || !json_is_object(input_filter_clause)) {
    return 0;
  } else {
    filter_clause_element = json_object_get(input_filter_clause, "element");
    if (filter_clause_element == NULL || !json_is_string(filter_clause_element)) {
      return 0;
    } else {
      element_value = json_string_value(filter_clause_element);
      if (strchr(element_value, '#') == NULL) {
        char * element_value_secured = msprintf("#%s#", element_value);
        if (strstr(FILTER_CLAUSE_ELEMENT_VALUE_LIST, element_value_secured) == NULL) {
          o_free(element_value_secured);
          return 0;
        } else {
          o_free(element_value_secured);
        }
      } else {
        return 0;
      }
    }

    filter_clause_operator = json_object_get(input_filter_clause, "operator");
    if (filter_clause_operator == NULL || !json_is_string(filter_clause_operator)) {
      return 0;
    } else {
      const char * operator_value = json_string_value(filter_clause_operator);
      if (strchr(operator_value, '#') == NULL) {
        char * operator_value_secured = msprintf("#%s#", operator_value);
        if (strstr(FILTER_CLAUSE_OPERATOR_VALUE_LIST, operator_value_secured) == NULL) {
          o_free(operator_value_secured);
          return 0;
        } else {
          o_free(operator_value_secured);
        }
      } else {
        return 0;
      }
    }
    
    filter_clause_value = json_object_get(input_filter_clause, "value");
    if (filter_clause_value == NULL) {
      return 0;
    } else {
      if (0 == o_strcmp("date", element_value) || 0 == o_strcmp("priority", element_value)) {
        if (!json_is_integer(filter_clause_value)) {
          return 0;
        } else if (0 == o_strcmp("date", element_value) && json_integer_value(filter_clause_value) < 0) {
          return 0;
        } else if (0 == o_strcmp("priority", element_value) && ((json_integer_value(filter_clause_value) < 0) || json_integer_value(filter_clause_value) > NB_PRIORITY)) {
          return 0;
        }
      } else if (0 == o_strcmp("source", element_value) || 0 == o_strcmp("message", element_value) || 0 == o_strcmp("tag", element_value)) {
        if (!json_is_string(filter_clause_value) || json_string_length(filter_clause_value) <= 0 || json_string_length(filter_clause_value) > 128) {
          return 0;
        }
      } else {
        return 0;
      }
    }
    return 1;
  }
}

json_t * parse_filter_clause_from_db(const json_t * j_result) {
  json_t * to_return = json_object();
  json_int_t tmp;
  
  if (j_result != NULL && json_is_object(j_result) && to_return != NULL) {
    
    switch(json_integer_value(json_object_get(j_result, COLUMN_FILTER_CLAUSE_ELEMENT))) {
      case FILTER_CLAUSE_ELEMENT_DATE:
        json_object_set_new(to_return, "element", json_string("date"));
        tmp = strtol(json_string_value(json_object_get(j_result, COLUMN_FILTER_CLAUSE_VALUE)), NULL, 10);
        json_object_set_new(to_return, "value", json_integer(tmp));
        break;
      case FILTER_CLAUSE_ELEMENT_PRIORITY:
        json_object_set_new(to_return, "element", json_string("priority"));
        tmp = strtol(json_string_value(json_object_get(j_result, COLUMN_FILTER_CLAUSE_VALUE)), NULL, 10);
        json_object_set_new(to_return, "value", json_integer(tmp));
        break;
      case FILTER_CLAUSE_ELEMENT_SOURCE:
        json_object_set_new(to_return, "element", json_string("source"));
        json_object_set_new(to_return, "value", json_copy(json_object_get(j_result, COLUMN_FILTER_CLAUSE_VALUE)));
        break;
      case FILTER_CLAUSE_ELEMENT_MESSAGE:
        json_object_set_new(to_return, "element", json_string("message"));
        json_object_set_new(to_return, "value", json_copy(json_object_get(j_result, COLUMN_FILTER_CLAUSE_VALUE)));
        break;
      case FILTER_CLAUSE_ELEMENT_TAG:
        json_object_set_new(to_return, "element", json_string("tag"));
        json_object_set_new(to_return, "value", json_copy(json_object_get(j_result, COLUMN_FILTER_CLAUSE_VALUE)));
        break;
      default:
        break;
    }
    
    switch(json_integer_value(json_object_get(j_result, COLUMN_FILTER_CLAUSE_OPERATOR))) {
      case FILTER_CLAUSE_OPERATOR_EQUAL:
        json_object_set_new(to_return, "operator", json_string("="));
        break;
      case FILTER_CLAUSE_OPERATOR_DIFFERENT:
        json_object_set_new(to_return, "operator", json_string("!="));
        break;
      case FILTER_CLAUSE_OPERATOR_LOWER:
        json_object_set_new(to_return, "operator", json_string("<"));
        break;
      case FILTER_CLAUSE_OPERATOR_LOWER_EQUAL:
        json_object_set_new(to_return, "operator", json_string("<="));
        break;
      case FILTER_CLAUSE_OPERATOR_HIGHER:
        json_object_set_new(to_return, "operator", json_string(">"));
        break;
      case FILTER_CLAUSE_OPERATOR_HIGHER_EQUAL:
        json_object_set_new(to_return, "operator", json_string(">="));
        break;
      case FILTER_CLAUSE_OPERATOR_CONTAINS:
        json_object_set_new(to_return, "operator", json_string("contains"));
        break;
      case FILTER_CLAUSE_OPERATOR_NOT_CONTAINS:
        json_object_set_new(to_return, "operator", json_string("notcontains"));
        break;
      default:
        break;
    }
    
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "parse_filter_clause_from_db - Error input parameters");
    json_decref(to_return);
    to_return = NULL;
  }
  return to_return;
}

json_t * parse_filter_clause_from_http(const json_t * filter_clause) {
  json_t * to_return = json_object();
  if (filter_clause != NULL && json_is_object(filter_clause) && to_return != NULL) {
    if (0 == o_strcmp(json_string_value(json_object_get(filter_clause, "element")), "date")) {
      json_object_set_new(to_return, COLUMN_FILTER_CLAUSE_ELEMENT, json_integer(FILTER_CLAUSE_ELEMENT_DATE));
    } else if (0 == o_strcmp(json_string_value(json_object_get(filter_clause, "element")), "priority")) {
      json_object_set_new(to_return, COLUMN_FILTER_CLAUSE_ELEMENT, json_integer(FILTER_CLAUSE_ELEMENT_PRIORITY));
    } else if (0 == o_strcmp(json_string_value(json_object_get(filter_clause, "element")), "source")) {
      json_object_set_new(to_return, COLUMN_FILTER_CLAUSE_ELEMENT, json_integer(FILTER_CLAUSE_ELEMENT_SOURCE));
    } else if (0 == o_strcmp(json_string_value(json_object_get(filter_clause, "element")), "message")) {
      json_object_set_new(to_return, COLUMN_FILTER_CLAUSE_ELEMENT, json_integer(FILTER_CLAUSE_ELEMENT_MESSAGE));
    } else if (0 == o_strcmp(json_string_value(json_object_get(filter_clause, "element")), "tag")) {
      json_object_set_new(to_return, COLUMN_FILTER_CLAUSE_ELEMENT, json_integer(FILTER_CLAUSE_ELEMENT_TAG));
    }
    
    if (0 == o_strcmp(json_string_value(json_object_get(filter_clause, "operator")), "=")) {
      json_object_set_new(to_return, COLUMN_FILTER_CLAUSE_OPERATOR, json_integer(FILTER_CLAUSE_OPERATOR_EQUAL));
    } else if (0 == o_strcmp(json_string_value(json_object_get(filter_clause, "operator")), "!=")) {
      json_object_set_new(to_return, COLUMN_FILTER_CLAUSE_OPERATOR, json_integer(FILTER_CLAUSE_OPERATOR_DIFFERENT));
    } else if (0 == o_strcmp(json_string_value(json_object_get(filter_clause, "operator")), "<")) {
      json_object_set_new(to_return, COLUMN_FILTER_CLAUSE_OPERATOR, json_integer(FILTER_CLAUSE_OPERATOR_LOWER));
    } else if (0 == o_strcmp(json_string_value(json_object_get(filter_clause, "operator")), "<=")) {
      json_object_set_new(to_return, COLUMN_FILTER_CLAUSE_OPERATOR, json_integer(FILTER_CLAUSE_OPERATOR_LOWER_EQUAL));
    } else if (0 == o_strcmp(json_string_value(json_object_get(filter_clause, "operator")), ">")) {
      json_object_set_new(to_return, COLUMN_FILTER_CLAUSE_OPERATOR, json_integer(FILTER_CLAUSE_OPERATOR_HIGHER));
    } else if (0 == o_strcmp(json_string_value(json_object_get(filter_clause, "operator")), ">=")) {
      json_object_set_new(to_return, COLUMN_FILTER_CLAUSE_OPERATOR, json_integer(FILTER_CLAUSE_OPERATOR_HIGHER_EQUAL));
    } else if (0 == o_strncasecmp(json_string_value(json_object_get(filter_clause, "operator")), "contains", strlen("contains"))) {
      json_object_set_new(to_return, COLUMN_FILTER_CLAUSE_OPERATOR, json_integer(FILTER_CLAUSE_OPERATOR_CONTAINS));
    } else if (0 == o_strncasecmp(json_string_value(json_object_get(filter_clause, "operator")), "notcontains", strlen("notcontains"))) {
      json_object_set_new(to_return, COLUMN_FILTER_CLAUSE_OPERATOR, json_integer(FILTER_CLAUSE_OPERATOR_NOT_CONTAINS));
    }
    json_object_set_new(to_return, COLUMN_FILTER_CLAUSE_VALUE, json_copy(json_object_get(filter_clause, "value")));
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "parse_filter_clause_from_http - Error input parameters");
    json_decref(to_return);
    to_return = NULL;
  }
  return to_return;
}

int add_filter_clause(struct _h_connection * conn, const json_t * filter_clause) {
  json_t * j_query = json_object();
  int res;
  
  if (conn == NULL || filter_clause == NULL || j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "add_filter_clause - Error input parameters");
    return 0;
  } else {
    json_object_set_new(j_query, "table", json_string(TABLE_FILTER_CLAUSE));
    json_object_set_new(j_query, "values", json_copy((json_t *)filter_clause));
    res = h_insert(conn, j_query, NULL);
    json_decref(j_query);
    return (res == H_OK);
  }
}

int remove_filter_clause(struct _h_connection * conn, const json_t * filter_id) {
  json_t * j_query = json_object();
  int res;
  
  if (conn == NULL || filter_id == NULL || j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "remove_filter_clause - Error input parameters");
    return 0;
  } else {
    json_object_set_new(j_query, "table", json_string(TABLE_FILTER_CLAUSE));
    json_object_set_new(j_query, "where", json_copy((json_t *)filter_id));
    res = h_delete(conn, j_query, NULL);
    json_decref(j_query);
    return (res == H_OK);
  }
}

/**
 * Filter alert management
 */
json_t * get_filter_alert(struct _h_connection * conn, json_t * filter_id) {
  json_t * where_clause, * j_result, * j_return = json_object(), * smtp_list = json_array(), * http_list = json_array(), * j_row, * j_query = json_object();
  int res;
  size_t index;
  
  if (conn == NULL || filter_id == NULL || j_return == NULL || smtp_list == NULL || http_list == NULL || j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "get_filter_alert - Error allocating resources or input parameters");
    json_decref(j_return);
    json_decref(smtp_list);
    json_decref(http_list);
    json_decref(j_query);
    return NULL;
  } else {
    where_clause = json_object();
    if (where_clause != NULL) {
      json_object_set(where_clause, COLUMN_FILTER_ALERT_ID, filter_id);
      
      json_object_set_new(j_query, "table", json_string(TABLE_FILTER_ALERT));
      json_object_set_new(j_query, "where", where_clause);
      res = h_select(conn, j_query, &j_result, NULL);
      json_decref(j_query);
      
      if (res == H_OK) {
        json_array_foreach(j_result, index, j_row) {
          if (json_object_get(j_row, COLUMN_ALERT_HTTP_NAME) != json_null()) {
            json_array_append_new(http_list, json_copy(json_object_get(j_row, COLUMN_ALERT_HTTP_NAME)));
          }
          if (json_object_get(j_row, COLUMN_ALERT_SMTP_NAME) != json_null()) {
            json_array_append_new(smtp_list, json_copy(json_object_get(j_row, COLUMN_ALERT_SMTP_NAME)));
          }
        }
        json_object_set_new(j_return, "smtp", smtp_list);
        json_object_set_new(j_return, "http", http_list);
        json_decref(j_result);
        return j_return;
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "get_filter_alert - Error getting filter clause from database");
        return NULL;
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "get_filter_alert - Error allocating where_clause, exiting");
      return NULL;
    }
  }
}

int is_filter_alert_valid(struct _h_connection * conn, const json_t * input_filter_alert) {
  json_t * alert_name, * alert_smtp_list, * alert_http_list, * j_tmp;
  size_t index;
  
  if (input_filter_alert == NULL || !json_is_object(input_filter_alert)) {
    return 0;
  } else {
    alert_smtp_list = json_object_get(input_filter_alert, "smtp");
    alert_http_list = json_object_get(input_filter_alert, "http");
    
    if (alert_smtp_list == NULL || alert_http_list == NULL || !json_is_array(alert_smtp_list) || !json_is_array(alert_http_list)) {
      return 0;
    }
    
    json_array_foreach(alert_smtp_list, index, alert_name) {
      j_tmp = get_smtp_alert(conn, json_string_value(alert_name));
      if (!json_is_string(alert_name) || j_tmp == NULL) {
        json_decref(j_tmp);
        return 0;
      }
      json_decref(j_tmp);
    }
    
    json_array_foreach(alert_http_list, index, alert_name) {
      j_tmp = get_http_alert(conn, json_string_value(alert_name));
      if (!json_is_string(alert_name) || j_tmp == NULL) {
        json_decref(j_tmp);
        return 0;
      }
      json_decref(j_tmp);
    }
    return 1;
  }
}

json_t * parse_filter_alert_from_http(const json_t * filter_alert, const json_t * filter_id) {
  json_t * to_return = json_array(), * filter_name, * row;
  size_t index;
  
  if (filter_alert == NULL || filter_id == NULL || to_return == NULL) {
    json_decref(to_return);
    return NULL;
  } else {
    json_array_foreach(json_object_get(filter_alert, "smtp"), index, filter_name) {
      row = json_object();
      if (row != NULL) {
        json_object_set_new(row, COLUMN_FILTER_ALERT_ID, json_copy((json_t *)filter_id));
        json_object_set_new(row, COLUMN_ALERT_HTTP_NAME, json_null());
        json_object_set_new(row, COLUMN_ALERT_SMTP_NAME, json_copy(filter_name));
        json_array_append_new(to_return, row);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "parse_filter_alert_from_http - Error allocating row");
      }
    }
    json_array_foreach(json_object_get(filter_alert, "http"), index, filter_name) {
      row = json_object();
      if (row != NULL) {
        json_object_set_new(row, COLUMN_FILTER_ALERT_ID, json_copy((json_t *)filter_id));
        json_object_set_new(row, COLUMN_ALERT_HTTP_NAME, json_copy(filter_name));
        json_object_set_new(row, COLUMN_ALERT_SMTP_NAME, json_null());
        json_array_append_new(to_return, row);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "parse_filter_alert_from_http - Error allocating row");
      }
    }
    
    return to_return;
  }
}

json_t * parse_filter_alert_from_db(const json_t * filter_alert) {
  json_t * to_return = json_object();
  
  if (filter_alert == NULL && to_return == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "parse_filter_alert_from_db - Error empty params");
    return NULL;
  } else {
    json_object_set_new(to_return, "smtp", json_copy(json_object_get(filter_alert, "smtp")));
    json_object_set_new(to_return, "http", json_copy(json_object_get(filter_alert, "http")));
    
    return to_return;
  }
}

int add_filter_alert(struct _h_connection * conn, const json_t * filter_alert) {
  int res;
  json_t * j_query = json_object();
  
  if (conn == NULL || filter_alert == NULL || j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "add_filter_alert - Error empty params");
    return 0;
  } else {
    json_object_set_new(j_query, "table", json_string(TABLE_FILTER_ALERT));
    json_object_set_new(j_query, "values", json_copy((json_t *)filter_alert));
    res = h_insert(conn, j_query, NULL);
    json_decref(j_query);
    return (res == H_OK);
  }
}

int remove_filter_alert(struct _h_connection * conn, const json_t * filter_id) {
  int res;
  json_t * j_query = json_object();

  if (conn == NULL || filter_id == NULL || j_query == NULL) {
    return 0;
  } else {
    json_object_set_new(j_query, "table", json_string(TABLE_FILTER_ALERT));
    json_object_set_new(j_query, "where", json_copy((json_t *)filter_id));
    res = h_delete(conn, j_query, NULL);
    json_decref(j_query);
    return (res == H_OK);
  }
}
