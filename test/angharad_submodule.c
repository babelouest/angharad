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

#define AUTH_SERVER_URI "http://localhost:4593/api/"
#define ANGHARAD_SERVER_URI "http://localhost:2473"
#define USER_LOGIN "user1"
#define USER_PASSWORD "MyUser1Password!"
#define USER_SCOPE_LIST "angharad"

struct _u_request user_req;
char * angharad_uri;

START_TEST(test_angharad_get_submodule_list)
{
  char * url = msprintf("%s/angharad/submodule", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_get_submodule)
{
  char * url = msprintf("%s/angharad/submodule/benoic", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_get_submodule_not_found)
{
  char * url = msprintf("%s/angharad/submodule/not_invented", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 404, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_get_submodule_benoic_enabled)
{
  char * url = msprintf("%s/angharad/submodule/benoic", angharad_uri);
  json_t * j_result = json_loads("{\
        \"name\": \"benoic\",\
        \"description\": \"House automation devices management\",\
        \"enabled\": true\
    }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, j_result, NULL, NULL);
  free(url);
  json_decref(j_result);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_get_submodule_carleon_enabled)
{
  char * url = msprintf("%s/angharad/submodule/carleon", angharad_uri);
  json_t * j_result = json_loads("{\
        \"name\": \"carleon\",\
        \"description\": \"House automation side services management\",\
        \"enabled\": true\
    }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, j_result, NULL, NULL);
  free(url);
  json_decref(j_result);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_get_submodule_gareth_enabled)
{
  char * url = msprintf("%s/angharad/submodule/gareth", angharad_uri);
  json_t * j_result = json_loads("{\
        \"name\": \"gareth\",\
        \"description\": \"Messenger service\",\
        \"enabled\": true\
    }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, j_result, NULL, NULL);
  free(url);
  json_decref(j_result);
	ck_assert_int_eq(res, 1);
}
END_TEST

static Suite *angharad_suite(void)
{
	Suite *s;
	TCase *tc_core;

	s = suite_create("Angharad submodules management");
	tc_core = tcase_create("test_angharad_submodule");
	tcase_add_test(tc_core, test_angharad_get_submodule_list);
	tcase_add_test(tc_core, test_angharad_get_submodule);
	tcase_add_test(tc_core, test_angharad_get_submodule_not_found);
	tcase_add_test(tc_core, test_angharad_get_submodule_benoic_enabled);
	tcase_add_test(tc_core, test_angharad_get_submodule_carleon_enabled);
	tcase_add_test(tc_core, test_angharad_get_submodule_gareth_enabled);
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
  
  y_init_logs("Angharad test", Y_LOG_MODE_CONSOLE, Y_LOG_LEVEL_DEBUG, NULL, "Starting Angharad test");
  
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
    
    s = angharad_suite();
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
