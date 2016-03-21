/**
 *
 * Angharad system
 *
 * Manage all subsystems and handles the house automation server:
 * - Benoic
 * - Gareth
 * - Carleon
 *
 * Unit tests
 *
 * Copyright 2016 Nicolas Mora <mail@babelouest.org>
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

#include <string.h>
#include <jansson.h>
#include <ulfius.h>
#include <orcania.h>

#define SERVER_URL "http://localhost:2473/"
#define PREFIX_ANGHARAD "angharad"
#define PREFIX_BENOIC "benoic"
#define PREFIX_CARLEON "carleon"

/**
 * decode a u_map into a string
 */
char * print_map(const struct _u_map * map) {
  char * line, * to_return = NULL;
  const char **keys;
  int len, i;
  if (map != NULL) {
    keys = u_map_enum_keys(map);
    for (i=0; keys[i] != NULL; i++) {
      len = snprintf(NULL, 0, "key is %s, value is %s\n", keys[i], u_map_get(map, keys[i]));
      line = malloc((len+1)*sizeof(char));
      snprintf(line, (len+1), "key is %s, value is %s\n", keys[i], u_map_get(map, keys[i]));
      if (to_return != NULL) {
        len = strlen(to_return) + strlen(line) + 1;
        to_return = realloc(to_return, (len+1)*sizeof(char));
      } else {
        to_return = malloc((strlen(line) + 1)*sizeof(char));
        to_return[0] = 0;
      }
      strcat(to_return, line);
      free(line);
    }
    return to_return;
  } else {
    return NULL;
  }
}

/**
 * Developper-friendly response print
 */
void print_response(struct _u_response * response) {
  char * dump_json = NULL;
  if (response != NULL) {
    printf("Status: %ld\n\n", response->status);
    if (response->json_body != NULL) {
      dump_json = json_dumps(response->json_body, JSON_INDENT(2));
      printf("Json body:\n%s\n\n", dump_json);
      free(dump_json);
    } else if (response->string_body != NULL) {
      printf("String body: %s\n\n", response->string_body);
    }
  }
}


int test_request_status(struct _u_request * req, long int expected_status, json_t * expected_contains) {
  int res, to_return = 0;
  struct _u_response response;
  
  ulfius_init_response(&response);
  res = ulfius_send_http_request(req, &response);
  if (res == U_OK) {
    if (response.status != expected_status) {
      printf("##########################\nError status (%s %s %ld)\n", req->http_verb, req->http_url, expected_status);
      print_response(&response);
      printf("##########################\n\n");
    } else if (expected_contains != NULL && (response.json_body == NULL || json_search(response.json_body, expected_contains) == NULL)) {
      char * dump_expected = json_dumps(expected_contains, JSON_ENCODE_ANY), * dump_response = json_dumps(response.json_body, JSON_ENCODE_ANY);
      printf("##########################\nError json (%s %s)\n", req->http_verb, req->http_url);
      printf("Expected result in response:\n%s\nWhile response is:\n%s\n", dump_expected, dump_response);
      printf("##########################\n\n");
      free(dump_expected);
      free(dump_response);
    } else {
      printf("Success (%s %s %ld)\n\n", req->http_verb, req->http_url, expected_status);
      to_return = 1;
    }
  } else {
    printf("Error in http request: %d\n", res);
  }
  ulfius_clean_response(&response);
  return to_return;
}

