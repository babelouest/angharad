/**
 *
 * Gareth messenger service
 *
 * Get messages from a REST Webservice
 * Send messages or digested data when previously parametered filters are triggered
 * Send protocols available: http, smtp
 *
 * messages related functions definitions
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

#define MESSAGE_PRIORITY_NONE     0
#define MESSAGE_PRIORITY_LOW      1
#define MESSAGE_PRIORITY_MEDIUM   2
#define MESSAGE_PRIORITY_HIGH     3
#define MESSAGE_PRIORITY_CRITICAL 4

#define MESSAGES_LIMIT_DEFAULT 20
#define MESSAGES_LIMIT_MAXIMUM 100

/**
 * Add a new message
 */
int add_message(struct _h_connection * conn, const json_t * message) {
  json_t * message_copy = json_copy((json_t *)message), * db_message = json_object(), * j_query = json_object(), * message_id, * alert_list, * alert;
  char * tags;
  size_t index;
  
  if (message == NULL || message_copy == NULL || db_message == NULL || j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "add_message - Error add_message input parameters");
    json_decref(message_copy);
    json_decref(db_message);
    return 0;
  } else {
    if (0 == o_strcmp(json_string_value(json_object_get(message_copy, "priority")), "NONE")) {
      json_object_set_new(db_message, COLUMN_MESSAGE_PRIORITY, json_integer(MESSAGE_PRIORITY_NONE));
    } else if (0 == o_strcmp(json_string_value(json_object_get(message_copy, "priority")), "LOW")) {
      json_object_set_new(db_message, COLUMN_MESSAGE_PRIORITY, json_integer(MESSAGE_PRIORITY_LOW));
    } else if (0 == o_strcmp(json_string_value(json_object_get(message_copy, "priority")), "MEDIUM")) {
      json_object_set_new(db_message, COLUMN_MESSAGE_PRIORITY, json_integer(MESSAGE_PRIORITY_MEDIUM));
    } else if (0 == o_strcmp(json_string_value(json_object_get(message_copy, "priority")), "HIGH")) {
      json_object_set_new(db_message, COLUMN_MESSAGE_PRIORITY, json_integer(MESSAGE_PRIORITY_HIGH));
    } else if (0 == o_strcmp(json_string_value(json_object_get(message_copy, "priority")), "CRITICAL")) {
      json_object_set_new(db_message, COLUMN_MESSAGE_PRIORITY, json_integer(MESSAGE_PRIORITY_CRITICAL));
    }
    json_object_set_new(db_message, COLUMN_MESSAGE_SOURCE, json_copy(json_object_get(message_copy, "source")));
    json_object_set_new(db_message, COLUMN_MESSAGE_TEXT, json_copy(json_object_get(message_copy, "text")));
    
    tags = json_dumps(json_object_get(message_copy, "tags"), JSON_ENCODE_ANY);
    json_object_set_new(db_message, COLUMN_MESSAGE_TAGS, json_string(tags));
    free(tags);
    
    json_object_set_new(j_query, "table", json_string(TABLE_MESSAGE));
    json_object_set_new(j_query, "values", db_message);
    if (h_insert(conn, j_query, NULL) != H_OK) {
      json_decref(j_query);
      json_decref(message_copy);
      return 0;
    } else {
      message_id = h_last_insert_id(conn);
      alert_list = get_corresponding_alert_list(conn, message_id);
      
      if (alert_list != NULL && message_id != NULL) {
        if (json_array_size(json_object_get(alert_list, "smtp")) > 0) {
          json_array_foreach(json_object_get(alert_list, "smtp"), index, alert) {
            if (!trigger_smtp_message(conn, json_string_value(alert), message)) {
              y_log_message(Y_LOG_LEVEL_ERROR, "add_message - Error sending smtp message with alert %s", json_string_value(alert));
            }
          }
        }
        
        if (json_array_size(json_object_get(alert_list, "http")) > 0) {  
          json_array_foreach(json_object_get(alert_list, "http"), index, alert) {
            if (!trigger_http_message(conn, json_string_value(alert), message)) {
              y_log_message(Y_LOG_LEVEL_ERROR, "add_message - Error sending http message with alert %s", json_string_value(alert));
            }
          }
        }
        json_decref(alert_list);
        json_decref(message_id);
      }
      
      json_decref(j_query);
      json_decref(message_copy);
      return 1;
    }
  }
}

