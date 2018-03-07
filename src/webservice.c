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

#include <string.h>
#include "angharad.h"

int callback_angharad_alert (const struct _u_request * request, struct _u_response * response, void * user_data) {  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_carleon_service_element_remove_tag - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    if (alert_received((struct config_elements *)user_data, u_map_get(request->map_url, "submodule_name"), u_map_get(request->map_url, "source"), u_map_get(request->map_url, "element"), u_map_get(request->map_url, "message")) != A_OK) {
      response->status = 500;
    }
    return U_CALLBACK_CONTINUE;
  }
}

int callback_angharad_submodule_list (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * j_result;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_carleon_service_element_remove_tag - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    j_result = submodule_get((struct config_elements *)user_data, NULL);
    if (j_result != NULL && json_integer_value(json_object_get(j_result, "result")) == A_ERROR_NOT_FOUND) {
      response->status = 404;
    } else if (j_result != NULL && json_integer_value(json_object_get(j_result, "result")) == A_OK) {
      set_response_json_body_and_clean(response, 200, json_copy(json_object_get(j_result, "submodules")));
    } else {
      response->status = 500;
    }
    json_decref(j_result);
    return U_CALLBACK_CONTINUE;
  }
}

int callback_angharad_submodule_get (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * j_result;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_carleon_service_element_remove_tag - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    j_result = submodule_get((struct config_elements *)user_data, u_map_get(request->map_url, "submodule_name"));
    if (j_result != NULL && json_integer_value(json_object_get(j_result, "result")) == A_ERROR_NOT_FOUND) {
      response->status = 404;
    } else if (j_result != NULL && json_integer_value(json_object_get(j_result, "result")) == A_OK) {
      set_response_json_body_and_clean(response, 200, json_copy(json_object_get(j_result, "submodule")));
    } else {
      response->status = 500;
    }
    json_decref(j_result);
    return U_CALLBACK_CONTINUE;
  }
}

int callback_angharad_script_list (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * j_script;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_script_list - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    j_script = script_get((struct config_elements *)user_data, NULL);
    if (j_script == NULL || json_integer_value(json_object_get(j_script, "result")) == A_ERROR) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_script_list - Error getting script list, aborting");
      response->status = 500;
    } else {
      set_response_json_body_and_clean(response, 200, json_copy(json_object_get(j_script, "scripts")));
    }
    json_decref(j_script);
    return U_CALLBACK_CONTINUE;
  }
}

int callback_angharad_script_get (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * j_script;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_script_get - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    j_script = script_get((struct config_elements *)user_data, u_map_get(request->map_url, "script_name"));
    if (j_script == NULL || json_integer_value(json_object_get(j_script, "result")) == A_ERROR) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_script_get - Error getting script, aborting");
      response->status = 500;
    } else if (json_integer_value(json_object_get(j_script, "result")) == A_ERROR_NOT_FOUND) {
      response->status = 404;
    } else {
      set_response_json_body_and_clean(response, 200, json_copy(json_object_get(j_script, "script")));
    }
    json_decref(j_script);
    return U_CALLBACK_CONTINUE;
  }
}

int callback_angharad_script_add (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * valid, * script;
  int res;
  json_t * json_body = ulfius_get_json_body_request(request, NULL);
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_script_get - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    valid = is_script_valid((struct config_elements *)user_data, json_body, 0);
    if (valid != NULL && json_array_size(valid) == 0) {
      script = script_get((struct config_elements *)user_data, json_string_value(json_object_get(json_body, "name")));
      if (script == NULL || json_integer_value(json_object_get(script, "result")) == A_ERROR) {
        response->status = 500;
      } else if (json_integer_value(json_object_get(script, "result")) == A_ERROR_NOT_FOUND) {
        res = script_add((struct config_elements *)user_data, json_body);
        if (res == A_ERROR) {
          response->status = 500;
        }
      } else {
        set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "script name already exist"));
      }
      json_decref(script);
    } else if (valid != NULL) {
      set_response_json_body_and_clean(response, 400, json_copy(valid));
    } else {
      response->status = 500;
    }
    json_decref(valid);
    json_decref(json_body);
    return U_CALLBACK_CONTINUE;
  }
}

