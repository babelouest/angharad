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
  response->json_body = json_pack("{ssssss}", "error", "page not found", "message", "The resource can not be found, check documentation", "url", request->http_url);
  response->status = 404;
  return H_OK;
}

int callback_angharad_alert (const struct _u_request * request, struct _u_response * response, void * user_data) {
  return U_OK;
}

int callback_angharad_submodule_list (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * j_result;
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_carleon_service_element_remove_tag - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else {
    j_result = submodule_get((struct config_elements *)user_data, NULL);
    if (j_result != NULL && json_integer_value(json_object_get(j_result, "result")) == WEBSERVICE_RESULT_NOT_FOUND) {
      response->status = 404;
    } else if (j_result != NULL && json_integer_value(json_object_get(j_result, "result")) == WEBSERVICE_RESULT_OK) {
      response->json_body = json_copy(json_object_get(j_result, "submodules"));
    } else {
      response->status = 500;
    }
    json_decref(j_result);
    return U_OK;
  }
}

int callback_angharad_submodule_get (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * j_result;
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_carleon_service_element_remove_tag - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else {
    j_result = submodule_get((struct config_elements *)user_data, u_map_get(request->map_url, "submodule_name"));
    if (j_result != NULL && json_integer_value(json_object_get(j_result, "result")) == WEBSERVICE_RESULT_NOT_FOUND) {
      response->status = 404;
    } else if (j_result != NULL && json_integer_value(json_object_get(j_result, "result")) == WEBSERVICE_RESULT_OK) {
      response->json_body = json_copy(json_object_get(j_result, "submodule"));
    } else {
      response->status = 500;
    }
    json_decref(j_result);
    return U_OK;
  }
}

int callback_angharad_submodule_enable (const struct _u_request * request, struct _u_response * response, void * user_data) {
  int res;
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_carleon_service_element_remove_tag - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else {
    res = submodule_enable((struct config_elements *)user_data, u_map_get(request->map_url, "submodule_name"), (0 == strcmp(u_map_get(request->map_url, "enabled"), "1")));
    if (res == A_ERROR_NOT_FOUND) {
      response->status = 404;
    } else if (res != A_OK) {
      response->status = 500;
    }
    return U_OK;
  }
}

int callback_angharad_script_list (const struct _u_request * request, struct _u_response * response, void * user_data) {
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_script_list - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else {
    response->json_body = script_get((struct config_elements *)user_data, NULL);
    if (response->json_body == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_script_list - Error getting script list, aborting");
      response->status = 500;
    }
    return U_OK;
  }
}

int callback_angharad_script_get (const struct _u_request * request, struct _u_response * response, void * user_data) {
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_script_get - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else {
    response->json_body = script_get((struct config_elements *)user_data, u_map_get(request->map_url, "script_id"));
    if (response->json_body == NULL) {
      response->status = 404;
    }
    return U_OK;
  }
}

int callback_angharad_script_add (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * valid;
  int res;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_script_get - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else {
    valid = is_script_valid((struct config_elements *)user_data, request->json_body);
    if (valid != NULL && json_array_size(valid) == 0) {
      json_decref(valid);
      res = script_add((struct config_elements *)user_data, request->json_body);
      if (res == WEBSERVICE_RESULT_ERROR) {
        response->status = 500;
      }
    } else if (valid != NULL) {
      response->json_body = valid;
      response->status = 400;
    } else {
      response->status = 500;
    }
    return U_OK;
  }
}

int callback_angharad_script_modify (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * valid;
  int res;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_script_modify - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else {
    valid = is_script_valid((struct config_elements *)user_data, request->json_body);
    if (valid != NULL && json_array_size(valid) == 0) {
      json_decref(valid);
      res = script_modify((struct config_elements *)user_data, u_map_get(request->map_url, "script_id"), request->json_body);
      if (res == WEBSERVICE_RESULT_ERROR) {
        response->status = 500;
      } else if (res == WEBSERVICE_RESULT_NOT_FOUND) {
        response->status = 404;
      }
    } else if (valid != NULL) {
      response->json_body = valid;
      response->status = 400;
    } else {
      response->status = 500;
    }
    return U_OK;
  }
}

int callback_angharad_script_remove (const struct _u_request * request, struct _u_response * response, void * user_data) {
  int res;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_script_remove - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else {
    res = script_delete((struct config_elements *)user_data, u_map_get(request->map_url, "script_id"));
    if (res == WEBSERVICE_RESULT_ERROR) {
      response->status = 500;
    } else if (res == WEBSERVICE_RESULT_NOT_FOUND) {
      response->status = 404;
    }
    return U_OK;
  }
}

int callback_angharad_script_execute (const struct _u_request * request, struct _u_response * response, void * user_data) {
  return U_OK;
}

int callback_angharad_script_add_tag (const struct _u_request * request, struct _u_response * response, void * user_data) {
  return U_OK;
}

int callback_angharad_script_remove_tag (const struct _u_request * request, struct _u_response * response, void * user_data) {
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

int callback_angharad_event_add_tag (const struct _u_request * request, struct _u_response * response, void * user_data) {
  return U_OK;
}

int callback_angharad_event_remove_tag (const struct _u_request * request, struct _u_response * response, void * user_data) {
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