/**
 * return 1 is the message has valid parameters
 */
json_t * is_message_valid(const json_t * message) {
  json_t * message_copy = json_copy((json_t *)message), * j_return = json_array(), * priority, * source, * text, * tag_list, * tag;
  const char * value;
  size_t index;
  
  if (message == NULL || !json_is_object(message) || j_return == NULL || message_copy == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "is_message_valid - Error allocating resources or input parameters");
    json_decref(message_copy);
    json_decref(j_return);
    return NULL;
  } else {
    priority = json_object_get(message_copy, "priority");
    if (priority != NULL || json_is_string(priority)) {
      value = json_string_value(priority);
      if (0 != o_strcmp(value, "NONE") && 0 != o_strcmp(value, "LOW") && 0 != o_strcmp(value, "MEDIUM") && 0 != o_strcmp(value, "HIGH") && 0 != o_strcmp(value, "CRITICAL")) {
        json_array_append_new(j_return, json_string("Priority is mandatory and must have the following string value: NONE, LOW, MEDIUM, HIGH, CRITICAL"));
      }
    } else {
      json_array_append_new(j_return, json_string("Priority is mandatory and must have the following string value: NONE, LOW, MEDIUM, HIGH, CRITICAL"));
    }
    
    source = json_object_get(message_copy, "source");
    if (source == NULL || !json_is_string(source) || json_string_length(source) > 64 || json_string_length(source) <= 0) {
      json_array_append_new(j_return, json_pack("{ss}", "source", "source attribute is mandatory and must be a string value between 1 and 64 characters"));
    }
    
    text = json_object_get(message_copy, "text");
    if (text == NULL || !json_is_string(text) || json_string_length(text) > 256 || json_string_length(text) <= 0) {
      json_array_append_new(j_return, json_pack("{ss}", "text", "text attribute is mandatory and must be a string value between 1 and 256 characters"));
    }
    
    tag_list = json_object_get(message_copy, "tags");
    if (tag_list != NULL && !json_is_array(tag_list) && json_array_size(tag_list) > 16) {
      json_array_append_new(j_return, json_pack("{ss}", "tags", "tags attribute must be an array of string values between 1 and 32 characters, maximum 16 tags"));
    } else {
      json_array_foreach(tag_list, index, tag) {
        if (!json_is_string(tag) || json_string_length(tag) > 32 || json_string_length(tag) <= 0) {
          json_array_append_new(j_return, json_pack("{ss}", "tags", "tags attribute must be an array of string values between 1 and 32 characters, maximum 16 tags"));
        }
      }
    }
  }
  json_decref(message_copy);
  return j_return;
}

/**
 * Get a message list corresponding to the filter_name specified and the additional_filters specified
 */
