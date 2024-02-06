/**
 *
 * Carleon House Automation side services
 *
 * Command house automation side services via an HTTP REST interface
 *
 * weathermap.org service module
 *
 * Copyright 2024 Nicolas Mora <mail@babelouest.org>
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

/** Macro to avoid compiler warning when some parameters are unused and that's ok **/
#define UNUSED(x) (void)(x)

#include <string.h>
#include <jansson.h>
#include <ulfius.h>
#include <hoel.h>
#include <orcania.h>
#include <yder.h>
#include "../carleon.h"

#define WEATHERMAP_TABLE_NAME      "c_service_weathermap"
#define WEATHERMAP_DATA_TABLE_NAME "c_service_weathermap_data"

static int weathermap_refresh_data(struct _carleon_config * config, json_t * j_weathermap) {
  struct _u_request req;
  struct _u_response resp;
  json_t * j_query, * j_values;
  int res, ret;
  char * str_values = NULL;

  ulfius_init_request(&req);
  ulfius_init_response(&resp);
  
  ulfius_set_request_properties(&req, U_OPT_HTTP_VERB, "GET",
                                      U_OPT_HTTP_URL, json_string_value(json_object_get(j_weathermap, "cw_uri")),
                                      U_OPT_URL_PARAMETER, "appid", json_string_value(json_object_get(j_weathermap, "cw_appid")),
                                      U_OPT_URL_PARAMETER, "lat", json_string_value(json_object_get(j_weathermap, "cw_lat")),
                                      U_OPT_URL_PARAMETER, "lon", json_string_value(json_object_get(j_weathermap, "cw_lon")),
                                      U_OPT_TIMEOUT, 10,
                                      U_OPT_NONE);
  if (!o_strnullempty(json_string_value(json_object_get(j_weathermap, "cw_units")))) {
    ulfius_set_request_properties(&req, U_OPT_URL_PARAMETER, "units", json_string_value(json_object_get(j_weathermap, "cw_units")), U_OPT_NONE);
  }
  if (!o_strnullempty(json_string_value(json_object_get(j_weathermap, "cw_lang")))) {
    ulfius_set_request_properties(&req, U_OPT_URL_PARAMETER, "lang", json_string_value(json_object_get(j_weathermap, "cw_lang")), U_OPT_NONE);
  }
  if (ulfius_send_http_request_with_limit(&req, &resp, 4096, 4) == U_OK) {
    if (resp.status == 200) {
      if ((j_values = ulfius_get_json_body_response(&resp, NULL)) != NULL) {
        if ((str_values = json_dumps(j_values, JSON_COMPACT)) != NULL) {
          j_query = json_pack("{sss{sOsiss}}",
                              "table", WEATHERMAP_DATA_TABLE_NAME,
                              "values",
                                "cw_id", json_object_get(j_weathermap, "cw_id"),
                                "cwd_status", 0,
                                "cwd_data", str_values);
          res = h_insert(config->conn, j_query, NULL);
          json_decref(j_query);
          if (res != H_OK) {
            y_log_message(Y_LOG_LEVEL_ERROR, "weathermap_refresh_data - Error executing j_query (5)");
            ret = C_ERROR_DB;
          } else {
            ret = C_OK;
          }
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "weathermap_refresh_data - Error dumping data for weathermap %s", json_string_value(json_object_get(j_weathermap, "cw_name")));
          j_query = json_pack("{sss{sOsi}}",
                              "table", WEATHERMAP_DATA_TABLE_NAME,
                              "values",
                                "cw_id", json_object_get(j_weathermap, "cw_id"),
                                "cwd_status", 2);
          res = h_insert(config->conn, j_query, NULL);
          json_decref(j_query);
          if (res != H_OK) {
            y_log_message(Y_LOG_LEVEL_ERROR, "weathermap_refresh_data - Error executing j_query (4)");
            ret = C_ERROR_DB;
          } else {
            ret = C_ERROR_PARAM;
          }
        }
        o_free(str_values);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "weathermap_refresh_data - Error parsing data for weathermap %s", json_string_value(json_object_get(j_weathermap, "cw_name")));
        j_query = json_pack("{sss{sOsi}}",
                            "table", WEATHERMAP_DATA_TABLE_NAME,
                            "values",
                              "cw_id", json_object_get(j_weathermap, "cw_id"),
                              "cwd_status", 2);
        res = h_insert(config->conn, j_query, NULL);
        json_decref(j_query);
        if (res != H_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "weathermap_refresh_data - Error executing j_query (3)");
          ret = C_ERROR_DB;
        } else {
          ret = C_ERROR_PARAM;
        }
      }
      json_decref(j_values);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "weathermap_refresh_data - Error http status %u for weathermap %s", resp.status, json_string_value(json_object_get(j_weathermap, "cw_name")));
      j_query = json_pack("{sss{sOsi}}",
                          "table", WEATHERMAP_DATA_TABLE_NAME,
                          "values",
                            "cw_id", json_object_get(j_weathermap, "cw_id"),
                            "cwd_status", 1);
      res = h_insert(config->conn, j_query, NULL);
      json_decref(j_query);
      if (res != H_OK) {
        y_log_message(Y_LOG_LEVEL_ERROR, "weathermap_refresh_data - Error executing j_query (2)");
        ret = C_ERROR_DB;
      } else {
        ret = C_ERROR_IO;
      }
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "weathermap_refresh_data - Error ulfius_send_http_request_with_limit");
    j_query = json_pack("{sss{sOsi}}",
                        "table", WEATHERMAP_DATA_TABLE_NAME,
                        "values",
                          "cw_id", json_object_get(j_weathermap, "cw_id"),
                          "cwd_status", 1);
    res = h_insert(config->conn, j_query, NULL);
    json_decref(j_query);
    if (res != H_OK) {
      y_log_message(Y_LOG_LEVEL_ERROR, "weathermap_refresh_data - Error executing j_query (1)");
      ret = C_ERROR_DB;
    } else {
      ret = C_ERROR_IO;
    }
  }
  ulfius_clean_request(&req);
  ulfius_clean_response(&resp);
  return ret;
}

