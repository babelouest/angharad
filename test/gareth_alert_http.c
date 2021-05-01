/* Public domain, no copyright. Use at your own risk. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include <check.h>
#include <orcania.h>
#include <yder.h>
#include <ulfius.h>
#include <rhonabwy.h>

#include "unit-tests.h"

#define ANGHARAD_SERVER_URI "http://localhost:2473"
#define USER_LOGIN "user1"
#define USER_SCOPE_LIST "angharad"
#define DEFAULT_JWKS_PATH "../private-test.jwks"

struct _u_request user_req;
char * angharad_uri;

START_TEST(test_gareth_alert_http_get_list)
{
  char * url = msprintf("%s/gareth/alert", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_gareth_alert_http_create_ok)
{
  char * url = msprintf("%s/gareth/alert/http", angharad_uri);
  json_t * alert = json_loads("{\
    \"name\": \"http1\",\
    \"method\": \"GET\",\
    \"url\": \"http://localhost/\",\
    \"body\": \"plop\",\
    \"http_headers\": [\
      {\"key\": \"header1\", \"value\": \"value1\"},\
      {\"key\": \"header2\", \"value\": \"value2\"}\
    ]\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "POST", url, NULL, NULL, alert, NULL, 200, NULL, NULL, NULL);
  free(url);
  json_decref(alert);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_gareth_alert_http_create_invalid)
{
  char * url = msprintf("%s/gareth/alert/http", angharad_uri);
  json_t * alert = json_loads("{\
    \"name\": \"http2\",\
    \"method\": 22,\
    \"url\": \"http://localhost/\",\
    \"body\": \"plop\"\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "POST", url, NULL, NULL, alert, NULL, 400, NULL, NULL, NULL);
  free(url);
  json_decref(alert);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_gareth_alert_http_get_ok)
{
  char * url = msprintf("%s/gareth/alert/http/http1", angharad_uri);
  json_t * alert = json_loads("{\
    \"name\": \"http1\",\
    \"method\": \"GET\",\
    \"url\": \"http://localhost/\",\
    \"body\": \"plop\",\
    \"http_headers\": [\
      {\"key\": \"header1\", \"value\": \"value1\"},\
      {\"key\": \"header2\", \"value\": \"value2\"}\
    ]\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, alert, NULL, NULL);
  free(url);
  json_decref(alert);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_gareth_alert_http_get_not_found)
{
  char * url = msprintf("%s/gareth/alert/http/http2", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 404, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_gareth_alert_http_update_ok)
{
  char * url = msprintf("%s/gareth/alert/http/http1", angharad_uri);
  json_t * alert = json_loads("{\
    \"method\": \"GET\",\
    \"url\": \"http://localhost/\",\
    \"body\": \"plop\",\
    \"http_headers\": [\
      {\"key\": \"header1\", \"value\": \"value1\"},\
      {\"key\": \"header2\", \"value\": \"value2\"}\
    ]\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "PUT", url, NULL, NULL, alert, NULL, 200, NULL, NULL, NULL);
  free(url);
  json_decref(alert);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_gareth_alert_http_update_invalid)
{
  char * url = msprintf("%s/gareth/alert/http/http1", angharad_uri);
  json_t * alert = json_loads("{\
    \"method\": \"GET\",\
    \"url\": \"http://localhost/\",\
    \"body\": \"plop\",\
    \"http_headers\": [\
      {\"key\": \"header1\", \"valuee\": \"value1\"},\
      {\"key\": \"header2\", \"value\": \"value2\"}\
    ]\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "PUT", url, NULL, NULL, alert, NULL, 400, NULL, NULL, NULL);
  free(url);
  json_decref(alert);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_gareth_alert_http_delete_not_found)
{
  char * url = msprintf("%s/gareth/alert/http/http2", angharad_uri);
  
  int res = run_simple_test(&user_req, "DELETE", url, NULL, NULL, NULL, NULL, 404, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_gareth_alert_http_delete_ok)
{
  char * url = msprintf("%s/gareth/alert/http/http1", angharad_uri);
  
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
	tc_core = tcase_create("test_gareth_alert_http");
	tcase_add_test(tc_core, test_gareth_alert_http_get_list);
	tcase_add_test(tc_core, test_gareth_alert_http_create_ok);
	tcase_add_test(tc_core, test_gareth_alert_http_create_invalid);
	tcase_add_test(tc_core, test_gareth_alert_http_get_ok);
	tcase_add_test(tc_core, test_gareth_alert_http_get_not_found);
	tcase_add_test(tc_core, test_gareth_alert_http_update_ok);
	tcase_add_test(tc_core, test_gareth_alert_http_update_invalid);
	tcase_add_test(tc_core, test_gareth_alert_http_delete_not_found);
	tcase_add_test(tc_core, test_gareth_alert_http_delete_ok);
	tcase_set_timeout(tc_core, 30);
	suite_add_tcase(s, tc_core);

	return s;
}

int main(int argc, char *argv[])
{
  int number_failed;
  Suite *s;
  SRunner *sr;
  jwt_t * jwt;
  jwks_t * jwks;
  char * str_jwks, * token, * bearer_token;
  json_t * j_claims;
  time_t now;
  
  y_init_logs("Taliesin test", Y_LOG_MODE_CONSOLE, Y_LOG_LEVEL_DEBUG, NULL, "Starting Taliesin test");
  
  angharad_uri = argc>2?argv[2]:ANGHARAD_SERVER_URI;

  str_jwks = read_file(argc>1?argv[1]:DEFAULT_JWKS_PATH);
  
  // Generate user and admin access tokens
  ulfius_init_request(&user_req);
  r_jwt_init(&jwt);
  r_jwt_set_header_str_value(jwt, "typ", "at+jwt");
  r_jwks_init(&jwks);
  r_jwks_import_from_str(jwks, str_jwks);
  r_jwt_add_sign_jwks(jwt, jwks, NULL);
  o_free(str_jwks);
  
  time(&now);
  j_claims = json_pack("{ss ss ss ss ss si si si ss}",
                       "iss", "https://glewlwyd.tld/",
                       "sub", USER_LOGIN,
                       "client_id", "client",
                       "jti", "abcdxyz1234",
                       "type", "access_token",
                       "iat", now,
                       "exp", now+3600,
                       "nbf", now,
                       "scope", USER_SCOPE_LIST);
  r_jwt_set_full_claims_json_t(jwt, j_claims);
  token = r_jwt_serialize_signed(jwt, NULL, 0);
  bearer_token = msprintf("Bearer %s", token);
  u_map_put(user_req.map_header, "Authorization", bearer_token);
  o_free(bearer_token);
  o_free(token);
  
  json_decref(j_claims);
  r_jwt_free(jwt);
  r_jwks_free(jwks);
  
  s = gareth_suite();
  sr = srunner_create(s);

  srunner_run_all(sr, CK_VERBOSE);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  
  ulfius_clean_request(&user_req);
  
  y_close_logs();
  
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
