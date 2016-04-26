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
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_carleon_service_element_remove_tag - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else {
    if (alert_received((struct config_elements *)user_data, u_map_get(request->map_url, "submodule_name"), u_map_get(request->map_url, "source"), u_map_get(request->map_url, "element"), u_map_get(request->map_url, "message")) != A_OK) {
      response->status = 500;
    }
    return U_OK;
  }
}

int callback_angharad_submodule_list (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * j_result;
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_carleon_service_element_remove_tag - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else {
    j_result = submodule_get((struct config_elements *)user_data, NULL);
    if (j_result != NULL && json_integer_value(json_object_get(j_result, "result")) == A_ERROR_NOT_FOUND) {
      response->status = 404;
    } else if (j_result != NULL && json_integer_value(json_object_get(j_result, "result")) == A_OK) {
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
    if (j_result != NULL && json_integer_value(json_object_get(j_result, "result")) == A_ERROR_NOT_FOUND) {
      response->status = 404;
    } else if (j_result != NULL && json_integer_value(json_object_get(j_result, "result")) == A_OK) {
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
    res = submodule_enable((struct config_elements *)user_data, u_map_get(request->map_url, "submodule_name"), (0 == nstrcmp(u_map_get(request->map_url, "enabled"), "1")));
    if (res == A_ERROR_NOT_FOUND) {
      response->status = 404;
    } else if (res != A_OK) {
      response->status = 500;
    }
    return U_OK;
  }
}

int callback_angharad_script_list (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * j_script;
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_script_list - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else {
    j_script = script_get((struct config_elements *)user_data, NULL);
    if (j_script == NULL || json_integer_value(json_object_get(j_script, "result")) == A_ERROR) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_script_list - Error getting script list, aborting");
      response->status = 500;
    } else {
      response->json_body = json_copy(json_object_get(j_script, "scripts"));
    }
    json_decref(j_script);
    return U_OK;
  }
}

int callback_angharad_script_get (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * j_script;
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_script_get - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else {
    j_script = script_get((struct config_elements *)user_data, u_map_get(request->map_url, "script_name"));
    if (j_script == NULL || json_integer_value(json_object_get(j_script, "result")) == A_ERROR) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_script_get - Error getting script, aborting");
      response->status = 500;
    } else if (json_integer_value(json_object_get(j_script, "result")) == A_ERROR_NOT_FOUND) {
      response->status = 404;
    } else {
      response->json_body = json_copy(json_object_get(j_script, "script"));
    }
    json_decref(j_script);
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
    valid = is_script_valid((struct config_elements *)user_data, request->json_body, 0);
    if (valid != NULL && json_array_size(valid) == 0) {
      json_decref(valid);
      valid = script_get((struct config_elements *)user_data, json_string_value(json_object_get(request->json_body, "name")));
      if (valid == NULL || json_integer_value(json_object_get(valid, "result")) == A_ERROR) {
        response->status = 500;
      } else if (json_integer_value(json_object_get(valid, "result")) == A_ERROR_NOT_FOUND) {
        json_decref(valid);
        res = script_add((struct config_elements *)user_data, request->json_body);
        if (res == A_ERROR) {
          response->status = 500;
        }
      } else {
        json_decref(valid);
        response->status = 400;
        response->json_body = json_pack("{ss}", "error", "script name already exist");
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
    valid = is_script_valid((struct config_elements *)user_data, request->json_body, 1);
    if (valid != NULL && json_array_size(valid) == 0) {
      json_decref(valid);
      res = script_modify((struct config_elements *)user_data, u_map_get(request->map_url, "script_name"), request->json_body);
      if (res == A_OK) {
        response->status = 200;
      } else if (res == A_ERROR_NOT_FOUND) {
        response->status = 404;
      } else {
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

int callback_angharad_script_remove (const struct _u_request * request, struct _u_response * response, void * user_data) {
  int res;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_script_remove - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else {
    res = script_delete((struct config_elements *)user_data, u_map_get(request->map_url, "script_name"));
    if (res == A_OK) {
      response->status = 200;
    } else if (res == A_ERROR_NOT_FOUND) {
      response->status = 404;
    } else {
      response->status = 500;
    }
    return U_OK;
  }
}

int callback_angharad_script_add_tag (const struct _u_request * request, struct _u_response * response, void * user_data) {
  int res;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_script_add_tag - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else if (strlen(u_map_get(request->map_url, "tag")) > 64) {
    response->status = 400;
    response->json_body = json_pack("{ss}", "error", "tag invalid");
    return U_OK;
  } else {
    res = script_add_tag((struct config_elements *)user_data, u_map_get(request->map_url, "script_name"), u_map_get(request->map_url, "tag"));
    if (res == A_OK) {
      return U_OK;
    } else if (res == A_ERROR_NOT_FOUND) {
      response->status = 404;
      response->json_body = json_pack("{ss}", "error", "script not found");
    } else if (res == A_ERROR_PARAM) {
      response->status = 400;
      response->json_body = json_pack("{ss}", "error", "tag invalid");
    } else {
      response->status = 500;
    }
    return U_OK;
  }
}

int callback_angharad_script_remove_tag (const struct _u_request * request, struct _u_response * response, void * user_data) {
  int res;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_carleon_service_element_remove_tag - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else if (strlen(u_map_get(request->map_url, "tag")) > 64) {
    response->status = 400;
    response->json_body = json_pack("{ss}", "error", "tag invalid");
    return U_OK;
  } else {
    res = script_remove_tag((struct config_elements *)user_data, u_map_get(request->map_url, "script_name"), u_map_get(request->map_url, "tag"));
    if (res == A_OK) {
      return U_OK;
    } else if (res == A_ERROR_NOT_FOUND) {
      response->status = 404;
      response->json_body = json_pack("{ss}", "error", "script not found");
    } else if (res == A_ERROR_PARAM) {
      response->status = 400;
      response->json_body = json_pack("{ss}", "error", "tag invalid");
    } else {
      response->status = 500;
    }
    return U_OK;
  }
}

int callback_angharad_script_run (const struct _u_request * request, struct _u_response * response, void * user_data) {
  int res;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_carleon_service_element_remove_tag - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else {
    res = script_run((struct config_elements *)user_data, u_map_get(request->map_url, "script_name"));
    if (res == A_OK) {
      return U_OK;
    } else if (res == A_ERROR_NOT_FOUND) {
      response->status = 404;
      response->json_body = json_pack("{ss}", "error", "script not found");
    } else {
      response->status = 500;
      response->json_body = json_pack("{ss}", "error", "Error running script");
    }
    return U_OK;
  }
}

int callback_angharad_scheduler_list (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * j_scheduler;
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_scheduler_list - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else {
    j_scheduler = scheduler_get((struct config_elements *)user_data, NULL, 0);
    if (j_scheduler == NULL || json_integer_value(json_object_get(j_scheduler, "result")) == A_ERROR) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_scheduler_list - Error getting scheduler list, aborting");
      response->status = 500;
    } else {
      response->json_body = json_copy(json_object_get(j_scheduler, "schedulers"));
    }
    json_decref(j_scheduler);
    return U_OK;
  }
}

int callback_angharad_scheduler_get (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * j_scheduler;
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_scheduler_get - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else {
    j_scheduler = scheduler_get((struct config_elements *)user_data, u_map_get(request->map_url, "scheduler_name"), 0);
    if (j_scheduler == NULL || json_integer_value(json_object_get(j_scheduler, "result")) == A_ERROR) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_scheduler_get - Error getting scheduler, aborting");
      response->status = 500;
    } else if (json_integer_value(json_object_get(j_scheduler, "result")) == A_ERROR_NOT_FOUND) {
      response->status = 404;
    } else {
      response->json_body = json_copy(json_object_get(j_scheduler, "scheduler"));
    }
    json_decref(j_scheduler);
    return U_OK;
  }
}

int callback_angharad_scheduler_add (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * valid;
  int res;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_scheduler_get - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else {
    valid = is_scheduler_valid((struct config_elements *)user_data, request->json_body, 0);
    if (valid != NULL && json_array_size(valid) == 0) {
      json_decref(valid);
      valid = scheduler_get((struct config_elements *)user_data, json_string_value(json_object_get(request->json_body, "name")), 0);
      if (valid == NULL || json_integer_value(json_object_get(valid, "result")) == A_ERROR) {
        response->status = 500;
      } else if (json_integer_value(json_object_get(valid, "result")) == A_ERROR_NOT_FOUND) {
        json_decref(valid);
        res = scheduler_add((struct config_elements *)user_data, request->json_body);
        if (res != A_OK) {
          response->status = 500;
        }
      } else {
        json_decref(valid);
        response->status = 400;
        response->json_body = json_pack("{ss}", "error", "scheduler name already exist");
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

int callback_angharad_scheduler_modify (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * valid;
  int res;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_scheduler_modify - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else {
    valid = is_scheduler_valid((struct config_elements *)user_data, request->json_body, 1);
    if (valid != NULL && json_array_size(valid) == 0) {
      json_decref(valid);
      res = scheduler_modify((struct config_elements *)user_data, u_map_get(request->map_url, "scheduler_name"), request->json_body);
      if (res == A_OK) {
        response->status = 200;
      } else if (res == A_ERROR_NOT_FOUND) {
        response->status = 404;
      } else {
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

int callback_angharad_scheduler_remove (const struct _u_request * request, struct _u_response * response, void * user_data) {
  int res;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_scheduler_remove - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else {
    res = scheduler_delete((struct config_elements *)user_data, u_map_get(request->map_url, "scheduler_name"));
    if (res == A_OK) {
      response->status = 200;
    } else if (res == A_ERROR_NOT_FOUND) {
      response->status = 404;
    } else {
      response->status = 500;
    }
    return U_OK;
  }
}

int callback_angharad_scheduler_add_tag (const struct _u_request * request, struct _u_response * response, void * user_data) {
  int res;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_scheduler_add_tag - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else if (strlen(u_map_get(request->map_url, "tag")) > 64) {
    response->status = 400;
    response->json_body = json_pack("{ss}", "error", "tag invalid");
    return U_OK;
  } else {
    res = scheduler_add_tag((struct config_elements *)user_data, u_map_get(request->map_url, "scheduler_name"), u_map_get(request->map_url, "tag"));
    if (res == A_OK) {
      return U_OK;
    } else if (res == A_ERROR_NOT_FOUND) {
      response->status = 404;
      response->json_body = json_pack("{ss}", "error", "scheduler not found");
    } else if (res == A_ERROR_PARAM) {
      response->status = 400;
      response->json_body = json_pack("{ss}", "error", "tag invalid");
    } else {
      response->status = 500;
    }
    return U_OK;
  }
}

int callback_angharad_scheduler_remove_tag (const struct _u_request * request, struct _u_response * response, void * user_data) {
  int res;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_carleon_service_element_remove_tag - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else if (strlen(u_map_get(request->map_url, "tag")) > 64) {
    response->status = 400;
    response->json_body = json_pack("{ss}", "error", "tag invalid");
    return U_OK;
  } else {
    res = scheduler_remove_tag((struct config_elements *)user_data, u_map_get(request->map_url, "scheduler_name"), u_map_get(request->map_url, "tag"));
    if (res == A_OK) {
      return U_OK;
    } else if (res == A_ERROR_NOT_FOUND) {
      response->status = 404;
      response->json_body = json_pack("{ss}", "error", "scheduler not found");
    } else if (res == A_ERROR) {
      response->status = 400;
      response->json_body = json_pack("{ss}", "error", "tag invalid");
    } else {
      response->status = 500;
    }
    return U_OK;
  }
}

int callback_angharad_trigger_list (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * j_trigger;
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_trigger_list - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else {
    j_trigger = trigger_get((struct config_elements *)user_data, NULL);
    if (j_trigger == NULL || json_integer_value(json_object_get(j_trigger, "result")) == A_ERROR) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_trigger_list - Error getting trigger list, aborting");
      response->status = 500;
    } else {
      response->json_body = json_copy(json_object_get(j_trigger, "triggers"));
    }
    json_decref(j_trigger);
    return U_OK;
  }
}

int callback_angharad_trigger_get (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * j_trigger;
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_trigger_get - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else {
    j_trigger = trigger_get((struct config_elements *)user_data, u_map_get(request->map_url, "trigger_name"));
    if (j_trigger == NULL || json_integer_value(json_object_get(j_trigger, "result")) == A_ERROR) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_trigger_get - Error getting trigger, aborting");
      response->status = 500;
    } else if (json_integer_value(json_object_get(j_trigger, "result")) == A_ERROR_NOT_FOUND) {
      response->status = 404;
    } else {
      response->json_body = json_copy(json_object_get(j_trigger, "trigger"));
    }
    json_decref(j_trigger);
    return U_OK;
  }
}

int callback_angharad_trigger_add (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * valid;
  int res;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_trigger_get - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else {
    valid = is_trigger_valid((struct config_elements *)user_data, request->json_body, 0);
    if (valid != NULL && json_array_size(valid) == 0) {
      json_decref(valid);
      valid = trigger_get((struct config_elements *)user_data, json_string_value(json_object_get(request->json_body, "name")));
      if (valid == NULL || json_integer_value(json_object_get(valid, "result")) == A_ERROR) {
        response->status = 500;
      } else if (json_integer_value(json_object_get(valid, "result")) == A_ERROR_NOT_FOUND) {
        json_decref(valid);
        res = trigger_add((struct config_elements *)user_data, request->json_body);
        if (res != A_OK) {
          response->status = 500;
        }
      } else {
        json_decref(valid);
        response->status = 400;
        response->json_body = json_pack("{ss}", "error", "trigger name already exist");
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

int callback_angharad_trigger_modify (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * valid;
  int res;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_trigger_modify - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else {
    valid = is_trigger_valid((struct config_elements *)user_data, request->json_body, 1);
    if (valid != NULL && json_array_size(valid) == 0) {
      json_decref(valid);
      res = trigger_modify((struct config_elements *)user_data, u_map_get(request->map_url, "trigger_name"), request->json_body);
      if (res == A_OK) {
        response->status = 200;
      } else if (res == A_ERROR_NOT_FOUND) {
        response->status = 404;
      } else {
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

int callback_angharad_trigger_remove (const struct _u_request * request, struct _u_response * response, void * user_data) {
  int res;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_trigger_remove - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else {
    res = trigger_delete((struct config_elements *)user_data, u_map_get(request->map_url, "trigger_name"));
    if (res == A_OK) {
      response->status = 200;
    } else if (res == A_ERROR_NOT_FOUND) {
      response->status = 404;
    } else {
      response->status = 500;
    }
    return U_OK;
  }
}

int callback_angharad_trigger_add_tag (const struct _u_request * request, struct _u_response * response, void * user_data) {
  int res;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_trigger_add_tag - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else if (strlen(u_map_get(request->map_url, "tag")) > 64) {
    response->status = 400;
    response->json_body = json_pack("{ss}", "error", "tag invalid");
    return U_OK;
  } else {
    res = trigger_add_tag((struct config_elements *)user_data, u_map_get(request->map_url, "trigger_name"), u_map_get(request->map_url, "tag"));
    if (res == A_OK) {
      return U_OK;
    } else if (res == A_ERROR_NOT_FOUND) {
      response->status = 404;
      response->json_body = json_pack("{ss}", "error", "trigger not found");
    } else if (res == A_ERROR_PARAM) {
      response->status = 400;
      response->json_body = json_pack("{ss}", "error", "tag invalid");
    } else {
      response->status = 500;
    }
    return U_OK;
  }
}

int callback_angharad_trigger_remove_tag (const struct _u_request * request, struct _u_response * response, void * user_data) {
  int res;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_carleon_service_element_remove_tag - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else if (strlen(u_map_get(request->map_url, "tag")) > 64) {
    response->status = 400;
    response->json_body = json_pack("{ss}", "error", "tag invalid");
    return U_OK;
  } else {
    res = trigger_remove_tag((struct config_elements *)user_data, u_map_get(request->map_url, "trigger_name"), u_map_get(request->map_url, "tag"));
    if (res == A_OK) {
      return U_OK;
    } else if (res == A_ERROR_NOT_FOUND) {
      response->status = 404;
      response->json_body = json_pack("{ss}", "error", "trigger not found");
    } else if (res == A_ERROR_PARAM) {
      response->status = 400;
      response->json_body = json_pack("{ss}", "error", "tag invalid");
    } else {
      response->status = 500;
    }
    return U_OK;
  }
}

int callback_angharad_static_file (const struct _u_request * request, struct _u_response * response, void * user_data) {
  void * buffer = NULL;
  long length;
  FILE * f;
  char * file_requested = request->http_url + strlen(((struct config_elements *)user_data)->static_files_prefix) + 1;
  char * file_path;
  const char * content_type;

  if (file_requested == NULL || strlen(file_requested) == 0 || 0 == nstrcmp("/", file_requested)) {
    file_requested = "/index.html";
  }
  
  file_path = msprintf("%s%s", ((struct config_elements *)user_data)->static_files_path, file_requested);

  if (access(file_path, F_OK) != -1) {
    f = fopen (file_path, "rb");
    if (f) {
      fseek (f, 0, SEEK_END);
      length = ftell (f);
      fseek (f, 0, SEEK_SET);
      buffer = malloc(length*sizeof(void));
      if (buffer) {
        fread (buffer, 1, length, f);
      }
      fclose (f);
    }

    if (buffer) {
      content_type = u_map_get_case(((struct config_elements *)user_data)->mime_types, get_filename_ext(file_requested));
      if (content_type == NULL) {
        content_type = u_map_get(((struct config_elements *)user_data)->mime_types, "*");
        y_log_message(Y_LOG_LEVEL_WARNING, "Unknown mime type for extension %s", get_filename_ext(file_requested));
      }
      response->binary_body = buffer;
      response->binary_body_length = length;
      u_map_put(response->map_header, "Content-Type", content_type);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "error in %s", request->http_url);
      response->json_body = json_pack("{ss}", "error", request->http_url);
      response->status = 500;
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "%s not found", request->http_url);
    response->json_body = json_pack("{ss}", "not found", request->http_url);
    response->status = 404;
  }
  free(file_path);
  return U_OK;
}

int callback_angharad_auth_get (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * result;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_auth_get - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else {
    result = auth_get((struct config_elements *)user_data, u_map_get(request->map_cookie, "ANGHARAD_SESSION_ID"));
    
    if (result != NULL && json_integer_value(json_object_get(result, "result")) == A_OK) {
      response->json_body = json_copy(json_object_get(result, "session"));
      ulfius_add_cookie_to_response(response, "ANGHARAD_SESSION_ID", json_string_value(json_object_get(json_object_get(result, "session"), "token")), NULL, json_integer_value(json_object_get(json_object_get(result, "session"), "validity")), NULL, "/", 0, 0);
    } else {
      response->status = 401;
      ulfius_add_cookie_to_response(response, "ANGHARAD_SESSION_ID", "", NULL, 0, NULL, NULL, 0, 0);
    }
    
    json_decref(result);
  }
  return U_OK;
}

int callback_angharad_auth_check (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * result;
  int validity = 0;
  time_t now;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_auth_check - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else if (json_object_get(request->json_body, "user") == NULL || !json_is_string(json_object_get(request->json_body, "user")) ||
             json_object_get(request->json_body, "password") == NULL || !json_is_string(json_object_get(request->json_body, "password")) ||
             (json_object_get(request->json_body, "validity") != NULL && !json_is_integer(json_object_get(request->json_body, "validity")))) {
    response->status = 400;
    response->json_body = json_pack("{ss}", "error", "Invalid parameters");
    return U_OK;
  } else {
    result = auth_check((struct config_elements *)user_data,
                        json_string_value(json_object_get(request->json_body, "user")),
                        json_string_value(json_object_get(request->json_body, "password")),
                        json_integer_value(json_object_get(request->json_body, "validity")));
    if (result != NULL && json_integer_value(json_object_get(result, "result")) == A_OK) {
      response->json_body = json_copy(json_object_get(result, "session"));
      if (json_object_get(request->json_body, "validity") != NULL) {
        time(&now);
        if (now < json_integer_value(json_object_get(request->json_body, "validity"))) {
          validity = json_integer_value(json_object_get(request->json_body, "validity")) - now;
        }
      }
      ulfius_add_cookie_to_response(response, "ANGHARAD_SESSION_ID", json_string_value(json_object_get(json_object_get(result, "session"), "token")), NULL, validity, NULL, "/", 0, 0);
    } else {
      ulfius_add_cookie_to_response(response, "ANGHARAD_SESSION_ID", "", NULL, 0, NULL, NULL, 0, 0);
      response->status = 401;
    }
    json_decref(result);
    return U_OK;
  }
}

int callback_angharad_auth_function (const struct _u_request * request, struct _u_response * response, void * user_data) {
  return U_OK;
}

int callback_angharad_no_auth_function (const struct _u_request * request, struct _u_response * response, void * user_data) {
  return U_OK;
}
