/**
 *
 * Carleon House Automation side services
 *
 * Command house automation side services via an HTTP REST interface
 *
 * Liquidsoap service module
 * Get current song and last 10 songs played from a liquidsoap instance
 * Can stop and play a liquidsoap instance
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

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#include <jansson.h>
#include <ulfius.h>
#include <hoel.h>
#include <orcania.h>
#include <yder.h>
#include "../carleon.h"

#define LIQUIDSOAP_TABLE_NAME "c_service_liquidsoap"

#define LIQUIDSOAP_FIELDS "$title$artist$albumartist$year$album$"
#define LIQUIDSOAP_COMMANDS "$skip$stop$start$status$remaining$"

int set_response_json_body_and_clean(struct _u_response * response, uint status, json_t * json_body) {
  int res = ulfius_set_json_body_response(response, status, json_body);
  json_decref(json_body);
  return res;
}

/**
 * get the specified liquidsoap or all liquidsoap
 */
json_t * liquidsoap_get(struct _carleon_config * config, const char * liquidsoap_name) {
  json_t * j_query = json_pack("{sss[ssssssss]}", 
                    "table", 
                      LIQUIDSOAP_TABLE_NAME,
                    "columns",
                      "csl_name AS name",
                      "csl_description AS description",
                      "csl_radio_url AS radio_url",
                      "csl_radio_type AS radio_type",
                      "csl_control_enabled",
                      "csl_control_host AS control_host",
                      "csl_control_port AS control_port",
                      "csl_control_request_name AS control_request_name"
                    ), * j_result, * to_return, * j_lqs;
  int res;
  size_t index;
  
  if (j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "liquidsoap_get - Error allocating resources for j_query");
    return json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  }
  
  if (liquidsoap_name != NULL) {
    json_object_set_new(j_query, "where", json_pack("{ss}", "csl_name", liquidsoap_name));
  }
  
  res = h_select(config->conn, j_query, &j_result, NULL);
  json_decref(j_query);
  if (res == H_OK) {
    if (liquidsoap_name == NULL) {
			json_array_foreach(j_result, index, j_lqs) {
				json_object_set_new(j_lqs, "control_enabled", json_integer_value(json_object_get(j_lqs, "csl_control_enabled"))==1?json_true():json_false());
				json_object_del(j_lqs, "csl_control_enabled");
			}
      to_return = json_pack("{siso}", "result", WEBSERVICE_RESULT_OK, "element", json_copy(j_result));
    } else {
      if (json_array_size(j_result) == 0) {
        to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_NOT_FOUND);
      } else {
				json_object_set_new(json_array_get(j_result, 0), "control_enabled", json_integer_value(json_object_get(json_array_get(j_result, 0), "csl_control_enabled"))==1?json_true():json_false());
				json_object_del(json_array_get(j_result, 0), "csl_control_enabled");
        to_return = json_pack("{siso}", "result", WEBSERVICE_RESULT_OK, "element", json_copy(json_array_get(j_result, 0)));
      }
    }
    json_decref(j_result);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "liquidsoap_get - Error executing j_query");
    return json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  }
  return to_return;
}

/**
 * check the specified liquidsoap has valid parameters
 */
