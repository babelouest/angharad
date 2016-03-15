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

#define SERVER_URL_PREFIX "http://localhost:2473/angharad"

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

void run_device_tests() {
  json_t * device_valid1 = json_loads("{\
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
  json_t * device_valid2 = json_loads("{\
    \"description\":\"second test\",\
    \"type_uid\":\"00-00-00\",\
    \"enabled\":true,\
    \"options\":{\
      \"uri\":\"dev1\",\
      \"baud\":6900,\
      \"do_not_check_certificate\":false,\
      \"device_specified\":\"TBD\"\
    }\
  }", JSON_DECODE_ANY, NULL);
  json_t * device_invalid1 = json_loads("{\
    \"name\":\"dev2\",\
    \"description\":\"first test\",\
    \"type_uid\":\"00-00-01\",\
    \"options\":{\
      \"uri\":\"dev1\",\
      \"baud\":6900,\
      \"do_not_check_certificate\":false\
    }\
  }", JSON_DECODE_ANY, NULL);
  json_t * device_invalid2 = json_loads("{\
    \"name\":\"dev2\",\
    \"description\":\"first test\",\
    \"type_uid\":\"00-00-00\",\
    \"options\":{\
      \"baud\":6900,\
      \"do_not_check_certificate\":false\
    }\
  }", JSON_DECODE_ANY, NULL);
  json_t * device_invalid3 = json_loads("{\
    \"name\":\"dev2\",\
    \"description\":\"first test\",\
    \"type_uid\":\"00-00-00\"\
    \"options\":{\
      \"baud\":6900,\
      \"do_not_check_certificate\":false\
    }\
  }", JSON_DECODE_ANY, NULL);
  
  struct _u_request req_list[] = {
    {"GET", SERVER_URL_PREFIX "/device/", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0}, // 200
    {"POST", SERVER_URL_PREFIX "/device/", NULL, NULL, NULL, NULL, NULL, NULL, NULL, device_valid1, NULL, 0, NULL, 0}, // 200
    {"POST", SERVER_URL_PREFIX "/device/", NULL, NULL, NULL, NULL, NULL, NULL, NULL, device_invalid1, NULL, 0, NULL, 0}, // 400
    {"POST", SERVER_URL_PREFIX "/device/", NULL, NULL, NULL, NULL, NULL, NULL, NULL, device_invalid2, NULL, 0, NULL, 0}, // 400
    {"POST", SERVER_URL_PREFIX "/device/", NULL, NULL, NULL, NULL, NULL, NULL, NULL, device_invalid3, NULL, 0, NULL, 0}, // 400
    {"POST", SERVER_URL_PREFIX "/device/", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0}, // 400
    {"GET", SERVER_URL_PREFIX "/device/", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0}, // 200
    {"GET", SERVER_URL_PREFIX "/device/dev1", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0}, // 200
    {"PUT", SERVER_URL_PREFIX "/device/dev1", NULL, NULL, NULL, NULL, NULL, NULL, NULL, device_valid2, NULL, 0, NULL, 0}, // 200
    {"PUT", SERVER_URL_PREFIX "/device/dev2", NULL, NULL, NULL, NULL, NULL, NULL, NULL, device_valid2, NULL, 0, NULL, 0}, // 404
    {"PUT", SERVER_URL_PREFIX "/device/dev1", NULL, NULL, NULL, NULL, NULL, NULL, NULL, device_invalid1, NULL, 0, NULL, 0}, // 400
    {"GET", SERVER_URL_PREFIX "/device/dev1", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0}, // 200
    {"GET", SERVER_URL_PREFIX "/device/dev1/connect", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0}, // 200
    {"GET", SERVER_URL_PREFIX "/device/dev1/ping", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0}, // 200
    {"GET", SERVER_URL_PREFIX "/device/dev1/disconnect", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0}, // 200
    {"DELETE", SERVER_URL_PREFIX "/device/dev2", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0}, // 404
    {"DELETE", SERVER_URL_PREFIX "/device/dev1", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0}, // 200
  };

  test_request_status(&req_list[0], 200, NULL);
  test_request_status(&req_list[1], 200, NULL);
  test_request_status(&req_list[2], 400, NULL);
  test_request_status(&req_list[3], 400, NULL);
  test_request_status(&req_list[4], 400, NULL);
  test_request_status(&req_list[5], 400, NULL);
  test_request_status(&req_list[6], 200, device_valid1);
  test_request_status(&req_list[7], 200, device_valid1);
  test_request_status(&req_list[8], 200, NULL);
  test_request_status(&req_list[9], 404, NULL);
  test_request_status(&req_list[10], 400, NULL);
  test_request_status(&req_list[11], 200, device_valid2);
  test_request_status(&req_list[12], 200, NULL);
  test_request_status(&req_list[13], 200, NULL);
  test_request_status(&req_list[14], 200, NULL);
  test_request_status(&req_list[15], 404, NULL);
  test_request_status(&req_list[16], 200, NULL);
  
  json_decref(device_valid1);
  json_decref(device_valid2);
  json_decref(device_invalid1);
  json_decref(device_invalid2);
}

int main(void) {
  printf("Press <enter> to run device tests\n");
  getchar();
  run_device_tests();
  return 0;
}
