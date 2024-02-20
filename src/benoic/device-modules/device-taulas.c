/**
 *
 * Benoic House Automation service
 *
 * Command house automation devices via an HTTP REST interface
 *
 * Taulas device module
 * Provides all the commands for a Taulas device
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

#include <string.h>
#include <jansson.h>
#include <yder.h>
#include <orcania.h>
#include <ulfius.h>

#define WEBSERVICE_RESULT_ERROR     0
#define WEBSERVICE_RESULT_OK        1
#define WEBSERVICE_RESULT_NOT_FOUND 2
#define WEBSERVICE_RESULT_TIMEOUT   3
#define WEBSERVICE_RESULT_PARAM     4

#define ELEMENT_TYPE_NONE   0
#define ELEMENT_TYPE_SENSOR 1
#define ELEMENT_TYPE_SWITCH 2
#define ELEMENT_TYPE_DIMMER 3
#define ELEMENT_TYPE_HEATER 4

#define BENOIC_ELEMENT_HEATER_MODE_OFF     "off"
#define BENOIC_ELEMENT_HEATER_MODE_MANUAL  "manual"
#define BENOIC_ELEMENT_HEATER_MODE_AUTO    "auto"

#define NB_SECONDS_PER_DAY 86400

/** Macro to avoid compiler warning when some parameters are unused and that's ok **/
#define UNUSED(x) (void)(x)

json_t * b_device_get_switch (json_t * device, const char * switch_name, void * device_ptr);
json_t * b_device_get_dimmer (json_t * device, const char * dimmer_name, void * device_ptr);
json_t * b_device_get_heater (json_t * device, const char * heater_name, void * device_ptr);

void init_request_for_device(struct _u_request * req, json_t * device, const char * command) {
  ulfius_init_request(req);
  // Set request timeout to 20 seconds
  req->timeout = 20;
  if (json_object_get(json_object_get(device, "options"), "do_not_check_certificate") == json_true()) {
    req->check_server_certificate = 0;
  }
  if (json_object_get(json_object_get(device, "options"), "user") != NULL && json_string_length(json_object_get(json_object_get(device, "options"), "user")) > 0) {
    req->auth_basic_user = o_strdup(json_string_value(json_object_get(json_object_get(device, "options"), "user")));
  }
  if (json_object_get(json_object_get(device, "options"), "user") != NULL && json_string_length(json_object_get(json_object_get(device, "options"), "user")) > 0) {
    req->auth_basic_user = o_strdup(json_string_value(json_object_get(json_object_get(device, "options"), "user")));
  }
  req->http_url = msprintf("%s%s", json_string_value(json_object_get(json_object_get(device, "options"), "uri")), command);
}

/**
 * Initializes the device type by getting its uid, name and description
 */
json_t * b_device_type_init () {
  json_t * options = json_array();
  json_array_append_new(options, json_pack("{ssssssso}", "name", "uri", "type", "string", "description", "uri to connect to the device", "optional", json_false()));
  json_array_append_new(options, json_pack("{ssssssso}", "name", "send_alert_uri", "type", "string", "description", "uri to use to send the server alert uri to the client", "optional", json_false()));
  json_array_append_new(options, json_pack("{ssssssso}", "name", "do_not_check_certificate", "type", "boolean", "description", "check the certificate of the device if needed", "optional", json_true()));
  json_array_append_new(options, json_pack("{ssssssso}", "name", "old_version", "type", "boolean", "description", "Is the device an old Taulas device or a new one?", "optional", json_true()));
  json_array_append_new(options, json_pack("{ssssssso}", "name", "user", "type", "string", "description", "Username to connect to the device", "optional", json_true()));
  json_array_append_new(options, json_pack("{ssssssso}", "name", "password", "type", "string", "description", "Password to connect to the device", "optional", json_true()));
  return json_pack("{sissssssso}", 
                    "result", WEBSERVICE_RESULT_OK,
                    "uid", "24-67-85", 
                    "name", "Taulas Device", 
                    "description", "Connect to a Taulas device", 
                    "options", options);
}