json_t * get_message_list(struct _h_connection * conn, const char * filter_name, const struct _u_map * map_url, long int limit, long int offset) {
  json_t * message_list = json_array(), * result, * message, * cur_message, * columns, * where_clause = NULL, * order_by = NULL, * j_query = json_object();
  uint cur_limit = limit>MESSAGES_LIMIT_MAXIMUM||limit<=0?MESSAGES_LIMIT_MAXIMUM:(uint)limit;
  size_t index;
  
  if (message_list == NULL || j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "get_message_list - Error allocating message_list or j_query, aborting");
    json_decref(message_list);
    json_decref(j_query);
    return NULL;
  }
  
  columns = json_pack("{sssss}", "UNIX_TIMESTAMP(m_date) AS m_date", "m_priority", "m_source", "m_text", "m_tags");
  
  if (filter_name != NULL) {
    where_clause = generate_where_clause_from_filter_name(conn, filter_name);
  }
  
  if (!append_where_clause_from_url_parameters(conn, &where_clause, map_url)) {
    y_log_message(Y_LOG_LEVEL_ERROR, "get_message_list - Error appending where clause, aborting");
    json_decref(message_list);
    json_decref(j_query);
    return NULL;
  }
  
  order_by = json_string(COLUMN_MESSAGE_DATE " DESC");
  
  json_object_set_new(j_query, "table", json_string(TABLE_MESSAGE));
  json_object_set_new(j_query, "columns", columns);
  json_object_set_new(j_query, "where", where_clause);
  json_object_set_new(j_query, "order_by", order_by);
  json_object_set_new(j_query, "limit", json_integer(cur_limit));
  json_object_set_new(j_query, "offset", json_integer(offset));
  if (h_select(conn, j_query, &result, NULL) != H_OK) {
    y_log_message(Y_LOG_LEVEL_ERROR, "get_message_list - Error executing sql get message list");
    json_decref(message_list);
    json_decref(j_query);
    return NULL;
  }
  json_decref(j_query);
  
  json_array_foreach(result, index, message) {
    cur_message = json_object();
    if (cur_message == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "get_message_list - Error allocating message object, aborting");
      json_decref(message_list);
      return NULL;
    }
    json_object_set_new(cur_message, "date", json_copy(json_object_get(message, COLUMN_MESSAGE_DATE)));
    json_object_set_new(cur_message, "source", json_copy(json_object_get(message, COLUMN_MESSAGE_SOURCE)));
    json_object_set_new(cur_message, "text", json_copy(json_object_get(message, COLUMN_MESSAGE_TEXT)));
    
    switch (json_integer_value(json_object_get(message, COLUMN_MESSAGE_PRIORITY))) {
      case MESSAGE_PRIORITY_NONE:
        json_object_set_new(cur_message, "priority", json_string("NONE"));
        break;
      case MESSAGE_PRIORITY_LOW:
        json_object_set_new(cur_message, "priority", json_string("LOW"));
        break;
      case MESSAGE_PRIORITY_MEDIUM:
        json_object_set_new(cur_message, "priority", json_string("MEDIUM"));
        break;
      case MESSAGE_PRIORITY_HIGH:
        json_object_set_new(cur_message, "priority", json_string("HIGH"));
        break;
      case MESSAGE_PRIORITY_CRITICAL:
        json_object_set_new(cur_message, "priority", json_string("CRITICAL"));
        break;
      default:
        json_object_set_new(cur_message, "priority", json_string("NONE"));
        break;
    }
    
    json_object_set_new(cur_message, "tags", json_loads(json_string_value(json_object_get(message, COLUMN_MESSAGE_TAGS)), JSON_DECODE_ANY, NULL));
    
    json_array_append_new(message_list, cur_message);
  }
  json_decref(result);
  return message_list;
}

int append_where_clause_from_url_parameters(struct _h_connection * conn, json_t ** where_clause, const struct _u_map * map_url) {
  const char ** keys = u_map_enum_keys(map_url);
  int i, clause;
  char * value, * ope, * str_element;
  
  if (keys != NULL && where_clause != NULL) {
    for (i=0; keys[i] != NULL; i++) {
      clause = 0;
      if (0 == o_strcasecmp("date", keys[i])) {
        clause = 1;
        if (*where_clause == NULL) {
          *where_clause = json_object();
          if (*where_clause == NULL) {
            y_log_message(Y_LOG_LEVEL_ERROR, "append_where_clause_from_url_parameters - Error allocating object, aborting");
          }
        }
        str_element = COLUMN_MESSAGE_DATE;
        ope = "raw";
        if (conn->type == HOEL_DB_TYPE_MARIADB) {
          value = msprintf(">= FROM_UNIXTIME(UNIX_TIMESTAMP(now() - %d))", strtol(u_map_get(map_url, keys[i]), NULL, 10));
        } else if (conn->type == HOEL_DB_TYPE_SQLITE) {
          value = msprintf(">= datetime(strftime('%%s', 'now')-%d, 'unixepoch')", strtol(u_map_get(map_url, keys[i]), NULL, 10));
        } else {
          value = NULL;
        }
      } else if (0 == o_strcasecmp("priority", keys[i])) {
        if (0 == o_strcasecmp("low", u_map_get(map_url, keys[i]))) {
          clause = 1;
          value = msprintf("%d", MESSAGE_PRIORITY_LOW);
        } else if (0 == o_strcasecmp("medium", u_map_get(map_url, keys[i]))) {
          clause = 1;
          value = msprintf("%d", MESSAGE_PRIORITY_MEDIUM);
        } else if (0 == o_strcasecmp("high", u_map_get(map_url, keys[i]))) {
          clause = 1;
          value = msprintf("%d", MESSAGE_PRIORITY_HIGH);
        } else if (0 == o_strcasecmp("critical", u_map_get(map_url, keys[i]))) {
          clause = 1;
          value = msprintf("%d", MESSAGE_PRIORITY_CRITICAL);
        }
        if (clause) {
          if (*where_clause == NULL) {
            *where_clause = json_object();
            if (*where_clause == NULL) {
              y_log_message(Y_LOG_LEVEL_ERROR, "append_where_clause_from_url_parameters - Error allocating object, aborting");
            }
          }
          str_element = COLUMN_MESSAGE_PRIORITY;
          ope = "=";
        }
      } else if (0 == o_strcasecmp("source", keys[i])) {
        clause = 1;
        if (*where_clause == NULL) {
          *where_clause = json_object();
          if (*where_clause == NULL) {
            y_log_message(Y_LOG_LEVEL_ERROR, "append_where_clause_from_url_parameters - Error allocating object, aborting");
          }
        }
        str_element = COLUMN_MESSAGE_SOURCE;
        ope = "=";
        value = o_strdup(u_map_get(map_url, keys[i]));
      } else if (0 == o_strcasecmp("text", keys[i])) {
        clause = 1;
        if (*where_clause == NULL) {
          *where_clause = json_object();
          if (*where_clause == NULL) {
            y_log_message(Y_LOG_LEVEL_ERROR, "append_where_clause_from_url_parameters - Error allocating object, aborting");
          }
        }
        str_element = COLUMN_MESSAGE_TEXT;
        ope = "LIKE";
        value = msprintf("%%%s%%", u_map_get(map_url, keys[i]));
      } else if (0 == o_strcasecmp("tags", keys[i])) {
        clause = 1;
        if (*where_clause == NULL) {
          *where_clause = json_object();
          if (*where_clause == NULL) {
            y_log_message(Y_LOG_LEVEL_ERROR, "append_where_clause_from_url_parameters - Error allocating object, aborting");
          }
        }
        str_element = COLUMN_MESSAGE_TAGS;
        ope = "LIKE";
        value = msprintf("%%%s%%", u_map_get(map_url, keys[i]));
      }
      if (clause && value != NULL) {
        if (json_object_get(*where_clause, str_element) != NULL) {
          // One can not have 2 where clauses of the same element, removing the old one
          json_object_del(*where_clause, str_element);
        }
        json_object_set_new(*where_clause, str_element, json_pack("{ssss}", "operator", ope, "value", value));
        o_free(value);
      }
    }
    return 1;
  }
  return 0;
}

