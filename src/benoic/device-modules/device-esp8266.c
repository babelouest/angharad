/**
 *
 * Benoic House Automation service
 *
 * Command house automation devices via an HTTP REST interface
 *
 * esp8266 device module
 *
 * Copyright 2023 Nicolas Mora <mail@babelouest.org>
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

/** Macro to avoid compiler warning when some parameters are unused and that's ok **/
#define UNUSED(x) (void)(x)

#include <string.h>
#include <jansson.h>
#include <math.h>
#include <time.h>
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

void init_request_for_device(struct _u_request * req, json_t * device, const char * command) {
  ulfius_init_request(req);
  // Set request timeout to 20 seconds
  req->timeout = 20;
  if (json_object_get(json_object_get(device, "options"), "do_not_check_certificate") == json_true()) {
    req->check_server_certificate = 0;
  }
  req->http_url = msprintf("%s/%s", json_string_value(json_object_get(json_object_get(device, "options"), "uri")), command);
}

/**
 * Initializes the device type by getting its uid, name and description
 */
json_t * b_device_type_init () {
  json_t * options = json_array();
  json_array_append_new(options, json_pack("{ssssssso}", "name", "uri", "type", "string", "description", "uri to connect to the device", "optional", json_false()));
  json_array_append_new(options, json_pack("{ssssssso}", "name", "do_not_check_certificate", "type", "boolean", "description", "check the certificate of the device if needed", "optional", json_true()));
  return json_pack("{sissssssso}", 
                    "result", WEBSERVICE_RESULT_OK,
                    "uid", "24-67-42", 
                    "name", "ESP8266 Device", 
                    "description", "ESP8266 Device", 
                    "options", options);
}

/**
 * connects the device
 */
json_t * b_device_connect (json_t * device, void ** device_ptr) {
  UNUSED(device_ptr);
  json_t * j_param;
  struct _u_request req;
  struct _u_response resp;
  
  init_request_for_device(&req, device, "");
  ulfius_init_response(&resp);
  if (ulfius_send_http_request(&req, &resp) == U_OK) {
    if (resp.status == 200) {
      j_param = json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "b_device_connect - Error invalid status: %d", resp.status);
      j_param = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "b_device_connect - Error ulfius_send_http_request");
    j_param = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  }
  ulfius_clean_request(&req);
  ulfius_clean_response(&resp);

  return j_param;
}

/**
 * disconnects the device
 */
json_t * b_device_disconnect (json_t * device, void * device_ptr) {
  UNUSED(device);
  UNUSED(device_ptr);
  return json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
}

/**
 * Ping the device type
 */
json_t * b_device_ping (json_t * device, void * device_ptr) {
  UNUSED(device_ptr);
  json_t * j_result;
  struct _u_request req;
  struct _u_response resp;
  
  init_request_for_device(&req, device, "");
  ulfius_init_response(&resp);
  if (ulfius_send_http_request(&req, &resp) == U_OK) {
    if (resp.status == 200) {
      j_result = json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
    } else {
      j_result = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
    }
  } else {
    j_result = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  }
  ulfius_clean_request(&req);
  ulfius_clean_response(&resp);

  return j_result;
}

/**
 * Get the device overview
 * Returns a mocked overview with 2 sensors, 2 switches, 2 dimmers and 2 heaters
 */
json_t * b_device_overview (json_t * device, void * device_ptr) {
  UNUSED(device_ptr);
  json_t * j_result, * j_resp = NULL;
  struct _u_request req;
  struct _u_response resp;
  
  init_request_for_device(&req, device, "overview");
  ulfius_init_response(&resp);
  if (ulfius_send_http_request(&req, &resp) == U_OK) {
    if (resp.status == 200) {
      if ((j_resp = ulfius_get_json_body_response(&resp, NULL)) != NULL) {
        j_result = json_pack("{sisO}", "result", WEBSERVICE_RESULT_OK, "sensors", j_resp);
      } else {
        j_result = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
      }
      json_decref(j_resp);
    } else {
      j_result = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
    }
  } else {
    j_result = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  }
  ulfius_clean_request(&req);
  ulfius_clean_response(&resp);

  return j_result;
}

/**
 * Get the sensor value
 */
json_t * b_device_get_sensor (json_t * device, const char * sensor_name, void * device_ptr) {
  UNUSED(device_ptr);
  json_t * j_result, * j_resp = NULL;
  struct _u_request req;
  struct _u_response resp;
  
  init_request_for_device(&req, device, sensor_name);
  ulfius_init_response(&resp);
  if (ulfius_send_http_request(&req, &resp) == U_OK) {
    if (resp.status == 200) {
      if ((j_resp = ulfius_get_json_body_response(&resp, NULL)) != NULL) {
        j_result = json_pack("{sisO}", "result", WEBSERVICE_RESULT_OK, "value", json_object_get(j_resp, sensor_name));
      } else {
        j_result = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
      }
      json_decref(j_resp);
    } else {
      j_result = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
    }
  } else {
    j_result = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  }
  ulfius_clean_request(&req);
  ulfius_clean_response(&resp);

  return j_result;
}

/**
 * Get the switch value
 */
json_t * b_device_get_switch (json_t * device, const char * switch_name, void * device_ptr) {
  UNUSED(device);
  UNUSED(switch_name);
  UNUSED(device_ptr);
  return json_pack("{si}", "result", WEBSERVICE_RESULT_NOT_FOUND);
}

/**
 * Set the switch command
 */
json_t * b_device_set_switch (json_t * device, const char * switch_name, const int command, void * device_ptr) {
  UNUSED(device);
  UNUSED(switch_name);
  UNUSED(command);
  UNUSED(device_ptr);
  return json_pack("{si}", "result", WEBSERVICE_RESULT_NOT_FOUND);
}

/**
 * Get the dimmer value
 */
json_t * b_device_get_dimmer (json_t * device, const char * dimmer_name, void * device_ptr) {
  UNUSED(device);
  UNUSED(dimmer_name);
  UNUSED(device_ptr);
  return json_pack("{si}", "result", WEBSERVICE_RESULT_NOT_FOUND);
}

/**
 * Set the dimmer command
 */
json_t * b_device_set_dimmer (json_t * device, const char * dimmer_name, const int command, void * device_ptr) {
  UNUSED(device);
  UNUSED(dimmer_name);
  UNUSED(command);
  UNUSED(device_ptr);
  return json_pack("{si}", "result", WEBSERVICE_RESULT_NOT_FOUND);
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
 * Return true if an element with the specified name and the specified type exist in this device
 */
int b_device_has_element (json_t * device, int element_type, const char * element_name, void * device_ptr) {
  UNUSED(device_ptr);
  UNUSED(element_type);
  struct _u_request req;
  struct _u_response resp;
  int ret = 0;
  
  init_request_for_device(&req, device, element_name);
  ulfius_init_response(&resp);
  if (ulfius_send_http_request(&req, &resp) == U_OK) {
    if (resp.status == 200) {
      ret = 1;
    }
  }
  ulfius_clean_request(&req);
  ulfius_clean_response(&resp);

  return ret;
}
