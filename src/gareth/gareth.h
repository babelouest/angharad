/**
 *
 * Gareth messenger service
 *
 * Get messages from a REST Webservice
 * Send messages or digested data when previously parametered filters are triggered
 * Send protocols available: http, smtp
 *
 * declarations for constants and prototypes
 *
 * Copyright 2015-2016 Nicolas Mora <mail@babelouest.org>
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

#ifndef __GARETH_H_
#define __GARETH_H_

#define _GARETH_VERSION 1.1

/** Macro to avoid compiler warning when some parameters are unused and that's ok **/
#define UNUSED(x) (void)(x)

#include <jansson.h>

/** Angharad libraries **/
#include <ulfius.h>
#include <yder.h>

#define _HOEL_MARIADB
#define _HOEL_SQLITE
#include <hoel.h>

#define GARETH_LOG_NAME "Gareth"

#define FILTER_CLAUSE_ELEMENT_VALUE_LIST "#date#priority#source#message#tag#"
#define FILTER_CLAUSE_OPERATOR_VALUE_LIST "#=#!=#<#<=#>#>=#contains#notcontains#"

#define FILTER_CLAUSE_OPERATOR_EQUAL        0
#define FILTER_CLAUSE_OPERATOR_DIFFERENT    1
#define FILTER_CLAUSE_OPERATOR_LOWER        2
#define FILTER_CLAUSE_OPERATOR_LOWER_EQUAL  3
#define FILTER_CLAUSE_OPERATOR_HIGHER       4
#define FILTER_CLAUSE_OPERATOR_HIGHER_EQUAL 5
#define FILTER_CLAUSE_OPERATOR_CONTAINS     6
#define FILTER_CLAUSE_OPERATOR_NOT_CONTAINS 7

#define FILTER_CLAUSE_ELEMENT_DATE     0
#define FILTER_CLAUSE_ELEMENT_PRIORITY 1
#define FILTER_CLAUSE_ELEMENT_SOURCE   2
#define FILTER_CLAUSE_ELEMENT_MESSAGE  3
#define FILTER_CLAUSE_ELEMENT_TAG      4

#define PRIORITY_NONE     0
#define PRIORITY_LOW      1
#define PRIORITY_MEDIUM   2
#define PRIORITY_HIGH     3
#define PRIORITY_CRITICAL 4
#define NB_PRIORITY       5

/** Database table names and columns **/
#define TABLE_MESSAGE           "g_message"
#define COLUMN_MESSAGE_ID       "m_id"
#define COLUMN_MESSAGE_DATE     "m_date"
#define COLUMN_MESSAGE_PRIORITY "m_priority"
#define COLUMN_MESSAGE_SOURCE   "m_source"
#define COLUMN_MESSAGE_TEXT     "m_text"
#define COLUMN_MESSAGE_TAGS     "m_tags"

#define TABLE_FILTER              "g_filter"
#define COLUMN_FILTER_ID          "f_id"
#define COLUMN_FILTER_NAME        "f_name"
#define COLUMN_FILTER_DESCRIPTION "f_description"

#define TABLE_FILTER_CLAUSE           "g_filter_clause"
#define COLUMN_FILTER_CLAUSE_ELEMENT  "fc_element"
#define COLUMN_FILTER_CLAUSE_OPERATOR "fc_operator"
#define COLUMN_FILTER_CLAUSE_VALUE    "fc_value"

#define TABLE_FILTER_ALERT      "g_filter_alert"
#define COLUMN_FILTER_ALERT_ID  "f_id"

#define TABLE_ALERT_SMTP           "g_alert_smtp"
#define COLUMN_ALERT_SMTP_NAME     "as_name"
#define COLUMN_ALERT_SMTP_HOST     "as_host"
#define COLUMN_ALERT_SMTP_PORT     "as_port"
#define COLUMN_ALERT_SMTP_TLS      "as_tls"
#define COLUMN_ALERT_SMTP_CHECK_CA "as_check_ca"
#define COLUMN_ALERT_SMTP_USER     "as_user"
#define COLUMN_ALERT_SMTP_PASSWORD "as_password"
#define COLUMN_ALERT_SMTP_FROM     "as_from"
#define COLUMN_ALERT_SMTP_TO       "as_to"
#define COLUMN_ALERT_SMTP_CC       "as_cc"
#define COLUMN_ALERT_SMTP_BCC      "as_bcc"
#define COLUMN_ALERT_SMTP_SUBJECT  "as_subject"
#define COLUMN_ALERT_SMTP_BODY     "as_body"

#define TABLE_ALERT_HTTP         "g_alert_http"
#define COLUMN_ALERT_HTTP_ID     "ah_id"
#define COLUMN_ALERT_HTTP_NAME   "ah_name"
#define COLUMN_ALERT_HTTP_METHOD "ah_method"
#define COLUMN_ALERT_HTTP_URL    "ah_url"
#define COLUMN_ALERT_HTTP_BODY   "ah_body"