json_t * is_liquidsoap_valid(struct _carleon_config * config, json_t * liquidsoap, int add) {
  json_t * to_return = json_array(), * j_result, * element;
  size_t index;
  
  if (to_return == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "is_liquidsoap_valid - Error allocating resources for to_return");
    return NULL;
  }
  
  if (liquidsoap == NULL || !json_is_object(liquidsoap)) {
    json_array_append_new(to_return, json_pack("{ss}", "liquidsoap", "liquidsoap must be a json object"));
  } else {
    if (add) {
      j_result = liquidsoap_get(config, NULL);
      json_array_foreach(json_object_get(j_result, "element"), index, element) {
        if (0 == o_strcmp(json_string_value(json_object_get(element, "name")), json_string_value(json_object_get(liquidsoap, "name")))) {
          json_array_append_new(to_return, json_pack("{ss}", "name", "a liquidsoap service with the same name already exist"));
        }
      }
      json_decref(j_result);
    }
    
    if (add && (json_object_get(liquidsoap, "name") == NULL || !json_is_string(json_object_get(liquidsoap, "name")) || json_string_length(json_object_get(liquidsoap, "name")) > 64)) {
      json_array_append_new(to_return, json_pack("{ss}", "name", "name is mandatory and must be a string up to 64 characters"));
    }
    
    if (json_object_get(liquidsoap, "description") != NULL && (!json_is_string(json_object_get(liquidsoap, "description")) || json_string_length(json_object_get(liquidsoap, "description")) > 128)) {
      json_array_append_new(to_return, json_pack("{ss}", "description", "description must be a string up to 128 characters"));
    }
    
    if (json_object_get(liquidsoap, "radio_url") == NULL || !json_is_string(json_object_get(liquidsoap, "radio_url")) || json_string_length(json_object_get(liquidsoap, "radio_url")) > 512) {
      json_array_append_new(to_return, json_pack("{ss}", "radio_url", "radio_url is mandatory and must be a string up to 512 characters"));
    }
    
    if (json_object_get(liquidsoap, "type") != NULL && (!json_is_string(json_object_get(liquidsoap, "type")) || json_string_length(json_object_get(liquidsoap, "type")) > 64)) {
      json_array_append_new(to_return, json_pack("{ss}", "type", "type must be a string up to 64 characters"));
    }
    
    if (json_object_get(liquidsoap, "control_enabled") != NULL && !json_is_boolean(json_object_get(liquidsoap, "control_enabled"))) {
      json_array_append_new(to_return, json_pack("{ss}", "control_enabled", "control_enabled must be a boolean"));
		} else if (json_object_get(liquidsoap, "control_enabled") != NULL && json_object_get(liquidsoap, "control_enabled") == json_true()) {
			if (json_object_get(liquidsoap, "control_host") == NULL || !json_is_string(json_object_get(liquidsoap, "control_host")) || json_string_length(json_object_get(liquidsoap, "control_host")) > 128) {
				json_array_append_new(to_return, json_pack("{ss}", "control_host", "control_host is mandatory and must be a string up to 128 characters"));
			}
			
			if (json_object_get(liquidsoap, "control_port") == NULL || !json_is_integer(json_object_get(liquidsoap, "control_port")) || json_integer_value(json_object_get(liquidsoap, "control_port")) < 1 || json_integer_value(json_object_get(liquidsoap, "control_port")) > 65535) {
				json_array_append_new(to_return, json_pack("{ss}", "control_port", "control_port is mandatory and must be an integer between 1 and 65535"));
			}
			
			if (json_object_get(liquidsoap, "control_request_name") == NULL || !json_is_string(json_object_get(liquidsoap, "control_request_name")) || json_string_length(json_object_get(liquidsoap, "control_request_name")) > 64) {
				json_array_append_new(to_return, json_pack("{ss}", "control_request_name", "control_request_name is mandatory and must be a string up to 64 characters"));
			}
			
		}
  }
  
  return to_return;
}

/**
 * add the specified liquidsoap
 */
json_t * liquidsoap_add(struct _carleon_config * config, json_t * liquidsoap) {
  json_t * j_query, * j_reasons;
  int res;
  
  j_reasons = is_liquidsoap_valid(config, liquidsoap, 1);
  if (j_reasons != NULL && json_array_size(j_reasons) == 0) {
		j_query = json_pack("{sss{sssssssssisssIss}}",
                      "table",
                        LIQUIDSOAP_TABLE_NAME,
                      "values",
                        "csl_name", json_string_value(json_object_get(liquidsoap, "name")),
                        "csl_description", json_object_get(liquidsoap, "description")!=NULL?json_string_value(json_object_get(liquidsoap, "description")):"",
                        "csl_radio_url", json_string_value(json_object_get(liquidsoap, "radio_url")),
                        "csl_radio_type", json_object_get(liquidsoap, "radio_type")!=NULL?json_string_value(json_object_get(liquidsoap, "radio_type")):"",
                        "csl_control_enabled", json_object_get(liquidsoap, "control_enabled")==json_true()?1:0,
                        "csl_control_host", json_object_get(liquidsoap, "control_enabled")==json_true()?json_string_value(json_object_get(liquidsoap, "control_host")):"",
                        "csl_control_port", json_object_get(liquidsoap, "control_enabled")==json_true()?json_integer_value(json_object_get(liquidsoap, "control_port")):0,
                        "csl_control_request_name", json_object_get(liquidsoap, "control_enabled")==json_true()?json_string_value(json_object_get(liquidsoap, "control_request_name")):"");
    res = h_insert(config->conn, j_query, NULL);
    json_decref(j_reasons);
    json_decref(j_query);
    if (res == H_OK) {
      return json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
    } else {
      return json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
    }
  } else if (j_reasons != NULL && json_array_size(j_reasons) > 0) {
    return json_pack("{siso}", "result", WEBSERVICE_RESULT_PARAM, "reason", j_reasons);
  } else {
    return json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  }
}