/**
 * connects the device
 */
json_t * b_device_connect (json_t * device, void ** device_ptr) {
  struct _u_request req;
  int res;
  json_t * j_param;
  
  * device_ptr = o_malloc(sizeof(struct _u_map));
  u_map_init((struct _u_map *)*device_ptr);
  
  init_request_for_device(&req, device, "MARCO");
  
  res = ulfius_send_http_request(&req, NULL);
  if (res == U_OK) {
    j_param = json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
  } else {
    j_param = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  }
  ulfius_clean_request(&req);

  return j_param;
}

/**
 * disconnects the device
 */
json_t * b_device_disconnect (json_t * device, void * device_ptr) {
  UNUSED(device);
  u_map_clean_full((struct _u_map *)device_ptr);
  return json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
}

/**
 * Ping the device type
 */
json_t * b_device_ping (json_t * device, void * device_ptr) {
  struct _u_request req;
  struct _u_response resp;
  int res;
  json_t * j_param;
  
  init_request_for_device(&req, device, "MARCO");
  ulfius_init_response(&resp);
  
  UNUSED(device_ptr);
  res = ulfius_send_http_request(&req, &resp);
  if (res == U_OK && o_strncmp("POLO", (const char *)resp.binary_body, strlen("POLO"))) {
	  if (json_object_get(json_object_get(device, "options"), "old_version") == json_true()) {
      j_param = json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
	  } else {
      json_t * json_body = ulfius_get_json_body_response(&resp, NULL);
      if (resp.status == 200 && json_body != NULL && 0 == o_strcmp(json_string_value(json_object_get(json_body, "value")), "POLO")) {
        j_param = json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
      } else {
        j_param = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
      }
      json_decref(json_body);
    }
  } else {
    j_param = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  }
  ulfius_clean_request(&req);
  ulfius_clean_response(&resp);

  return j_param;
}

/**
 * Get the device overview
 */