json_t * generate_where_clause_from_filter_name(struct _h_connection * conn, const char * filter_name) {
  json_t * j_filter, * j_filter_name, * j_filter_clause_list, * j_filter_clause, * where_clause = json_object(), * j_query = json_object();
  size_t index;
  char * str_value, * str_operator, * str_compare, * str_element, * escape;
  
  j_filter_name = json_object();
  if (where_clause == NULL || j_query == NULL || j_filter_name == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "generate_where_clause_from_filter_name - Error allocating object or input parameters, aborting");
    json_decref(where_clause);
    json_decref(j_query);
    json_decref(j_filter_name);
    return NULL;
  }
  
  json_object_set_new(j_filter_name, COLUMN_FILTER_NAME, json_string(filter_name));
  
  json_object_set_new(j_query, "table", json_string(TABLE_FILTER));
  json_object_set_new(j_query, "where", j_filter_name);
  if (h_select(conn, j_query, &j_filter, NULL) == H_OK) {
    json_decref(j_query);
    j_query = json_object();
    if (j_query == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "generate_where_clause_from_filter_name - Error allocating object, aborting");
      json_decref(where_clause);
      json_decref(j_query);
      json_decref(j_filter_name);
      return NULL;
    }
    json_object_set_new(j_query, "table", json_string(TABLE_FILTER_CLAUSE));
    json_object_set_new(j_query, "where", json_pack("{si}", COLUMN_FILTER_ALERT_ID, json_integer_value(json_object_get(json_array_get(j_filter, 0), COLUMN_FILTER_ID))));
    
    if (h_select(conn, j_query, &j_filter_clause_list, NULL) == H_OK) {
      json_decref(j_query);
      json_array_foreach(j_filter_clause_list, index, j_filter_clause) {
        
        if (FILTER_CLAUSE_ELEMENT_DATE == json_integer_value(json_object_get(j_filter_clause, COLUMN_FILTER_CLAUSE_ELEMENT))) {
          if (conn->type == HOEL_DB_TYPE_MARIADB) {
            str_value = msprintf("FROM_UNIXTIME(UNIX_TIMESTAMP(now() - %s))", json_string_value(json_object_get(j_filter_clause, COLUMN_FILTER_CLAUSE_VALUE)));
          } else if (conn->type == HOEL_DB_TYPE_SQLITE) {
            str_value = msprintf("datetime(strftime('%%s', 'now')-%s, 'unixepoch')", json_string_value(json_object_get(j_filter_clause, COLUMN_FILTER_CLAUSE_VALUE)));
          } else {
            str_value = NULL;
          }
        } else if (FILTER_CLAUSE_ELEMENT_TAG == json_integer_value(json_object_get(j_filter_clause, COLUMN_FILTER_CLAUSE_ELEMENT))) {
          escape = h_escape_string(conn, json_string_value(json_object_get(j_filter_clause, "fc_value")));
          str_value = msprintf("'%\"%s\"%'", escape);
          o_free(escape);
        } else if (FILTER_CLAUSE_ELEMENT_MESSAGE == json_integer_value(json_object_get(j_filter_clause, COLUMN_FILTER_CLAUSE_ELEMENT)) 
					&& (json_integer_value(json_object_get(j_filter_clause, COLUMN_FILTER_CLAUSE_OPERATOR)) == FILTER_CLAUSE_OPERATOR_CONTAINS || json_integer_value(json_object_get(j_filter_clause, COLUMN_FILTER_CLAUSE_OPERATOR)) == FILTER_CLAUSE_OPERATOR_NOT_CONTAINS)) {
          escape = h_escape_string(conn, json_string_value(json_object_get(j_filter_clause, "fc_value")));
          str_value = msprintf("'%%%s%%'", escape);
          o_free(escape);
        } else {
          escape = h_escape_string(conn, json_string_value(json_object_get(j_filter_clause, COLUMN_FILTER_CLAUSE_VALUE)));
          str_value = msprintf("'%s'", escape);
          o_free(escape);
        }
        
        switch (json_integer_value(json_object_get(j_filter_clause, COLUMN_FILTER_CLAUSE_OPERATOR))) {
          case FILTER_CLAUSE_OPERATOR_EQUAL:
            str_operator = "=";
            break;
          case FILTER_CLAUSE_OPERATOR_DIFFERENT:
            str_operator = "!=";
            break;
          case FILTER_CLAUSE_OPERATOR_LOWER:
            str_operator = "<";
            break;
          case FILTER_CLAUSE_OPERATOR_LOWER_EQUAL:
            str_operator = "<=";
            break;
          case FILTER_CLAUSE_OPERATOR_HIGHER:
            str_operator = ">";
            break;
          case FILTER_CLAUSE_OPERATOR_HIGHER_EQUAL:
            str_operator = ">=";
            break;
          case FILTER_CLAUSE_OPERATOR_CONTAINS:
            str_operator = "LIKE";
            break;
          case FILTER_CLAUSE_OPERATOR_NOT_CONTAINS:
            str_operator = "NOT LIKE";
            break;
          default:
            str_operator = "=";
            break;
        }
        
        switch (json_integer_value(json_object_get(j_filter_clause, COLUMN_FILTER_CLAUSE_ELEMENT))) {
          case FILTER_CLAUSE_ELEMENT_DATE:
            str_element = COLUMN_MESSAGE_DATE;
            break;
          case FILTER_CLAUSE_ELEMENT_PRIORITY:
            str_element = COLUMN_MESSAGE_PRIORITY;
            break;
          case FILTER_CLAUSE_ELEMENT_SOURCE:
            str_element = COLUMN_MESSAGE_SOURCE;
            break;
          case FILTER_CLAUSE_ELEMENT_MESSAGE:
            str_element = COLUMN_MESSAGE_TEXT;
            break;
          case FILTER_CLAUSE_ELEMENT_TAG:
            str_element = COLUMN_MESSAGE_TAGS;
            break;
          default:
            y_log_message(Y_LOG_LEVEL_ERROR, "generate_where_clause_from_filter_name - Error, element not found in filter \"%s\"", filter_name);
            str_element = NULL;
            break;
        }
        
        str_compare = msprintf("%s %s", str_operator, str_value);
        if (json_object_get(where_clause, str_element) != NULL) {
          // One can not have 2 where clauses of the same element, removing the old one
          json_object_del(where_clause, str_element);
        }
        json_object_set_new(where_clause, str_element, json_pack("{ssss}", "operator", "raw", "value", str_compare));
        
        o_free(str_value);
        o_free(str_compare);
      }
      json_decref(j_filter_clause_list);
    } else {
      json_decref(j_query);
      y_log_message(Y_LOG_LEVEL_ERROR, "generate_where_clause_from_filter_name - Error, filter \"%s\" not found", filter_name);
    }
  } else {
    json_decref(j_query);
    y_log_message(Y_LOG_LEVEL_ERROR, "generate_where_clause_from_filter_name - Error, filter \"%s\" not found", filter_name);
  }
  json_decref(j_filter);
  return where_clause;
}

