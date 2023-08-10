/**
 *
 * Benoic House Automation service
 *
 * Command house automation devices via an HTTP REST interface
 *
 * Mock device module
 * Provides all the commands for a fake device
 * Used to develop and validate benoic infrastructure above
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
#include <math.h>
#include <time.h>
#include <yder.h>
#include <orcania.h>

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
#define ELEMENT_TYPE_BLIND  5

#define BENOIC_ELEMENT_HEATER_MODE_OFF     "off"
#define BENOIC_ELEMENT_HEATER_MODE_MANUAL  "manual"
#define BENOIC_ELEMENT_HEATER_MODE_AUTO    "auto"

#define NB_SECONDS_PER_DAY 86400

json_t * b_device_get_switch (json_t * device, const char * switch_name, void * device_ptr);
json_t * b_device_get_dimmer (json_t * device, const char * dimmer_name, void * device_ptr);
json_t * b_device_get_heater (json_t * device, const char * heater_name, void * device_ptr);
json_t * b_device_get_blind (json_t * device, const char * heater_name, void * device_ptr);

/**
 * Get the value sensor of a sensor using a sinus model
 */
double get_sensor_value(const char * sensor_name) {
  time_t now = time(0);
  struct tm local;
  gmtime_r(&now, &local);
  int current_nb_seconds = local.tm_sec + (60 * local.tm_min) + (60 * 60 * local.tm_hour);
  if (strtol(sensor_name+2, NULL, 10)%2) {
    return sin((double)current_nb_seconds / (double)NB_SECONDS_PER_DAY);
  } else {
    return ((sin(((double)((current_nb_seconds + (NB_SECONDS_PER_DAY / 2)) % NB_SECONDS_PER_DAY)) / (double)NB_SECONDS_PER_DAY) * 5.0) + 15.0);
  }
}

/**
 * Initializes the device type by getting its uid, name and description
 */
json_t * b_device_type_init () {
  json_t * options = json_array();
  json_array_append_new(options, json_pack("{ssssssso}", "name", "uri", "type", "string", "description", "uri to connect to the device", "optional", json_false()));
  json_array_append_new(options, json_pack("{ssssssso}", "name", "baud", "type", "numeric", "description", "speed of the device communication", "optional", json_false()));
  json_array_append_new(options, json_pack("{ssssssso}", "name", "nb_switch", "type", "numeric", "description", "number of switches", "optional", json_true()));
  json_array_append_new(options, json_pack("{ssssssso}", "name", "nb_dimmer", "type", "numeric", "description", "number of dimmers", "optional", json_true()));
  json_array_append_new(options, json_pack("{ssssssso}", "name", "nb_heater", "type", "numeric", "description", "number of heaters", "optional", json_true()));
  json_array_append_new(options, json_pack("{ssssssso}", "name", "nb_sensor", "type", "numeric", "description", "number of sensors", "optional", json_true()));
  json_array_append_new(options, json_pack("{ssssssso}", "name", "nb_blind", "type", "numeric", "description", "number of blinds", "optional", json_true()));
  json_array_append_new(options, json_pack("{ssssssso}", "name", "do_not_check_certificate", "type", "boolean", "description", "check the certificate of the device if needed", "optional", json_true()));
  json_array_append_new(options, json_pack("{ssssssso}", "name", "device_specified", "type", "string", "description", "specified by the device when connected for the first time, then must be sent back at every other connection", "optional", json_true()));
  return json_pack("{sissssssso}", 
                    "result", WEBSERVICE_RESULT_OK,
                    "uid", "00-00-00", 
                    "name", "Another Mock Device", 
                    "description", "This is another mock device, for development and debug purposes", 
                    "options", options);
}

/**
 * connects the device
 */