json_t * b_device_overview (json_t * device, void * device_ptr) {
  struct _u_request req;
  struct _u_response resp;
  int res;
  json_t * overview = NULL, * j_elt;
  char * saved_body, * str, * token, * token_dup, * saveptr = NULL, * element, * saveptr2 = NULL, * name, * value, * saveptr3 = NULL, * endptr = NULL;
  json_int_t i_value;
  double d_value;
  const char * elt_name;
  struct _u_map * elements = (struct _u_map *)device_ptr;
  
  init_request_for_device(&req, device, "OVERVIEW");
  ulfius_init_response(&resp);
  
  res = ulfius_send_http_request(&req, &resp);
  if (res == U_OK) {
	  if (json_object_get(json_object_get(device, "options"), "old_version") == json_true()) {
      overview = json_object();
      saved_body = o_strndup((const char *)resp.binary_body, resp.binary_body_length);
      saved_body[strlen(saved_body) - 1] = '\0';
      str = saved_body;
      token = strtok_r(str + sizeof(char), ";", &saveptr);
      while (token != NULL) {
        token_dup = o_strdup(token);
        if (o_strncmp("SWITCHES", token_dup, strlen("SWITCHES")) == 0 && o_strstr(token_dup, ",") != NULL) {
          json_object_set_new(overview, "switches", json_object());
          element = strtok_r(token_dup + strlen("SWITCHES,"), ",", &saveptr2);
          while (element != NULL) {
            name = strtok_r(element, ":", &saveptr3);
            value = strtok_r(NULL, ":", &saveptr3);
            if (name != NULL && value != NULL) {
              i_value = strtol(value, &endptr, 10);
              if (value != endptr) {
                json_object_set_new(json_object_get(overview, "switches"), name, json_integer(i_value));
                u_map_put(elements, name, "");
              }
            }
            element = strtok_r(NULL, ",", &saveptr2);
          }
        } else if (o_strncmp("SENSORS", token_dup, strlen("SENSORS")) == 0 && o_strstr(token_dup, ",") != NULL) {
          json_object_set_new(overview, "sensors", json_object());
          element = strtok_r(token_dup + strlen("SENSORS,"), ",", &saveptr2);
          while (element != NULL) {
            name = strtok_r(element, ":", &saveptr3);
            value = strtok_r(NULL, ":", &saveptr3);
            if (name != NULL && value != NULL) {
              d_value = strtod(value, &endptr);
              if (value == endptr) {
                i_value = strtol(value, &endptr, 10);
                if (value == endptr) {
                  json_object_set_new(json_object_get(overview, "sensors"), name, json_string(value));
                  u_map_put(elements, name, "");
                } else {
                  json_object_set_new(json_object_get(overview, "sensors"), name, json_integer(i_value));
                  u_map_put(elements, name, "");
                }
              } else {
                json_object_set_new(json_object_get(overview, "sensors"), name, json_real(d_value));
                u_map_put(elements, name, "");
              }
            }
            element = strtok_r(NULL, ",", &saveptr2);
          }
        } else if (o_strncmp("DIMMERS", token_dup, strlen("DIMMERS")) == 0 && o_strstr(token_dup, ",") != NULL) {
          json_object_set_new(overview, "dimmers", json_object());
          element = strtok_r(token_dup + strlen("DIMMERS,"), ",", &saveptr2);
          while (element != NULL) {
            name = strtok_r(element, ":", &saveptr3);
            value = strtok_r(NULL, ":", &saveptr3);
            if (name != NULL && value != NULL) {
              i_value = strtol(value, &endptr, 10);
              if (value != endptr) {
                json_object_set_new(json_object_get(overview, "dimmers"), name, json_integer(i_value));
                u_map_put(elements, name, "");
              }
            }
            element = strtok_r(NULL, ",", &saveptr2);
          }
        }
        o_free(token_dup);
        token = strtok_r(NULL, ";", &saveptr);
      }
      o_free(saved_body);
      json_object_set_new(overview, "result", json_integer(WEBSERVICE_RESULT_OK));
	  } else {
      json_t * json_body = ulfius_get_json_body_response(&resp, NULL);
      if (resp.status == 200 && json_body != NULL) {
        overview = json_copy(json_body);
        json_object_set_new(overview, "result", json_integer(WEBSERVICE_RESULT_OK));
        
        json_object_foreach(json_object_get(overview, "sensors"), elt_name, j_elt) {
          u_map_put(elements, elt_name, "");
        }
        json_object_foreach(json_object_get(overview, "switches"), elt_name, j_elt) {
          u_map_put(elements, elt_name, "");
        }
        json_object_foreach(json_object_get(overview, "dimmers"), elt_name, j_elt) {
          u_map_put(elements, elt_name, "");
        }
        json_object_foreach(json_object_get(overview, "heaters"), elt_name, j_elt) {
          u_map_put(elements, elt_name, "");
        }
      } else {
        char * str_body = json_dumps(json_body, JSON_ENCODE_ANY);
        y_log_message(Y_LOG_LEVEL_ERROR, "Error getting device overview, url was %s, status code is %ld, json body is %s", req.http_url, resp.status, str_body);
        free(str_body);
        overview = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
      }
      json_decref(json_body);
    }
  } else {
    overview = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  }
  ulfius_clean_request(&req);
  ulfius_clean_response(&resp);

  return overview;
}

/**
 * Get the sensor value
 */
