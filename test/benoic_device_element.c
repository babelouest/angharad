/* Public domain, no copyright. Use at your own risk. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include <check.h>
#include <ulfius.h>
#include <orcania.h>
#include <yder.h>

#include "unit-tests.h"

#define AUTH_SERVER_URI "http://localhost:4593/api"
#define ANGHARAD_SERVER_URI "http://localhost:2473"
#define USER_LOGIN "user1"
#define USER_PASSWORD "MyUser1Password!"
#define USER_SCOPE_LIST "angharad"

struct _u_request user_req;
char * angharad_uri;

START_TEST(test_benoic_device_create_ok)
{
  char * url = msprintf("%s/benoic/device", angharad_uri);
  json_t * device = json_loads("{\
    \"name\":\"dev1\",\
    \"description\":\"first test\",\
    \"type_uid\":\"00-00-00\",\
    \"enabled\":true,\
    \"options\":{\
      \"uri\":\"dev1\",\
      \"baud\":6900,\
      \"do_not_check_certificate\":false,\
      \"device_specified\":\"TBD\"\
    }\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "POST", url, NULL, NULL, device, NULL, 200, NULL, NULL, NULL);
  free(url);
  json_decref(device);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_benoic_device_sensor_get_unavailable)
{
  char * url = msprintf("%s/benoic/device/dev1/sensor/se1", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 400, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_benoic_device_connect_ok)
{
  char * url = msprintf("%s/benoic/device/dev1/connect", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_benoic_device_overview_ok)
{
  char * url = msprintf("%s/benoic/device/dev1/overview", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_benoic_device_sensor_get_ok)
{
  char * url = msprintf("%s/benoic/device/dev1/sensor/se1", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_benoic_device_sensor_update_ok)
{
  char * url = msprintf("%s/benoic/device/dev1/sensor/se1", angharad_uri);
  json_t * sensor = json_loads("\
  {\
    \"display\": \"Sensor one\",\
    \"description\": \"First sensor\",\
    \"enabled\": true,\
    \"monitored_every\": 0,\
    \"options\":\
    {\
      \"unit\": \"°C\"\
    },\
    \"monitored\": false\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "PUT", url, NULL, NULL, sensor, NULL, 200, NULL, NULL, NULL);
  free(url);
  json_decref(sensor);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_benoic_device_sensor_update_error_enabled)
{
  char * url = msprintf("%s/benoic/device/dev1/sensor/se1", angharad_uri);
  json_t * sensor = json_loads("\
  {\
    \"display\": \"Sensor one\",\
    \"description\": \"First sensor\",\
    \"enabled\": \"error\",\
    \"monitored_every\": 0,\
    \"options\":\
    {\
      \"unit\": \"°C\"\
    },\
    \"monitored\": false\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "PUT", url, NULL, NULL, sensor, NULL, 400, NULL, NULL, NULL);
  free(url);
  json_decref(sensor);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_benoic_device_sensor_update_error_json_format)
{
  char * url = msprintf("%s/benoic/device/dev1/sensor/se1", angharad_uri);
  json_t * sensor = json_loads("\
  {\
    \"display\": \"Sensor one\",\
    \"description\": \"First sensor\",\
    \"enabled\": true,\
    \"monitored_every\": 0\
    \"options\":\
    {\
      \"unit\": \"°C\"\
    },\
    \"monitored\": false\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "PUT", url, NULL, NULL, sensor, NULL, 400, NULL, NULL, NULL);
  free(url);
  json_decref(sensor);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_benoic_device_sensor_update_error_empty)
{
  char * url = msprintf("%s/benoic/device/dev1/sensor/se1", angharad_uri);
  
  int res = run_simple_test(&user_req, "PUT", url, NULL, NULL, NULL, NULL, 400, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_benoic_device_sensor_get_updated_ok)
{
  char * url = msprintf("%s/benoic/device/dev1/sensor/se1", angharad_uri);
  json_t * sensor = json_loads("\
  {\
    \"display\": \"Sensor one\",\
    \"description\": \"First sensor\",\
    \"enabled\": true,\
    \"monitored_every\": 0,\
    \"options\":\
    {\
      \"unit\": \"°C\"\
    },\
    \"monitored\": false\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, sensor, NULL, NULL);
  free(url);
  json_decref(sensor);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_benoic_device_sensor_get_not_found)
{
  char * url = msprintf("%s/benoic/device/dev1/sensor/se3", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 404, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_benoic_device_switch_get_ok)
{
  char * url = msprintf("%s/benoic/device/dev1/switch/sw1", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_benoic_device_switch_update_ok)
{
  char * url = msprintf("%s/benoic/device/dev1/switch/sw1", angharad_uri);
  json_t * switcher = json_loads("\
  {\
    \"value\": 0,\
    \"display\": \"Switch one\",\
    \"enabled\": true,\
    \"monitored_every\": 0,\
    \"description\": \"First switch\",\
    \"options\":\
    {\
    },\
    \"monitored\": false\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "PUT", url, NULL, NULL, switcher, NULL, 200, NULL, NULL, NULL);
  free(url);
  json_decref(switcher);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_benoic_device_switch_get_updated_ok)
{
  char * url = msprintf("%s/benoic/device/dev1/switch/sw1", angharad_uri);
  json_t * switcher = json_loads("\
  {\
    \"value\": 0,\
    \"display\": \"Switch one\",\
    \"enabled\": true,\
    \"monitored_every\": 0,\
    \"description\": \"First switch\",\
    \"options\":\
    {\
    },\
    \"monitored\": false\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, switcher, NULL, NULL);
  free(url);
  json_decref(switcher);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_benoic_device_switch_toggle_ok)
{
  char * url = msprintf("%s/benoic/device/dev1/switch/sw1/1", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_benoic_device_switch_toggle_error)
{
  char * url = msprintf("%s/benoic/device/dev1/switch/sw1/error", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 400, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_benoic_device_switch_toggle_not_found)
{
  char * url = msprintf("%s/benoic/device/dev1/switch/sw3/1", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 404, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_benoic_device_switch_get_not_found)
{
  char * url = msprintf("%s/benoic/device/dev1/switch/sw3", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 404, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_benoic_device_dimmer_get_ok)
{
  char * url = msprintf("%s/benoic/device/dev1/dimmer/di1", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_benoic_device_dimmer_update_ok)
{
  char * url = msprintf("%s/benoic/device/dev1/dimmer/di1", angharad_uri);
  json_t * dimmer = json_loads("\
  {\
    \"value\": 42,\
    \"display\": \"Dimmer one\",\
    \"description\": \"First dimmer\",\
    \"enabled\": true,\
    \"options\":\
    {\
    },\
    \"monitored_every\": 0,\
    \"monitored\": false\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "PUT", url, NULL, NULL, dimmer, NULL, 200, NULL, NULL, NULL);
  free(url);
  json_decref(dimmer);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_benoic_device_dimmer_get_updated_ok)
{
  char * url = msprintf("%s/benoic/device/dev1/dimmer/di1", angharad_uri);
  json_t * dimmer = json_loads("\
  {\
    \"value\": 42,\
    \"display\": \"Dimmer one\",\
    \"description\": \"First dimmer\",\
    \"enabled\": true,\
    \"options\":\
    {\
    },\
    \"monitored_every\": 0,\
    \"monitored\": false\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, dimmer, NULL, NULL);
  free(url);
  json_decref(dimmer);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_benoic_device_dimmer_set_ok)
{
  char * url = msprintf("%s/benoic/device/dev1/dimmer/di1/12", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_benoic_device_dimmer_set_error)
{
  char * url = msprintf("%s/benoic/device/dev1/dimmer/di1/error", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 400, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_benoic_device_dimmer_set_not_found)
{
  char * url = msprintf("%s/benoic/device/dev1/dimmer/di3/1", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 404, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_benoic_device_dimmer_get_not_found)
{
  char * url = msprintf("%s/benoic/device/dev1/dimmer/di3", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 404, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_benoic_device_heater_get_ok)
{
  char * url = msprintf("%s/benoic/device/dev1/heater/he1", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_benoic_device_heater_update_ok)
{
  char * url = msprintf("%s/benoic/device/dev1/heater/he1", angharad_uri);
  json_t * heater = json_loads("\
  {\
    \"display\": \"Heater one\",\
    \"enabled\": true,\
    \"monitored_every\": 0,\
    \"description\": \"First heater\",\
    \"command\": 18.0,\
    \"monitored\": false,\
    \"options\":\
    {\
      \"unit\": \"°C\"\
    },\
    \"mode\": \"auto\"\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "PUT", url, NULL, NULL, heater, NULL, 200, NULL, NULL, NULL);
  free(url);
  json_decref(heater);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_benoic_device_heater_get_updated_ok)
{
  char * url = msprintf("%s/benoic/device/dev1/heater/he1", angharad_uri);
  json_t * heater = json_loads("\
  {\
    \"display\": \"Heater one\",\
    \"enabled\": true,\
    \"monitored_every\": 0,\
    \"description\": \"First heater\",\
    \"command\": 18.0,\
    \"monitored\": false,\
    \"options\":\
    {\
      \"unit\": \"°C\"\
    },\
    \"mode\": \"auto\"\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, heater, NULL, NULL);
  free(url);
  json_decref(heater);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_benoic_device_heater_set_ok)
{
  char * url = msprintf("%s/benoic/device/dev1/heater/he1/20?mode=manual", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_benoic_device_heater_set_no_mode_ok)
{
  char * url = msprintf("%s/benoic/device/dev1/heater/he1/21", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_benoic_device_heater_set_error)
{
  char * url = msprintf("%s/benoic/device/dev1/heater/he1/error", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 400, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_benoic_device_heater_set_mode_error)
{
  char * url = msprintf("%s/benoic/device/dev1/heater/he1/20?mode=error", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 400, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_benoic_device_heater_set_not_found)
{
  char * url = msprintf("%s/benoic/device/dev1/heater/he3/20?mode=manual", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 404, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_benoic_device_heater_get_not_found)
{
  char * url = msprintf("%s/benoic/device/dev1/heater/he3", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 404, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_benoic_device_delete_ok)
{
  char * url = msprintf("%s/benoic/device/dev1", angharad_uri);
  
  int res = run_simple_test(&user_req, "DELETE", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

static Suite *benoic_suite(void)
{
	Suite *s;
	TCase *tc_core;

	s = suite_create("Benoic device management");
	tc_core = tcase_create("test_benoic_device");
	tcase_add_test(tc_core, test_benoic_device_create_ok);
	tcase_add_test(tc_core, test_benoic_device_sensor_get_unavailable);
	tcase_add_test(tc_core, test_benoic_device_connect_ok);
	tcase_add_test(tc_core, test_benoic_device_overview_ok);
	tcase_add_test(tc_core, test_benoic_device_sensor_get_ok);
	tcase_add_test(tc_core, test_benoic_device_sensor_update_ok);
	tcase_add_test(tc_core, test_benoic_device_sensor_update_error_enabled);
	tcase_add_test(tc_core, test_benoic_device_sensor_update_error_json_format);
	tcase_add_test(tc_core, test_benoic_device_sensor_update_error_empty);
	tcase_add_test(tc_core, test_benoic_device_sensor_get_updated_ok);
	tcase_add_test(tc_core, test_benoic_device_sensor_get_not_found);
	tcase_add_test(tc_core, test_benoic_device_switch_get_ok);
	tcase_add_test(tc_core, test_benoic_device_switch_update_ok);
	tcase_add_test(tc_core, test_benoic_device_switch_get_updated_ok);
	tcase_add_test(tc_core, test_benoic_device_switch_toggle_ok);
	tcase_add_test(tc_core, test_benoic_device_switch_toggle_error);
	tcase_add_test(tc_core, test_benoic_device_switch_toggle_not_found);
	tcase_add_test(tc_core, test_benoic_device_switch_get_not_found);
	tcase_add_test(tc_core, test_benoic_device_dimmer_get_ok);
	tcase_add_test(tc_core, test_benoic_device_dimmer_update_ok);
	tcase_add_test(tc_core, test_benoic_device_dimmer_get_updated_ok);
	tcase_add_test(tc_core, test_benoic_device_dimmer_set_ok);
	tcase_add_test(tc_core, test_benoic_device_dimmer_set_error);
	tcase_add_test(tc_core, test_benoic_device_dimmer_set_not_found);
	tcase_add_test(tc_core, test_benoic_device_dimmer_get_not_found);
	tcase_add_test(tc_core, test_benoic_device_heater_get_ok);
	tcase_add_test(tc_core, test_benoic_device_heater_update_ok);
	tcase_add_test(tc_core, test_benoic_device_heater_get_updated_ok);
	tcase_add_test(tc_core, test_benoic_device_heater_set_ok);
	tcase_add_test(tc_core, test_benoic_device_heater_set_no_mode_ok);
	tcase_add_test(tc_core, test_benoic_device_heater_set_error);
	tcase_add_test(tc_core, test_benoic_device_heater_set_mode_error);
	tcase_add_test(tc_core, test_benoic_device_heater_set_not_found);
	tcase_add_test(tc_core, test_benoic_device_heater_get_not_found);
	tcase_add_test(tc_core, test_benoic_device_delete_ok);
	tcase_set_timeout(tc_core, 30);
	suite_add_tcase(s, tc_core);

	return s;
}

int main(int argc, char *argv[])
{
  int number_failed;
  Suite *s;
  SRunner *sr;
  struct _u_request auth_req;
  struct _u_response auth_resp;
  int res;
	char * bearer_token;
  
  y_init_logs("Benoic test", Y_LOG_MODE_CONSOLE, Y_LOG_LEVEL_DEBUG, NULL, "Starting Benoic test");
  
  // Getting a refresh_token
  ulfius_init_request(&auth_req);
  ulfius_init_request(&user_req);
  ulfius_init_response(&auth_resp);
  angharad_uri = argc>5?argv[5]:ANGHARAD_SERVER_URI;
  auth_req.http_verb = strdup("POST");
  auth_req.http_url = msprintf("%s/token/", argc>4?argv[4]:AUTH_SERVER_URI);
  u_map_put(auth_req.map_post_body, "grant_type", "password");
  u_map_put(auth_req.map_post_body, "username", argc>1?argv[1]:USER_LOGIN);
  u_map_put(auth_req.map_post_body, "password", argc>2?argv[2]:USER_PASSWORD);
  u_map_put(auth_req.map_post_body, "scope", argc>3?argv[3]:USER_SCOPE_LIST);
  res = ulfius_send_http_request(&auth_req, &auth_resp);
  if (res == U_OK && auth_resp.status == 200) {
    json_t * json_body = ulfius_get_json_body_response(&auth_resp, NULL);
    bearer_token = msprintf("Bearer %s", json_string_value(json_object_get(json_body, "access_token")));
	  y_log_message(Y_LOG_LEVEL_INFO, "User %s authenticated", argc>1?argv[1]:USER_LOGIN, json_dumps(json_body, JSON_ENCODE_ANY), auth_resp.status);
    u_map_put(user_req.map_header, "Authorization", bearer_token);
    free(bearer_token);
    json_decref(json_body);
    
    s = benoic_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_VERBOSE);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
  
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "Error authentication user %s", argc>1?argv[1]:USER_LOGIN);
  }
  ulfius_clean_request(&auth_req);
  ulfius_clean_response(&auth_resp);
  
  ulfius_clean_request(&user_req);
  
  y_close_logs();
  
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