json_t * b_device_connect (json_t * device, void ** device_ptr) {
  char * param;
  json_t * j_param;
  json_int_t i, max;
  char name[65] = {0};
  
  if (device_ptr != NULL) {
    // Allocating *device_ptr for further use
    *device_ptr = (json_t *)json_pack("{s{}s{}s{}s{}s{}s{}}",
                             "switches",
                             "dimmers",
                             "dimmers_values",
                             "heaters",
                             "sensors",
                             "blinds");
  }
  if (json_object_get(json_object_get(device, "options"), "nb_switch") == NULL || (max = json_integer_value(json_object_get(json_object_get(device, "options"), "nb_switch"))) < 0 || max > 64) {
    max = 2;
  }
  for (i=0; i<max; i++) {
    snprintf(name, 64, "sw%"JSON_INTEGER_FORMAT, i);
    json_object_set_new(json_object_get((json_t *)*device_ptr, "switches"), name, json_integer(i%2));
  }
  if (json_object_get(json_object_get(device, "options"), "nb_dimmer") == NULL || (max = json_integer_value(json_object_get(json_object_get(device, "options"), "nb_dimmer"))) < 0 || max > 64) {
    max = 2;
  }
  for (i=0; i<max; i++) {
    snprintf(name, 64, "di%"JSON_INTEGER_FORMAT, i);
    json_object_set_new(json_object_get((json_t *)*device_ptr, "dimmers"), name, json_integer(i%2?42:5));
    json_object_set_new(json_object_get((json_t *)*device_ptr, "dimmers_values"), name, json_integer(i%2?42:5));
  }
  if (json_object_get(json_object_get(device, "options"), "nb_heater") == NULL || (max = json_integer_value(json_object_get(json_object_get(device, "options"), "nb_heater"))) < 0 || max > 64) {
    max = 2;
  }
  for (i=0; i<max; i++) {
    snprintf(name, 64, "he%"JSON_INTEGER_FORMAT, i);
    json_object_set_new(json_object_get((json_t *)*device_ptr, "heaters"), name, json_pack("{sssfsos[sss]}", "mode", "auto", "command", i%2?20:18.0, "on", json_true(), "availableModes", "auto", "manual", "off"));
  }
  if (json_object_get(json_object_get(device, "options"), "nb_sensor") == NULL || (max = json_integer_value(json_object_get(json_object_get(device, "options"), "nb_sensor"))) < 0 || max > 64) {
    max = 2;
  }
  for (i=0; i<max; i++) {
    snprintf(name, 64, "se%"JSON_INTEGER_FORMAT, i);
    json_object_set_new(json_object_get((json_t *)*device_ptr, "sensors"), name, json_real(0.0));
  }
  if (json_object_get(json_object_get(device, "options"), "nb_blind") == NULL || (max = json_integer_value(json_object_get(json_object_get(device, "options"), "nb_blind"))) < 0 || max > 64) {
    max = 2;
  }
  for (i=0; i<max; i++) {
    snprintf(name, 64, "bl%"JSON_INTEGER_FORMAT, i);
    json_object_set_new(json_object_get((json_t *)*device_ptr, "blinds"), name, json_integer(i%2?42:5));
  }
  
  if (o_strstr(json_string_value(json_object_get(json_object_get(device, "options"), "device_specified")), "batman") == NULL) {
    param = msprintf("%s says I'm batman with the alert_url %s", json_string_value(json_object_get(device, "name")), json_string_value(json_object_get(json_object_get(device, "options"), "alert_url")));
    j_param = json_pack("{sis{ss}}", "result", WEBSERVICE_RESULT_OK, "options", "device_specified", param);
    o_free(param);
  } else {
    j_param = json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
  }
  return j_param;
}

/**
 * disconnects the device
 */
json_t * b_device_disconnect (json_t * device, void * device_ptr) {
  UNUSED(device);
  if (device_ptr != NULL) {
    // Free device_ptr
    json_decref((json_t *)device_ptr);
  }
  return json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
}

/**
 * Ping the device type
 */