int callback_angharad_script_modify (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * valid;
  int res;
  json_t * json_body = ulfius_get_json_body_request(request, NULL);
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_script_modify - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    valid = is_script_valid((struct config_elements *)user_data, json_body, 1);
    if (valid != NULL && json_array_size(valid) == 0) {
      json_decref(valid);
      res = script_modify((struct config_elements *)user_data, u_map_get(request->map_url, "script_name"), json_body);
      if (res == A_OK) {
        response->status = 200;
      } else if (res == A_ERROR_NOT_FOUND) {
        response->status = 404;
      } else {
        response->status = 500;
      }
    } else if (valid != NULL) {
      set_response_json_body_and_clean(response, 400, valid);
    } else {
      response->status = 500;
    }
    json_decref(json_body);
    return U_CALLBACK_CONTINUE;
  }
}

int callback_angharad_script_remove (const struct _u_request * request, struct _u_response * response, void * user_data) {
  int res;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_script_remove - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    res = script_delete((struct config_elements *)user_data, u_map_get(request->map_url, "script_name"));
    if (res == A_OK) {
      response->status = 200;
    } else if (res == A_ERROR_NOT_FOUND) {
      response->status = 404;
    } else {
      response->status = 500;
    }
    return U_CALLBACK_CONTINUE;
  }
}

int callback_angharad_script_add_tag (const struct _u_request * request, struct _u_response * response, void * user_data) {
  int res;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_script_add_tag - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else if (strlen(u_map_get(request->map_url, "tag")) > 64) {
    set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "tag invalid"));
    return U_CALLBACK_CONTINUE;
  } else {
    res = script_add_tag((struct config_elements *)user_data, u_map_get(request->map_url, "script_name"), u_map_get(request->map_url, "tag"));
    if (res == A_OK) {
      return U_CALLBACK_CONTINUE;
    } else if (res == A_ERROR_NOT_FOUND) {
      set_response_json_body_and_clean(response, 404, json_pack("{ss}", "error", "script not found"));
    } else if (res == A_ERROR_PARAM) {
      set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "tag invalid"));
    } else {
      response->status = 500;
    }
    return U_CALLBACK_CONTINUE;
  }
}

int callback_angharad_script_remove_tag (const struct _u_request * request, struct _u_response * response, void * user_data) {
  int res;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_carleon_service_element_remove_tag - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else if (strlen(u_map_get(request->map_url, "tag")) > 64) {
    set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "tag invalid"));
    return U_CALLBACK_CONTINUE;
  } else {
    res = script_remove_tag((struct config_elements *)user_data, u_map_get(request->map_url, "script_name"), u_map_get(request->map_url, "tag"));
    if (res == A_OK) {
      return U_CALLBACK_CONTINUE;
    } else if (res == A_ERROR_NOT_FOUND) {
      set_response_json_body_and_clean(response, 404, json_pack("{ss}", "error", "script not found"));
    } else if (res == A_ERROR_PARAM) {
      set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "tag invalid"));
    } else {
      response->status = 500;
    }
    return U_CALLBACK_CONTINUE;
  }
}

int callback_angharad_script_run (const struct _u_request * request, struct _u_response * response, void * user_data) {
  int res;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_carleon_service_element_remove_tag - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    res = script_run((struct config_elements *)user_data, u_map_get(request->map_url, "script_name"));
    if (res == A_OK) {
      return U_CALLBACK_CONTINUE;
    } else if (res == A_ERROR_NOT_FOUND) {
      set_response_json_body_and_clean(response, 404, json_pack("{ss}", "error", "script not found"));
    } else {
      set_response_json_body_and_clean(response, 500, json_pack("{ss}", "error", "Error running script"));
    }
    return U_CALLBACK_CONTINUE;
  }
}