/**
 * update the specified liquidsoap
 */
json_t * liquidsoap_set(struct _carleon_config * config, const char * name, json_t * liquidsoap) {
  json_t * j_query,* j_reasons;
  int res;
  
  j_reasons = is_liquidsoap_valid(config, liquidsoap, 0);
  if (j_reasons != NULL && json_array_size(j_reasons) == 0) {
		j_query = json_pack("{sss{sssssssisssIss}s{ss}}",
                      "table",
                        LIQUIDSOAP_TABLE_NAME,
                      "set",
                        "csl_description", json_object_get(liquidsoap, "description")!=NULL?json_string_value(json_object_get(liquidsoap, "description")):"",
                        "csl_radio_url", json_string_value(json_object_get(liquidsoap, "radio_url")),
                        "csl_radio_type", json_object_get(liquidsoap, "radio_type")!=NULL?json_string_value(json_object_get(liquidsoap, "radio_type")):"",
                        "csl_control_enabled", json_object_get(liquidsoap, "control_enabled")==json_true()?1:0,
                        "csl_control_host", json_object_get(liquidsoap, "control_enabled")==json_true()?json_string_value(json_object_get(liquidsoap, "control_host")):"",
                        "csl_control_port", json_object_get(liquidsoap, "control_enabled")==json_true()?json_integer_value(json_object_get(liquidsoap, "control_port")):0,
                        "csl_control_request_name", json_object_get(liquidsoap, "control_enabled")==json_true()?json_string_value(json_object_get(liquidsoap, "control_request_name")):"",
                      "where",
                        "csl_name", name);
    res = h_update(config->conn, j_query, NULL);
    json_decref(j_reasons);
    json_decref(j_query);
    if (res == H_OK) {
      return json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
    } else {
      return json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
    }
  } else if (j_reasons != NULL && json_array_size(j_reasons) > 0) {
    return json_pack("{siso}", "result", WEBSERVICE_RESULT_PARAM, "reason", j_reasons);
  } else {
    return json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  }
}

/**
 * remove the specified liquidsoap
 */
json_t * liquidsoap_remove(struct _carleon_config * config, const char * name) {
  json_t * j_query = json_pack("{sss{ss}}",
                      "table",
                        LIQUIDSOAP_TABLE_NAME,
                      "where",
                        "csl_name", name);
  int res;
  
  res = h_delete(config->conn, j_query, NULL);
  json_decref(j_query);
  
  if (res == H_OK) {
    return json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
  } else {
    return json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  }
}

char * socket_send_command(const char * host, int port, const char * command) {
	int sockfd;
	struct sockaddr_in serv_addr;
	struct hostent * server;
	ssize_t len;
  size_t ret_len = 0;
	char buffer[256];
	char * to_return = NULL;
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		y_log_message(Y_LOG_LEVEL_ERROR, "ERROR opening socket");
		return NULL;
	}
	
	server = gethostbyname(host);
	if (server == NULL) {
		y_log_message(Y_LOG_LEVEL_ERROR, "ERROR, no such host");
		return NULL;
	}
	
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, 
			 (char *)&serv_addr.sin_addr.s_addr,
			 (size_t)server->h_length);
	serv_addr.sin_port = htons((uint16_t)port);
	
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
		y_log_message(Y_LOG_LEVEL_ERROR, "ERROR connecting to host %s port %d", host, port);
		return NULL;
	}
	
	len = write(sockfd, command, strlen(command));
	if (len < 0) {
		y_log_message(Y_LOG_LEVEL_ERROR, "ERROR writing to socket command %s", command);
		return NULL;
	}

	while ((len = read(sockfd, buffer, 255)) > 0) {
		to_return = o_realloc(to_return, ret_len + (size_t)len + 1);
		memcpy(to_return + ret_len, buffer, (size_t)len);
		ret_len += (size_t)len;
    to_return[ret_len] = '\0';
	}
	close(sockfd);
	
	return to_return;
}