json_t * b_device_ping (json_t * device, void * device_ptr) {
  UNUSED(device);
  UNUSED(device_ptr);
  return json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
}

/**
 * Get the device overview
 * Returns a mocked overview with 2 sensors, 2 switches, 2 dimmers and 2 heaters
 */
json_t * b_device_overview (json_t * device, void * device_ptr) {
  y_log_message(Y_LOG_LEVEL_INFO, "device-mock - Running command overview for device %s", json_string_value(json_object_get(device, "name")));
  const char * name = NULL;
  json_t * j_element = NULL, * j_copy = NULL;
  int i = 0;
  
  json_t * result = json_pack("{sis{}s{}s{}s{}s{}}",
                              "result", 
                              WEBSERVICE_RESULT_OK,
                              "sensors",
                              "switches",
                              "dimmers",
                              "heaters",
                              "blinds");
  json_object_foreach(json_object_get((json_t *)device_ptr, "sensors"), name, j_element) {
    json_object_set_new(json_object_get(result, "sensors"), name, json_pack("{sssf}", "unit", i%2?"C":"P", "value", get_sensor_value(name)));
    i++;
  }
  json_object_foreach(json_object_get((json_t *)device_ptr, "switches"), name, j_element) {
    json_object_set(json_object_get(result, "switches"), name, j_element);
  }
  json_object_foreach(json_object_get((json_t *)device_ptr, "dimmers"), name, j_element) {
    json_object_set(json_object_get(result, "dimmers"), name, j_element);
  }
  json_object_foreach(json_object_get((json_t *)device_ptr, "heaters"), name, j_element) {
    j_copy = json_deep_copy(j_element);
    json_object_del(j_copy, "result");
    json_object_set_new(json_object_get(result, "heaters"), name, j_copy);
  }
  json_object_foreach(json_object_get((json_t *)device_ptr, "blinds"), name, j_element) {
    json_object_set(json_object_get(result, "blinds"), name, j_element);
  }
  return result;
}

/**
 * Get the sensor value
 */
json_t * b_device_get_sensor (json_t * device, const char * sensor_name, void * device_ptr) {
  UNUSED(device);
  UNUSED(device_ptr);
  y_log_message(Y_LOG_LEVEL_INFO, "device-mock - Running command sensor for sensor %s on device %s", sensor_name, json_string_value(json_object_get(device, "name")));
  if (json_object_get(json_object_get((json_t *)device_ptr, "sensors"), sensor_name) != NULL) {
    return json_pack("{sisf}", "result", WEBSERVICE_RESULT_OK, "value", get_sensor_value(sensor_name));
  } else {
    return json_pack("{si}", "result", WEBSERVICE_RESULT_NOT_FOUND);
  }
}

/**
 * Get the switch value
 */
json_t * b_device_get_switch (json_t * device, const char * switch_name, void * device_ptr) {
  y_log_message(Y_LOG_LEVEL_INFO, "device-mock - Running command get_switch for switch %s on device %s", switch_name, json_string_value(json_object_get(device, "name")));
  if (json_object_get(json_object_get((json_t *)device_ptr, "switches"), switch_name) != NULL) {
    return json_pack("{sisi}", "result", WEBSERVICE_RESULT_OK, "value", json_integer_value(json_object_get(json_object_get((json_t *)device_ptr, "switches"), switch_name)));
  } else {
    return json_pack("{si}", "result", WEBSERVICE_RESULT_NOT_FOUND);
  }
}

/**
 * Set the switch command
 */
json_t * b_device_set_switch (json_t * device, const char * switch_name, const int command, void * device_ptr) {
  y_log_message(Y_LOG_LEVEL_INFO, "device-mock - Running command set_switch for switch %s on device %s with the value %d", switch_name, json_string_value(json_object_get(device, "name")), command);
  if (json_object_get(json_object_get((json_t *)device_ptr, "switches"), switch_name) != NULL) {
    json_object_set_new(json_object_get((json_t *)device_ptr, "switches"), switch_name, json_integer(command));
    return json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
  } else {
    return json_pack("{si}", "result", WEBSERVICE_RESULT_NOT_FOUND);
  }
}