#define TABLE_ALERT_HTTP_HEADER        "g_alert_http_header"
#define COLUMN_ALERT_HTTP_HEADER_KEY   "ahh_key"
#define COLUMN_ALERT_HTTP_HEADER_VALUE "ahh_value"

int init_gareth(struct _u_instance * instance, const char * url_prefix, struct _h_connection * conn);
int close_gareth(struct _u_instance * instance, const char * url_prefix);
int set_response_json_body_and_clean(struct _u_response * response, uint status, json_t * json_body);

// SMTP alert management
json_t * get_smtp_alert(struct _h_connection * conn, const char * name);
json_t * parse_smtp_alert_from_db(const json_t * j_result);
json_t * parse_smtp_alert_from_http(const json_t * input_smtp);
json_t * is_smtp_alert_valid(const json_t * input_smtp, int update);
int add_smtp_alert(struct _h_connection * conn, const json_t * alert_smtp);
int update_smtp_alert(struct _h_connection * conn, const json_t * alert_smtp, const char * name);
int remove_smtp_alert(struct _h_connection * conn, const char * name);

// HTTP Alert management
json_t * get_http_alert(struct _h_connection * conn, const char * name);
json_t * get_http_header_alert(struct _h_connection * conn, json_t * http_alert_id);
json_t * parse_http_alert_from_db(const json_t * j_result);
json_t * parse_http_alert_from_http(const json_t * input_http);
json_t * is_http_alert_valid(const json_t * input_http, int update);
int add_http_alert(struct _h_connection * conn, const json_t * alert_http);
int update_http_alert(struct _h_connection * conn, const json_t * alert_http, const char * name);
int remove_http_alert(struct _h_connection * conn, const char * name);
int add_http_header_alert(struct _h_connection * conn, const json_t * alert_http_header);
int remove_http_header_alert(struct _h_connection * conn, const json_t * alert_http_id);

// Filter management
json_t * get_filter(struct _h_connection * conn, const char * name);
json_t * get_filter_clause(struct _h_connection * conn, json_t * filter_id);
json_t * get_filter_alert(struct _h_connection * conn, json_t * filter_id);
json_t * parse_filter_from_db(const json_t * j_result);
json_t * parse_filter_from_http(const json_t * input_filter);
json_t * parse_filter_clause_from_db(const json_t * j_result);
json_t * parse_filter_clause_from_http(const json_t * filter_clause);
json_t * parse_filter_alert_from_db(const json_t * filter_alert);
json_t * parse_filter_alert_from_http(const json_t * filter_alert, const json_t * filter_id);
json_t * is_filter_valid(struct _h_connection * conn, const json_t * filter, int update);
int is_filter_clause_valid(const json_t * input_filter_clause);
int is_filter_alert_valid(struct _h_connection * conn, const json_t * input_filter_alert);
int add_filter(struct _h_connection * conn, const json_t * filter);
int update_filter(struct _h_connection * conn, const json_t * filter, const char * name);
int remove_filter(struct _h_connection * conn, const char * name);
int add_filter_clause(struct _h_connection * conn, const json_t * alert_filter);
int remove_filter_clause(struct _h_connection * conn, const json_t * filter_id);
int add_filter_alert(struct _h_connection * conn, const json_t * filter_alert);
int remove_filter_alert(struct _h_connection * conn, const json_t * filter_id);

// Message management
int add_message(struct _h_connection * conn, const json_t * message);
json_t * is_message_valid(const json_t * message);
json_t * get_message_list(struct _h_connection * conn, const char * filter_name, const struct _u_map * map_url, uint limit, uint offset);
json_t * generate_where_clause_from_filter_name(struct _h_connection * conn, const char * filter_name);
int append_where_clause_from_url_parameters(struct _h_connection * conn, json_t ** where_clause, const struct _u_map * map_url);
json_t * get_corresponding_alert_list(struct _h_connection * conn, json_t * message_id);

// Alert send
int trigger_smtp_message(struct _h_connection * conn, const char * smtp_alert, const json_t * message);
int trigger_http_message(struct _h_connection * conn, const char * http_alert, const json_t * message);
char * parse_string_with_message(const char * format, const json_t * message);

// Callback functions

// Alert management callback functions
int callback_gareth_get_alert_list (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_gareth_get_alert (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_gareth_add_alert (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_gareth_modify_alert (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_gareth_delete_alert (const struct _u_request * request, struct _u_response * response, void * user_data);

// Filter management callback functions
int callback_gareth_get_filter_list (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_gareth_get_filter (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_gareth_add_filter (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_gareth_modify_filter (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_gareth_delete_filter (const struct _u_request * request, struct _u_response * response, void * user_data);

// Message management callback functions
int callback_gareth_get_messages (const struct _u_request * request, struct _u_response * response, void * user_data);
int callback_gareth_add_messages (const struct _u_request * request, struct _u_response * response, void * user_data);

#endif
