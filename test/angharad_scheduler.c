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

START_TEST(test_angharad_device_create_ok)
{
  char * url = msprintf("%s/benoic/device", angharad_uri);
  json_t * j_param = json_loads("{\
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
  
  int res = run_simple_test(&user_req, "POST", url, NULL, NULL, j_param, NULL, 200, NULL, NULL, NULL);
  free(url);
  json_decref(j_param);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_device_connect_ok)
{
  char * url = msprintf("%s/benoic/device/dev1/connect", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_service_add_ok)
{
  char * url = msprintf("%s/carleon/mock-service", angharad_uri);
  json_t * j_param = json_loads("{\
    \"name\":\"mock1\",\
    \"description\":\"Description for mock1\"\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "POST", url, NULL, NULL, j_param, NULL, 200, NULL, NULL, NULL);
  free(url);
  json_decref(j_param);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_script1_add_ok)
{
  char * url = msprintf("%s/angharad/script", angharad_uri);
  json_t * j_param = json_loads("{\
    \"name\": \"script1\",\
    \"description\": \"description for script1\",\
    \"options\": {\
      \"tags\": [\"tag1\", \"tag2\"]\
    },\
    \"actions\": [\
      {\
        \"submodule\": \"benoic\",\
        \"element\": \"sw1\",\
        \"command\": 1,\
        \"parameters\": {\
          \"device\": \"dev1\",\
          \"element_type\": \"switch\"\
        }\
      },\
      {\
        \"submodule\": \"carleon\",\
        \"element\": \"mock1\",\
        \"command\": \"exec1\",\
        \"parameters\": {\
          \"service\": \"mock-service\",\
          \"param1\": \"plop\",\
          \"param2\": 3,\
          \"param3\": 4.4\
        }\
      }\
    ]\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "POST", url, NULL, NULL, j_param, NULL, 200, NULL, NULL, NULL);
  free(url);
  json_decref(j_param);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_script2_add_ok)
{
  char * url = msprintf("%s/angharad/script", angharad_uri);
  json_t * j_param = json_loads("{\
    \"name\": \"script2\",\
    \"description\": \"description for modified script1\",\
    \"options\": {\
      \"tags\": [\"tag2\"]\
    },\
    \"actions\": [\
      {\
        \"submodule\": \"benoic\",\
        \"element\": \"sw1\",\
        \"command\": 1,\
        \"parameters\": {\
          \"device\": \"dev1\",\
          \"element_type\": \"switch\"\
        }\
      },\
      {\
        \"submodule\": \"carleon\",\
        \"element\": \"mock1\",\
        \"command\": \"exec1\",\
        \"parameters\": {\
          \"service\": \"mock-service\",\
          \"param1\": \"plop\",\
          \"param2\": 3,\
          \"param3\": 4.4\
        }\
      }\
    ]\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "POST", url, NULL, NULL, j_param, NULL, 200, NULL, NULL, NULL);
  free(url);
  json_decref(j_param);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_scheduler_get_list)
{
  char * url = msprintf("%s/angharad/scheduler", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_scheduler_create_ok)
{
  char * url = msprintf("%s/angharad/scheduler", angharad_uri);
  char * str_scheduler_valid = msprintf("{\
    \"name\":\"sch1\",\
    \"description\":\"first scheduler\",\
    \"enabled\":true,\
    \"next_time\":1620263217,\
    \"repeat\":0,\
    \"repeat_value\":2,\
    \"remove_after\":false,\
    \"scripts\":[\
      {\
        \"name\":\"script1\",\
        \"enabled\":true\
      },\
      {\
        \"name\":\"script2\",\
        \"enabled\":true\
      }\
    ],\
    \"options\":{\
      \"tags\":[\
        \"tag1\",\
        \"tag2\",\
        \"tag3\"\
      ]\
    }\
  }");
  json_t * scheduler_valid = json_loads(str_scheduler_valid, JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "POST", url, NULL, NULL, scheduler_valid, NULL, 200, NULL, NULL, NULL);
  free(url);
  free(str_scheduler_valid);
  json_decref(scheduler_valid);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_scheduler_create_error_name)
{
  char * url = msprintf("%s/angharad/scheduler/sch1", angharad_uri);
  char * str_scheduler_valid = msprintf("{\
    \"name\":\"sch1\",\
    \"description\":\"first scheduler\",\
    \"enabled\":true,\
    \"next_time\":1620263217,\
    \"repeat\":0,\
    \"repeat_value\":2,\
    \"remove_after\":false,\
    \"scripts\":[\
      {\
        \"name\":\"script1\",\
        \"enabled\":true\
      },\
      {\
        \"name\":\"script2\",\
        \"enabled\":true\
      }\
    ],\
    \"options\":{\
      \"tags\":[\
        \"tag1\",\
        \"tag2\",\
        \"tag3\"\
      ]\
    },\
  }");
  json_t * scheduler_valid = json_loads(str_scheduler_valid, JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, scheduler_valid, NULL, NULL);
  free(url);
  free(str_scheduler_valid);
  json_decref(scheduler_valid);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_scheduler_get_ok)
{
  char * url = msprintf("%s/angharad/scheduler/sch1", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_scheduler_create_invalid)
{
  char * url = msprintf("%s/angharad/scheduler/", angharad_uri);
  json_t * scheduler_invalid = json_loads("{\
    \"name\":\"sch2\",\
    \"description\":\"bad scheduler\",\
    \"enabled\":1,\
    \"next_time\":\"e\",\
    \"repeat\":\"z\",\
    \"repeat_value\":-3,\
    \"remove_after\":4,\
    \"scripts\":[\
      {\
        \"name\":\"script12\",\
        \"enabled\":4\
      },\
      {\
        \"name\":9,\
        \"enabled\":true\
      }\
    ],\
    \"options\":{\
      \"tags\":[\
        \"tag1\",\
        3\
      ]\
    }\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "POST", url, NULL, NULL, scheduler_invalid, NULL, 400, NULL, NULL, NULL);
  free(url);
  json_decref(scheduler_invalid);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_scheduler_update_ok)
{
  char * url = msprintf("%s/angharad/scheduler/sch1", angharad_uri);
  char * str_scheduler_valid2 = msprintf("{\
    \"name\":\"sch1\",\
    \"description\":\"first scheduler enhanced\",\
    \"enabled\":true,\
    \"next_time\":1620263217,\
    \"repeat\":0,\
    \"repeat_value\":3,\
    \"remove_after\":false,\
    \"scripts\":[\
      {\
        \"name\":\"script1\",\
        \"enabled\":false\
      },\
      {\
        \"name\":\"script2\",\
        \"enabled\":true\
      }\
    ],\
    \"options\":{\
      \"tags\":[\
        \"tag1\",\
        \"tag3\"\
      ]\
    }\
  }");
  json_t * scheduler_valid2 = json_loads(str_scheduler_valid2, JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "PUT", url, NULL, NULL, scheduler_valid2, NULL, 200, NULL, NULL, NULL);
  free(url);
  free(str_scheduler_valid2);
  json_decref(scheduler_valid2);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_scheduler_update_not_found)
{
  char * url = msprintf("%s/angharad/scheduler/sch2", angharad_uri);
  char * str_scheduler_valid2 = msprintf("{\
    \"name\":\"sch1\",\
    \"description\":\"first scheduler enhanced\",\
    \"enabled\":true,\
    \"next_time\":1620263217,\
    \"repeat\":0,\
    \"repeat_value\":3,\
    \"remove_after\":false,\
    \"scripts\":[\
      {\
        \"name\":\"script1\",\
        \"enabled\":false\
      },\
      {\
        \"name\":\"script2\",\
        \"enabled\":true\
      }\
    ],\
    \"options\":{\
      \"tags\":[\
        \"tag1\",\
        \"tag3\"\
      ]\
    }\
  }");
  json_t * scheduler_valid2 = json_loads(str_scheduler_valid2, JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "PUT", url, NULL, NULL, scheduler_valid2, NULL, 404, NULL, NULL, NULL);
  free(url);
  free(str_scheduler_valid2);
  json_decref(scheduler_valid2);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_scheduler_get_updated_ok)
{
  char * url = msprintf("%s/angharad/scheduler/sch1", angharad_uri);
  char * str_scheduler_valid2 = msprintf("{\
    \"name\":\"sch1\",\
    \"description\":\"first scheduler enhanced\",\
    \"enabled\":true,\
    \"next_time\":1620263217,\
    \"repeat\":0,\
    \"repeat_value\":3,\
    \"remove_after\":false,\
    \"scripts\":[\
      {\
        \"name\":\"script1\",\
        \"enabled\":false\
      },\
      {\
        \"name\":\"script2\",\
        \"enabled\":true\
      }\
    ],\
    \"options\":{\
      \"tags\":[\
        \"tag1\",\
        \"tag3\"\
      ]\
    }\
  }");
  json_t * scheduler_valid2 = json_loads(str_scheduler_valid2, JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, scheduler_valid2, NULL, NULL);
  free(url);
  free(str_scheduler_valid2);
  json_decref(scheduler_valid2);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_scheduler_delete_ok)
{
  char * url = msprintf("%s/angharad/scheduler/sch1", angharad_uri);
  
  int res = run_simple_test(&user_req, "DELETE", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_script1_delete_ok)
{
  char * url = msprintf("%s/angharad/script/script1", angharad_uri);
  
  int res = run_simple_test(&user_req, "DELETE", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_script2_delete_ok)
{
  char * url = msprintf("%s/angharad/script/script2", angharad_uri);
  
  int res = run_simple_test(&user_req, "DELETE", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_device_delete_ok)
{
  char * url = msprintf("%s/benoic/device/dev1", angharad_uri);
  
  int res = run_simple_test(&user_req, "DELETE", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_service_delete_ok)
{
  char * url = msprintf("%s/carleon/mock-service/mock1", angharad_uri);
  
  int res = run_simple_test(&user_req, "DELETE", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

static Suite *angharad_suite(void)
{
	Suite *s;
	TCase *tc_core;

	s = suite_create("Angharad scheduler management");
	tc_core = tcase_create("test_angharad_scheduler");
	tcase_add_test(tc_core, test_angharad_device_create_ok);
	tcase_add_test(tc_core, test_angharad_device_connect_ok);
	tcase_add_test(tc_core, test_angharad_service_add_ok);
	tcase_add_test(tc_core, test_angharad_script1_add_ok);
	tcase_add_test(tc_core, test_angharad_script2_add_ok);
	tcase_add_test(tc_core, test_angharad_scheduler_get_list);
	tcase_add_test(tc_core, test_angharad_scheduler_create_ok);
	tcase_add_test(tc_core, test_angharad_scheduler_create_error_name);
	tcase_add_test(tc_core, test_angharad_scheduler_get_ok);
	tcase_add_test(tc_core, test_angharad_scheduler_create_invalid);
	tcase_add_test(tc_core, test_angharad_scheduler_update_ok);
	tcase_add_test(tc_core, test_angharad_scheduler_update_not_found);
	tcase_add_test(tc_core, test_angharad_scheduler_get_updated_ok);
	tcase_add_test(tc_core, test_angharad_scheduler_delete_ok);
	tcase_add_test(tc_core, test_angharad_script1_delete_ok);
	tcase_add_test(tc_core, test_angharad_script2_delete_ok);
	tcase_add_test(tc_core, test_angharad_device_delete_ok);
	tcase_add_test(tc_core, test_angharad_service_delete_ok);
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
  r_jwks_import_from_json_str(jwks, str_jwks);
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
  
  s = angharad_suite();
  sr = srunner_create(s);

  srunner_run_all(sr, CK_VERBOSE);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  
  ulfius_clean_request(&user_req);
  
  y_close_logs();
  
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