int callback_angharad_scheduler_list (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * j_scheduler;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_scheduler_list - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    j_scheduler = scheduler_get((struct config_elements *)user_data, NULL, 0);
    if (j_scheduler == NULL || json_integer_value(json_object_get(j_scheduler, "result")) == A_ERROR) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_scheduler_list - Error getting scheduler list, aborting");
      response->status = 500;
    } else {
      set_response_json_body_and_clean(response, 200, json_copy(json_object_get(j_scheduler, "schedulers")));
    }
    json_decref(j_scheduler);
    return U_CALLBACK_CONTINUE;
  }
}

int callback_angharad_scheduler_get (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * j_scheduler;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_scheduler_get - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    j_scheduler = scheduler_get((struct config_elements *)user_data, u_map_get(request->map_url, "scheduler_name"), 0);
    if (j_scheduler == NULL || json_integer_value(json_object_get(j_scheduler, "result")) == A_ERROR) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_scheduler_get - Error getting scheduler, aborting");
      response->status = 500;
    } else if (json_integer_value(json_object_get(j_scheduler, "result")) == A_ERROR_NOT_FOUND) {
      response->status = 404;
    } else {
      set_response_json_body_and_clean(response, 200, json_copy(json_object_get(j_scheduler, "scheduler")));
    }
    json_decref(j_scheduler);
    return U_CALLBACK_CONTINUE;
  }
}

int callback_angharad_scheduler_enable (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * j_scheduler;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_scheduler_enable - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    j_scheduler = scheduler_get((struct config_elements *)user_data, u_map_get(request->map_url, "scheduler_name"), 0);
    if (j_scheduler == NULL || json_integer_value(json_object_get(j_scheduler, "result")) == A_ERROR) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_scheduler_enable - Error getting scheduler, aborting");
      response->status = 500;
    } else if (json_integer_value(json_object_get(j_scheduler, "result")) == A_ERROR_NOT_FOUND) {
      response->status = 404;
    } else {
      if (scheduler_enable((struct config_elements *)user_data, json_object_get(j_scheduler, "scheduler"), 0==o_strcmp(u_map_get(request->map_url, "enabled"), "1")?1:0) != A_OK) {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_scheduler_enable - Error setting scheduler enabled");
        response->status = 500;
      } else {
        set_response_json_body_and_clean(response, 200, json_copy(json_object_get(j_scheduler, "scheduler")));
      }
    }
    json_decref(j_scheduler);
    return U_CALLBACK_CONTINUE;
  }
}

int callback_angharad_scheduler_add (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * valid;
  int res;
  json_t * json_body = ulfius_get_json_body_request(request, NULL);
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_scheduler_get - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    valid = is_scheduler_valid((struct config_elements *)user_data, json_body, 0);
    if (valid != NULL && json_array_size(valid) == 0) {
      json_decref(valid);
      valid = scheduler_get((struct config_elements *)user_data, json_string_value(json_object_get(json_body, "name")), 0);
      if (valid == NULL || json_integer_value(json_object_get(valid, "result")) == A_ERROR) {
        response->status = 500;
      } else if (json_integer_value(json_object_get(valid, "result")) == A_ERROR_NOT_FOUND) {
        json_decref(valid);
        res = scheduler_add((struct config_elements *)user_data, json_body);
        if (res != A_OK) {
          response->status = 500;
        }
      } else {
        json_decref(valid);
        set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "scheduler name already exist"));
      }
    } else if (valid != NULL) {
      set_response_json_body_and_clean(response, 400, valid);
    } else {
      response->status = 500;
    }
    json_decref(json_body);
    return U_CALLBACK_CONTINUE;
  }
}

int callback_angharad_scheduler_modify (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * valid;
  int res;
  json_t * json_body = ulfius_get_json_body_request(request, NULL);
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_scheduler_modify - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    valid = is_scheduler_valid((struct config_elements *)user_data, json_body, 1);
    if (valid != NULL && json_array_size(valid) == 0) {
      json_decref(valid);
      res = scheduler_modify((struct config_elements *)user_data, u_map_get(request->map_url, "scheduler_name"), json_body);
      if (res == A_OK) {
        response->status = 200;
      } else if (res == A_ERROR_NOT_FOUND) {
        response->status = 404;
      } else {
        response->status = 500;
      }
    } else if (valid != NULL) {
      set_response_json_body_and_clean(response, 400, valid);
    } else {
      response->status = 500;
    }
    json_decref(json_body);
    return U_CALLBACK_CONTINUE;
  }
}

