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

#include <libconfig.h>
#include <getopt.h>
#include <signal.h>
#include <jansson.h>

// Angharad libraries
#include <ulfius.h>
#define _HOEL_MARIADB
#define _HOEL_SQLITE
#include <hoel.h>
#include <orcania.h>
#include <yder.h>

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
#define ANGHARAD_DEFAULT_REALM  "angharad"

#define BENOIC_DEFAULT_PREFIX       "benoic"
#define CARLEON_DEFAULT_PREFIX      "carleon"
#define GARETH_DEFAULT_PREFIX       "gareth"
#define ALLOW_ORIGIN_DEFAULT        "*"
#define STATIC_FILES_PREFIX_DEFAULT "sagremor"

#define A_OK              0
#define A_ERROR           1
#define A_ERROR_DB        2
#define A_ERROR_IO        3
#define A_ERROR_NOT_FOUND 4
#define A_ERROR_PARAM     5
#define A_ERROR_MEMORY    6

#define ANGHARAD_STATUS_RUN      0
#define ANGHARAD_STATUS_STOPPING 1
#define ANGHARAD_STATUS_STOP     2

#define ANGHARAD_RESULT_ERROR     0
#define ANGHARAD_RESULT_OK        1
#define ANGHARAD_RESULT_NOT_FOUND 2

#define ANGHARAD_SUBMODULE_BENOIC  "benoic"
#define ANGHARAD_SUBMODULE_CARLEON "carleon"
#define ANGHARAD_SUBMODULE_GARETH  "gareth"
#define ANGHARAD_SUBMODULE_FERGUS  "fergus"

#define ANGHARAD_TABLE_SUBMODULE       "a_submodule"
#define ANGHARAD_TABLE_SESSION         "a_session"
#define ANGHARAD_TABLE_SCRIPT          "a_script"
#define ANGHARAD_TABLE_SCHEDULER       "a_scheduler"
#define ANGHARAD_TABLE_TRIGGER         "a_trigger"
#define ANGHARAD_TABLE_EVENT           "a_event"
#define ANGHARAD_TABLE_EVENT_SCHEDULER "a_event_scheduler"
#define ANGHARAD_TABLE_EVENT_TRIGGER   "a_event_trigger"

struct config_elements {
  char *                   config_file;
  char *                   url_prefix_angharad;
  char *                   url_prefix_benoic;
  char *                   url_prefix_carleon;
  char *                   url_prefix_gareth;
  char *                   angharad_realm;
  char *                   static_files_prefix;
  char *                   static_files_path;
  struct _u_map          * mime_types;
  char *                   allow_origin;
  unsigned long            log_mode;
  unsigned long            log_level;
  char *                   log_file;
  struct _u_instance     * instance;
  struct _h_connection   * conn;
  struct _benoic_config  * b_config;
  struct _carleon_config * c_config;
  unsigned int             angharad_status;
};

int global_handler_variable;

int  build_config_from_args(int argc, char ** argv, struct config_elements * config);
int  build_config_from_file(struct config_elements * config);
int  check_config(struct config_elements * config);
void exit_handler(int handler);
void exit_server(struct config_elements ** config, int exit_value);
void print_help(FILE * output);

json_t * submodule_get(struct config_elements * config, const char * submodule);
int submodule_enable(struct config_elements * config, const char * submodule, int enabled);

int init_angharad(struct config_elements * config);
int close_angharad(struct config_elements * config);
int angharad_run_thread(struct config_elements * config);

void * thread_event_run(void * args);

json_t * script_get(struct config_elements * config, const char * script_name);
int script_add(struct config_elements * config, json_t * j_script);
int script_modify(struct config_elements * config, const char * script_name, json_t * j_script);
int script_delete(struct config_elements * config, const char * script_name);
json_t * is_script_valid(struct config_elements * config, json_t * j_script);
int script_run(struct config_elements * config, const char * script_name);
json_t * is_actions_valid(struct config_elements * config, json_t * j_action_list);
json_t * is_action_valid(struct config_elements * config, json_t * j_action);
int script_add_tag(struct config_elements * config, const char * script_name, const char * tag);
int script_remove_tag(struct config_elements * config, const char * script_name, const char * tag);
int action_run(struct config_elements * config, json_t * j_action);

const char * get_filename_ext(const char *path);

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

int callback_angharad_event_list (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_angharad_event_get (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_angharad_event_add (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_angharad_event_modify (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_angharad_event_remove (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_angharad_event_add_tag (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_angharad_event_remove_tag (const struct _u_request * request, struct _u_response * response, void * user_data);

int callback_angharad_scheduler_list (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_angharad_scheduler_get (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_angharad_scheduler_add (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_angharad_scheduler_modify (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_angharad_scheduler_remove (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_angharad_scheduler_add_tag (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_angharad_scheduler_remove_tag (const struct _u_request * request, struct _u_response * response, void * user_data);

int callback_angharad_static_file (const struct _u_request * request, struct _u_response * response, void * user_data);

int callback_angharad_root_url (const struct _u_request * request, struct _u_response * response, void * user_data);

int callback_angharad_default (const struct _u_request * request, struct _u_response * response, void * user_data);

int callback_angharad_auth_get (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_angharad_auth_check (const struct _u_request * request, struct _u_response * response, void * user_data);

int callback_angharad_auth_function (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_angharad_no_auth_function (const struct _u_request * request, struct _u_response * response, void * user_data);

#endif