static int check_result_value(json_t * result, const int value) {
  return (json_is_integer(json_object_get(result, "result")) &&
          json_integer_value(json_object_get(result, "result")) == value);
}

static json_t * get_weathermap_by_name(struct _carleon_config * config, const char * name) {
  json_t * j_query, * j_result, * j_return;
  int res;

  if (!o_strnullempty(name)) {
    j_query = json_pack("{sss[ssssssss]s{ss}}",
                        "table",
                        WEATHERMAP_TABLE_NAME,
                        "columns",
                          "cw_id",
                          "cw_name",
                          "cw_appid",
                          "cw_uri",
                          "cw_lat",
                          "cw_lon",
                          "cw_units",
                          "cw_lang",
                        "where",
                          "cw_name", name);
    res = h_select(config->conn, j_query, &j_result, NULL);
    json_decref(j_query);
    if (res == H_OK) {
      if (json_array_size(j_result) == 1) {
        j_return = json_pack("{sisO}", "result", C_OK, "weathermap", json_array_get(j_result, 0));
      } else {
        j_return = json_pack("{si}", "result", C_ERROR_NOT_FOUND);
      }
      json_decref(j_result);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "get_weathermap_by_name - Error executing j_query");
      j_return = json_pack("{si}", "result", C_ERROR_DB);
    }
  } else {
    j_return = json_pack("{si}", "result", C_ERROR_NOT_FOUND);
  }
  return j_return;
}

static json_t * get_weathermap_id(struct _carleon_config * config, const char * name) {
  json_t * j_query, * j_result, * j_return;
  int res;

  if (!o_strnullempty(name)) {
    j_query = json_pack("{sss[s]s{ss}}",
                        "table",
                        WEATHERMAP_TABLE_NAME,
                        "columns",
                          "cw_id",
                        "where",
                          "cw_name", name);
    res = h_select(config->conn, j_query, &j_result, NULL);
    json_decref(j_query);
    if (res == H_OK) {
      if (json_array_size(j_result) == 1) {
        j_return = json_pack("{sisO}", "result", C_OK, "cw_id", json_object_get(json_array_get(j_result, 0), "cw_id"));
      } else {
        j_return = json_pack("{si}", "result", C_ERROR_NOT_FOUND);
      }
      json_decref(j_result);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "get_weathermap_id - Error executing j_query");
      j_return = json_pack("{si}", "result", C_ERROR_DB);
    }
  } else {
    j_return = json_pack("{si}", "result", C_ERROR_NOT_FOUND);
  }
  return j_return;
}