json_t * get_corresponding_alert_list(struct _h_connection * conn, json_t * message_id) {
  json_t * filter_list, * j_query = json_object(), * j_query_message, * filter, * generated_where_clause, * message_match, * alert_list = json_pack("{s[]s[]}", "smtp", "http"), * filter_alerts;
  size_t index;
  
  if (j_query != NULL && alert_list != NULL) {
    // Get filters that have at least one alert set
    json_object_set_new(j_query, "table", json_string(TABLE_FILTER));
    json_object_set_new(j_query, "colums", json_pack("[ss]", COLUMN_FILTER_NAME, COLUMN_FILTER_ALERT_ID));
    json_object_set_new(j_query, "where", json_pack("{s{ssss}}", COLUMN_FILTER_ALERT_ID, "operator", "raw", "value", "in (select " COLUMN_FILTER_ID " from " TABLE_FILTER ")"));
    if (h_select(conn, j_query, &filter_list, NULL) == H_OK) {
      json_decref(j_query);
      json_array_foreach(filter_list, index, filter) {
        generated_where_clause = generate_where_clause_from_filter_name(conn, json_string_value(json_object_get(filter, COLUMN_FILTER_NAME)));
        if (generated_where_clause != NULL) {
          j_query_message = json_object();
          if (j_query_message != NULL) {
            json_object_set_new(j_query_message, "table", json_string(TABLE_MESSAGE));
            json_object_set_new(generated_where_clause, COLUMN_MESSAGE_ID, json_copy(message_id));
            json_object_set_new(j_query_message, "where", generated_where_clause);
            if (h_select(conn, j_query_message, &message_match, NULL) == H_OK) {
              if (json_array_size(message_match) > 0) {
                // Message matches the filter, get the alert lists corresponding
                filter_alerts = get_filter_alert(conn, json_object_get(filter, COLUMN_FILTER_ALERT_ID));
                json_array_extend(json_object_get(alert_list, "smtp"), json_object_get(filter_alerts, "smtp"));
                json_array_extend(json_object_get(alert_list, "http"), json_object_get(filter_alerts, "http"));
                json_decref(filter_alerts);
              }
              json_decref(message_match);
            } else {
              y_log_message(Y_LOG_LEVEL_ERROR, "get_corresponding_alert_list - Error get_corresponding_alert_list, error getting message match from filter");
            }
            json_decref(j_query_message);
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "get_corresponding_alert_list - Error allocating j_query_message");
          }
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "get_corresponding_alert_list - Error get_corresponding_alert_list, error getting where_clause from filter name");
        }
      }
      json_decref(filter_list);
    } else {
      json_decref(j_query);
      y_log_message(Y_LOG_LEVEL_ERROR, "get_corresponding_alert_list - Error get_corresponding_alert_list, error getting filters list");
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "get_corresponding_alert_list - Error allocating get_corresponding_alert_list parameters");
    json_decref(j_query);
    json_decref(alert_list);
  }
  return alert_list;
}

