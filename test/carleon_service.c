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

START_TEST(test_carleon_service_list)
{
  char * url = msprintf("%s/carleon/service", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_carleon_service_mock_disable)
{
  char * url = msprintf("%s/carleon/service/mock-service/enable/0", angharad_uri);
  
  int res = run_simple_test(&user_req, "PUT", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_carleon_service_mock_enable)
{
  char * url = msprintf("%s/carleon/service/mock-service/enable/1", angharad_uri);
  
  int res = run_simple_test(&user_req, "PUT", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_carleon_service_get_ok)
{
  char * url = msprintf("%s/carleon/service/mock-service", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_carleon_service_create_mock1_ok)
{
  char * url = msprintf("%s/carleon/mock-service", angharad_uri);
  json_t * mock = json_loads("{\
    \"name\":\"mock1\",\
    \"description\":\"Description for mock1\"\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "POST", url, NULL, NULL, mock, NULL, 200, NULL, NULL, NULL);
  free(url);
  json_decref(mock);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_carleon_service_create_mock2_ok)
{
  char * url = msprintf("%s/carleon/mock-service", angharad_uri);
  json_t * mock = json_loads("{\
    \"name\":\"mock2\",\
    \"description\":\"Description for mock2\"\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "POST", url, NULL, NULL, mock, NULL, 200, NULL, NULL, NULL);
  free(url);
  json_decref(mock);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_carleon_service_get_mock1_ok)
{
  char * url = msprintf("%s/carleon/mock-service/mock1", angharad_uri);
  json_t * mock = json_loads("{\
    \"name\":\"mock1\",\
    \"description\":\"Description for mock1\"\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, mock, NULL, NULL);
  free(url);
  json_decref(mock);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_carleon_service_get_mock2_ok)
{
  char * url = msprintf("%s/carleon/mock-service/mock2", angharad_uri);
  json_t * mock = json_loads("{\
    \"name\":\"mock2\",\
    \"description\":\"Description for mock2\"\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, mock, NULL, NULL);
  free(url);
  json_decref(mock);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_carleon_service_get_mock3_not_found)
{
  char * url = msprintf("%s/carleon/mock-service/mock3", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 404, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_carleon_service_update_mock2_ok)
{
  char * url = msprintf("%s/carleon/mock-service/mock2", angharad_uri);
  json_t * mock = json_loads("{\
    \"description\":\"New description for mock2\"\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "PUT", url, NULL, NULL, mock, NULL, 200, NULL, NULL, NULL);
  free(url);
  json_decref(mock);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_carleon_service_get_upated_mock2_ok)
{
  char * url = msprintf("%s/carleon/mock-service/mock2", angharad_uri);
  json_t * mock = json_loads("{\
    \"description\":\"New description for mock2\"\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, mock, NULL, NULL);
  free(url);
  json_decref(mock);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_carleon_service_mock1_add_tag1_ok)
{
  char * url = msprintf("%s/carleon/service/mock-service/tag/mock1/tag1", angharad_uri);
  
  int res = run_simple_test(&user_req, "PUT", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_carleon_service_mock1_add_tag2_ok)
{
  char * url = msprintf("%s/carleon/service/mock-service/tag/mock1/tag2", angharad_uri);
  
  int res = run_simple_test(&user_req, "PUT", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_carleon_service_mock2_add_tag1_ok)
{
  char * url = msprintf("%s/carleon/service/mock-service/tag/mock2/tag1", angharad_uri);
  
  int res = run_simple_test(&user_req, "PUT", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_carleon_service_mock2_add_tag1_duplicate_ok)
{
  char * url = msprintf("%s/carleon/service/mock-service/tag/mock2/tag1", angharad_uri);
  
  int res = run_simple_test(&user_req, "PUT", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_carleon_service_get_mock1_with_tags_ok)
{
  char * url = msprintf("%s/carleon/service/mock-service/mock1", angharad_uri);
  json_t * mock = json_loads("{\
    \"name\": \"mock1\", \
    \"description\": \"Description for mock 1\",\
    \"tag\":\
    [\
      \"tag1\",\
      \"tag2\"\
    ]\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, mock, NULL, NULL);
  free(url);
  json_decref(mock);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_carleon_service_get_mock2_with_tags_ok)
{
  char * url = msprintf("%s/carleon/service/mock-service/mock2", angharad_uri);
  json_t * mock = json_loads("{\
    \"name\": \"mock2\", \
    \"description\": \"Description for mock 1\",\
    \"tag\":\
    [\
      \"tag1\",\
      \"tag1\"\
    ]\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, mock, NULL, NULL);
  free(url);
  json_decref(mock);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_carleon_service_mock2_delete_tag1_ok)
{
  char * url = msprintf("%s/carleon/service/mock-service/tag/mock2/tag1", angharad_uri);
  
  int res = run_simple_test(&user_req, "DELETE", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_carleon_service_get_list_with_tags_but_not_all_ok)
{
  char * url = msprintf("%s/carleon/service/", angharad_uri);
  json_t * mock = json_loads("{\
    \"name\": \"mock1\", \
    \"description\": \"Description for mock 1\",\
    \"tag\":\
    [\
      \"tag2\"\
    ]\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, mock, NULL, NULL);
  free(url);
  json_decref(mock);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_carleon_service_mock1_run_command_ok)
{
  char * url = msprintf("%s/carleon/mock-service/mock1/command/exec1/batman/42/15.7", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_carleon_service_delete_mock1_ok)
{
  char * url = msprintf("%s/carleon/mock-service/mock1", angharad_uri);
  
  int res = run_simple_test(&user_req, "DELETE", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_carleon_service_delete_mock2_ok)
{
  char * url = msprintf("%s/carleon/mock-service/mock2", angharad_uri);
  
  int res = run_simple_test(&user_req, "DELETE", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_carleon_service_cleanup_mock1_ok)
{
  char * url = msprintf("%s/carleon/mock-service/cleanup/mock1", angharad_uri);
  
  int res = run_simple_test(&user_req, "DELETE", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_carleon_service_cleanup_mock2_ok)
{
  char * url = msprintf("%s/carleon/mock-service/cleanup/mock2", angharad_uri);
  
  int res = run_simple_test(&user_req, "DELETE", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

static Suite *carleon_suite(void)
{
	Suite *s;
	TCase *tc_core;

	s = suite_create("Carleon service mock management");
	tc_core = tcase_create("test_carleon_service_mock");
	tcase_add_test(tc_core, test_carleon_service_list);
	tcase_add_test(tc_core, test_carleon_service_mock_disable);
	tcase_add_test(tc_core, test_carleon_service_mock_enable);
	tcase_add_test(tc_core, test_carleon_service_get_ok);
	tcase_add_test(tc_core, test_carleon_service_create_mock1_ok);
	tcase_add_test(tc_core, test_carleon_service_create_mock2_ok);
	tcase_add_test(tc_core, test_carleon_service_get_mock1_ok);
	tcase_add_test(tc_core, test_carleon_service_get_mock2_ok);
	tcase_add_test(tc_core, test_carleon_service_get_mock3_not_found);
	tcase_add_test(tc_core, test_carleon_service_update_mock2_ok);
	tcase_add_test(tc_core, test_carleon_service_get_upated_mock2_ok);
	tcase_add_test(tc_core, test_carleon_service_mock1_add_tag1_ok);
	tcase_add_test(tc_core, test_carleon_service_mock1_add_tag2_ok);
	tcase_add_test(tc_core, test_carleon_service_mock2_add_tag1_ok);
	tcase_add_test(tc_core, test_carleon_service_mock2_add_tag1_duplicate_ok);
	tcase_add_test(tc_core, test_carleon_service_get_mock1_with_tags_ok);
	tcase_add_test(tc_core, test_carleon_service_get_mock2_with_tags_ok);
	tcase_add_test(tc_core, test_carleon_service_mock2_delete_tag1_ok);
	tcase_add_test(tc_core, test_carleon_service_get_list_with_tags_but_not_all_ok);
	tcase_add_test(tc_core, test_carleon_service_mock1_run_command_ok);
	tcase_add_test(tc_core, test_carleon_service_delete_mock1_ok);
	tcase_add_test(tc_core, test_carleon_service_delete_mock2_ok);
	tcase_add_test(tc_core, test_carleon_service_cleanup_mock1_ok);
	tcase_add_test(tc_core, test_carleon_service_cleanup_mock2_ok);
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
  
  s = carleon_suite();
  sr = srunner_create(s);

  srunner_run_all(sr, CK_VERBOSE);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  
  ulfius_clean_request(&user_req);
  
  y_close_logs();
  
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
