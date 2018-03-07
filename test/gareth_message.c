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

START_TEST(test_gareth_message_create_1_ok)
{
  char * url = msprintf("%s/gareth/message", angharad_uri);
  json_t * message = json_loads("{\
    \"priority\": \"NONE\",\
    \"source\": \"Socrates\",\
    \"text\": \"δαίμων\",\
    \"tags\": [\"tag1\", \"tag2\", \"tag3\"]\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "POST", url, NULL, NULL, message, NULL, 200, NULL, NULL, NULL);
  free(url);
  json_decref(message);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_gareth_message_create_2_ok)
{
  char * url = msprintf("%s/gareth/message", angharad_uri);
  json_t * message = json_loads("{\
    \"priority\": \"LOW\",\
    \"source\": \"Socrates\",\
    \"text\": \"ἔλεγχος\",\
    \"tags\": [\"tag1\"]\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "POST", url, NULL, NULL, message, NULL, 200, NULL, NULL, NULL);
  free(url);
  json_decref(message);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_gareth_message_create_3_ok)
{
  char * url = msprintf("%s/gareth/message", angharad_uri);
  json_t * message = json_loads("{\
    \"priority\": \"MEDIUM\",\
    \"source\": \"Plato\",\
    \"text\": \"διάνοια diánoia\",\
    \"tags\": [\"tag3\"]\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "POST", url, NULL, NULL, message, NULL, 200, NULL, NULL, NULL);
  free(url);
  json_decref(message);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_gareth_message_create_4_ok)
{
  char * url = msprintf("%s/gareth/message", angharad_uri);
  json_t * message = json_loads("{\
    \"priority\": \"HIGH\",\
    \"source\": \"Aristotle\",\
    \"text\": \"νοῦς τῆς διατριβῆς\",\
    \"tags\": [\"tag2\", \"tag3\"]\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "POST", url, NULL, NULL, message, NULL, 200, NULL, NULL, NULL);
  free(url);
  json_decref(message);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_gareth_message_create_5_ok)
{
  char * url = msprintf("%s/gareth/message", angharad_uri);
  json_t * message = json_loads("{\
    \"priority\": \"CRITICAL\",\
    \"source\": \"Democritus\",\
    \"text\": \"η ιδέα ά-τομος\",\
    \"tags\": [\"tag1\", \"tag3\"]\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "POST", url, NULL, NULL, message, NULL, 200, NULL, NULL, NULL);
  free(url);
  json_decref(message);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_gareth_message_create_1_invalid)
{
  char * url = msprintf("%s/gareth/message", angharad_uri);
  json_t * message = json_loads("{\
    \"priority\": \"LOWW\",\
    \"source\": \"Unit test\",\
    \"text\": \"This is a invalid test!\",\
    \"tags\": [\"tag1\", \"tag2\", \"tag3\"]\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "POST", url, NULL, NULL, message, NULL, 400, NULL, NULL, NULL);
  free(url);
  json_decref(message);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_gareth_message_create_2_invalid)
{
  char * url = msprintf("%s/gareth/message", angharad_uri);
  json_t * message = json_loads("{\
    \"priority\": \"LOW\",\
    \"source\": \"Unit test\",\
    \"text\": \"This is a invalid too long testdddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddssssssssThis is a invalid testdddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddssssssss!\",\
    \"tags\": [\"tag1\", \"tag2\", \"tag3\"]\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "POST", url, NULL, NULL, message, NULL, 400, NULL, NULL, NULL);
  free(url);
  json_decref(message);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_gareth_message_create_3_invalid)
{
  char * url = msprintf("%s/gareth/message", angharad_uri);
  json_t * message = json_loads("{\
    \"priority\": \"LOW\",\
    \"source\": \"Unit test LONG SOURCE eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee\",\
    \"text\": \"This is a invalid test!\",\
    \"tags\": [\"tag1\", \"tag2\", \"tag3\"]\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "POST", url, NULL, NULL, message, NULL, 400, NULL, NULL, NULL);
  free(url);
  json_decref(message);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_gareth_message_create_4_invalid)
{
  char * url = msprintf("%s/gareth/message", angharad_uri);
  json_t * message = json_loads("{\
    \"priority\": \"LOW\",\
    \"source\": \"Unit test\",\
    \"text\": \"This is a invalid test!\",\
    \"tags\": [1, \"tag2\", \"tag3\"]\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "POST", url, NULL, NULL, message, NULL, 400, NULL, NULL, NULL);
  free(url);
  json_decref(message);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_gareth_message_create_5_invalid)
{
  char * url = msprintf("%s/gareth/message", angharad_uri);
  json_t * message = json_loads("{\
    \"priority\": \"LOW\",\
    \"source\": \"Unit test\",\
    \"text\": \"This is a invalid test!\",\
    \"tags\": [\"tag2 invalideeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee\", \"tag3\"]\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "POST", url, NULL, NULL, message, NULL, 400, NULL, NULL, NULL);
  free(url);
  json_decref(message);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_gareth_message_get_list_ok)
{
  char * url = msprintf("%s/gareth/message", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_gareth_filter_1_create_ok)
{
  char * url = msprintf("%s/gareth/filter", angharad_uri);
  json_t * filter = json_loads("{\
    \"name\": \"filter1\",\
    \"description\": \"valid test\",\
    \"filter_clauses\": [\
      {\"element\": \"priority\", \"operator\": \"=\", \"value\": 2}\
    ]\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "POST", url, NULL, NULL, filter, NULL, 200, NULL, NULL, NULL);
  free(url);
  json_decref(filter);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_gareth_filter_2_create_ok)
{
  char * url = msprintf("%s/gareth/filter", angharad_uri);
  json_t * filter = json_loads("{\
    \"name\": \"filter2\",\
    \"description\": \"valid test\",\
    \"filter_clauses\": [\
      {\"element\": \"priority\", \"operator\": \">\", \"value\": 2}\
    ]\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "POST", url, NULL, NULL, filter, NULL, 200, NULL, NULL, NULL);
  free(url);
  json_decref(filter);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_gareth_filter_3_create_ok)
{
  char * url = msprintf("%s/gareth/filter", angharad_uri);
  json_t * filter = json_loads("{\
    \"name\": \"filter3\",\
    \"description\": \"valid test\",\
    \"filter_clauses\": [\
      {\"element\": \"tag\", \"operator\": \"contains\", \"value\": \"tag3\"}\
    ]\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "POST", url, NULL, NULL, filter, NULL, 200, NULL, NULL, NULL);
  free(url);
  json_decref(filter);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_gareth_filter_4_create_ok)
{
  char * url = msprintf("%s/gareth/filter", angharad_uri);
  json_t * filter = json_loads("{\
    \"name\": \"filter4\",\
    \"description\": \"valid test\",\
    \"filter_clauses\": [\
      {\"element\": \"date\", \"operator\": \">=\", \"value\": 1000}\
    ]\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "POST", url, NULL, NULL, filter, NULL, 200, NULL, NULL, NULL);
  free(url);
  json_decref(filter);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_gareth_filter_1_delete_ok)
{
  char * url = msprintf("%s/gareth/filter/filter1", angharad_uri);
  
  int res = run_simple_test(&user_req, "DELETE", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_gareth_filter_2_delete_ok)
{
  char * url = msprintf("%s/gareth/filter/filter2", angharad_uri);
  
  int res = run_simple_test(&user_req, "DELETE", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_gareth_filter_3_delete_ok)
{
  char * url = msprintf("%s/gareth/filter/filter3", angharad_uri);
  
  int res = run_simple_test(&user_req, "DELETE", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_gareth_filter_4_delete_ok)
{
  char * url = msprintf("%s/gareth/filter/filter4", angharad_uri);
  
  int res = run_simple_test(&user_req, "DELETE", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

static Suite *gareth_suite(void)
{
	Suite *s;
	TCase *tc_core;

	s = suite_create("Gareth alert management");
	tc_core = tcase_create("test_gareth_alert_smtp");
	tcase_add_test(tc_core, test_gareth_message_create_1_ok);
	tcase_add_test(tc_core, test_gareth_message_create_2_ok);
	tcase_add_test(tc_core, test_gareth_message_create_3_ok);
	tcase_add_test(tc_core, test_gareth_message_create_4_ok);
	tcase_add_test(tc_core, test_gareth_message_create_5_ok);
	tcase_add_test(tc_core, test_gareth_message_create_1_invalid);
	tcase_add_test(tc_core, test_gareth_message_create_2_invalid);
	tcase_add_test(tc_core, test_gareth_message_create_3_invalid);
	tcase_add_test(tc_core, test_gareth_message_create_4_invalid);
	tcase_add_test(tc_core, test_gareth_message_create_5_invalid);
	tcase_add_test(tc_core, test_gareth_message_get_list_ok);
	tcase_add_test(tc_core, test_gareth_filter_1_create_ok);
	tcase_add_test(tc_core, test_gareth_filter_2_create_ok);
	tcase_add_test(tc_core, test_gareth_filter_3_create_ok);
	tcase_add_test(tc_core, test_gareth_filter_4_create_ok);
	tcase_add_test(tc_core, test_gareth_filter_1_delete_ok);
	tcase_add_test(tc_core, test_gareth_filter_2_delete_ok);
	tcase_add_test(tc_core, test_gareth_filter_3_delete_ok);
	tcase_add_test(tc_core, test_gareth_filter_4_delete_ok);
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
    
    s = gareth_suite();
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