/**
 * Thread executed when an smtp message is to be sent
 */
void * thread_smtp_message_run(void * args) {
  json_t * db_alert = (json_t *)args, * message;
  char * subject = NULL, * body = NULL;
  int res;

  if (db_alert != NULL) {
    message = json_object_get(db_alert, "message");
    subject = parse_string_with_message(json_string_value(json_object_get(db_alert, "subject")), message);
    body = parse_string_with_message(json_string_value(json_object_get(db_alert, "body")), message);
    
    if (subject != NULL && body != NULL) {
      res = ulfius_send_smtp_email(json_string_value(json_object_get(db_alert, "host")),
                                    (int)json_integer_value(json_object_get(db_alert, "port")),
                                    (int)json_integer_value(json_object_get(db_alert, "tls")),
                                    (int)json_integer_value(json_object_get(db_alert, "check_ca")),
                                    json_string_value(json_object_get(db_alert, "user")),
                                    json_string_value(json_object_get(db_alert, "password")),
                                    json_string_value(json_object_get(db_alert, "from")),
                                    json_string_value(json_object_get(db_alert, "to")),
                                    json_string_value(json_object_get(db_alert, "cc")),
                                    json_string_value(json_object_get(db_alert, "bcc")),
                                    subject,
                                    body);
    } else {
      res = U_ERROR;
      y_log_message(Y_LOG_LEVEL_ERROR, "thread_smtp_message_run - Error parse_string_with_message with body or subject, aborting");
    }
    o_free(subject);
    o_free(body);
    if (res != U_OK) {
      y_log_message(Y_LOG_LEVEL_ERROR, "thread_smtp_message_run - Error sending smtp message %s to %s", json_string_value(json_object_get(db_alert, "name")), json_string_value(json_object_get(db_alert, "to")));
    }
    json_decref(db_alert);
    return NULL;
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "thread_smtp_message_run - Error input argument");
    return NULL;
  }
}