static json_t * is_weathermap_valid(json_t * j_data, int add) {
  json_t * j_return, * j_error = json_array();

  if (j_error != NULL) {
    if (j_data != NULL && json_is_object(j_data)) {
      if (add) {
        if (!json_is_string(json_object_get(j_data, "name")) || !json_string_length(json_object_get(j_data, "name")) || json_string_length(json_object_get(j_data, "name")) > 64) {
          json_array_append_new(j_error, json_string("name is mandatory and must be a non empty string up to 64 characters"));
        }
      }
      if (json_object_get(j_data, "display") != NULL && (!json_is_string(json_object_get(j_data, "display")) || !json_string_length(json_object_get(j_data, "display")) || json_string_length(json_object_get(j_data, "display")) > 128)) {
        json_array_append_new(j_error, json_string("display is optionnal and must be a non empty string up to 128 characters"));
      }
      if (!json_is_string(json_object_get(j_data, "appid")) || !json_string_length(json_object_get(j_data, "appid")) || json_string_length(json_object_get(j_data, "appid")) > 128) {
        json_array_append_new(j_error, json_string("appid is mandatory and must be a non empty string up to 128 characters"));
      }
      if (!json_is_string(json_object_get(j_data, "uri")) || !json_string_length(json_object_get(j_data, "uri")) || json_string_length(json_object_get(j_data, "uri")) > 128) {
        json_array_append_new(j_error, json_string("uri is mandatory and must be a non empty string up to 128 characters"));
      }
      if (json_object_get(j_data, "lat") != NULL && (!json_is_string(json_object_get(j_data, "lat")) || !json_string_length(json_object_get(j_data, "lat")) || json_string_length(json_object_get(j_data, "lat")) > 16)) {
        json_array_append_new(j_error, json_string("lat is optionnal and must be a non empty string up to 16 characters"));
      }
      if (json_object_get(j_data, "lon") != NULL && (!json_is_string(json_object_get(j_data, "lon")) || !json_string_length(json_object_get(j_data, "lon")) || json_string_length(json_object_get(j_data, "lon")) > 16)) {
        json_array_append_new(j_error, json_string("lon is optionnal and must be a non empty string up to 16 characters"));
      }
      if (json_object_get(j_data, "units") != NULL && (!json_is_string(json_object_get(j_data, "units")) || !json_string_length(json_object_get(j_data, "units")) || json_string_length(json_object_get(j_data, "units")) > 16)) {
        json_array_append_new(j_error, json_string("units is optionnal and must be a non empty string up to 16 characters"));
      }
      if (json_object_get(j_data, "lang") != NULL && (!json_is_string(json_object_get(j_data, "lang")) || !json_string_length(json_object_get(j_data, "lang")) || json_string_length(json_object_get(j_data, "lang")) > 16)) {
        json_array_append_new(j_error, json_string("lang is optionnal and must be a non empty string up to 16 characters"));
      }
      if (json_array_size(j_error)) {
        j_return = json_pack("{sisO}", "result", C_ERROR_PARAM, "error", j_error);
      } else {
        j_return = json_pack("{si}", "result", C_OK);
      }
    } else {
      j_return = json_pack("{sis[s]}", "result", C_ERROR_PARAM, "error", "Invalid JSON parameter, must be a JSON object");
    }
    json_decref(j_error);
  } else {
    j_return = json_pack("{sis[s]}", "result", C_ERROR, "error", "Internal error");
  }
  return j_return;
}

static json_t * get_weathermap(struct _carleon_config * config, const char * name) {
  json_t * j_query, * j_result, * j_return;
  int res;

  j_query = json_pack("{sss[sssssss]}",
                      "table",
                      WEATHERMAP_TABLE_NAME,
                      "columns",
                        "cw_name AS name",
                        "cw_appid AS appid",
                        "cw_uri AS uri",
                        "cw_lat AS lat",
                        "cw_lon AS lon",
                        "cw_units AS units",
                        "cw_lang AS lang");
  if (!o_strnullempty(name)) {
    json_object_set_new(j_query, "where", json_pack("{ss}", "cw_name", name));
  }
  res = h_select(config->conn, j_query, &j_result, NULL);
  json_decref(j_query);
  if (res == H_OK) {
    if (!o_strnullempty(name) && !json_array_size(j_result)) {
      j_return = json_pack("{si}", "result", C_ERROR_NOT_FOUND);
    } else {
      j_return = json_pack("{sisO}", "result", C_OK, "weathermap", j_result);
    }
    json_decref(j_result);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "get_weathermap - Error executing j_query");
    j_return = json_pack("{si}", "result", C_ERROR_DB);
  }
  return j_return;
}