/**
 * Get the dimmer value
 */
json_t * b_device_get_dimmer (json_t * device, const char * dimmer_name, void * device_ptr) {
  y_log_message(Y_LOG_LEVEL_INFO, "device-mock - Running command get_dimmer for dimmer %s on device %s", dimmer_name, json_string_value(json_object_get(device, "name")));
  if (json_object_get(json_object_get((json_t *)device_ptr, "dimmers"), dimmer_name) != NULL) {
    return json_pack("{sisI}", "result", WEBSERVICE_RESULT_OK, "value", json_integer_value(json_object_get(json_object_get((json_t *)device_ptr, "dimmers"), dimmer_name)));
  } else {
    return json_pack("{si}", "result", WEBSERVICE_RESULT_NOT_FOUND);
  }
}

/**
 * Set the dimmer command
 */
json_t * b_device_set_dimmer (json_t * device, const char * dimmer_name, const int command, void * device_ptr) {
  y_log_message(Y_LOG_LEVEL_INFO, "device-mock - Running command set_dimmer for dimmer %s on device %s with the value %d", dimmer_name, json_string_value(json_object_get(device, "name")), command);
  if (json_object_get(json_object_get((json_t *)device_ptr, "dimmers"), dimmer_name) != NULL) {
    if (command < 101) {
      json_object_set_new(json_object_get((json_t *)device_ptr, "dimmers"), dimmer_name, json_integer(command));
      if (command > 0) {
        json_object_set_new(json_object_get((json_t *)device_ptr, "dimmers_values"), dimmer_name, json_integer(command));
      }
    } else {
      json_object_set_new(json_object_get((json_t *)device_ptr, "dimmers"), dimmer_name, json_copy(json_object_get(json_object_get((json_t *)device_ptr, "dimmers_values"), dimmer_name)));
    }
    return json_pack("{sisI}", "result", WEBSERVICE_RESULT_OK, "value", json_integer_value(json_object_get(json_object_get((json_t *)device_ptr, "dimmers"), dimmer_name)));
  } else {
    return json_pack("{si}", "result", WEBSERVICE_RESULT_NOT_FOUND);
  }
}

/**
 * Get the heater value
 */
json_t * b_device_get_heater (json_t * device, const char * heater_name, void * device_ptr) {
  y_log_message(Y_LOG_LEVEL_INFO, "device-mock - Running command get_heater for heater %s on device %s", heater_name, json_string_value(json_object_get(device, "name")));
  if (json_object_get(json_object_get((json_t *)device_ptr, "heaters"), heater_name) != NULL) {
    json_t * heater = json_copy(json_object_get(json_object_get((json_t *)device_ptr, "heaters"), heater_name));
    json_object_set_new(heater, "result", json_integer(WEBSERVICE_RESULT_OK));
    return heater;
  } else {
    return json_pack("{si}", "result", WEBSERVICE_RESULT_NOT_FOUND);
  }
}

/**
 * Set the heater command
 */
