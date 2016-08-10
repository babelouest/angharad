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

int callback_angharad_scheduler_enable (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * j_scheduler;
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_scheduler_enable - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else {
    j_scheduler = scheduler_get((struct config_elements *)user_data, u_map_get(request->map_url, "scheduler_name"), 0);
    if (j_scheduler == NULL || json_integer_value(json_object_get(j_scheduler, "result")) == A_ERROR) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_scheduler_enable - Error getting scheduler, aborting");
      response->status = 500;
    } else if (json_integer_value(json_object_get(j_scheduler, "result")) == A_ERROR_NOT_FOUND) {
      response->status = 404;
    } else {
      if (scheduler_enable((struct config_elements *)user_data, json_object_get(j_scheduler, "scheduler"), 0==nstrcmp(u_map_get(request->map_url, "enabled"), "1")?1:0) != A_OK) {
		  y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_scheduler_enable - Error setting scheduler enabled");
		  response->status = 500;
	  }
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

int callback_angharad_trigger_enable (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * j_trigger;
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_trigger_enable - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else {
    j_trigger = trigger_get((struct config_elements *)user_data, u_map_get(request->map_url, "trigger_name"));
    if (j_trigger == NULL || json_integer_value(json_object_get(j_trigger, "result")) == A_ERROR) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_trigger_enable - Error getting trigger, aborting");
      response->status = 500;
    } else if (json_integer_value(json_object_get(j_trigger, "result")) == A_ERROR_NOT_FOUND) {
      response->status = 404;
    } else {
      if (trigger_enable((struct config_elements *)user_data, json_object_get(j_trigger, "trigger"), 0==nstrcmp(u_map_get(request->map_url, "enabled"), "1")?1:0) != A_OK) {
		  y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_trigger_enable - Error setting trigger enabled");
		  response->status = 500;
	  }
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

int callback_carleon_profile_list (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * j_list;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_carleon_profile_list - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else {
    j_list = profile_list((struct config_elements *)user_data);
    if (json_integer_value(json_object_get(j_list, "result")) != WEBSERVICE_RESULT_OK) {
      response->status = 500;
    } else {
      response->json_body = json_copy(json_object_get(j_list, "list"));
    }
    json_decref(j_list);
    return U_OK;
  }
}

int callback_carleon_profile_get (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * j_profile;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_carleon_profile_list - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else {
    j_profile = profile_get((struct config_elements *)user_data, u_map_get(request->map_url, "profile_id"));
    if (json_integer_value(json_object_get(j_profile, "result")) == WEBSERVICE_RESULT_OK) {
      response->json_body = json_copy(json_object_get(j_profile, "profile"));
    } else if (json_integer_value(json_object_get(j_profile, "result")) == WEBSERVICE_RESULT_NOT_FOUND) {
      response->status = 404;
    } else {
      response->status = 500;
    }
    json_decref(j_profile);
    return U_OK;
  }
}

int callback_carleon_profile_set (const struct _u_request * request, struct _u_response * response, void * user_data) {
  int res;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_carleon_profile_list - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else {
    res = profile_modify((struct config_elements *)user_data, u_map_get(request->map_url, "profile_id"), request->json_body);
    if (res == C_ERROR_PARAM) {
      response->status = 400;
    } else if (res != C_OK) {
      response->status = 500;
    }
    return U_OK;
  }
}

int callback_carleon_profile_remove (const struct _u_request * request, struct _u_response * response, void * user_data) {
  int res;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_carleon_profile_list - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else {
    res = profile_delete((struct config_elements *)user_data, u_map_get(request->map_url, "profile_id"));
    if (res == C_ERROR_PARAM) {
      response->status = 400;
    } else if (res == C_ERROR_NOT_FOUND) {
      response->status = 404;
    } else if (res != C_OK) {
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
  } else {
    if (strchr(file_requested, '?') != NULL) {
      *strchr(file_requested, '?') = '\0';
    }
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
      y_log_message(Y_LOG_LEVEL_INFO, "Static File Server - Serving file %s", file_requested);
      content_type = u_map_get_case(((struct config_elements *)user_data)->mime_types, get_filename_ext(file_requested));
      if (content_type == NULL) {
        content_type = u_map_get(((struct config_elements *)user_data)->mime_types, "*");
        y_log_message(Y_LOG_LEVEL_WARNING, "Static File Server - Unknown mime type for extension %s", get_filename_ext(file_requested));
      }
      response->binary_body = buffer;
      response->binary_body_length = length;
      u_map_put(response->map_header, "Content-Type", content_type);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "Static File Server - Internal error in %s", request->http_url);
      response->json_body = json_pack("{ss}", "error", request->http_url);
      response->status = 500;
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "Static File Server - File %s not found", request->http_url);
    response->json_body = json_pack("{ss}", "not found", request->http_url);
    response->status = 404;
  }
  free(file_path);
  return U_OK;
}

int callback_angharad_auth_get (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * result;
  time_t now;
  const char * session_id;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_auth_get - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else if (!((struct config_elements *)user_data)->has_auth_database && !((struct config_elements *)user_data)->has_auth_ldap) {
    return U_OK;
  } else {
    session_id = u_map_get(request->map_cookie, "ANGHARAD_SESSION_ID");
    if (session_id == NULL) {
      session_id = u_map_get(request->map_header, "ANGHARAD_SESSION_ID");
    }
    
    result = auth_get((struct config_elements *)user_data, session_id);
    
    if (result != NULL && json_integer_value(json_object_get(result, "result")) == A_OK) {
      if (auth_update_last_seen((struct config_elements *)user_data, json_string_value(json_object_get(json_object_get(result, "session"), "token"))) == A_OK) {
        time(&now);
        response->json_body = json_copy(json_object_get(result, "session"));
        ulfius_add_cookie_to_response(response, "ANGHARAD_SESSION_ID", json_string_value(json_object_get(json_object_get(result, "session"), "token")), NULL, (json_integer_value(json_object_get(json_object_get(result, "session"), "validity")) - now), NULL, "/", 0, 0);
      } else {
        response->status = 500;
      }
    } else {
      response->status = 401;
    }
    
    json_decref(result);
  }
  return U_OK;
}

int callback_angharad_auth_check (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * result;
  int cookie_max_age = 0, user_validity = 0;
  time_t now;
  struct tm* tm;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_auth_check - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else if (!((struct config_elements *)user_data)->has_auth_database && !((struct config_elements *)user_data)->has_auth_ldap) {
    return U_OK;
  } else if (request->json_body == NULL || json_object_get(request->json_body, "user") == NULL || !json_is_string(json_object_get(request->json_body, "user")) ||
             json_object_get(request->json_body, "password") == NULL || !json_is_string(json_object_get(request->json_body, "password")) ||
             (json_object_get(request->json_body, "validity") != NULL && !json_is_integer(json_object_get(request->json_body, "validity")))) {
    response->status = 400;
    response->json_body = json_pack("{ss}", "error", "Invalid parameters");
    return U_OK;
  } else {
    time(&now);
    if (json_integer_value(json_object_get(request->json_body, "validity")) < 60 || json_object_get(request->json_body, "validity") == NULL) {
      // Session token
      user_validity = 0;
    } else if (json_integer_value(json_object_get(request->json_body, "validity")) <= (60*60)) {
      // 1 hour session
      user_validity = now + (60*60);
    } else if (json_integer_value(json_object_get(request->json_body, "validity")) <= (12*60*60)) {
      // 12 hour session
      user_validity = now + (12*60*60);
    } else if (json_integer_value(json_object_get(request->json_body, "validity")) <= (24*60*60)) {
      // 1 day session
      user_validity = now + (24*60*60);
    } else if (json_integer_value(json_object_get(request->json_body, "validity")) <= (7*12*60*60)) {
      // 1 week session
      user_validity = now + (7*24*60*60);
    } else {
      // 10 years session
      tm = localtime(&now);
      tm->tm_year += 10;
      user_validity = mktime(tm);
    }
    result = auth_check((struct config_elements *)user_data,
                        json_string_value(json_object_get(request->json_body, "user")),
                        json_string_value(json_object_get(request->json_body, "password")),
                        user_validity);
    if (result != NULL && json_integer_value(json_object_get(result, "result")) == A_OK) {
      response->json_body = json_copy(json_object_get(result, "session"));
      if (json_object_get(request->json_body, "validity") != NULL) {
        if (now < json_integer_value(json_object_get(request->json_body, "validity"))) {
          cookie_max_age = user_validity - now;
        }
      }
      ulfius_add_cookie_to_response(response, "ANGHARAD_SESSION_ID", json_string_value(json_object_get(json_object_get(result, "session"), "token")), NULL, cookie_max_age, NULL, "/", 0, 0);
    } else {
      // Invalidate cookie
      ulfius_add_cookie_to_response(response, "ANGHARAD_SESSION_ID", "deleted", "Thu, 01 Jan 1970 00:00:00 GMT", 0, NULL, "/", 0, 0);
      response->status = 401;
    }
    json_decref(result);
    return U_OK;
  }
}

// Invalidate session if exist
int callback_angharad_auth_delete (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * result;
  const char * session_id;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_auth_get - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else {
    session_id = u_map_get(request->map_cookie, "ANGHARAD_SESSION_ID");
    if (session_id == NULL) {
      session_id = u_map_get(request->map_header, "ANGHARAD_SESSION_ID");
    }
    result = auth_get((struct config_elements *)user_data, session_id);
    
    if (result != NULL && json_integer_value(json_object_get(result, "result")) == A_OK) {
      if (auth_invalidate((struct config_elements *)user_data, session_id) != A_OK) {
        response->status = 500;
      } else {
        ulfius_add_cookie_to_response(response, "ANGHARAD_SESSION_ID", "deleted", "Thu, 01 Jan 1970 00:00:00 GMT", 0, NULL, "/", 0, 0);
      }
    } else {
      response->status = 401;
    }
    
    json_decref(result);
  }
  return U_OK;
}

int callback_angharad_auth_function (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * result;
  const char * session_id;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_auth_get - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else if (!((struct config_elements *)user_data)->has_auth_database && !((struct config_elements *)user_data)->has_auth_ldap) {
    return U_OK;
  } else {
    session_id = u_map_get(request->map_cookie, "ANGHARAD_SESSION_ID");
    if (session_id == NULL) {
      session_id = u_map_get(request->map_header, "ANGHARAD_SESSION_ID");
    }
    result = auth_get((struct config_elements *)user_data, session_id);
    
    if (result != NULL && json_integer_value(json_object_get(result, "result")) == A_OK) {
      json_decref(result);
      if (auth_update_last_seen((struct config_elements *)user_data, session_id) == A_OK) {
        return U_OK;
      } else {
        return U_ERROR_PARAMS;
      }
    } else {
      json_decref(result);
      return U_ERROR_UNAUTHORIZED;
    }
    
  }
  return U_OK;
}

int callback_angharad_no_auth_function (const struct _u_request * request, struct _u_response * response, void * user_data) {
  return U_OK;
}

int callback_angharad_options (const struct _u_request * request, struct _u_response * response, void * user_data) {
  u_map_put(response->map_header, "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
  u_map_put(response->map_header, "Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept, ANGHARAD_SESSION_ID");
  u_map_put(response->map_header, "Access-Control-Max-Age", "1800");
  return U_OK;
}
