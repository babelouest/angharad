/**
 *
 * Angharad system
 *
 * Manage all subsystems and handles the house automation server:
 * - Benoic
 * - Gareth
 * - Carleon
 *
 * Declarations for constants and prototypes
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

#ifndef __ANGHARAD_H_
#define __ANGHARAD_H_

#define _ANGHARAD_VERSION 1.1

// Angharad libraries
#include <orcania.h>
#include <yder.h>
#define U_DISABLE_WEBSOCKET
#include <ulfius.h>
#define _HOEL_MARIADB
#define _HOEL_SQLITE
#include <hoel.h>

#include <jwt.h>

// Angharad submodules
#include "benoic/benoic.h"
#include "carleon/carleon.h"
#include "gareth/gareth.h"

#define ANGHARAD_RUNNING  0
#define ANGHARAD_STOP     1
#define ANGHARAD_ERROR    2

#define ANGHARAD_LOG_NAME "Angharad"

// Configuration management
#define ANGHARAD_DEFAULT_PREFIX "angharad"
#define ANGHARAD_DEFAULT_PORT   2473

#define BENOIC_DEFAULT_PREFIX       "benoic"
#define CARLEON_DEFAULT_PREFIX      "carleon"
#define GARETH_DEFAULT_PREFIX       "gareth"
#define ALLOW_ORIGIN_DEFAULT        "*"
#define STATIC_FILES_PREFIX_DEFAULT "sagremor"

#define PREFIX_BEARER               "Bearer "

#define MODULE_RESULT_ERROR     0
#define MODULE_RESULT_OK        1
#define MODULE_RESULT_NOT_FOUND 2
#define MODULE_RESULT_TIMEOUT   3

#define A_OK                 0
#define A_ERROR              1
#define A_ERROR_DB           2
#define A_ERROR_IO           3
#define A_ERROR_NOT_FOUND    4
#define A_ERROR_PARAM        5
#define A_ERROR_MEMORY       6
#define A_ERROR_TRUE         7
#define A_ERROR_FALSE        8
#define A_ERROR_UNAUTHORIZED 9
#define A_ERROR_EXPIRED      10

#define ANGHARAD_STATUS_RUN      0
#define ANGHARAD_STATUS_STOPPING 1
#define ANGHARAD_STATUS_STOP     2

#define ANGHARAD_SUBMODULE_BENOIC  "benoic"
#define ANGHARAD_SUBMODULE_CARLEON "carleon"
#define ANGHARAD_SUBMODULE_GARETH  "gareth"
#define ANGHARAD_SUBMODULE_FERGUS  "fergus"

#define ANGHARAD_TABLE_SUBMODULE        "a_submodule"
#define ANGHARAD_TABLE_SESSION          "a_session"
#define ANGHARAD_TABLE_SCRIPT           "a_script"
#define ANGHARAD_TABLE_SCHEDULER        "a_scheduler"
#define ANGHARAD_TABLE_TRIGGER          "a_trigger"
#define ANGHARAD_TABLE_SCHEDULER_SCRIPT "a_scheduler_script"
#define ANGHARAD_TABLE_TRIGGER_SCRIPT   "a_trigger_script"
#define ANGHARAD_TABLE_USER             "a_user"
#define ANGHARAD_TABLE_PROFILE          "a_profile"

#define ANGHARAD_AUTH_TYPE_NONE     0
#define ANGHARAD_AUTH_TYPE_DATABASE 1
#define ANGHARAD_AUTH_TYPE_LDAP     2

#define SCHEDULER_REPEAT_NONE         -1
#define SCHEDULER_REPEAT_MINUTE       0
#define SCHEDULER_REPEAT_HOUR         1
#define SCHEDULER_REPEAT_DAY          2
#define SCHEDULER_REPEAT_DAY_OF_WEEK  3
#define SCHEDULER_REPEAT_MONTH        4
#define SCHEDULER_REPEAT_YEAR         5

#define TRIGGER_MESSAGE_MATCH_NONE         0
#define TRIGGER_MESSAGE_MATCH_EQUAL        1
#define TRIGGER_MESSAGE_MATCH_DIFFERENT    2
#define TRIGGER_MESSAGE_MATCH_CONTAINS     3
#define TRIGGER_MESSAGE_MATCH_NOT_CONTAINS 4
#define TRIGGER_MESSAGE_MATCH_EMPTY        5
#define TRIGGER_MESSAGE_MATCH_NOT_EMPTY    6

struct _auth_ldap {
  char * uri;
  char * bind_dn;
  char * bind_passwd;
  char * filter;
  char * login_property;
  char * base_search;
};

struct config_elements {
  char *                             config_file;
  char *                             url_prefix_angharad;
  char *                             url_prefix_benoic;
  char *                             url_prefix_carleon;
  char *                             url_prefix_gareth;
  char *                             static_files_prefix;
  char *                             static_files_path;
  char *                             alert_url;
  struct _u_map                    * mime_types;
  char *                             allow_origin;
  unsigned long                      log_mode;
  unsigned long                      log_level;
  char *                             log_file;
  struct _u_instance               * instance;
  struct _h_connection             * conn;
  struct _benoic_config            * b_config;
  struct _carleon_config           * c_config;
  unsigned int                       angharad_status;
  struct _glewlwyd_resource_config * glewlwyd_client_config;
};

int global_handler_variable;

int  build_config_from_args(int argc, char ** argv, struct config_elements * config);
int  build_config_from_file(struct config_elements * config);
int  check_config(struct config_elements * config);
void exit_handler(int handler);
void exit_server(struct config_elements ** config, int exit_value);
void print_help(FILE * output);
char * get_alert_url(struct config_elements * config);
char * get_file_content(const char * file_path);

json_t * submodule_get(struct config_elements * config, const char * submodule);
int submodule_enable(struct config_elements * config, const char * submodule, int enabled);

int init_angharad(struct config_elements * config);
int close_angharad(struct config_elements * config);
int angharad_run_thread(struct config_elements * config);

void * thread_scheduler_run(void * args);

int alert_received(struct config_elements * config, const char * submodule_name, const char * source, const char * element, const char * message);

json_t * script_get(struct config_elements * config, const char * script_name);
int script_add(struct config_elements * config, json_t * j_script);
int script_modify(struct config_elements * config, const char * script_name, json_t * j_script);
int script_delete(struct config_elements * config, const char * script_name);
json_t * is_script_valid(struct config_elements * config, json_t * j_script, const int update);
int script_run(struct config_elements * config, const char * script_name);
json_t * is_actions_valid(struct config_elements * config, json_t * j_action_list);
json_t * is_action_valid(struct config_elements * config, json_t * j_action, const int is_condition);
int script_add_tag(struct config_elements * config, const char * script_name, const char * tag);
int script_remove_tag(struct config_elements * config, const char * script_name, const char * tag);
int action_run(struct config_elements * config, json_t * j_action);

json_t * is_condition_list_valid(struct config_elements * config, json_t * condition_list);
json_t * is_condition_valid(struct config_elements * config, json_t * condition);
int condition_list_check(struct config_elements * config, json_t * condition_list);
int condition_check(struct config_elements * config, json_t * condition);
int compare_values(json_t * value1, json_t * value2, const char * operator);

json_t * is_script_list_valid(struct config_elements * config, json_t * script_list);

json_t * scheduler_get(struct config_elements * config, const char * scheduler_name, const int runnable);
json_t * scheduler_get_next_schedules(struct config_elements * config);
int scheduler_enable(struct config_elements * config, json_t * j_scheduler, int enabled);
json_t * scheduler_get_script_list(struct config_elements * config, const char * scheduler_name);
int scheduler_add(struct config_elements * config, json_t * j_scheduler);
int scheduler_set_script_list(struct config_elements * config, const char * scheduler_name, json_t * script_list);
json_t * is_scheduler_valid(struct config_elements * config, json_t * j_scheduler, const int update);
int scheduler_modify(struct config_elements * config, const char * scheduler_name, json_t * j_scheduler);
int scheduler_delete(struct config_elements * config, const char * scheduler_name);
int scheduler_add_tag(struct config_elements * config, const char * scheduler_name, const char * tag);
int scheduler_remove_tag(struct config_elements * config, const char * scheduler_name, const char * tag);
time_t scheduler_calculate_next_time(time_t from, int schedule_type, unsigned int schedule_value);

json_t * trigger_get(struct config_elements * config, const char * trigger_name);
int trigger_enable(struct config_elements * config, json_t * j_trigger, int enabled);
json_t * trigger_get_script_list(struct config_elements * config, const char * trigger_name);
int trigger_add(struct config_elements * config, json_t * j_trigger);
int trigger_set_script_list(struct config_elements * config, const char * trigger_name, json_t * script_list);
json_t * is_trigger_valid(struct config_elements * config, json_t * j_trigger, const int update);
int trigger_modify(struct config_elements * config, const char * trigger_name, json_t * j_trigger);
int trigger_delete(struct config_elements * config, const char * trigger_name);
int trigger_add_tag(struct config_elements * config, const char * trigger_name, const char * tag);
int trigger_remove_tag(struct config_elements * config, const char * trigger_name, const char * tag);

// profiles core functions
json_t * profile_list(struct config_elements * config);
json_t * profile_get(struct config_elements * config, const char * profile_id);
int profile_add(struct config_elements * config, const char * profile_id, json_t * profile_data);
int profile_modify(struct config_elements * config, const char * profile_id, json_t * profile_data);
int profile_delete(struct config_elements * config, const char * profile_id);

json_t * user_get(struct config_elements * config, const char * user_name);
int user_add(struct config_elements * config, json_t * j_user);
int user_modify(struct config_elements * config, const char * user_name, json_t * j_user);
int user_delete(struct config_elements * config, const char * user_name);
json_t * is_user_valid(struct config_elements * config, json_t * j_user, const int update);

json_t * token_get_list(struct config_elements * config, const char * login, const char * enabled);
int token_revoke(struct config_elements * config, json_t * token);

const char * get_filename_ext(const char *path);
int set_response_json_body_and_clean(struct _u_response * response, uint status, json_t * json_body);

json_t * auth_get(struct config_elements * config, const char * session_id);
int auth_invalidate(struct config_elements * config, const char * session_id);
int auth_update_last_seen(struct config_elements * config, const char * session_id);
json_t * auth_check(struct config_elements * config, const char * login, const char * password, const int validity);
int auth_check_credentials(struct config_elements * config, const char * login, const char * password);
int auth_check_credentials_database(struct config_elements * config, const char * login, const char * password);
int auth_check_credentials_ldap(struct config_elements * config, const char * login, const char * password);
json_t * auth_generate_new_token(struct config_elements * config, const char * login, int validity);

// Callback functions for webservices
int callback_angharad_alert (const struct _u_request * request, struct _u_response * response, void * user_data);

int callback_angharad_submodule_list (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_angharad_submodule_get (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_angharad_submodule_enable (const struct _u_request * request, struct _u_response * response, void * user_data);

int callback_angharad_script_list (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_angharad_script_get (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_angharad_script_add (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_angharad_script_modify (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_angharad_script_remove (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_angharad_script_add_tag (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_angharad_script_remove_tag (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_angharad_script_run (const struct _u_request * request, struct _u_response * response, void * user_data);

int callback_angharad_scheduler_list (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_angharad_scheduler_get (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_angharad_scheduler_enable (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_angharad_scheduler_add (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_angharad_scheduler_modify (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_angharad_scheduler_remove (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_angharad_scheduler_add_tag (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_angharad_scheduler_remove_tag (const struct _u_request * request, struct _u_response * response, void * user_data);

int callback_angharad_trigger_list (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_angharad_trigger_get (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_angharad_trigger_enable (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_angharad_trigger_add (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_angharad_trigger_modify (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_angharad_trigger_remove (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_angharad_trigger_add_tag (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_angharad_trigger_remove_tag (const struct _u_request * request, struct _u_response * response, void * user_data);

int callback_carleon_profile_list (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_carleon_profile_get (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_carleon_profile_add (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_carleon_profile_set (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_carleon_profile_remove (const struct _u_request * request, struct _u_response * response, void * user_data);

int callback_angharad_static_file (const struct _u_request * request, struct _u_response * response, void * user_data);

int callback_angharad_options (const struct _u_request * request, struct _u_response * response, void * user_data);

int callback_angharad_root_url (const struct _u_request * request, struct _u_response * response, void * user_data);

int callback_angharad_default (const struct _u_request * request, struct _u_response * response, void * user_data);

int callback_angharad_no_auth_function (const struct _u_request * request, struct _u_response * response, void * user_data);

#endif