int callback_angharad_scheduler_remove (const struct _u_request * request, struct _u_response * response, void * user_data) {
  int res;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_scheduler_remove - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    res = scheduler_delete((struct config_elements *)user_data, u_map_get(request->map_url, "scheduler_name"));
    if (res == A_OK) {
      response->status = 200;
    } else if (res == A_ERROR_NOT_FOUND) {
      response->status = 404;
    } else {
      response->status = 500;
    }
    return U_CALLBACK_CONTINUE;
  }
}

int callback_angharad_scheduler_add_tag (const struct _u_request * request, struct _u_response * response, void * user_data) {
  int res;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_scheduler_add_tag - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else if (strlen(u_map_get(request->map_url, "tag")) > 64) {
    set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "tag invalid"));
    return U_CALLBACK_CONTINUE;
  } else {
    res = scheduler_add_tag((struct config_elements *)user_data, u_map_get(request->map_url, "scheduler_name"), u_map_get(request->map_url, "tag"));
    if (res == A_OK) {
      return U_CALLBACK_CONTINUE;
    } else if (res == A_ERROR_NOT_FOUND) {
      set_response_json_body_and_clean(response, 404, json_pack("{ss}", "error", "scheduler not found"));
    } else if (res == A_ERROR_PARAM) {
      set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "tag invalid"));
    } else {
      response->status = 500;
    }
    return U_CALLBACK_CONTINUE;
  }
}

int callback_angharad_scheduler_remove_tag (const struct _u_request * request, struct _u_response * response, void * user_data) {
  int res;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_carleon_service_element_remove_tag - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else if (strlen(u_map_get(request->map_url, "tag")) > 64) {
    set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "tag invalid"));
    return U_CALLBACK_CONTINUE;
  } else {
    res = scheduler_remove_tag((struct config_elements *)user_data, u_map_get(request->map_url, "scheduler_name"), u_map_get(request->map_url, "tag"));
    if (res == A_OK) {
      return U_CALLBACK_CONTINUE;
    } else if (res == A_ERROR_NOT_FOUND) {
      set_response_json_body_and_clean(response, 404, json_pack("{ss}", "error", "scheduler not found"));
    } else if (res == A_ERROR) {
      set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "tag invalid"));
    } else {
      response->status = 500;
    }
    return U_CALLBACK_CONTINUE;
  }
}

int callback_angharad_trigger_list (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * j_trigger;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_trigger_list - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    j_trigger = trigger_get((struct config_elements *)user_data, NULL);
    if (j_trigger == NULL || json_integer_value(json_object_get(j_trigger, "result")) == A_ERROR) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_trigger_list - Error getting trigger list, aborting");
      response->status = 500;
    } else {
      set_response_json_body_and_clean(response, 200, json_copy(json_object_get(j_trigger, "triggers")));
    }
    json_decref(j_trigger);
    return U_CALLBACK_CONTINUE;
  }
}

int callback_angharad_trigger_get (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * j_trigger;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_trigger_get - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    j_trigger = trigger_get((struct config_elements *)user_data, u_map_get(request->map_url, "trigger_name"));
    if (j_trigger == NULL || json_integer_value(json_object_get(j_trigger, "result")) == A_ERROR) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_trigger_get - Error getting trigger, aborting");
      response->status = 500;
    } else if (json_integer_value(json_object_get(j_trigger, "result")) == A_ERROR_NOT_FOUND) {
      response->status = 404;
    } else {
      set_response_json_body_and_clean(response, 200, json_copy(json_object_get(j_trigger, "trigger")));
    }
    json_decref(j_trigger);
    return U_CALLBACK_CONTINUE;
  }
}