json_t * liquidsoap_list(struct _carleon_config * config, json_t * liquidsoap) {
  UNUSED(config);
	char * command = msprintf("%s.metadata\nquit\n", json_string_value(json_object_get(liquidsoap, "control_request_name")));
	char * result = socket_send_command(json_string_value(json_object_get(liquidsoap, "control_host")), (int)json_integer_value(json_object_get(liquidsoap, "control_port")), command);
	char * result_save, * token, * saveptr, * tmp;
	
	json_t * to_return, * cur_song = NULL;
	
	if (result != NULL) {
		to_return = json_pack("{sis[]}", "result", WEBSERVICE_RESULT_OK, "list");
		result_save = result;
		token = strtok_r(result_save, "\n", &saveptr);
		while (token != NULL) {
			if (o_strncmp(token, "--- ", 4 * sizeof(char)) == 0 && o_strncmp(token + strlen(token) - 4, " ---", 4 * sizeof(char)) == 0) {
				// New song
				if (cur_song != NULL) {
					json_array_insert_new(json_object_get(to_return, "list"), 0, cur_song);
				}
				cur_song = json_object();
			} else if (cur_song != NULL) {
				if (0 != o_strncmp(token, "END", strlen("END")) && 0 != o_strncmp(token, "Bye!", strlen("Bye!"))) {
					char * key, * value;
					if (strchr(token, '=') != NULL) {
						key = o_strndup(token, (size_t)(strchr(token, '=') - token));
						value = o_strdup(strchr(token, '=') + 2);
						if (strlen(value) > 0) {
							value[strlen(value) - 1] = '\0';
						}
						tmp = msprintf("$%s$", key);
						if (o_strstr(LIQUIDSOAP_FIELDS, tmp) != NULL) {
							json_object_set_new(cur_song, key, json_string(value));
						}
						o_free(tmp);
						o_free(key);
						o_free(value);
					}
				} else {
					json_array_insert_new(json_object_get(to_return, "list"), 0, cur_song);
					cur_song = NULL;
				}
			}
			token = strtok_r(NULL, "\n", &saveptr);
		}
		o_free(result_save);
	} else {
		y_log_message(Y_LOG_LEVEL_ERROR, "liquidsoap_list - Error result is NULL");
		to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
	}
	
	o_free(command);
	return to_return;
}

json_t * liquidsoap_on_air(struct _carleon_config * config, json_t * liquidsoap) {
  UNUSED(config);
	char * command = msprintf("%s.metadata\nquit\n", json_string_value(json_object_get(liquidsoap, "control_request_name")));
	char * result = socket_send_command(json_string_value(json_object_get(liquidsoap, "control_host")), (int)json_integer_value(json_object_get(liquidsoap, "control_port")), command);
	char * result_save, * token, * saveptr, * tmp;
	
	json_t * to_return, * cur_song = json_object();
	
	if (result != NULL && cur_song != NULL) {
		to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
		result_save = result;
		token = strtok_r(result_save, "\n", &saveptr);
		while (token != NULL) {
			if (0 != o_strncmp(token, "END", strlen("END")) && 0 != o_strncmp(token, "Bye!", strlen("Bye!"))) {
				char * key, * value;
				if (strchr(token, '=') != NULL) {
					key = o_strndup(token, (size_t)(strchr(token, '=') - token));
					value = o_strdup(strchr(token, '=') + 2);
					if (strlen(value) > 0) {
						value[strlen(value) - 1] = '\0';
					}
					tmp = msprintf("$%s$", key);
					if (o_strstr(LIQUIDSOAP_FIELDS, tmp) != NULL) {
						json_object_set_new(cur_song, key, json_string(value));
					}
					o_free(tmp);
					o_free(key);
					o_free(value);
				}
			} else {
				json_object_set_new(to_return, "on_air", cur_song);
				break;
			}
			token = strtok_r(NULL, "\n", &saveptr);
		}
		o_free(result_save);
	} else {
		y_log_message(Y_LOG_LEVEL_ERROR, "liquidsoap_on_air - Error result is NULL");
		json_decref(cur_song);
		to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
	}
	
	o_free(command);
	return to_return;
}