void run_submodule_tests() {
  json_t * submodule_list = json_loads("[\
    {\
        \"name\": \"benoic\",\
        \"description\": \"House automation devices management\",\
        \"enabled\": true\
    },\
    {\
        \"name\": \"carleon\",\
        \"description\": \"House automation side services management\",\
        \"enabled\": true\
    },\
    {\
        \"name\": \"gareth\",\
        \"description\": \"Messenger service\",\
        \"enabled\": true\
    }\
]", JSON_DECODE_ANY, NULL);
  json_t * submodule_benoic_disabled = json_loads("{\
        \"name\": \"benoic\",\
        \"description\": \"House automation devices management\",\
        \"enabled\": false\
    }", JSON_DECODE_ANY, NULL);
  json_t * submodule_benoic_enabled = json_loads("{\
        \"name\": \"benoic\",\
        \"description\": \"House automation devices management\",\
        \"enabled\": true\
    }", JSON_DECODE_ANY, NULL);
  json_t * submodule_carleon_disabled = json_loads("{\
        \"name\": \"carleon\",\
        \"description\": \"House automation side services management\",\
        \"enabled\": false\
    }", JSON_DECODE_ANY, NULL);
  json_t * submodule_carleon_enabled = json_loads("{\
        \"name\": \"carleon\",\
        \"description\": \"House automation side services management\",\
        \"enabled\": true\
    }", JSON_DECODE_ANY, NULL);
  json_t * submodule_gareth_disabled = json_loads("{\
        \"name\": \"gareth\",\
        \"description\": \"Messenger service\",\
        \"enabled\": false\
    }", JSON_DECODE_ANY, NULL);
  json_t * submodule_gareth_enabled = json_loads("{\
        \"name\": \"gareth\",\
        \"description\": \"Messenger service\",\
        \"enabled\": true\
    }", JSON_DECODE_ANY, NULL);
  
  struct _u_request req_list[] = {
    {"GET", SERVER_URL PREFIX_ANGHARAD "/submodule/", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0}, // 200
    {"GET", SERVER_URL PREFIX_ANGHARAD "/submodule/benoic", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0}, // 200
    {"GET", SERVER_URL PREFIX_ANGHARAD "/submodule/notInvented", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0}, // 404
    {"GET", SERVER_URL PREFIX_ANGHARAD "/submodule/benoic/enable/0", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0}, // 200
    {"GET", SERVER_URL PREFIX_ANGHARAD "/submodule/benoic", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0}, // 200
    {"GET", SERVER_URL PREFIX_ANGHARAD "/submodule/benoic/enable/1", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0}, // 200
    {"GET", SERVER_URL PREFIX_ANGHARAD "/submodule/benoic", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0}, // 200
    {"GET", SERVER_URL PREFIX_ANGHARAD "/submodule/carleon/enable/0", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0}, // 200
    {"GET", SERVER_URL PREFIX_ANGHARAD "/submodule/carleon", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0}, // 200
    {"GET", SERVER_URL PREFIX_ANGHARAD "/submodule/carleon/enable/1", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0}, // 200
    {"GET", SERVER_URL PREFIX_ANGHARAD "/submodule/carleon", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0}, // 200
    {"GET", SERVER_URL PREFIX_ANGHARAD "/submodule/gareth/enable/0", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0}, // 200
    {"GET", SERVER_URL PREFIX_ANGHARAD "/submodule/gareth", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0}, // 200
    {"GET", SERVER_URL PREFIX_ANGHARAD "/submodule/gareth/enable/1", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0}, // 200
    {"GET", SERVER_URL PREFIX_ANGHARAD "/submodule/gareth", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0}, // 200
  };

  test_request_status(&req_list[0], 200, submodule_list);
  test_request_status(&req_list[1], 200, NULL);
  test_request_status(&req_list[2], 404, NULL);
  test_request_status(&req_list[3], 200, NULL);
  test_request_status(&req_list[4], 200, submodule_benoic_disabled);
  test_request_status(&req_list[5], 200, NULL);
  test_request_status(&req_list[6], 200, submodule_benoic_enabled);
  test_request_status(&req_list[7], 200, NULL);
  test_request_status(&req_list[8], 200, submodule_carleon_disabled);
  test_request_status(&req_list[9], 200, NULL);
  test_request_status(&req_list[10], 200, submodule_carleon_enabled);
  test_request_status(&req_list[11], 200, NULL);
  test_request_status(&req_list[12], 200, submodule_gareth_disabled);
  test_request_status(&req_list[13], 200, NULL);
  test_request_status(&req_list[14], 200, submodule_gareth_enabled);
  
  json_decref(submodule_list);
  json_decref(submodule_benoic_disabled);
  json_decref(submodule_benoic_enabled);
  json_decref(submodule_carleon_disabled);
  json_decref(submodule_carleon_enabled);
  json_decref(submodule_gareth_disabled);
  json_decref(submodule_gareth_enabled);
}