json_t * b_device_set_heater (json_t * device, const char * heater_name, const char * mode, const float command, void * device_ptr) {
  y_log_message(Y_LOG_LEVEL_INFO, "device-mock - Running command set_heater for heater %s on device %s with the value %f and the mode %s", heater_name, json_string_value(json_object_get(device, "name")), command, mode);
  if (json_object_get(json_object_get((json_t *)device_ptr, "heaters"), heater_name) != NULL) {
    json_t * heater = json_object_get(json_object_get((json_t *)device_ptr, "heaters"), heater_name);
    if (mode != NULL && 0 == o_strcmp(mode, BENOIC_ELEMENT_HEATER_MODE_MANUAL)) {
      json_object_set_new(heater, "mode", json_string(BENOIC_ELEMENT_HEATER_MODE_MANUAL));
    } else if (mode != NULL && 0 == o_strcmp(mode,  BENOIC_ELEMENT_HEATER_MODE_AUTO)) {
      json_object_set_new(heater, "mode", json_string(BENOIC_ELEMENT_HEATER_MODE_AUTO));
    } else if (mode != NULL && 0 == o_strcmp(mode, BENOIC_ELEMENT_HEATER_MODE_OFF)) {
      json_object_set_new(heater, "mode", json_string(BENOIC_ELEMENT_HEATER_MODE_OFF));
    } else if (mode != NULL) {
      return json_pack("{si}", "result", WEBSERVICE_RESULT_PARAM);
    }
    json_object_set_new(heater, "command", json_real(command));
    return json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
  } else {
    return json_pack("{si}", "result", WEBSERVICE_RESULT_NOT_FOUND);
  }
}

/**
 * Get the blind value
 */
json_t * b_device_get_blind (json_t * device, const char * blind_name, void * device_ptr) {
  UNUSED(device);
  y_log_message(Y_LOG_LEVEL_INFO, "device-mock - Running command get_blind for blind ");
  if (json_object_get(json_object_get((json_t *)device_ptr, "blinds"), blind_name) != NULL) {
    return json_pack("{sisI}", "result", WEBSERVICE_RESULT_OK, "value", json_integer_value(json_object_get(json_object_get((json_t *)device_ptr, "blinds"), blind_name)));
  } else {
    return json_pack("{si}", "result", WEBSERVICE_RESULT_NOT_FOUND);
  }
}

/**
 * Set the blind command
 */
json_t * b_device_set_blind (json_t * device, const char * blind_name, int command, void * device_ptr) {
  y_log_message(Y_LOG_LEVEL_INFO, "device-mock - Running command set_blind for blind %s on device %s with the value %d", blind_name, json_string_value(json_object_get(device, "name")), command);
  if (json_object_get(json_object_get((json_t *)device_ptr, "blinds"), blind_name) != NULL) {
    if (command <= 100 && command >= 0) {
      json_object_set_new(json_object_get((json_t *)device_ptr, "blinds"), blind_name, json_integer(command));
    }
    return json_pack("{sisI}", "result", WEBSERVICE_RESULT_OK, "value", json_integer_value(json_object_get(json_object_get((json_t *)device_ptr, "blinds"), blind_name)));
  } else {
    return json_pack("{si}", "result", WEBSERVICE_RESULT_NOT_FOUND);
  }
}

/**
 * Return true if an element with the specified name and the specified type exist in this device
 */
int b_device_has_element (json_t * device, int element_type, const char * element_name, void * device_ptr) {
  UNUSED(device);
  UNUSED(device_ptr);
  y_log_message(Y_LOG_LEVEL_INFO, "device-mock - Checking if element '%s' of type %d exists in device %s", element_name, element_type, json_string_value(json_object_get(device, "name")));
  switch (element_type) {
    case ELEMENT_TYPE_SENSOR:
      return (json_object_get(json_object_get((json_t *)device_ptr, "sensors"), element_name) != NULL);
      break;
    case ELEMENT_TYPE_SWITCH:
      return (json_object_get(json_object_get((json_t *)device_ptr, "switches"), element_name) != NULL);
      break;
    case ELEMENT_TYPE_DIMMER:
      return (json_object_get(json_object_get((json_t *)device_ptr, "dimmers"), element_name) != NULL);
      break;
    case ELEMENT_TYPE_HEATER:
      return (json_object_get(json_object_get((json_t *)device_ptr, "heaters"), element_name) != NULL);
      break;
    case ELEMENT_TYPE_BLIND:
      return (json_object_get(json_object_get((json_t *)device_ptr, "blinds"), element_name) != NULL);
      break;
    default:
      return 0;
      break;
  }
}