json_t * liquidsoap_command(struct _carleon_config * config, json_t * liquidsoap, const char * command) {
  UNUSED(config);
	char * str_command, * result, * check;
	json_t * to_return = NULL;
	
	check = msprintf("$%s$", command);
	if (strchr(command, '$') == NULL && strstr(LIQUIDSOAP_COMMANDS, check) != NULL) {
		str_command = msprintf("%s.%s\nquit\n", json_string_value(json_object_get(liquidsoap, "control_request_name")), command);
		result = socket_send_command(json_string_value(json_object_get(liquidsoap, "control_host")), (int)json_integer_value(json_object_get(liquidsoap, "control_port")), str_command);
		
		if (result != NULL) {
			char * value = o_strndup(result, (size_t)(strchr(result, '\n') - result - 1));
			to_return = json_pack("{siss}", "result", WEBSERVICE_RESULT_OK, "value", value);
			o_free(value);
		} else {
			to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
		}
		o_free(str_command);
		o_free(result);
	} else {
		to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_PARAM);
	}
	o_free(check);
	
	return to_return;
}

/**
 * Callback function
 * get the specified liquidsoap or all liquidsoap
 */
int callback_service_liquidsoap_get (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * j_service_liquidsoap;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_liquidsoap_get - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    j_service_liquidsoap = liquidsoap_get((struct _carleon_config *)user_data, u_map_get(request->map_url, "element_id"));
    if (j_service_liquidsoap != NULL && json_integer_value(json_object_get(j_service_liquidsoap, "result")) == WEBSERVICE_RESULT_OK) {
      set_response_json_body_and_clean(response, 200, json_copy(json_object_get(j_service_liquidsoap, "element")));
    } else if (j_service_liquidsoap != NULL && json_integer_value(json_object_get(j_service_liquidsoap, "result")) == WEBSERVICE_RESULT_NOT_FOUND) {
      response->status = 404;
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_liquidsoap_get - Error get_service_motion_list");
      response->status = 500;
    }
    json_decref(j_service_liquidsoap);
  }
  return U_CALLBACK_CONTINUE;
}

/**
 * Callback function
 * add the specified liquidsoap
 */
int callback_service_liquidsoap_add (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * result, * json_body = ulfius_get_json_body_request(request, NULL);
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_liquidsoap_add - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    result = liquidsoap_add((struct _carleon_config *)user_data, json_body);
    if (result == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_liquidsoap_add - Error in liquidsoap_add");
      response->status = 500;
    } else if (json_integer_value(json_object_get(result, "result")) == WEBSERVICE_RESULT_PARAM) {
      set_response_json_body_and_clean(response, 400, json_copy(json_object_get(result, "reason")));
    }
    json_decref(result);
  }
  json_decref(json_body);
  return U_CALLBACK_CONTINUE;
}

/**
 * Callback function
 * update the specified liquidsoap
 */
int callback_service_liquidsoap_set (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * result, * liquidsoap, * json_body = ulfius_get_json_body_request(request, NULL);
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_liquidsoap_set - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    liquidsoap = liquidsoap_get((struct _carleon_config *)user_data, u_map_get(request->map_url, "element_id"));
    if (liquidsoap != NULL && json_integer_value(json_object_get(liquidsoap, "result")) == WEBSERVICE_RESULT_NOT_FOUND) {
      response->status = 404;
    } else if (liquidsoap != NULL && json_integer_value(json_object_get(liquidsoap, "result")) == WEBSERVICE_RESULT_OK) {
      result = liquidsoap_set((struct _carleon_config *)user_data, u_map_get(request->map_url, "element_id"), json_body);
      if (result == NULL) {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_liquidsoap_set - Error in liquidsoap_set");
        response->status = 500;
      } else if (json_integer_value(json_object_get(result, "result")) == WEBSERVICE_RESULT_PARAM) {
        set_response_json_body_and_clean(response, 400, json_copy(json_object_get(result, "reason")));
      }
      json_decref(result);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_liquidsoap_set - Error in liquidsoap_get");
      response->status = 500;
    }
    json_decref(liquidsoap);
  }
  json_decref(json_body);
  return U_CALLBACK_CONTINUE;
}

/**
 * Callback function
 * remove the specified liquidsoap
 */