static int callback_weathermap_get (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct _carleon_config * config = (struct _carleon_config *)user_data;
  json_t * j_result = get_weathermap(config, u_map_get(request->map_url, "name"));

  if (check_result_value(j_result, C_OK)) {
    ulfius_set_json_body_response(response, 200, json_object_get(j_result, "weathermap"));
  } else if (check_result_value(j_result, C_ERROR_NOT_FOUND)) {
    response->status = 404;
  } else {
    response->status = 500;
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_weathermap_get - Error get_weathermap");
  }
  json_decref(j_result);
  return U_CALLBACK_CONTINUE;
}

static int callback_weathermap_add (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct _carleon_config * config = (struct _carleon_config *)user_data;
  json_t * j_query, * j_data = ulfius_get_json_body_request(request, NULL), * j_result, * j_weathermap_id = NULL, * j_tmp;
  int res;

  j_result = is_weathermap_valid(j_data, 1);
  if (check_result_value(j_result, C_OK)) {
    j_weathermap_id = get_weathermap_id(config, json_string_value(json_object_get(j_data, "name")));
    if (check_result_value(j_weathermap_id, C_ERROR_NOT_FOUND)) {
      j_query = json_pack("{sss{sOsO*sOsOsO*sO*sO*sO*}}",
                          "table",
                          WEATHERMAP_TABLE_NAME,
                          "values",
                            "cw_name", json_object_get(j_data, "name"),
                            "cw_display", json_object_get(j_data, "display"),
                            "cw_appid", json_object_get(j_data, "appid"),
                            "cw_uri", json_object_get(j_data, "uri"),
                            "cw_lat", json_object_get(j_data, "lat"),
                            "cw_lon", json_object_get(j_data, "lon"),
                            "cw_units", json_object_get(j_data, "units"),
                            "cw_lang", json_object_get(j_data, "lang"));
      res = h_insert(config->conn, j_query, NULL);
      json_decref(j_query);
      if (res != H_OK) {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_weathermap_add - Error executing j_query");
        response->status = 500;
      }
    } else if (check_result_value(j_weathermap_id, C_OK)) {
      j_tmp = json_pack("[s]", "name already exist");
      ulfius_set_json_body_response(response, 400, j_tmp);
      json_decref(j_tmp);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_weathermap_add - Error get_weathermap_id");
      response->status = 500;
    }
    json_decref(j_weathermap_id);
  } else if (check_result_value(j_result, C_ERROR_PARAM)) {
    ulfius_set_json_body_response(response, 400, json_object_get(j_result, "error"));
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_weathermap_add - Error is_weathermap_valid");
    response->status = 500;
  }
  json_decref(j_result);
  json_decref(j_data);
  return U_CALLBACK_CONTINUE;
}