int callback_angharad_trigger_enable (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * j_trigger;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_trigger_enable - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    j_trigger = trigger_get((struct config_elements *)user_data, u_map_get(request->map_url, "trigger_name"));
    if (j_trigger == NULL || json_integer_value(json_object_get(j_trigger, "result")) == A_ERROR) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_trigger_enable - Error getting trigger, aborting");
      response->status = 500;
    } else if (json_integer_value(json_object_get(j_trigger, "result")) == A_ERROR_NOT_FOUND) {
      response->status = 404;
    } else {
      if (trigger_enable((struct config_elements *)user_data, json_object_get(j_trigger, "trigger"), 0==o_strcmp(u_map_get(request->map_url, "enabled"), "1")?1:0) != A_OK) {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_trigger_enable - Error setting trigger enabled");
        response->status = 500;
      } else {
        set_response_json_body_and_clean(response, 200, json_copy(json_object_get(j_trigger, "trigger")));
      }
    }
    json_decref(j_trigger);
    return U_CALLBACK_CONTINUE;
  }
}

int callback_angharad_trigger_add (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * valid;
  int res;
  json_t * json_body = ulfius_get_json_body_request(request, NULL);
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_trigger_get - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    valid = is_trigger_valid((struct config_elements *)user_data, json_body, 0);
    if (valid != NULL && json_array_size(valid) == 0) {
      json_decref(valid);
      valid = trigger_get((struct config_elements *)user_data, json_string_value(json_object_get(json_body, "name")));
      if (valid == NULL || json_integer_value(json_object_get(valid, "result")) == A_ERROR) {
        response->status = 500;
      } else if (json_integer_value(json_object_get(valid, "result")) == A_ERROR_NOT_FOUND) {
        json_decref(valid);
        res = trigger_add((struct config_elements *)user_data, json_body);
        if (res != A_OK) {
          response->status = 500;
        }
      } else {
        json_decref(valid);
        set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "trigger name already exist"));
      }
    } else if (valid != NULL) {
      set_response_json_body_and_clean(response, 400, valid);
    } else {
      response->status = 500;
    }
    json_decref(json_body);
    return U_CALLBACK_CONTINUE;
  }
}

int callback_angharad_trigger_modify (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * valid;
  int res;
  json_t * json_body = ulfius_get_json_body_request(request, NULL);
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_trigger_modify - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    valid = is_trigger_valid((struct config_elements *)user_data, json_body, 1);
    if (valid != NULL && json_array_size(valid) == 0) {
      json_decref(valid);
      res = trigger_modify((struct config_elements *)user_data, u_map_get(request->map_url, "trigger_name"), json_body);
      if (res == A_OK) {
        response->status = 200;
      } else if (res == A_ERROR_NOT_FOUND) {
        response->status = 404;
      } else {
        response->status = 500;
      }
    } else if (valid != NULL) {
      set_response_json_body_and_clean(response, 400, valid);
    } else {
      response->status = 500;
    }
    json_decref(json_body);
    return U_CALLBACK_CONTINUE;
  }
}

int callback_angharad_trigger_remove (const struct _u_request * request, struct _u_response * response, void * user_data) {
  int res;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_trigger_remove - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    res = trigger_delete((struct config_elements *)user_data, u_map_get(request->map_url, "trigger_name"));
    if (res == A_OK) {
      response->status = 200;
    } else if (res == A_ERROR_NOT_FOUND) {
      response->status = 404;
    } else {
      response->status = 500;
    }
    return U_CALLBACK_CONTINUE;
  }
}

int callback_angharad_trigger_add_tag (const struct _u_request * request, struct _u_response * response, void * user_data) {
  int res;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_angharad_trigger_add_tag - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else if (strlen(u_map_get(request->map_url, "tag")) > 64) {
    set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "tag invalid"));
    return U_CALLBACK_CONTINUE;
  } else {
    res = trigger_add_tag((struct config_elements *)user_data, u_map_get(request->map_url, "trigger_name"), u_map_get(request->map_url, "tag"));
    if (res == A_OK) {
      return U_CALLBACK_CONTINUE;
    } else if (res == A_ERROR_NOT_FOUND) {
      set_response_json_body_and_clean(response, 404, json_pack("{ss}", "error", "trigger not found"));
    } else if (res == A_ERROR_PARAM) {
      set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "tag invalid"));
    } else {
      response->status = 500;
    }
    return U_CALLBACK_CONTINUE;
  }
}