int callback_service_liquidsoap_remove (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * result, * liquidsoap;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_liquidsoap_remove - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    liquidsoap = liquidsoap_get((struct _carleon_config *)user_data, u_map_get(request->map_url, "element_id"));
    if (liquidsoap != NULL && json_integer_value(json_object_get(liquidsoap, "result")) == WEBSERVICE_RESULT_NOT_FOUND) {
      response->status = 404;
    } else if (liquidsoap != NULL && json_integer_value(json_object_get(liquidsoap, "result")) == WEBSERVICE_RESULT_OK) {
      result = liquidsoap_remove((struct _carleon_config *)user_data, u_map_get(request->map_url, "element_id"));
      if (result == NULL) {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_liquidsoap_remove - Error in liquidsoap_remove");
        response->status = 500;
      } else if (json_integer_value(json_object_get(result, "result")) == WEBSERVICE_RESULT_PARAM) {
        set_response_json_body_and_clean(response, 400, json_copy(json_object_get(result, "reason")));
      }
      json_decref(result);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_liquidsoap_remove - Error in liquidsoap_get");
      response->status = 500;
    }
    json_decref(liquidsoap);
  }
  return U_CALLBACK_CONTINUE;
}

int callback_service_liquidsoap_list (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * result, * liquidsoap;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_liquidsoap_list - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    liquidsoap = liquidsoap_get((struct _carleon_config *)user_data, u_map_get(request->map_url, "element_id"));
    if (liquidsoap != NULL && json_integer_value(json_object_get(liquidsoap, "result")) == WEBSERVICE_RESULT_NOT_FOUND) {
      response->status = 404;
    } else if (liquidsoap != NULL && json_integer_value(json_object_get(liquidsoap, "result")) == WEBSERVICE_RESULT_OK) {
      result = liquidsoap_list((struct _carleon_config *)user_data, json_object_get(liquidsoap, "element"));
      if (result == NULL || json_integer_value(json_object_get(result, "result")) == WEBSERVICE_RESULT_ERROR) {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_liquidsoap_list - Error in liquidsoap_list");
        response->status = 500;
      } else if (json_integer_value(json_object_get(result, "result")) == WEBSERVICE_RESULT_PARAM) {
        set_response_json_body_and_clean(response, 400, json_copy(json_object_get(result, "reason")));
      } else {
        set_response_json_body_and_clean(response, 200, json_copy(json_object_get(result, "list")));
			}
      json_decref(result);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_liquidsoap_list - Error in liquidsoap_get");
      response->status = 500;
    }
    json_decref(liquidsoap);
  }
  return U_CALLBACK_CONTINUE;
}

int callback_service_liquidsoap_on_air (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * result, * liquidsoap;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_liquidsoap_on_air - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    liquidsoap = liquidsoap_get((struct _carleon_config *)user_data, u_map_get(request->map_url, "element_id"));
    if (liquidsoap != NULL && json_integer_value(json_object_get(liquidsoap, "result")) == WEBSERVICE_RESULT_NOT_FOUND) {
      response->status = 404;
    } else if (liquidsoap != NULL && json_integer_value(json_object_get(liquidsoap, "result")) == WEBSERVICE_RESULT_OK) {
      result = liquidsoap_on_air((struct _carleon_config *)user_data, json_object_get(liquidsoap, "element"));
      if (result == NULL || json_integer_value(json_object_get(result, "result")) == WEBSERVICE_RESULT_ERROR) {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_liquidsoap_on_air - Error in liquidsoap_on_air");
        response->status = 500;
      } else if (json_integer_value(json_object_get(result, "result")) == WEBSERVICE_RESULT_PARAM) {
        set_response_json_body_and_clean(response, 400, json_copy(json_object_get(result, "reason")));
      } else {
        set_response_json_body_and_clean(response, 200, json_copy(json_object_get(result, "on_air")));
			}
      json_decref(result);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_liquidsoap_on_air - Error in liquidsoap_get");
      response->status = 500;
    }
    json_decref(liquidsoap);
  }
  return U_CALLBACK_CONTINUE;
}

