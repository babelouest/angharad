/**
 *
 * Carleon House Automation side services
 *
 * Command house automation side services via an HTTP REST interface
 *
 * Mock service module
 * Provides all the commands for a fake service
 * Used to develop and validate carleon infrastructure above
 *
 * Copyright 2016 Nicolas Mora <mail@babelouest.org>
 *
 * Licence MIT
 *
 */

/** Macro to avoid compiler warning when some parameters are unused and that's ok **/
#define UNUSED(x) (void)(x)

#include <string.h>
#include <jansson.h>
#include <ulfius.h>
#include <hoel.h>
#include <orcania.h>
#include <yder.h>
#include "../carleon.h"

int set_response_json_body_and_clean(struct _u_response * response, uint status, json_t * json_body) {
  int res = ulfius_set_json_body_response(response, status, json_body);
  json_decref(json_body);
  return res;
}

/**
 * The returned value is used by events conditions
 * 
 * The c_service_exec must return a json_t * object of the following format:
 * {
 *    "result": numeric (one of the RESULT_* values)
 *    "value": any value relevant for the use (string, numeric, object, etc.)
 * }
 */
json_t * c_service_exec(struct _carleon_config * config, const char * command, const char * element, json_t * parameters) {
  UNUSED(config);
  char * str_parameters = json_dumps(parameters, JSON_COMPACT);
  y_log_message(Y_LOG_LEVEL_INFO, "mock-service - Executing command '%s' to element '%s' with parameters %s", command, element, str_parameters);
  free(str_parameters);
  if (0 == o_strcmp(command, "exec1") || 0 == o_strcmp(command, "exec4")) {
    return json_pack("{sis{siso}}", "result", WEBSERVICE_RESULT_OK, "value", "value1", 1, "value2", json_true());
  } else {
    return json_pack("{siss}", "result", WEBSERVICE_RESULT_OK, "value", "ok");
  }
}

json_t * mock_element_get(struct _carleon_config * config, const char * element_id) {
  json_t * j_query = json_object(), * j_result;
  int res;
  
  json_object_set_new(j_query, "table", json_string("c_mock_service"));
  json_object_set_new(j_query, "columns", json_pack("[ss]", "cms_name AS name", "cms_description AS description"));
  if (element_id != NULL) {
    json_object_set_new(j_query, "where", json_pack("{ss}", "cms_name", element_id));
  }
  
  res = h_select(config->conn, j_query, &j_result, NULL);
  json_decref(j_query);
  if (res == H_OK) {
    if (json_array_size(j_result) == 0 && element_id != NULL) {
      json_decref(j_result);
      return json_pack("{si}", "result", WEBSERVICE_RESULT_NOT_FOUND);
    } else {
      if (element_id == NULL) {
        json_t * to_return = json_pack("{siso}", "result", WEBSERVICE_RESULT_OK, "element", json_copy(j_result));
        json_decref(j_result);
        return to_return;
      } else {
        json_t * element = json_copy(json_array_get(j_result, 0));
        json_decref(j_result);
        return json_pack("{siso}", "result", WEBSERVICE_RESULT_OK, "element", element);
      }
    }
  } else {
    return json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  }
}

int mock_element_add(struct _carleon_config * config, json_t * element) {
  json_t * cur_element = mock_element_get(config, json_string_value(json_object_get(element, "name"))), 
          * j_query = json_pack("{sss[{ssss}]}", 
                                "table", "c_mock_service", 
                                "values", 
                                  "cms_name", json_string_value(json_object_get(element, "name")),
                                  "cms_description", json_object_get(element, "description")!=NULL?json_string_value(json_object_get(element, "description")):"");
  int res;
  
  if (json_integer_value(json_object_get(cur_element, "result")) == WEBSERVICE_RESULT_OK || json_integer_value(json_object_get(cur_element, "result")) == WEBSERVICE_RESULT_ERROR) {
    json_decref(cur_element);
    json_decref(j_query);
    return WEBSERVICE_RESULT_ERROR;
  }
  json_decref(cur_element);
  
  res = h_insert(config->conn, j_query, NULL);
  json_decref(j_query);
  if (res == H_OK) {
    return WEBSERVICE_RESULT_OK;
  } else {
    return WEBSERVICE_RESULT_ERROR;
  }
}