void run_script_tests() {
  json_t * device_valid = json_loads("{\
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
  json_t * mock_valid = json_loads("{\
    \"name\":\"mock1\",\
    \"description\":\"Description for mock1\"\
  }", JSON_DECODE_ANY, NULL);
  json_t * script_valid = json_loads("{\
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
        \"command\": \"exec\",\
        \"parameters\": {\
          \"service\": \"00-00-00\",\
          \"param1\": \"plop\",\
          \"param2\": 3,\
          \"param3\": 4.4\
        }\
      }\
    ]\
  }", JSON_DECODE_ANY, NULL);
  json_t * script_valid_with_tags = json_loads("{\
    \"name\": \"script1\",\
    \"description\": \"description for script1\",\
    \"options\": {\
      \"tags\": [\"tag1\", \"tag2\", \"new_tag1\", \"new_tag2\"]\
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
        \"command\": \"exec\",\
        \"parameters\": {\
          \"service\": \"00-00-00\",\
          \"param1\": \"plop\",\
          \"param2\": 3,\
          \"param3\": 4.4\
        }\
      }\
    ]\
  }", JSON_DECODE_ANY, NULL);
  json_t * script_invalid1 = json_loads("{\
    \"name\": \"script2\",\
    \"description\": \"description for script2\",\
    \"options\": {\
      \"tags\": [\"tag1\", \"tag2\", 22]\
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
        \"command\": \"exec\",\
        \"parameters\": {\
          \"service\": \"00-00-00\",\
          \"param1\": \"plop\",\
          \"param2\": 3,\
          \"param3\": 4.4\
        }\
      }\
    ]\
  }", JSON_DECODE_ANY, NULL);
  json_t * script_invalid2 = json_loads("{\
    \"name\": \"script2\",\
    \"description\": \"description for script2\",\
    \"options\": {\
      \"tags\": [\"tag1\", \"tag2\"]\
    },\
    \"actions\": [\
      {\
        \"submodule\": \"benoic\",\
        \"element\": \"sw12\",\
        \"command\": 4,\
        \"parameters\": {\
          \"device\": \"dev12\",\
          \"element_type\": \"switche\"\
        }\
      },\
      {\
        \"submodule\": \"carleon\",\
        \"element\": \"mock1\",\
        \"command\": \"exec\",\
        \"parameters\": {\
          \"service\": \"00-00-00\",\
          \"param1\": \"plop\",\
          \"param2\": 3,\
          \"param3\": 4.4\
        }\
      }\
    ]\
  }", JSON_DECODE_ANY, NULL);
  json_t * script_invalid3 = json_loads("{\
    \"name\": \"script2\",\
    \"description\": \"description for script2\",\
    \"options\": {\
      \"tags\": [\"tag1\", \"tag2\"]\
    },\
    \"actions\": [\
      {\
        \"submodule\": \"benoice\",\
        \"element\": \"sw1\",\
        \"command\": 1,\
        \"parameters\": {\
          \"device\": \"dev1\",\
          \"element_type\": \"switch\"\
        }\
      },\
      {\
        \"submodule\": \"carleon\",\
        \"element\": \"mock12\",\
        \"command\": \"exece\",\
        \"parameters\": {\
          \"service\": \"00-00-00e\",\
          \"param1\": \"plop\",\
          \"param2\": 3,\
          \"param3\": \"lolo\"\
        }\
      }\
    ]\
  }", JSON_DECODE_ANY, NULL);

  struct _u_request req_list[] = {
    {"POST", SERVER_URL PREFIX_BENOIC "/device/", NULL, NULL, NULL, NULL, NULL, NULL, NULL, device_valid, NULL, 0, NULL, 0}, // 200
    {"GET", SERVER_URL PREFIX_BENOIC "/device/dev1/connect", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0}, // 200
    {"POST", SERVER_URL PREFIX_CARLEON "/mock-service/", NULL, NULL, NULL, NULL, NULL, NULL, NULL, mock_valid, NULL, 0, NULL, 0}, // 200
    {"POST", SERVER_URL PREFIX_ANGHARAD "/script/", NULL, NULL, NULL, NULL, NULL, NULL, NULL, script_valid, NULL, 0, NULL, 0}, // 200
    {"GET", SERVER_URL PREFIX_ANGHARAD "/script/", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0}, // 200
    {"POST", SERVER_URL PREFIX_ANGHARAD "/script/", NULL, NULL, NULL, NULL, NULL, NULL, NULL, script_invalid1, NULL, 0, NULL, 0}, // 400
    {"POST", SERVER_URL PREFIX_ANGHARAD "/script/", NULL, NULL, NULL, NULL, NULL, NULL, NULL, script_invalid2, NULL, 0, NULL, 0}, // 400
    {"POST", SERVER_URL PREFIX_ANGHARAD "/script/", NULL, NULL, NULL, NULL, NULL, NULL, NULL, script_invalid3, NULL, 0, NULL, 0}, // 400
    {"GET", SERVER_URL PREFIX_ANGHARAD "/script/script1", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0}, // 200
    {"PUT", SERVER_URL PREFIX_ANGHARAD "/script/script1/new_tag1", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0}, // 200
    {"PUT", SERVER_URL PREFIX_ANGHARAD "/script/script1/new_tag2", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0}, // 200
    {"GET", SERVER_URL PREFIX_ANGHARAD "/script/script1", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0}, // 200
    {"DELETE", SERVER_URL PREFIX_ANGHARAD "/script/script1/new_tag1", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0}, // 200
    {"DELETE", SERVER_URL PREFIX_ANGHARAD "/script/script1", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0}, // 200
    {"DELETE", SERVER_URL PREFIX_BENOIC "/device/dev1", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0}, // 200
    {"DELETE", SERVER_URL PREFIX_CARLEON "/mock-service/mock1", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0}, // 200
  };

  test_request_status(&req_list[0], 200, NULL);
  test_request_status(&req_list[1], 200, NULL);
  test_request_status(&req_list[2], 200, NULL);
  test_request_status(&req_list[3], 200, NULL);
  test_request_status(&req_list[4], 200, NULL);
  test_request_status(&req_list[5], 400, NULL);
  test_request_status(&req_list[6], 400, NULL);
  test_request_status(&req_list[7], 400, NULL);
  test_request_status(&req_list[8], 200, script_valid);
  test_request_status(&req_list[9], 200, NULL);
  test_request_status(&req_list[10], 200, NULL);
  test_request_status(&req_list[11], 200, script_valid_with_tags);
  test_request_status(&req_list[12], 200, NULL);
  test_request_status(&req_list[13], 200, NULL);
  test_request_status(&req_list[14], 200, NULL);
  test_request_status(&req_list[15], 200, NULL);

  json_decref(device_valid);
  json_decref(mock_valid);
  json_decref(script_valid);
  json_decref(script_valid_with_tags);
  json_decref(script_invalid1);
  json_decref(script_invalid2);
  json_decref(script_invalid3);
}

int main(void) {
  printf("Press <enter> to run submodule tests\n");
  getchar();
  run_submodule_tests();
  printf("Press <enter> to run script tests\n");
  getchar();
  run_script_tests();
  return 0;
}