int callback_service_liquidsoap_command (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * liquidsoap, * j_result;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_liquidsoap_command - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    liquidsoap = liquidsoap_get((struct _carleon_config *)user_data, u_map_get(request->map_url, "element_id"));
    if (liquidsoap != NULL && json_integer_value(json_object_get(liquidsoap, "result")) == WEBSERVICE_RESULT_NOT_FOUND) {
      response->status = 404;
    } else if (liquidsoap != NULL && json_integer_value(json_object_get(liquidsoap, "result")) == WEBSERVICE_RESULT_OK) {
			j_result = liquidsoap_command((struct _carleon_config *)user_data, json_object_get(liquidsoap, "element"), u_map_get(request->map_url, "command_name"));
			if (j_result == NULL || json_integer_value(json_object_get(j_result, "result")) == WEBSERVICE_RESULT_ERROR) {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_liquidsoap_command - Error in liquidsoap_command");
				response->status = 500;
      } else if (json_integer_value(json_object_get(j_result, "result")) == WEBSERVICE_RESULT_PARAM) {
				response->status = 400;
      } else if (json_object_get(j_result, "value") != NULL) {
        set_response_json_body_and_clean(response, 200, json_pack("{so}", "value", json_copy(json_object_get(j_result, "value"))));
			}
			json_decref(j_result);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_liquidsoap_command - Error in liquidsoap_get");
      response->status = 500;
    }
    json_decref(liquidsoap);
  }
  return U_CALLBACK_CONTINUE;
}

json_t * c_service_init(struct _carleon_config * config) {
  if (config != NULL) {
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix, "/service-liquidsoap/@element_id", 2, &callback_service_liquidsoap_get, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix, "/service-liquidsoap/", 2, &callback_service_liquidsoap_get, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "POST", config->url_prefix, "/service-liquidsoap/", 2, &callback_service_liquidsoap_add, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "PUT", config->url_prefix, "/service-liquidsoap/@element_id", 2, &callback_service_liquidsoap_set, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "DELETE", config->url_prefix, "/service-liquidsoap/@element_id", 2, &callback_service_liquidsoap_remove, (void*)config);

    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix, "/service-liquidsoap/@element_id/list", 2, &callback_service_liquidsoap_list, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix, "/service-liquidsoap/@element_id/on_air", 2, &callback_service_liquidsoap_on_air, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix, "/service-liquidsoap/@element_id/command/@command_name", 2, &callback_service_liquidsoap_command, (void*)config);
    
    return json_pack("{sissss}", 
                      "result", WEBSERVICE_RESULT_OK,
                      "name", "service-liquidsoap",
                      "description", "Service to control a liquidsoap instance");
  } else {
    return json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  }
}

json_t * c_service_close(struct _carleon_config * config) {
  if (config != NULL) {
    ulfius_remove_endpoint_by_val(config->instance, "GET", config->url_prefix, "/service-liquidsoap/@element_id");
    ulfius_remove_endpoint_by_val(config->instance, "GET", config->url_prefix, "/service-liquidsoap/");
    ulfius_remove_endpoint_by_val(config->instance, "POST", config->url_prefix, "/service-liquidsoap/");
    ulfius_remove_endpoint_by_val(config->instance, "PUT", config->url_prefix, "/service-liquidsoap/@element_id");
    ulfius_remove_endpoint_by_val(config->instance, "DELETE", config->url_prefix, "/service-liquidsoap/@element_id");

    ulfius_remove_endpoint_by_val(config->instance, "GET", config->url_prefix, "/service-liquidsoap/@element_id/list");
    ulfius_remove_endpoint_by_val(config->instance, "GET", config->url_prefix, "/service-liquidsoap/@element_id/on_air");
    ulfius_remove_endpoint_by_val(config->instance, "GET", config->url_prefix, "/service-liquidsoap/@element_id/command/@command_name");
    
    return json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
  } else {
    return json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  }
}

/**
 * 4 commands
 */
json_t * c_service_command_get_list(struct _carleon_config * config) {
  UNUSED(config);
  json_t * to_return = json_pack("{siso}", "result", WEBSERVICE_RESULT_OK, "commands", json_object());
  
  if (to_return != NULL) {
    json_object_set_new(json_object_get(to_return, "commands"), "stop", json_object());
    json_object_set_new(json_object_get(to_return, "commands"), "pause", json_object());
  }
  return to_return;
}

json_t * c_service_element_get_list(struct _carleon_config * config) {
  return liquidsoap_get(config, NULL);
}

json_t * c_service_exec(struct _carleon_config * config, const char * command, const char * element, json_t * parameters) {
  UNUSED(config);
  UNUSED(command);
  UNUSED(element);
  UNUSED(parameters);
	return json_object();
}