int mock_element_modify(struct _carleon_config * config, const char * element_id, json_t * element) {
  json_t * cur_element = mock_element_get(config, element_id),
          * j_query = json_pack("{sss{ss}s{ss}}", 
                                "table", "c_mock_service", 
                                "set", 
                                  "cms_description", json_object_get(element, "description")!=NULL?json_string_value(json_object_get(element, "description")):"",
                                "where",
                                  "cms_name", element_id);
  int res;
  
  if (json_integer_value(json_object_get(cur_element, "result")) == WEBSERVICE_RESULT_NOT_FOUND) {
    json_decref(cur_element);
    json_decref(j_query);
    return WEBSERVICE_RESULT_NOT_FOUND;
  }
  json_decref(cur_element);

  res = h_update(config->conn, j_query, NULL);
  json_decref(j_query);
  if (res == H_OK) {
    return WEBSERVICE_RESULT_OK;
  } else {
    return WEBSERVICE_RESULT_ERROR;
  }
}

int mock_element_delete(struct _carleon_config * config, const char * element_id) {
  json_t * cur_element = mock_element_get(config, element_id),
          * j_query = json_pack("{sss{ss}}", 
                                "table", "c_mock_service", 
                                "where", 
                                  "cms_name", element_id);
  int res;
  
  if (json_integer_value(json_object_get(cur_element, "result")) == WEBSERVICE_RESULT_NOT_FOUND) {
    json_decref(cur_element);
    json_decref(j_query);
    return WEBSERVICE_RESULT_NOT_FOUND;
  }
  json_decref(cur_element);

  res = h_delete(config->conn, j_query, NULL);
  json_decref(j_query);
  if (res == H_OK) {
    return WEBSERVICE_RESULT_OK;
  } else {
    return WEBSERVICE_RESULT_ERROR;
  }
}

int callback_mock_service_command (const struct _u_request * request, struct _u_response * response, void * user_data) {
  int i_param2;
  float f_param3;
  json_t * parameters, * j_result;
  
  if (o_strcmp(u_map_get(request->map_url, "command_name"), "exec1") == 0 && u_map_has_key(request->map_url, "param2") && u_map_has_key(request->map_url, "param3")) {
    i_param2 = (int)strtol(u_map_get(request->map_url, "param2"), NULL, 10);
    f_param3 = strtof(u_map_get(request->map_url, "param3"), NULL);
    parameters = json_pack("{sssisf}", "param1", u_map_get(request->map_url, "param1"), "param2", i_param2, "param3", f_param3);
  } else if (o_strcmp(u_map_get(request->map_url, "command_name"), "exec2") == 0) {
    parameters = json_pack("{ss}", "param1", u_map_get(request->map_url, "param1"));
  } else {
    parameters = json_object();
  }
  
  j_result = c_service_exec((struct _carleon_config *)user_data, u_map_get(request->map_url, "command_name"), u_map_get(request->map_url, "element_id"), parameters);
  if (j_result == NULL || json_integer_value(json_object_get(j_result, "result")) != WEBSERVICE_RESULT_OK) {
    response->status = 500;
  } else {
    json_object_del(j_result, "result");
    set_response_json_body_and_clean(response, 200, j_result);
  }
  json_decref(parameters);
  return U_CALLBACK_CONTINUE;
}

int callback_mock_service (const struct _u_request * request, struct _u_response * response, void * user_data) {
  if (0 == o_strcmp(request->http_verb, "GET")) {
    json_t * element = mock_element_get((struct _carleon_config *)user_data, u_map_get(request->map_url, "element_id"));
    if (json_integer_value(json_object_get(element, "result")) == WEBSERVICE_RESULT_ERROR) {
      response->status = 500;
    } else if (json_integer_value(json_object_get(element, "result")) == WEBSERVICE_RESULT_NOT_FOUND) {
      response->status = 404;
    } else {
      set_response_json_body_and_clean(response, 200, json_copy(json_object_get(element, "element")));
    }
    json_decref(element);
  } else if (0 == o_strcmp(request->http_verb, "POST")) {
    json_t * json_body = ulfius_get_json_body_request(request, NULL);
    if (json_object_get(json_body, "name") != NULL && json_is_string(json_object_get(json_body, "name")) && json_string_length(json_object_get(json_body, "name")) <= 64 &&
        json_object_get(json_body, "description") != NULL && json_is_string(json_object_get(json_body, "description")) && json_string_length(json_object_get(json_body, "description")) <= 128) {
      int res = mock_element_add((struct _carleon_config *)user_data, json_body);
      if (res == WEBSERVICE_RESULT_ERROR) {
        response->status = 500;
      }
    } else {
      response->status = 400;
    }
    json_decref(json_body);
  } else if (0 == o_strcmp(request->http_verb, "PUT")) {
    json_t * element = mock_element_get((struct _carleon_config *)user_data, u_map_get(request->map_url, "element_id"));
    json_t * json_body = ulfius_get_json_body_request(request, NULL);
    if (element == NULL) {
      response->status = 404;
    } else if (json_object_get(json_body, "description") != NULL && json_is_string(json_object_get(json_body, "description")) && json_string_length(json_object_get(json_body, "description")) <= 128) {
      if (mock_element_modify((struct _carleon_config *)user_data, u_map_get(request->map_url, "element_id"), json_body) == WEBSERVICE_RESULT_ERROR) {
        response->status = 500;
      }
    } else {
      response->status = 400;
    }
    json_decref(json_body);
    json_decref(element);
  } else if (0 == o_strcmp(request->http_verb, "DELETE")) {
    json_t * element = mock_element_get((struct _carleon_config *)user_data, u_map_get(request->map_url, "element_id"));
    if (element == NULL) {
      response->status = 404;
    } else if (mock_element_delete((struct _carleon_config *)user_data, u_map_get(request->map_url, "element_id")) == WEBSERVICE_RESULT_ERROR) {
      response->status = 500;
    }
    json_decref(element);
  }
  return U_CALLBACK_CONTINUE;
}

