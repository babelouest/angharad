/**
 *
 * Angharad system
 *
 * Manage all subsystems and handles the house automation server:
 * - Benoic
 * - Gareth
 * - Carleon
 * 
 * ulfius webservices callback functions
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

#include "angharad.h"

int callback_angharad_root_url (const struct _u_request * request, struct _u_response * response, void * user_data) {
  return U_OK;
}

int callback_angharad_default (const struct _u_request * request, struct _u_response * response, void * user_data) {
  return U_OK;
}

int callback_angharad_alert (const struct _u_request * request, struct _u_response * response, void * user_data) {
  return U_OK;
}

int callback_angharad_submodule_list (const struct _u_request * request, struct _u_response * response, void * user_data) {
  return U_OK;
}

int callback_angharad_submodule_get (const struct _u_request * request, struct _u_response * response, void * user_data) {
  return U_OK;
}

int callback_angharad_submodule_command_list (const struct _u_request * request, struct _u_response * response, void * user_data) {
  return U_OK;
}

int callback_angharad_submodule_element_list (const struct _u_request * request, struct _u_response * response, void * user_data) {
  return U_OK;
}

int callback_angharad_script_list (const struct _u_request * request, struct _u_response * response, void * user_data) {
  return U_OK;
}

int callback_angharad_script_get (const struct _u_request * request, struct _u_response * response, void * user_data) {
  return U_OK;
}

int callback_angharad_script_execute (const struct _u_request * request, struct _u_response * response, void * user_data) {
  return U_OK;
}

int callback_angharad_script_add (const struct _u_request * request, struct _u_response * response, void * user_data) {
  return U_OK;
}

int callback_angharad_script_modify (const struct _u_request * request, struct _u_response * response, void * user_data) {
  return U_OK;
}

int callback_angharad_script_remove (const struct _u_request * request, struct _u_response * response, void * user_data) {
  return U_OK;
}

int callback_angharad_event_list (const struct _u_request * request, struct _u_response * response, void * user_data) {
  return U_OK;
}

int callback_angharad_event_get (const struct _u_request * request, struct _u_response * response, void * user_data) {
  return U_OK;
}

int callback_angharad_event_add (const struct _u_request * request, struct _u_response * response, void * user_data) {
  return U_OK;
}

int callback_angharad_event_modify (const struct _u_request * request, struct _u_response * response, void * user_data) {
  return U_OK;
}

int callback_angharad_event_remove (const struct _u_request * request, struct _u_response * response, void * user_data) {
  return U_OK;
}

int callback_angharad_static_file (const struct _u_request * request, struct _u_response * response, void * user_data) {
  return U_OK;
}

int callback_angharad_auth_get (const struct _u_request * request, struct _u_response * response, void * user_data) {
  return U_OK;
}

int callback_angharad_auth_check (const struct _u_request * request, struct _u_response * response, void * user_data) {
  return U_OK;
}

int callback_angharad_auth_function (const struct _u_request * request, struct _u_response * response, void * user_data) {
  return U_OK;
}

int callback_angharad_no_auth_function (const struct _u_request * request, struct _u_response * response, void * user_data) {
  return U_OK;
}