static int callback_weathermap_set (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct _carleon_config * config = (struct _carleon_config *)user_data;
  json_t * j_query, * j_data = ulfius_get_json_body_request(request, NULL), * j_result, * j_weathermap_id = NULL;
  int res;

  j_weathermap_id = get_weathermap_id(config, u_map_get(request->map_url, "name"));
  if (check_result_value(j_weathermap_id, C_OK)) {
    j_result = is_weathermap_valid(j_data, 0);
    if (check_result_value(j_result, C_OK)) {
        j_query = json_pack("{sss{sOsO*sOsOsO*sO*sO*sO*}s{sO}}",
                            "table",
                            WEATHERMAP_TABLE_NAME,
                            "set",
                              "cw_name", json_object_get(j_data, "name"),
                              "cw_display", json_object_get(j_data, "display"),
                              "cw_appid", json_object_get(j_data, "appid"),
                              "cw_uri", json_object_get(j_data, "uri"),
                              "cw_lat", json_object_get(j_data, "lat"),
                              "cw_lon", json_object_get(j_data, "lon"),
                              "cw_units", json_object_get(j_data, "units"),
                              "cw_lang", json_object_get(j_data, "lang"),
                            "where",
                              "cw_id", json_object_get(j_weathermap_id, "cw_id"));
        res = h_update(config->conn, j_query, NULL);
        json_decref(j_query);
        if (res != H_OK) {
          y_log_message(Y_LOG_LEVEL_ERROR, "callback_weathermap_set - Error executing j_query");
          response->status = 500;
        }
    } else if (check_result_value(j_result, C_ERROR_PARAM)) {
      ulfius_set_json_body_response(response, 400, json_object_get(j_result, "error"));
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_weathermap_set - Error is_weathermap_valid");
      response->status = 500;
    }
    json_decref(j_result);
  } else if (check_result_value(j_weathermap_id, C_ERROR_NOT_FOUND)) {
    response->status = 404;
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_weathermap_set - Error get_weathermap_id");
    response->status = 500;
  }
  json_decref(j_weathermap_id);
  json_decref(j_data);
  return U_CALLBACK_CONTINUE;
}

static int callback_weathermap_remove (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct _carleon_config * config = (struct _carleon_config *)user_data;
  json_t * j_query, * j_weathermap_id = NULL;
  int res;

  j_weathermap_id = get_weathermap_id(config, u_map_get(request->map_url, "name"));
  if (check_result_value(j_weathermap_id, C_OK)) {
    j_query = json_pack("{sss{sO}}",
                        "table",
                        WEATHERMAP_TABLE_NAME,
                        "where",
                          "cw_id", json_object_get(j_weathermap_id, "cw_id"));
    res = h_delete(config->conn, j_query, NULL);
    json_decref(j_query);
    if (res != H_OK) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_weathermap_remove - Error executing j_query");
      response->status = 500;
    }
  } else if (check_result_value(j_weathermap_id, C_ERROR_NOT_FOUND)) {
    response->status = 404;
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_weathermap_remove - Error get_weathermap_id");
    response->status = 500;
  }
  json_decref(j_weathermap_id);
  return U_CALLBACK_CONTINUE;
}

static int callback_weathermap_get_data (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct _carleon_config * config = (struct _carleon_config *)user_data;
  json_t * j_query, * j_result, * j_weathermap_id = NULL, * j_values = NULL, * j_tmp;
  int res;

  j_weathermap_id = get_weathermap_id(config, u_map_get(request->map_url, "name"));
  if (check_result_value(j_weathermap_id, C_OK)) {
    j_query = json_pack("{sss[ss]s{sOsi}sssi}",
                        "table",
                        WEATHERMAP_DATA_TABLE_NAME,
                        "columns",
                          "cwd_data",
                          "UNIX_TIMESTAMP(cwd_date) AS date",
                        "where",
                          "cw_id", json_object_get(j_weathermap_id, "cw_id"),
                          "cwd_status", 0,
                          "order_by", "cwd_date DESC",
                          "limit", 1);
    res = h_select(config->conn, j_query, &j_result, NULL);
    json_decref(j_query);
    if (res == H_OK) {
      if (json_array_size(j_result)) {
        if ((j_values = json_loads(json_string_value(json_object_get(json_array_get(j_result, 0), "cwd_data")), JSON_DECODE_ANY, NULL)) != NULL) {
          j_tmp = json_pack("{sOsO}", "date", json_object_get(json_array_get(j_result, 0), "date"), "data", j_values);
          ulfius_set_json_body_response(response, 200, j_tmp);
          json_decref(j_tmp);
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "callback_weathermap_get_data - Error parsing data");
          response->status = 500;
        }
        json_decref(j_values);
      } else {
        response->status = 204;
      }
      json_decref(j_result);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_weathermap_get_data - Error executing j_query");
      response->status = 500;
    }
  } else if (check_result_value(j_weathermap_id, C_ERROR_NOT_FOUND)) {
    response->status = 404;
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_weathermap_get_data - Error get_weathermap_id");
    response->status = 500;
  }
  json_decref(j_weathermap_id);
  return U_CALLBACK_CONTINUE;
}