int trigger_smtp_message(struct _h_connection * conn, const char * smtp_alert, const json_t * message) {
  json_t * db_alert = get_smtp_alert(conn, smtp_alert);
  pthread_t thread_smtp_message;
  int thread_ret_smtp_message = 0, thread_detach_smtp_message = 0;
  
  if (db_alert != NULL) {
    json_object_set_new(db_alert, "message", json_copy((json_t *)message));
    y_log_message(Y_LOG_LEVEL_INFO, "Send smtp message triggered by '%s' to address '%s', cc '%s', bcc '%s', using alert '%s'", json_string_value(json_object_get(message, "source")), json_string_value(json_object_get(db_alert, "to")), json_string_value(json_object_get(db_alert, "cc")), json_string_value(json_object_get(db_alert, "bcc")), json_string_value(json_object_get(db_alert, "name")));
    thread_ret_smtp_message = pthread_create(&thread_smtp_message, NULL, thread_smtp_message_run, (void *)db_alert);
    thread_detach_smtp_message = pthread_detach(thread_smtp_message);
    if (thread_ret_smtp_message || thread_detach_smtp_message) {
      y_log_message(Y_LOG_LEVEL_ERROR, "trigger_smtp_message - Error creating or detaching smtp message thread, return code: %d, detach code: %d",
                  thread_ret_smtp_message, thread_detach_smtp_message);
    }
    return 1;
  } else {
    return 0;
  }
}

/**
 * Thread executed when a http message is to be sent
 */
void * thread_http_message_run(void * args) {
  struct _u_request * request = (struct _u_request *)args;
  int res;
  res = ulfius_send_http_request(request, NULL);
  if (res != U_OK) {
    y_log_message(Y_LOG_LEVEL_ERROR, "Error sending http message: %d", res);
  }
  ulfius_clean_request_full(request);
  return NULL;
}

int trigger_http_message(struct _h_connection * conn, const char * http_alert, const json_t * message) {
  struct _u_request * request = o_malloc(sizeof(struct _u_request));
  json_t * header, * db_alert = get_http_alert(conn, http_alert);
  size_t index;
  char * str_body;
  pthread_t thread_http_message;
  int thread_ret_http_message = 0, thread_detach_http_message = 0;
  
  if (conn == NULL || http_alert == NULL || message == NULL || db_alert == NULL || request == NULL) {
    json_decref(db_alert);
    o_free(request);
    y_log_message(Y_LOG_LEVEL_ERROR, "trigger_http_message - Error input parameters");
    return 0;
  }
  ulfius_init_request(request);
  request->http_url = parse_string_with_message(json_string_value(json_object_get(db_alert, "url")), message);
  request->http_verb = o_strdup(json_string_value(json_object_get(db_alert, "method")));
  if (request->http_url == NULL || request->http_verb == NULL) {
    json_decref(db_alert);
    ulfius_clean_request_full(request);
    y_log_message(Y_LOG_LEVEL_ERROR, "trigger_http_message - Error allocating resources");
    return 0;
  }
  json_array_foreach(json_object_get(db_alert, "http_headers"), index, header) {
    char * key = parse_string_with_message(json_string_value(json_object_get(header, "key")), message);
    char * value = parse_string_with_message(json_string_value(json_object_get(header, "value")), message);
    u_map_put(request->map_header, key, value);
    o_free(key);
    o_free(value);
  }
  if (json_object_get(db_alert, "body") != json_null()) {
    str_body = parse_string_with_message(json_string_value(json_object_get(db_alert, "body")), message);
    if (str_body != NULL) {
      request->binary_body = str_replace(str_body, " ", "+");
      request->binary_body_length = strlen(request->binary_body);
      o_free(str_body);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "trigger_http_message - Error parse_string_with_message, aborting");
      json_decref(db_alert);
      return 0;
    }
  }
  y_log_message(Y_LOG_LEVEL_INFO, "Send http message triggered by '%s' to endpoint '%s' using alert '%s'", json_string_value(json_object_get(message, "source")), request->http_url, json_string_value(json_object_get(db_alert, "name")));
  thread_ret_http_message = pthread_create(&thread_http_message, NULL, thread_http_message_run, (void *)request);
  thread_detach_http_message = pthread_detach(thread_http_message);
  if (thread_ret_http_message || thread_detach_http_message) {
    y_log_message(Y_LOG_LEVEL_ERROR, "trigger_http_message - Error creating or detaching http message thread, return code: %d, detach code: %d",
                thread_ret_http_message, thread_detach_http_message);
  }
  json_decref(db_alert);
  return 1;
}