int callback_angharad_trigger_remove_tag (const struct _u_request * request, struct _u_response * response, void * user_data) {
  int res;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_carleon_service_element_remove_tag - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else if (strlen(u_map_get(request->map_url, "tag")) > 64) {
    set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "tag invalid"));
    return U_CALLBACK_CONTINUE;
  } else {
    res = trigger_remove_tag((struct config_elements *)user_data, u_map_get(request->map_url, "trigger_name"), u_map_get(request->map_url, "tag"));
    if (res == A_OK) {
      return U_CALLBACK_CONTINUE;
    } else if (res == A_ERROR_NOT_FOUND) {
      set_response_json_body_and_clean(response, 404, json_pack("{ss}", "error", "trigger not found"));
    } else if (res == A_ERROR_PARAM) {
      set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "tag invalid"));
    } else {
      response->status = 500;
    }
    return U_CALLBACK_CONTINUE;
  }
}

int callback_carleon_profile_list (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * j_list;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_carleon_profile_list - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    j_list = profile_list((struct config_elements *)user_data);
    if (json_integer_value(json_object_get(j_list, "result")) != WEBSERVICE_RESULT_OK) {
      response->status = 500;
    } else {
      set_response_json_body_and_clean(response, 200, json_copy(json_object_get(j_list, "list")));
    }
    json_decref(j_list);
    return U_CALLBACK_CONTINUE;
  }
}

int callback_carleon_profile_get (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * j_profile;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_carleon_profile_list - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    j_profile = profile_get((struct config_elements *)user_data, u_map_get(request->map_url, "profile_id"));
    if (json_integer_value(json_object_get(j_profile, "result")) == WEBSERVICE_RESULT_OK) {
      set_response_json_body_and_clean(response, 200, json_copy(json_object_get(j_profile, "profile")));
    } else if (json_integer_value(json_object_get(j_profile, "result")) == WEBSERVICE_RESULT_NOT_FOUND) {
      response->status = 404;
    } else {
      response->status = 500;
    }
    json_decref(j_profile);
    return U_CALLBACK_CONTINUE;
  }
}

int callback_carleon_profile_set (const struct _u_request * request, struct _u_response * response, void * user_data) {
  int res;
  json_t * json_body = ulfius_get_json_body_request(request, NULL);
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_carleon_profile_list - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    res = profile_modify((struct config_elements *)user_data, u_map_get(request->map_url, "profile_id"), json_body);
    if (res == C_ERROR_PARAM) {
      response->status = 400;
    } else if (res != C_OK) {
      response->status = 500;
    }
    json_decref(json_body);
    return U_CALLBACK_CONTINUE;
  }
}

int callback_carleon_profile_remove (const struct _u_request * request, struct _u_response * response, void * user_data) {
  int res;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_carleon_profile_list - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    res = profile_delete((struct config_elements *)user_data, u_map_get(request->map_url, "profile_id"));
    if (res == C_ERROR_PARAM) {
      response->status = 400;
    } else if (res == C_ERROR_NOT_FOUND) {
      response->status = 404;
    } else if (res != C_OK) {
      response->status = 500;
    }
    return U_CALLBACK_CONTINUE;
  }
}

int callback_angharad_options (const struct _u_request * request, struct _u_response * response, void * user_data) {
  u_map_put(response->map_header, "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
  u_map_put(response->map_header, "Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept, Authorization");
  u_map_put(response->map_header, "Access-Control-Max-Age", "1800");
  return U_CALLBACK_COMPLETE;
}

/**
 * Last endpoint called, clean response->shared_data
 */
int callback_clean (const struct _u_request * request, struct _u_response * response, void * user_data) {
  return U_CALLBACK_COMPLETE;
}