static int callback_weathermap_refresh_data (const struct _u_request * request, struct _u_response * response, void * user_data) {
  struct _carleon_config * config = (struct _carleon_config *)user_data;
  json_t * j_weathermap = NULL;

  j_weathermap = get_weathermap_by_name(config, u_map_get(request->map_url, "name"));
  if (check_result_value(j_weathermap, C_OK)) {
    if (weathermap_refresh_data(config, json_object_get(j_weathermap, "weathermap")) != C_OK) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_weathermap_refresh_data - Error weathermap_refresh_data");
      response->status = 500;
    }
  } else if (check_result_value(j_weathermap, C_ERROR_NOT_FOUND)) {
    response->status = 404;
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_weathermap_refresh_data - Error get_weathermap_by_name");
    response->status = 500;
  }
  json_decref(j_weathermap);
  return U_CALLBACK_CONTINUE;
}

json_t * c_service_init(struct _carleon_config * config) {
  if (config != NULL) {
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix, "/service-weathermap/@name", 2, &callback_weathermap_get, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix, "/service-weathermap/", 2, &callback_weathermap_get, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "POST", config->url_prefix, "/service-weathermap/", 2, &callback_weathermap_add, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "PUT", config->url_prefix, "/service-weathermap/@name", 2, &callback_weathermap_set, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "DELETE", config->url_prefix, "/service-weathermap/@name", 2, &callback_weathermap_remove, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix, "/service-weathermap/@name/data", 2, &callback_weathermap_get_data, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "PUT", config->url_prefix, "/service-weathermap/@name/data", 2, &callback_weathermap_refresh_data, (void*)config);
    return json_pack("{sissss}", 
                      "result", WEBSERVICE_RESULT_OK,
                      "name", "service-weathermap",
                      "description", "Weathermap.org service for Carleon");
  } else {
    return json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  }
}

json_t * c_service_close(struct _carleon_config * config) {
  if (config != NULL) {
    ulfius_remove_endpoint_by_val(config->instance, "GET", config->url_prefix, "/service-weathermap/@name");
    ulfius_remove_endpoint_by_val(config->instance, "GET", config->url_prefix, "/service-weathermap/");
    ulfius_remove_endpoint_by_val(config->instance, "POST", config->url_prefix, "/service-weathermap/");
    ulfius_remove_endpoint_by_val(config->instance, "PUT", config->url_prefix, "/service-weathermap/@name");
    ulfius_remove_endpoint_by_val(config->instance, "DELETE", config->url_prefix, "/service-weathermap/@name");
    ulfius_remove_endpoint_by_val(config->instance, "GET", config->url_prefix, "/service-weathermap/@name/data");
    ulfius_remove_endpoint_by_val(config->instance, "PUT", config->url_prefix, "/service-weathermap/@name/data");
    return json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
  } else {
    return json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  }
}

json_t * c_service_command_get_list(struct _carleon_config * config) {
  UNUSED(config);
  return json_pack("{sis{s{}}}", "result", WEBSERVICE_RESULT_OK, "commands", "refresh");
}

json_t * c_service_element_get_list(struct _carleon_config * config) {
  json_t * j_result = get_weathermap(config, NULL), * j_return;
  
  if (check_result_value(j_result, C_OK)) {
    j_return = json_pack("{sisO}", "result", WEBSERVICE_RESULT_OK, "element", json_object_get(j_result, "weathermap"));
  } else {
    j_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  }
  json_decref(j_result);
  return j_return;
}

json_t * c_service_exec(struct _carleon_config * config, const char * command, const char * element, json_t * parameters) {
  UNUSED(parameters);
  json_t * j_weathermap = get_weathermap_by_name(config, element), * j_return;
  
  if (0 == o_strcmp(command, "refresh")) {
    if (check_result_value(j_weathermap, C_OK)) {
      if (weathermap_refresh_data(config, json_object_get(j_weathermap, "weathermap")) == C_OK) {
        j_return = json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "c_service_exec - Error weathermap_refresh_data");
        j_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "c_service_exec - Error get_weathermap_by_name");
      j_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
    }
  } else {
    j_return = json_pack("{si}", "result", WEBSERVICE_RESULT_PARAM);
  }
  return j_return;
}