/**
 * Parse a message pattern with the input message data
 */
char * parse_string_with_message(const char * format, const json_t * message) {
  char * tmp = NULL, * to_return = o_strdup(format), * str_tags = NULL;
  json_t * element, * tag;
  size_t index;
  
  if (format == NULL || message == NULL || to_return == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "parse_string_with_message - Error input parameters");
    o_free(to_return);
    return NULL;
  } else {
    element = json_object_get(message, "priority");
    if (element != NULL && json_is_string(element)) {
      tmp = str_replace(to_return, "{priority}", json_string_value(element));
      if (tmp == NULL) {
        o_free(to_return);
        y_log_message(Y_LOG_LEVEL_ERROR, "parse_string_with_message - Error allocating resources");
        return NULL;
      }
      o_free(to_return);
      to_return = tmp;
    }
    
    element = json_object_get(message, "source");
    if (element != NULL && json_is_string(element)) {
      tmp = str_replace(to_return, "{source}", json_string_value(element));
      if (tmp == NULL) {
        o_free(to_return);
        y_log_message(Y_LOG_LEVEL_ERROR, "parse_string_with_message - Error allocating resources");
        return NULL;
      }
      o_free(to_return);
      to_return = tmp;
    }
    
    element = json_object_get(message, "text");
    if (element != NULL && json_is_string(element)) {
      tmp = str_replace(to_return, "{message}", json_string_value(element));
      if (tmp == NULL) {
        o_free(to_return);
        y_log_message(Y_LOG_LEVEL_ERROR, "parse_string_with_message - Error allocating resources");
        return NULL;
      }
      o_free(to_return);
      to_return = tmp;
    }
    
    element = json_object_get(message, "tags");
    if (element != NULL && json_is_array(element)) {
      json_array_foreach(element, index, tag) {
        if (str_tags == NULL) {
          str_tags = o_strdup(json_string_value(tag));
          if (str_tags == NULL) {
            y_log_message(Y_LOG_LEVEL_ERROR, "parse_string_with_message - Error allocating resources");
            o_free(to_return);
            return NULL;
          }
        } else {
          tmp = msprintf("%s, %s", str_tags, json_string_value(tag));
          if (tmp == NULL) {
            y_log_message(Y_LOG_LEVEL_ERROR, "parse_string_with_message - Error allocating resources");
            o_free(to_return);
            return NULL;
          }
          o_free(str_tags);
          str_tags = tmp;
        }
      }
      tmp = str_replace(to_return, "{tags}", str_tags);
      o_free(to_return);
      o_free(str_tags);
      to_return = tmp;
    }
    
    if (strstr(to_return, "{date}") != NULL) {
      time_t mytime;
      struct tm timeinfo;
      char buffer[26] = {0};
      time(&mytime);
      gmtime_r(&mytime, &timeinfo);
      strftime(buffer, 26, "%Y/%m/%d - %H:%M:%S", &timeinfo);
      tmp = str_replace(to_return, "{date}", buffer);
      o_free(to_return);
      to_return = tmp;
    }
    
    return to_return;
  }
}