json_t * b_device_get_sensor (json_t * device, const char * sensor_name, void * device_ptr) {
  struct _u_request req;
  struct _u_response resp;
  int res;
  char * path, * endptr, * value;
  json_t * j_result = NULL;
  json_int_t i_value;
  double d_value;
  
  UNUSED(device_ptr);
  path = msprintf("%s/%s", "SENSOR", sensor_name);
  init_request_for_device(&req, device, path);
  ulfius_init_response(&resp);
  
  res = ulfius_send_http_request(&req, &resp);
  if (res == U_OK) {
	  if (json_object_get(json_object_get(device, "options"), "old_version") == json_true()) {
      j_result = json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
      value = ((char*)resp.binary_body) + 1;
      value[strlen(value) - 1] = '\0';
      d_value = strtod(value, &endptr);
      if (value == endptr) {
        i_value = strtol(value, &endptr, 10);
        if (value == endptr) {
          json_object_set_new(j_result, "value", json_string(value));
        } else {
          json_object_set_new(j_result, "value", json_integer(i_value));
        }
      } else {
        json_object_set_new(j_result, "value", json_real(d_value));
      }
	  } else {
      json_t * json_body = ulfius_get_json_body_response(&resp, NULL);
      if (resp.status == 200 && json_body != NULL) {
        j_result = json_copy(json_body);
        json_object_set_new(j_result, "result", json_integer(WEBSERVICE_RESULT_OK));
      } else {
        j_result = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
      }
      json_decref(json_body);
    }
  } else {
    j_result = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  }
  o_free(path);
  ulfius_clean_request(&req);
  ulfius_clean_response(&resp);
  return j_result;
}

/**
 * Get the switch value
 */
json_t * b_device_get_switch (json_t * device, const char * switch_name, void * device_ptr) {
  struct _u_request req;
  struct _u_response resp;
  int res;
  char * path, * endptr, * value;
  json_t * j_result = NULL;
  json_int_t i_value;
  
  UNUSED(device_ptr);
  path = msprintf("%s/%s", "GETSWITCH", switch_name);
  init_request_for_device(&req, device, path);
  ulfius_init_response(&resp);
  
  res = ulfius_send_http_request(&req, &resp);
  if (res == U_OK) {
	  if (json_object_get(json_object_get(device, "options"), "old_version") == json_true()) {
      value = ((char*)resp.binary_body) + 1;
      value[strlen(value) - 1] = '\0';
      i_value = strtol(value, &endptr, 10);
      if (value == endptr) {
        j_result = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
      } else {
        j_result = json_pack("{sisI}", "result", WEBSERVICE_RESULT_OK, "value", i_value);
      }
	  } else {
      json_t * json_body = ulfius_get_json_body_response(&resp, NULL);
      if (resp.status == 200 && json_body != NULL) {
        j_result = json_copy(json_body);
        json_object_set_new(j_result, "result", json_integer(WEBSERVICE_RESULT_OK));
      } else {
        j_result = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
      }
      json_decref(json_body);
    }
  } else {
    j_result = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  }
  o_free(path);
  ulfius_clean_request(&req);
  ulfius_clean_response(&resp);
  return j_result;
}

/**
 * Set the switch command
 */
json_t * b_device_set_switch (json_t * device, const char * switch_name, const int command, void * device_ptr) {
  struct _u_request req;
  struct _u_response resp;
  int res;
  char * path;
  json_t * j_result = NULL;
  
  UNUSED(device_ptr);
  path = msprintf("%s/%s/%d", "SETSWITCH", switch_name, command);
  init_request_for_device(&req, device, path);
  ulfius_init_response(&resp);
  
  res = ulfius_send_http_request(&req, &resp);
  if (res == U_OK) {
	  if (json_object_get(json_object_get(device, "options"), "old_version") == json_true()) {
      j_result = json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
	  } else {
      if (resp.status == 200) {
        j_result = json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
      } else {
        j_result = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
      }
    }
  } else {
    j_result = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  }
  o_free(path);
  ulfius_clean_request(&req);
  ulfius_clean_response(&resp);
  return j_result;
}

/**
 * Get the dimmer value
 */
