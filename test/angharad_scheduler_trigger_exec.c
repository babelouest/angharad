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
  json_t * script1 = json_loads("{\
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
  
  int res = run_simple_test(&user_req, "POST", url, NULL, NULL, script1, NULL, 200, NULL, NULL, NULL);
  free(url);
  json_decref(script1);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_script2_add_ok)
{
  char * url = msprintf("%s/angharad/script", angharad_uri);
  json_t * script2 = json_loads("{\
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
  
  int res = run_simple_test(&user_req, "POST", url, NULL, NULL, script2, NULL, 200, NULL, NULL, NULL);
  free(url);
  json_decref(script2);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_scheduler_checked1_add_ok)
{
  char * url = msprintf("%s/angharad/scheduler", angharad_uri);
  char * str_scheduler = msprintf("{\
    \"name\":\"sch1\",\
    \"description\":\"first scheduler\",\
    \"enabled\":true,\
    \"next_time\":%u,\
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
        \"enabled\":false\
      }\
    ],\
    \"options\":{\
      \"tags\":[\
      ]\
    },\
    \"conditions\":[\
      {\
        \"submodule\": \"benoic\",\
        \"element\": \"sw1\",\
        \"command\": 1,\
        \"parameters\": {\
          \"device\": \"dev1\",\
          \"element_type\": \"switch\"\
        },\
        \"condition\":\"==\",\
        \"value\":0\
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
        },\
        \"condition\":\"contains\",\
        \"value\":{\
          \"value1\":1\
        }\
      }\
    ]\
  }", (unsigned)time(NULL));
  json_t * scheduler = json_loads(str_scheduler, JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "POST", url, NULL, NULL, scheduler, NULL, 200, NULL, NULL, NULL);
  free(url);
  free(str_scheduler);
  json_decref(scheduler);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_scheduler_checked2_add_ok)
{
  char * url = msprintf("%s/angharad/scheduler", angharad_uri);
  char * str_scheduler = msprintf("{\
    \"name\":\"sch2\",\
    \"description\":\"second scheduler\",\
    \"enabled\":true,\
    \"next_time\":%u,\
    \"repeat\":0,\
    \"repeat_value\":5,\
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
      ]\
    },\
    \"conditions\":[\
      {\
        \"submodule\": \"benoic\",\
        \"element\": \"sw2\",\
        \"command\": 1,\
        \"parameters\": {\
          \"device\": \"dev1\",\
          \"element_type\": \"switch\"\
        },\
        \"condition\":\"==\",\
        \"value\":1\
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
        },\
        \"condition\":\"not contains\",\
        \"value\":{\
          \"value1\":0\
        }\
      }\
    ]\
  }", (unsigned)time(NULL));
  json_t * scheduler = json_loads(str_scheduler, JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "POST", url, NULL, NULL, scheduler, NULL, 200, NULL, NULL, NULL);
  free(url);
  free(str_scheduler);
  json_decref(scheduler);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_scheduler_unchecked_add_ok)
{
  char * url = msprintf("%s/angharad/scheduler", angharad_uri);
  char * str_scheduler = msprintf("{\
    \"name\":\"sch3\",\
    \"description\":\"third scheduler\",\
    \"enabled\":true,\
    \"next_time\":%u,\
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
      ]\
    },\
    \"conditions\":[\
      {\
        \"submodule\": \"benoic\",\
        \"element\": \"sw2\",\
        \"command\": 1,\
        \"parameters\": {\
          \"device\": \"dev1\",\
          \"element_type\": \"switch\"\
        },\
        \"condition\":\"==\",\
        \"value\":0\
      },\
      {\
        \"submodule\": \"carleon\",\
        \"element\": \"mock1\",\
        \"command\": \"exec2\",\
        \"parameters\": {\
          \"service\": \"mock-service\",\
          \"param1\": \"plop\"\
        },\
        \"condition\":\"==\",\
        \"value\":\"plop\"\
      }\
    ]\
  }", (unsigned)time(NULL));
  json_t * scheduler = json_loads(str_scheduler, JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "POST", url, NULL, NULL, scheduler, NULL, 200, NULL, NULL, NULL);
  free(url);
  free(str_scheduler);
  json_decref(scheduler);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_trigger_checked1_add_ok)
{
  char * url = msprintf("%s/angharad/trigger", angharad_uri);
  json_t * trigger = json_loads("{\
    \"name\":\"tri1\",\
    \"description\":\"first trigger\",\
    \"enabled\":true,\
    \"submodule\":\"benoic\",\
    \"source\":\"dev1\",\
    \"element\":\"sw1\",\
    \"message\":\"hello\",\
    \"message_match\":0,\
    \"options\":{\
      \"tags\":[\
      ]\
    },\
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
    \"conditions\":[\
      {\
        \"submodule\": \"benoic\",\
        \"element\": \"sw1\",\
        \"command\": 1,\
        \"parameters\": {\
          \"device\": \"dev1\",\
          \"element_type\": \"switch\"\
        },\
        \"condition\":\"==\",\
        \"value\":0\
      },\
      {\
        \"submodule\": \"carleon\",\
        \"element\": \"mock1\",\
        \"command\": \"exec2\",\
        \"parameters\": {\
          \"service\": \"mock-service\",\
          \"param1\": \"plop\"\
        },\
        \"condition\":\"contains\",\
        \"value\":\"ok\"\
      }\
    ]\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "POST", url, NULL, NULL, trigger, NULL, 200, NULL, NULL, NULL);
  free(url);
  json_decref(trigger);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_trigger_checked2_add_ok)
{
  char * url = msprintf("%s/angharad/trigger", angharad_uri);
  json_t * trigger = json_loads("{\
    \"name\":\"tri2\",\
    \"description\":\"second trigger\",\
    \"enabled\":true,\
    \"submodule\":\"benoic\",\
    \"source\":\"dev1\",\
    \"element\":\"di1\",\
    \"message\":\"hello\",\
    \"message_match\":0,\
    \"options\":{\
      \"tags\":[\
      ]\
    },\
    \"scripts\":[\
      {\
        \"name\":\"script1\",\
        \"enabled\":true\
      },\
      {\
        \"name\":\"script2\",\
        \"enabled\":false\
      }\
    ],\
    \"conditions\":[\
      {\
        \"submodule\": \"benoic\",\
        \"element\": \"sw2\",\
        \"command\": 1,\
        \"parameters\": {\
          \"device\": \"dev1\",\
          \"element_type\": \"switch\"\
        },\
        \"condition\":\"==\",\
        \"value\":1\
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
        },\
        \"condition\":\"contains\",\
        \"value\":{\
          \"value1\":1\
        }\
      }\
    ]\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "POST", url, NULL, NULL, trigger, NULL, 200, NULL, NULL, NULL);
  free(url);
  json_decref(trigger);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_trigger_unchecked_add_ok)
{
  char * url = msprintf("%s/angharad/trigger", angharad_uri);
  json_t * trigger = json_loads("{\
    \"name\":\"tri3\",\
    \"description\":\"third trigger\",\
    \"enabled\":true,\
    \"submodule\":\"benoic\",\
    \"source\":\"dev1\",\
    \"element\":\"he1\",\
    \"message\":\"hello\",\
    \"message_match\":0,\
    \"options\":{\
      \"tags\":[\
      ]\
    },\
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
    \"conditions\":[\
      {\
        \"submodule\": \"benoic\",\
        \"element\": \"sw2\",\
        \"command\": 1,\
        \"parameters\": {\
          \"device\": \"dev1\",\
          \"element_type\": \"switch\"\
        },\
        \"condition\":\"==\",\
        \"value\":0\
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
        },\
        \"condition\":\"contains\",\
        \"value\":{\
          \"value1\":1\
        }\
      }\
    ]\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "POST", url, NULL, NULL, trigger, NULL, 200, NULL, NULL, NULL);
  free(url);
  json_decref(trigger);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_trigger_1)
{
  char * url = msprintf("%s/angharad/alert/benoic/dev1/sw1/plop", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_trigger_2)
{
  char * url = msprintf("%s/angharad/alert/benoic/dev1/di1/plop", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_trigger_3)
{
  char * url = msprintf("%s/angharad/alert/benoic/dev1/he1/plop", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_scheduler_checked1_delete_ok)
{
  char * url = msprintf("%s/angharad/scheduler/sch1", angharad_uri);
  
  int res = run_simple_test(&user_req, "DELETE", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_scheduler_checked2_delete_ok)
{
  char * url = msprintf("%s/angharad/scheduler/sch2", angharad_uri);
  
  int res = run_simple_test(&user_req, "DELETE", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_scheduler_unchecked_delete_ok)
{
  char * url = msprintf("%s/angharad/scheduler/sch3", angharad_uri);
  
  int res = run_simple_test(&user_req, "DELETE", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_trigger_checked1_delete_ok)
{
  char * url = msprintf("%s/angharad/trigger/tri1", angharad_uri);
  
  int res = run_simple_test(&user_req, "DELETE", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_trigger_checked2_delete_ok)
{
  char * url = msprintf("%s/angharad/trigger/tri2", angharad_uri);
  
  int res = run_simple_test(&user_req, "DELETE", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_trigger_unchecked_delete_ok)
{
  char * url = msprintf("%s/angharad/trigger/tri3", angharad_uri);
  
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

	s = suite_create("Angharad submodules management");
	tc_core = tcase_create("test_angharad_submodule");
	tcase_add_test(tc_core, test_angharad_device_create_ok);
	tcase_add_test(tc_core, test_angharad_device_connect_ok);
	tcase_add_test(tc_core, test_angharad_service_add_ok);
	tcase_add_test(tc_core, test_angharad_script1_add_ok);
	tcase_add_test(tc_core, test_angharad_script2_add_ok);
	tcase_add_test(tc_core, test_angharad_scheduler_checked1_add_ok);
	tcase_add_test(tc_core, test_angharad_scheduler_checked2_add_ok);
	tcase_add_test(tc_core, test_angharad_scheduler_unchecked_add_ok);
	tcase_add_test(tc_core, test_angharad_trigger_checked1_add_ok);
	tcase_add_test(tc_core, test_angharad_trigger_checked2_add_ok);
	tcase_add_test(tc_core, test_angharad_trigger_unchecked_add_ok);
	tcase_add_test(tc_core, test_angharad_trigger_1);
	tcase_add_test(tc_core, test_angharad_trigger_2);
	tcase_add_test(tc_core, test_angharad_trigger_3);
	tcase_add_test(tc_core, test_angharad_scheduler_checked1_delete_ok);
	tcase_add_test(tc_core, test_angharad_scheduler_checked2_delete_ok);
	tcase_add_test(tc_core, test_angharad_scheduler_unchecked_delete_ok);
	tcase_add_test(tc_core, test_angharad_trigger_checked1_delete_ok);
	tcase_add_test(tc_core, test_angharad_trigger_checked2_delete_ok);
	tcase_add_test(tc_core, test_angharad_trigger_unchecked_delete_ok);
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

  if (argv[1] != NULL) {
    str_jwks = read_file(argv[1]);
    
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
    
    s = angharad_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_VERBOSE);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    
    ulfius_clean_request(&user_req);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "Error, no jwks file path specified");
    number_failed = 1;
  }
  
  y_close_logs();
  
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
