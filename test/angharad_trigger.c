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

START_TEST(test_angharad_trigger_get_list)
{
  char * url = msprintf("%s/angharad/trigger", angharad_uri);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, NULL, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_trigger_create_ok)
{
  char * url = msprintf("%s/angharad/trigger", angharad_uri);
  json_t * trigger_valid = json_loads("{\
    \"name\":\"tri1\",\
    \"description\":\"first trigger\",\
    \"enabled\":true,\
    \"submodule\":\"benoic\",\
    \"source\":\"dev1\",\
    \"element\":\"elt1\",\
    \"message\":\"hello\",\
    \"message_match\":1,\
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
    \"conditions\":[\
      {\
        \"submodule\": \"benoic\",\
        \"element\": \"di1\",\
        \"command\": 1,\
        \"parameters\": {\
          \"device\": \"dev1\",\
          \"element_type\": \"dimmer\"\
        },\
        \"condition\":\"==\",\
        \"value\":10\
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
        \"condition\":\"==\",\
        \"value\":0\
      }\
    ]\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "POST", url, NULL, NULL, trigger_valid, NULL, 200, NULL, NULL, NULL);
  free(url);
  json_decref(trigger_valid);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_trigger_create_error_name)
{
  char * url = msprintf("%s/angharad/trigger/tri1", angharad_uri);
  json_t * trigger_valid = json_loads("{\
    \"name\":\"tri1\",\
    \"description\":\"first trigger\",\
    \"enabled\":true,\
    \"submodule\":\"benoic\",\
    \"source\":\"dev1\",\
    \"element\":\"elt1\",\
    \"message\":\"hello\",\
    \"message_match\":1,\
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
    \"conditions\":[\
      {\
        \"submodule\": \"benoic\",\
        \"element\": \"di1\",\
        \"command\": 1,\
        \"parameters\": {\
          \"device\": \"dev1\",\
          \"element_type\": \"dimmer\"\
        },\
        \"condition\":\"==\",\
        \"value\":10\
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
        \"condition\":\"==\",\
        \"value\":0\
      }\
    ]\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, trigger_valid, NULL, NULL);
  free(url);
  json_decref(trigger_valid);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_trigger_get_ok)
{
  char * url = msprintf("%s/angharad/trigger/tri1", angharad_uri);
  json_t * trigger_valid = json_loads("{\
    \"name\":\"tri1\",\
    \"description\":\"first trigger\",\
    \"enabled\":true,\
    \"submodule\":\"benoic\",\
    \"source\":\"dev1\",\
    \"element\":\"elt1\",\
    \"message\":\"hello\",\
    \"message_match\":1,\
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
    \"conditions\":[\
      {\
        \"submodule\": \"benoic\",\
        \"element\": \"di1\",\
        \"command\": 1,\
        \"parameters\": {\
          \"device\": \"dev1\",\
          \"element_type\": \"dimmer\"\
        },\
        \"condition\":\"==\",\
        \"value\":10\
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
        \"condition\":\"==\",\
        \"value\":0\
      }\
    ]\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, trigger_valid, NULL, NULL);
  free(url);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_trigger_create_invalid)
{
  char * url = msprintf("%s/angharad/trigger/", angharad_uri);
  json_t * trigger_invalid = json_loads("{\
    \"name\":\"tri2\",\
    \"description\":\"bad trigger\",\
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
        \"value\":\"err\"\
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
        \"condition\":\"err\",\
        \"value\":0\
      }\
    ]\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "POST", url, NULL, NULL, trigger_invalid, NULL, 400, NULL, NULL, NULL);
  free(url);
  json_decref(trigger_invalid);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_trigger_update_ok)
{
  char * url = msprintf("%s/angharad/trigger/tri1", angharad_uri);
  json_t * trigger_valid2 = json_loads("{\
    \"name\":\"tri1\",\
    \"description\":\"first trigger improved\",\
    \"enabled\":true,\
    \"submodule\":\"benoic\",\
    \"source\":\"dev1\",\
    \"element\":\"elt1\",\
    \"message\":\"helloooo\",\
    \"message_match\":1,\
    \"options\":{\
      \"tags\":[\
        \"tag1\",\
        \"tag3\"\
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
        \"condition\":\">=\",\
        \"value\":0\
      }\
    ]\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "PUT", url, NULL, NULL, trigger_valid2, NULL, 200, NULL, NULL, NULL);
  free(url);
  json_decref(trigger_valid2);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_trigger_update_not_found)
{
  char * url = msprintf("%s/angharad/trigger/tri2", angharad_uri);
  json_t * trigger_valid2 = json_loads("{\
    \"name\":\"tri1\",\
    \"description\":\"first trigger improved\",\
    \"enabled\":true,\
    \"submodule\":\"benoic\",\
    \"source\":\"dev1\",\
    \"element\":\"elt1\",\
    \"message\":\"helloooo\",\
    \"message_match\":1,\
    \"options\":{\
      \"tags\":[\
        \"tag1\",\
        \"tag3\"\
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
        \"condition\":\">=\",\
        \"value\":0\
      }\
    ]\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "PUT", url, NULL, NULL, trigger_valid2, NULL, 404, NULL, NULL, NULL);
  free(url);
  json_decref(trigger_valid2);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_trigger_get_updated_ok)
{
  char * url = msprintf("%s/angharad/trigger/tri1", angharad_uri);
  json_t * trigger_valid2 = json_loads("{\
    \"name\":\"tri1\",\
    \"description\":\"first trigger improved\",\
    \"enabled\":true,\
    \"submodule\":\"benoic\",\
    \"source\":\"dev1\",\
    \"element\":\"elt1\",\
    \"message\":\"helloooo\",\
    \"message_match\":1,\
    \"options\":{\
      \"tags\":[\
        \"tag1\",\
        \"tag3\"\
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
        \"condition\":\">=\",\
        \"value\":0\
      }\
    ]\
  }", JSON_DECODE_ANY, NULL);
  
  int res = run_simple_test(&user_req, "GET", url, NULL, NULL, NULL, NULL, 200, trigger_valid2, NULL, NULL);
  free(url);
  json_decref(trigger_valid2);
	ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_angharad_trigger_delete_ok)
{
  char * url = msprintf("%s/angharad/trigger/tri1", angharad_uri);
  
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

	s = suite_create("Angharad trigger management");
	tc_core = tcase_create("test_angharad_trigger");
	tcase_add_test(tc_core, test_angharad_device_create_ok);
	tcase_add_test(tc_core, test_angharad_device_connect_ok);
	tcase_add_test(tc_core, test_angharad_service_add_ok);
	tcase_add_test(tc_core, test_angharad_script1_add_ok);
	tcase_add_test(tc_core, test_angharad_script2_add_ok);
	tcase_add_test(tc_core, test_angharad_trigger_get_list);
	tcase_add_test(tc_core, test_angharad_trigger_create_ok);
	tcase_add_test(tc_core, test_angharad_trigger_create_error_name);
	tcase_add_test(tc_core, test_angharad_trigger_get_ok);
	tcase_add_test(tc_core, test_angharad_trigger_create_invalid);
	tcase_add_test(tc_core, test_angharad_trigger_update_ok);
	tcase_add_test(tc_core, test_angharad_trigger_update_not_found);
	tcase_add_test(tc_core, test_angharad_trigger_get_updated_ok);
	tcase_add_test(tc_core, test_angharad_trigger_delete_ok);
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