json_t * c_service_init(struct _carleon_config * config) {
  if (config != NULL) {
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix, "/mock-service/@element_id", 2, &callback_mock_service, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix, "/mock-service/", 2, &callback_mock_service, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "POST", config->url_prefix, "/mock-service/", 2, &callback_mock_service, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "PUT", config->url_prefix, "/mock-service/@element_id", 2, &callback_mock_service, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "DELETE", config->url_prefix, "/mock-service/@element_id", 2, &callback_mock_service, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix, "/mock-service/@element_id/command/@command_name/@param1/@param2/@param3", 2, &callback_mock_service_command, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix, "/mock-service/@element_id/command/@command_name/@param1/", 2, &callback_mock_service_command, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix, "/mock-service/@element_id/command/@command_name/", 2, &callback_mock_service_command, (void*)config);
    
    return json_pack("{sissss}", 
                      "result", WEBSERVICE_RESULT_OK,
                      "name", "mock-service",
                      "description", "Mock service for Carleon development");
  } else {
    return json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  }
}

json_t * c_service_close(struct _carleon_config * config) {
  if (config != NULL) {
    ulfius_remove_endpoint_by_val(config->instance, "GET", config->url_prefix, "/mock-service/@element_id");
    ulfius_remove_endpoint_by_val(config->instance, "GET", config->url_prefix, "/mock-service/");
    ulfius_remove_endpoint_by_val(config->instance, "POST", config->url_prefix, "/mock-service/");
    ulfius_remove_endpoint_by_val(config->instance, "PUT", config->url_prefix, "/mock-service/@element_id");
    ulfius_remove_endpoint_by_val(config->instance, "DELETE", config->url_prefix, "/mock-service/@element_id");
    ulfius_remove_endpoint_by_val(config->instance, "GET", config->url_prefix, "/mock-service/@element_id/command/@command_name/@param1/@param2/@param3");
    ulfius_remove_endpoint_by_val(config->instance, "GET", config->url_prefix, "/mock-service/@element_id/command/@command_name/@param1/");
    ulfius_remove_endpoint_by_val(config->instance, "GET", config->url_prefix, "/mock-service/@element_id/command/@command_name/");
    
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
  return json_pack("{\
                    si\
                    s{\
                      s{\
                        s{\
                          s{\
                            ssso\
                          }\
                          s{\
                            ssso\
                          }\
                          s{\
                            ss\
                          }\
                        }\
                        s{\
                          s{\
                            ss\
                          }\
                          s{\
                            ss\
                          }\
                        }\
                      }\
                      s{\
                        s{\
                          s{\
                            ssso\
                          }\
                        }\
                        s{\
                          ss\
                        }\
                      }\
                      s{\
                        s{\
                          ss\
                        }\
                      }\
                      s{\
                        s{\
                          s{\
                            ss\
                          }\
                        s{\
                          ss\
                        }\
                      }\
                    }\
                  }\
                }",
                    "result", WEBSERVICE_RESULT_OK,
                    "commands",
                      "exec1",
                        "parameters",
                          "param1",
                            "type", "string", "required", json_true(),
                          "param2",
                            "type", "integer", "required", json_false(),
                          "param3",
                            "type", "real",
                        "result",
                          "value1",
                            "type", "integer",
                          "value2",
                            "type", "boolean",
                      "exec2",
                        "parameters",
                          "param1",
                            "type", "string", "required", json_true(),
                        "result",
                          "type", "string",
                      "exec3",
                        "result",
                          "type", "string",
                      "exec4",
                        "result",
                          "value1",
                            "type", "integer",
                          "value2",
                            "type", "boolean");
}

json_t * c_service_element_get_list(struct _carleon_config * config) {
  return mock_element_get(config, NULL);
}