json_t * b_device_get_dimmer (json_t * device, const char * dimmer_name, void * device_ptr) {
  struct _u_request req;
  struct _u_response resp;
  int res;
  char * path, * endptr, * value;
  json_t * j_result = NULL;
  json_int_t i_value;
  
  UNUSED(device_ptr);
  path = msprintf("%s/%s", "GETDIMMER", dimmer_name);
  init_request_for_device(&req, device, path);
  ulfius_init_response(&resp);
  
  res = ulfius_send_http_request(&req, &resp);
  if (res == U_OK) {
	  if (json_object_get(json_object_get(device, "options"), "old_version") == json_true()) {
      value = ((char*)resp.binary_body) + 1;
      value[strlen(value) - 1] = '\0';
      i_value = strtol(value, &endptr, 10);
      if (value == endptr) {
        j_result = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
      } else {
        j_result = json_pack("{sisI}", "result", WEBSERVICE_RESULT_OK, "value", i_value);
      }
	  } else {
      json_t * json_body = ulfius_get_json_body_response(&resp, NULL);
      if (resp.status == 200 && json_body != NULL) {
        j_result = json_copy(json_body);
        json_object_set_new(j_result, "result", json_integer(WEBSERVICE_RESULT_OK));
      } else {
        j_result = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
      }
      json_decref(json_body);
    }
  } else {
    j_result = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  }
  o_free(path);
  ulfius_clean_request(&req);
  ulfius_clean_response(&resp);
  return j_result;
}

/**
 * Set the dimmer command
 */
json_t * b_device_set_dimmer (json_t * device, const char * dimmer_name, const int command, void * device_ptr) {
  struct _u_request req;
  struct _u_response resp;
  int res;
  char * path;
  json_t * j_result = NULL;
  
  UNUSED(device_ptr);
  path = msprintf("%s/%s/%d", "SETDIMMER", dimmer_name, command);
  init_request_for_device(&req, device, path);
  ulfius_init_response(&resp);
  
  res = ulfius_send_http_request(&req, &resp);
  if (res == U_OK) {
	  if (json_object_get(json_object_get(device, "options"), "old_version") == json_true()) {
      j_result = json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
	  } else {
      if (resp.status == 200) {
        j_result = json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
      } else {
        j_result = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
      }
    }
  } else {
    j_result = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  }
  o_free(path);
  ulfius_clean_request(&req);
  ulfius_clean_response(&resp);
  return j_result;
}

/**
 * Get the heater value
 */
json_t * b_device_get_heater (json_t * device, const char * heater_name, void * device_ptr) {
  UNUSED(device);
  UNUSED(heater_name);
  UNUSED(device_ptr);
  return json_pack("{si}", "result", WEBSERVICE_RESULT_NOT_FOUND);
}

/**
 * Set the heater command
 */
json_t * b_device_set_heater (json_t * device, const char * heater_name, const char * mode, const float command, void * device_ptr) {
  UNUSED(device);
  UNUSED(heater_name);
  UNUSED(mode);
  UNUSED(command);
  UNUSED(device_ptr);
  return json_pack("{si}", "result", WEBSERVICE_RESULT_NOT_FOUND);
}

/**
 * Get the blind value
 */
json_t * b_device_get_blind (json_t * device, const char * blind_name, void * device_ptr) {
  UNUSED(device);
  UNUSED(blind_name);
  UNUSED(device_ptr);
  return json_pack("{si}", "result", WEBSERVICE_RESULT_NOT_FOUND);
}

/**
 * Set the blind command
 */
json_t * b_device_set_blind (json_t * device, const char * blind_name, int command, void * device_ptr) {
  UNUSED(device);
  UNUSED(blind_name);
  UNUSED(command);
  UNUSED(device_ptr);
  return json_pack("{si}", "result", WEBSERVICE_RESULT_NOT_FOUND);
}

/**
 * Return true if an element with the specified name and the specified type exist in this device
 */
int b_device_has_element (json_t * device, int element_type, const char * element_name, void * device_ptr) {
  UNUSED(device);
  UNUSED(element_type);
  return u_map_has_key((struct _u_map *)device_ptr, element_name);
}
