/**
 *
 * Carleon House Automation side services
 *
 * Command house automation side services via an HTTP REST interface
 *
 * MPD service module
 * Provides a controller to a MPD application
 * - get the current status
 * - play/pause/stop
 * - add and play a song
 * - get available playlists
 * - load and play a specified playlist
 * - set the volume
 *
 * library mpdclient is required to install
 * sudo apt-get install libmpdclient-dev
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
#define _HOEL_SQLITE
#define _HOEL_MARIADB
#include <hoel.h>
#include <orcania.h>
#include <yder.h>
#include "../carleon.h"

#include <mpd/client.h>
#include <mpd/status.h>
#include <mpd/player.h>
#include <mpd/playlist.h>
#include <mpd/mixer.h>

#define MPD_TABLE_NAME "c_service_mpd"
#define MPD_WEBSOCKET_TABLE_NAME "c_mpd_websocket"
#define MPD_CONNECT_TIMEOUT 10000
#define MPD_WEBSOCKET_NAME_SIZE 32

int set_response_json_body_and_clean(struct _u_response * response, uint status, json_t * json_body) {
  int res = ulfius_set_json_body_response(response, status, json_body);
  json_decref(json_body);
  return res;
}

/**
 * get the specified mpd or all mpd
 */
json_t * mpd_get(struct _carleon_config * config, const char * mpd_name) {
  json_t * j_query = json_pack("{sss[ssssss]}", 
                    "table", 
                      MPD_TABLE_NAME,
                    "columns",
                      "cmpd_name AS name",
                      "cmpd_description AS description",
                      "cmpd_host AS host",
                      "cmpd_port AS port",
                      "cmpd_device AS device",
                      "cmpd_switch AS switch"
                    ), * j_result, * to_return;
  int res;
  
  if (j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "mpd_get - Error allocating resources for j_query");
    return json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  }
  
  if (mpd_name != NULL) {
    json_object_set_new(j_query, "where", json_pack("{ss}", "cmpd_name", mpd_name));
  }
  
  res = h_select(config->conn, j_query, &j_result, NULL);
  json_decref(j_query);
  if (res == H_OK) {
    if (mpd_name == NULL) {
      to_return = json_pack("{sisO}", "result", WEBSERVICE_RESULT_OK, "element", j_result);
    } else {
      if (json_array_size(j_result) == 0) {
        to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_NOT_FOUND);
      } else {
        to_return = json_pack("{sisO}", "result", WEBSERVICE_RESULT_OK, "element", json_array_get(j_result, 0));
      }
    }
    json_decref(j_result);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "mpd_get - Error executing j_query");
    return json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  }
  return to_return;
}

/**
 * check the specified mpd has valid parameters
 */
json_t * is_mpd_valid(struct _carleon_config * config, json_t * mpd, int add) {
  json_t * to_return = json_array(), * j_result, * element;
  size_t index;
  
  if (to_return == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "is_mpd_valid - Error allocating resources for to_return");
    return NULL;
  }
  
  if (mpd == NULL || !json_is_object(mpd)) {
    json_array_append_new(to_return, json_pack("{ss}", "mpd", "mpd must be a json object"));
  } else {
    if (add) {
			if (json_object_get(mpd, "name") == NULL || !json_is_string(json_object_get(mpd, "name")) || json_string_length(json_object_get(mpd, "name")) > 64) {
				json_array_append_new(to_return, json_pack("{ss}", "name", "name is mandatory and must be a string up to 64 characters"));
			} else {
				j_result = mpd_get(config, NULL);
				json_array_foreach(json_object_get(j_result, "element"), index, element) {
					if (0 == o_strcmp(json_string_value(json_object_get(element, "name")), json_string_value(json_object_get(mpd, "name")))) {
						json_array_append_new(to_return, json_pack("{ss}", "name", "a mpd service with the same name already exist"));
					}
				}
				json_decref(j_result);
			}
    }
    
    if (json_object_get(mpd, "description") != NULL && (!json_is_string(json_object_get(mpd, "description")) || json_string_length(json_object_get(mpd, "description")) > 128)) {
      json_array_append_new(to_return, json_pack("{ss}", "description", "description must be a string up to 128 characters"));
    }
    
    if (json_object_get(mpd, "host") == NULL || !json_is_string(json_object_get(mpd, "host")) || json_string_length(json_object_get(mpd, "host")) > 128) {
      json_array_append_new(to_return, json_pack("{ss}", "host", "host is mandatory and must be a string up to 128 characters"));
    }
    
    if (json_object_get(mpd, "port") != NULL && (!json_is_integer(json_object_get(mpd, "port")) || json_integer_value(json_object_get(mpd, "port")) < 0 || json_integer_value(json_object_get(mpd, "port")) > 65535)) {
      json_array_append_new(to_return, json_pack("{ss}", "port", "port must be an integer between 0 and 65535"));
    }
    
    if (json_object_get(mpd, "password") != NULL && (!json_is_string(json_object_get(mpd, "password")) || json_string_length(json_object_get(mpd, "password")) > 128)) {
      json_array_append_new(to_return, json_pack("{ss}", "password", "password must be a string up to 128 characters"));
    }
    
    if (json_object_get(mpd, "device") != NULL && (!json_is_string(json_object_get(mpd, "device")) || json_string_length(json_object_get(mpd, "device")) > 64)) {
      json_array_append_new(to_return, json_pack("{ss}", "device", "device must be a string up to 64 characters"));
    }
    
    if (json_object_get(mpd, "switch") != NULL && (!json_is_string(json_object_get(mpd, "switch")) || json_string_length(json_object_get(mpd, "switch")) > 64)) {
      json_array_append_new(to_return, json_pack("{ss}", "switch", "switch must be a string up to 64 characters"));
    }
    
  }
  
  return to_return;
}

/**
 * add the specified mpd
 */
json_t * mpd_add(struct _carleon_config * config, json_t * mpd) {
  json_t * j_query = json_pack("{sss{sssssssIsssOsO}}",
                      "table",
                        MPD_TABLE_NAME,
                      "values",
                        "cmpd_name", json_string_value(json_object_get(mpd, "name")),
                        "cmpd_description", json_object_get(mpd, "description")!=NULL?json_string_value(json_object_get(mpd, "description")):"",
                        "cmpd_host", json_string_value(json_object_get(mpd, "host")),
                        "cmpd_port", json_object_get(mpd, "port")!=NULL?json_integer_value(json_object_get(mpd, "port")):0,
                        "cmpd_password", json_object_get(mpd, "password")!=NULL?json_string_value(json_object_get(mpd, "password")):"",
                        "cmpd_device", json_object_get(mpd, "device")!=NULL?json_object_get(mpd, "device"):json_null(),
                        "cmpd_switch", json_object_get(mpd, "switch")!=NULL?json_object_get(mpd, "switch"):json_null()),
          * j_reasons, * j_return;
  int res;
  
  j_reasons = is_mpd_valid(config, mpd, 1);
  if (j_reasons != NULL && json_array_size(j_reasons) == 0) {
    res = h_insert(config->conn, j_query, NULL);
    json_decref(j_query);
    if (res == H_OK) {
      j_return = json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
    } else {
      j_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
    }
  } else if (j_reasons != NULL && json_array_size(j_reasons) > 0) {
    j_return = json_pack("{sisO}", "result", WEBSERVICE_RESULT_PARAM, "reason", j_reasons);
  } else {
    j_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  }
  json_decref(j_reasons);
  return j_return;
}

/**
 * update the specified mpd
 */
json_t * mpd_set(struct _carleon_config * config, const char * name, json_t * mpd) {
  json_t * j_query = json_pack("{sss{sssssIsssOsO}s{ss}}",
                      "table",
                        MPD_TABLE_NAME,
                      "set",
                        "cmpd_description", json_object_get(mpd, "description")!=NULL?json_string_value(json_object_get(mpd, "description")):"",
                        "cmpd_host", json_string_value(json_object_get(mpd, "host")),
                        "cmpd_port", json_object_get(mpd, "port")!=NULL?json_integer_value(json_object_get(mpd, "port")):0,
                        "cmpd_password", json_object_get(mpd, "password")!=NULL?json_string_value(json_object_get(mpd, "password")):"",
                        "cmpd_device", json_object_get(mpd, "device")!=NULL?json_object_get(mpd, "device"):json_null(),
                        "cmpd_switch", json_object_get(mpd, "switch")!=NULL?json_object_get(mpd, "switch"):json_null(),
                      "where",
                        "cmpd_name", name),
          * j_reasons, * j_return;
  int res;
  
  j_reasons = is_mpd_valid(config, mpd, 0);
  if (j_reasons != NULL && json_array_size(j_reasons) == 0) {
    res = h_update(config->conn, j_query, NULL);
    json_decref(j_query);
    if (res == H_OK) {
      j_return =  json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
    } else {
      j_return =  json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
    }
  } else if (j_reasons != NULL && json_array_size(j_reasons) > 0) {
    j_return =  json_pack("{sisO}", "result", WEBSERVICE_RESULT_PARAM, "reason", j_reasons);
  } else {
    j_return =  json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  }
  json_decref(j_reasons);
  return j_return;
}

/**
 * remove the specified mpd
 */
json_t * mpd_remove(struct _carleon_config * config, const char * name) {
  json_t * j_query = json_pack("{sss{ss}}",
                      "table",
                        MPD_TABLE_NAME,
                      "where",
                        "cmpd_name", name);
  int res;
  
  res = h_delete(config->conn, j_query, NULL);
  json_decref(j_query);
  
  if (res == H_OK) {
    return json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
  } else {
    return json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  }
}

/**
 * Callback function
 * get the specified mpd or all mpd
 */
int callback_service_mpd_get (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * j_service_mpd;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_get - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    j_service_mpd = mpd_get((struct _carleon_config *)user_data, u_map_get(request->map_url, "name"));
    if (j_service_mpd != NULL && json_integer_value(json_object_get(j_service_mpd, "result")) == WEBSERVICE_RESULT_OK) {
			ulfius_set_json_body_response(response, 200, json_object_get(j_service_mpd, "element"));
    } else if (j_service_mpd != NULL && json_integer_value(json_object_get(j_service_mpd, "result")) == WEBSERVICE_RESULT_NOT_FOUND) {
      response->status = 404;
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_get - Error get_service_motion_list");
      response->status = 500;
    }
    json_decref(j_service_mpd);
  }
  return U_CALLBACK_CONTINUE;
}

/**
 * Callback function
 * add the specified mpd
 */
int callback_service_mpd_add (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * result, * json_body = ulfius_get_json_body_request(request, NULL);
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_add - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    result = mpd_add((struct _carleon_config *)user_data, json_body);
    if (result != NULL && json_integer_value(json_object_get(result, "result")) == WEBSERVICE_RESULT_PARAM) {
      ulfius_set_json_body_response(response, 400, json_object_get(result, "reason"));
    } else if (result == NULL || json_integer_value(json_object_get(result, "result")) != WEBSERVICE_RESULT_OK) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_add - Error in mpd_add");
      response->status = 500;
    }
    json_decref(result);
  }
  json_decref(json_body);
  return U_CALLBACK_CONTINUE;
}

/**
 * Callback function
 * update the specified mpd
 */
int callback_service_mpd_set (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * result, * mpd, * json_body = ulfius_get_json_body_request(request, NULL);
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_set - Error, user_data is NULL");
    return U_ERROR_PARAMS;
  } else {
    mpd = mpd_get((struct _carleon_config *)user_data, u_map_get(request->map_url, "name"));
    if (mpd != NULL && json_integer_value(json_object_get(mpd, "result")) == WEBSERVICE_RESULT_NOT_FOUND) {
      response->status = 404;
    } else if (mpd != NULL && json_integer_value(json_object_get(mpd, "result")) == WEBSERVICE_RESULT_OK) {
      result = mpd_set((struct _carleon_config *)user_data, u_map_get(request->map_url, "name"), json_body);
			if (result != NULL && json_integer_value(json_object_get(result, "result")) == WEBSERVICE_RESULT_PARAM) {
				ulfius_set_json_body_response(response, 400, json_object_get(result, "reason"));
			} else if (result == NULL || json_integer_value(json_object_get(result, "result")) != WEBSERVICE_RESULT_OK) {
				y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_add - Error in mpd_add");
				response->status = 500;
			}
      json_decref(result);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_set - Error in mpd_get");
      response->status = 500;
    }
    json_decref(mpd);
  }
  json_decref(json_body);
  return U_CALLBACK_CONTINUE;
}

/**
 * Callback function
 * remove the specified mpd
 */
int callback_service_mpd_remove (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * result, * mpd;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_remove - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    mpd = mpd_get((struct _carleon_config *)user_data, u_map_get(request->map_url, "name"));
    if (mpd != NULL && json_integer_value(json_object_get(mpd, "result")) == WEBSERVICE_RESULT_NOT_FOUND) {
      response->status = 404;
    } else if (mpd != NULL && json_integer_value(json_object_get(mpd, "result")) == WEBSERVICE_RESULT_OK) {
      result = mpd_remove((struct _carleon_config *)user_data, u_map_get(request->map_url, "name"));
      if (result == NULL) {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_remove - Error in mpd_remove");
        response->status = 500;
      } else if (json_integer_value(json_object_get(result, "result")) == WEBSERVICE_RESULT_PARAM) {
				ulfius_set_json_body_response(response, 200, json_object_get(result, "reason"));
      }
      json_decref(result);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_remove - Error in mpd_get");
      response->status = 500;
    }
    json_decref(mpd);
  }
  return U_CALLBACK_CONTINUE;
}

/**
 * get the status the specified mpd
 */
json_t * mpd_get_status(struct _carleon_config * config, json_t * mpd) {
  struct mpd_connection * conn;
  json_t * to_return;
  struct mpd_status * status;
  struct mpd_song * song;
  int auth = 1;

  UNUSED(config);
  conn = mpd_connection_new(json_string_value(json_object_get(mpd, "host")), json_integer_value(json_object_get(mpd, "port")), MPD_CONNECT_TIMEOUT);
  
  if (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS) {
    y_log_message(Y_LOG_LEVEL_ERROR, "mpd_get_status - Error mpd_connection_new, message is %s", mpd_connection_get_error_message(conn));
    to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  } else {
    if (json_object_get(mpd, "password") != NULL && json_is_string(json_object_get(mpd, "password")) && json_string_length(json_object_get(mpd, "password")) > 0) {
      if (!mpd_run_password(conn, json_string_value(json_object_get(mpd, "password")))) {
        auth = 0;
      }
    }
    if (auth) {
      mpd_command_list_begin(conn, true);
      mpd_send_status(conn);
      mpd_send_current_song(conn);
      mpd_command_list_end(conn);

      status = mpd_recv_status(conn);
      if (status == NULL) {
        y_log_message(Y_LOG_LEVEL_ERROR, "mpd_get_status - Error mpd_recv_status, message is %s", mpd_connection_get_error_message(conn));
        to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
      } else {
        char * state = "";
        
        switch (mpd_status_get_state(status)) {
          case MPD_STATE_PLAY:
            state = "play";
            break;
          case MPD_STATE_PAUSE:
            state = "pause";
            break;
          case MPD_STATE_STOP:
            state = "stop";
            break;
          default:
            state = "unknown";
            break;
        }
        to_return = json_pack("{sis{sssisisisisoso}}", 
                              "result", 
                                WEBSERVICE_RESULT_OK,
                              "status",
                                "state", state,
                                "volume", mpd_status_get_volume(status),
                                "elapsed_time", mpd_status_get_elapsed_time(status),
                                "total_time", mpd_status_get_total_time(status),
                                "song_pos", mpd_status_get_song_pos(status),
                                "random", mpd_status_get_random(status)==true?json_true():json_false(),
                                "repeat", mpd_status_get_repeat(status)==true?json_true():json_false());
        mpd_status_free(status);
        mpd_response_next(conn);
        
        song = mpd_recv_song(conn);
        if (song != NULL && to_return != NULL) {
          json_object_set_new(json_object_get(to_return, "status"), "title", json_string(mpd_song_get_tag(song, MPD_TAG_TITLE, 0)));
          json_object_set_new(json_object_get(to_return, "status"), "artist", json_string(mpd_song_get_tag(song, MPD_TAG_ARTIST, 0)));
          json_object_set_new(json_object_get(to_return, "status"), "album", json_string(mpd_song_get_tag(song, MPD_TAG_ALBUM, 0)));
          json_object_set_new(json_object_get(to_return, "status"), "date", json_string(mpd_song_get_tag(song, MPD_TAG_DATE, 0)));
          json_object_set_new(json_object_get(to_return, "status"), "name", json_string(mpd_song_get_tag(song, MPD_TAG_NAME, 0)));
          json_object_set_new(json_object_get(to_return, "status"), "track", json_string(mpd_song_get_tag(song, MPD_TAG_TRACK, 0)));
          mpd_song_free(song);
        }
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "mpd_get_status - Error mpd_run_password, message is %s", mpd_connection_get_error_message(conn));
      to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
    }
  }
  mpd_connection_free(conn);
  return to_return;
}

/**
 * Callback function
 * get the status the specified mpd
 */
int callback_service_mpd_status (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * result, * mpd;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_status - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    mpd = mpd_get((struct _carleon_config *)user_data, u_map_get(request->map_url, "name"));
    if (mpd == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_status - Error getting mpd");
      response->status = 500;
    } else {
      if (json_integer_value(json_object_get(mpd, "result")) == WEBSERVICE_RESULT_NOT_FOUND) {
        response->status = 404;
      } else if (json_integer_value(json_object_get(mpd, "result")) == WEBSERVICE_RESULT_OK) {
        result = mpd_get_status((struct _carleon_config *)user_data, json_object_get(mpd, "element"));
        if (result != NULL && json_integer_value(json_object_get(result, "result")) == WEBSERVICE_RESULT_OK) {
					ulfius_set_json_body_response(response, 200, json_object_get(result, "status"));
        } else {
          response->status = 500;
        }
        json_decref(result);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_status - Error getting mpd");
        response->status = 500;
      }
    }
    json_decref(mpd);
  }
  return U_CALLBACK_CONTINUE;
}

/**
 * Run an action on the specified mpd (play/pause/stop)
 */
json_t * mpd_set_action(struct _carleon_config * config, json_t * mpd, const char * action, const char * parameter) {
  struct mpd_connection *conn;
  json_t * to_return;
  int auth = 1;

  UNUSED(config);
  conn = mpd_connection_new(json_string_value(json_object_get(mpd, "host")), json_integer_value(json_object_get(mpd, "port")), MPD_CONNECT_TIMEOUT);
  
  if (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS) {
    y_log_message(Y_LOG_LEVEL_ERROR, "mpd_set_action - Error mpd_connection_new, message is %s", mpd_connection_get_error_message(conn));
    to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  } else {
    if (json_object_get(mpd, "password") != NULL && json_is_string(json_object_get(mpd, "password")) && json_string_length(json_object_get(mpd, "password")) > 0) {
      if (!mpd_run_password(conn, json_string_value(json_object_get(mpd, "password")))) {
        auth = 0;
      }
    }
    if (auth) {
      if (0 == o_strcmp(action, "stop")) {
        if (!mpd_run_stop(conn)) {
          to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
        } else {
          to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
        }
      } else if (0 == o_strcmp(action, "play")) {
        if (!mpd_run_play(conn)) {
          to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
        } else {
          to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
        }
      } else if (0 == o_strcmp(action, "pause")) {
        if (!mpd_run_pause(conn, true)) {
          to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
        } else {
          to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
        }
      } else if (0 == o_strcmp(action, "next")) {
        if (!mpd_run_next(conn)) {
          to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
        } else {
          to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
        }
      } else if (0 == o_strcmp(action, "previous")) {
        if (!mpd_run_previous(conn)) {
          to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
        } else {
          to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
        }
      } else if (0 == o_strcmp(action, "repeat")) {
        if (!mpd_run_repeat(conn, 0==o_strcmp(parameter, "1")?true:false)) {
          to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
        } else {
          to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
        }
      } else if (0 == o_strcmp(action, "random")) {
        if (!mpd_run_random(conn, 0==o_strcmp(parameter, "1")?true:false)) {
          to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
        } else {
          to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
        }
      } else {
        to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "mpd_set_action - Error mpd_run_password, message is %s", mpd_connection_get_error_message(conn));
      to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
    }
  }
  mpd_connection_free(conn);
  return to_return;
}

/**
 * Callback function
 * Run an action on the specified mpd (play/pause/stop)
 */
int callback_service_mpd_action (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * result, * mpd;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_action - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    mpd = mpd_get((struct _carleon_config *)user_data, u_map_get(request->map_url, "name"));
    if (mpd == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_action - Error getting mpd");
      response->status = 500;
    } else {
      if (json_integer_value(json_object_get(mpd, "result")) == WEBSERVICE_RESULT_NOT_FOUND) {
        response->status = 404;
      } else if (json_integer_value(json_object_get(mpd, "result")) == WEBSERVICE_RESULT_OK) {
        result = mpd_set_action((struct _carleon_config *)user_data, json_object_get(mpd, "element"), u_map_get(request->map_url, "action_name"), u_map_get(request->map_url, "action_param"));
        if (result != NULL && json_integer_value(json_object_get(result, "result")) == WEBSERVICE_RESULT_OK) {
					ulfius_set_json_body_response(response, 200, json_object_get(result, "status"));
        } else {
          response->status = 500;
        }
        json_decref(result);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_action - Error getting mpd");
        response->status = 500;
      }
    }
    json_decref(mpd);
  }
  return U_CALLBACK_CONTINUE;
}

/**
 * Clear the current playlist, add a song, then play it
 */
json_t * mpd_play_song(struct _carleon_config * config, json_t * mpd, const char * uri) {
  struct mpd_connection *conn;
  json_t * to_return;
  int auth = 1;

  UNUSED(config);
  conn = mpd_connection_new(json_string_value(json_object_get(mpd, "host")), json_integer_value(json_object_get(mpd, "port")), MPD_CONNECT_TIMEOUT);
  
  if (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS) {
    y_log_message(Y_LOG_LEVEL_ERROR, "mpd_play_song - Error mpd_connection_new, message is %s", mpd_connection_get_error_message(conn));
    to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  } else {
    if (json_object_get(mpd, "password") != NULL && json_is_string(json_object_get(mpd, "password")) && json_string_length(json_object_get(mpd, "password")) > 0) {
      if (!mpd_run_password(conn, json_string_value(json_object_get(mpd, "password")))) {
        auth = 0;
      }
    }
    if (auth) {
      if (mpd_run_clear(conn) && mpd_run_add(conn, uri) && mpd_run_play(conn)) {
        to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "mpd_play_song - Error play uri, message is %s", mpd_connection_get_error_message(conn));
        to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "mpd_play_song - Error mpd_run_password, message is %s", mpd_connection_get_error_message(conn));
      to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
    }
  }
  mpd_connection_free(conn);
  return to_return;
}

/**
 * Callback function
 * Clear the current playlist, add a song, then play it
 */
int callback_service_mpd_play_song (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * result, * mpd;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_play_song - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    mpd = mpd_get((struct _carleon_config *)user_data, u_map_get(request->map_url, "name"));
    if (mpd == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_play_song - Error getting mpd");
      response->status = 500;
    } else {
      if (json_integer_value(json_object_get(mpd, "result")) == WEBSERVICE_RESULT_NOT_FOUND) {
        response->status = 404;
      } else if (json_integer_value(json_object_get(mpd, "result")) == WEBSERVICE_RESULT_OK) {
        result = mpd_play_song((struct _carleon_config *)user_data, json_object_get(mpd, "element"), u_map_get(request->map_url, "uri"));
        if (result == NULL || json_integer_value(json_object_get(result, "result")) == WEBSERVICE_RESULT_ERROR) {
          response->status = 500;
        }
        json_decref(result);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_play_song - Error getting result");
        response->status = 500;
      }
    }
    json_decref(mpd);
  }
  return U_CALLBACK_CONTINUE;
}

/**
 * Play the song at the index pos in the queue
 */
json_t * mpd_play_pos(struct _carleon_config * config, json_t * mpd, unsigned int pos) {
  struct mpd_connection *conn;
  json_t * to_return;
  int auth = 1;

  UNUSED(config);
  conn = mpd_connection_new(json_string_value(json_object_get(mpd, "host")), json_integer_value(json_object_get(mpd, "port")), MPD_CONNECT_TIMEOUT);
  
  if (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS) {
    y_log_message(Y_LOG_LEVEL_ERROR, "mpd_play_pos - Error mpd_connection_new, message is %s", mpd_connection_get_error_message(conn));
    to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  } else {
    if (json_object_get(mpd, "password") != NULL && json_is_string(json_object_get(mpd, "password")) && json_string_length(json_object_get(mpd, "password")) > 0) {
      if (!mpd_run_password(conn, json_string_value(json_object_get(mpd, "password")))) {
        auth = 0;
      }
    }
    if (auth) {
      if (mpd_run_play_pos(conn, pos)) {
        to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "mpd_play_pos - Error play pos, message is %s", mpd_connection_get_error_message(conn));
        to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "mpd_play_pos - Error mpd_run_password, message is %s", mpd_connection_get_error_message(conn));
      to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
    }
  }
  mpd_connection_free(conn);
  return to_return;
}

/**
 * Callback function
 * Play the song at the index pos in the queue
 */
int callback_service_mpd_play_pos (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * result, * mpd;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_play_pos - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    mpd = mpd_get((struct _carleon_config *)user_data, u_map_get(request->map_url, "name"));
    if (mpd == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_play_pos - Error getting mpd");
      response->status = 500;
    } else {
      if (json_integer_value(json_object_get(mpd, "result")) == WEBSERVICE_RESULT_NOT_FOUND) {
        response->status = 404;
      } else if (json_integer_value(json_object_get(mpd, "result")) == WEBSERVICE_RESULT_OK) {
        result = mpd_play_pos((struct _carleon_config *)user_data, json_object_get(mpd, "element"), strtol(u_map_get(request->map_url, "pos"), NULL, 10));
        if (result == NULL || json_integer_value(json_object_get(result, "result")) == WEBSERVICE_RESULT_ERROR) {
          y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_play_pos - Error mpd_play_pos");
          response->status = 500;
        } else if (json_integer_value(json_object_get(result, "result")) == WEBSERVICE_RESULT_PARAM) {
          response->status = 400;
        }
        json_decref(result);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_play_pos - Error getting mpd");
        response->status = 500;
      }
    }
    json_decref(mpd);
  }
  return U_CALLBACK_CONTINUE;
}

/**
 * Get the current playlist song list
 */
json_t * mpd_playlist_get_list(struct _carleon_config * config, json_t * mpd) {
  struct mpd_connection * conn;
  struct mpd_status * status;
  struct mpd_song * song;
  json_t * to_return, * j_song_array;
  int auth = 1;
  unsigned int queue_size, pos;

  UNUSED(config);
  conn = mpd_connection_new(json_string_value(json_object_get(mpd, "host")), json_integer_value(json_object_get(mpd, "port")), MPD_CONNECT_TIMEOUT);
  
  if (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS) {
    y_log_message(Y_LOG_LEVEL_ERROR, "mpd_playlist_get_list - Error mpd_connection_new, message is %s", mpd_connection_get_error_message(conn));
    to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  } else {
    if (json_object_get(mpd, "password") != NULL && json_is_string(json_object_get(mpd, "password")) && json_string_length(json_object_get(mpd, "password")) > 0) {
      if (!mpd_run_password(conn, json_string_value(json_object_get(mpd, "password")))) {
        auth = 0;
      }
    }
    if (auth) {
      j_song_array = json_array();
      if (j_song_array != NULL) {
        status = mpd_run_status(conn);
        queue_size = mpd_status_get_queue_length(status);
        
        for (pos=0; pos<queue_size; pos++) {
          song = mpd_run_get_queue_song_pos(conn, pos);
          json_array_append_new(j_song_array, json_string(mpd_song_get_uri(song)));
          mpd_song_free(song);
        }
        mpd_status_free(status);
        to_return = json_pack("{siso}", "result", WEBSERVICE_RESULT_OK, "list", j_song_array);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "mpd_playlist_get_list - Error allocating resources for j_song_array");
        to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "mpd_playlist_get_list - Error mpd_run_password, message is %s", mpd_connection_get_error_message(conn));
      to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
    }
  }
  mpd_connection_free(conn);
  return to_return;
}

/**
 * Callback function
 * Get the current playlist song list
 */
int callback_service_mpd_playlist_get_list (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * result, * mpd;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_playlist_get_list - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    mpd = mpd_get((struct _carleon_config *)user_data, u_map_get(request->map_url, "name"));
    if (mpd == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_playlist_get_list - Error getting mpd");
      response->status = 500;
    } else {
      if (json_integer_value(json_object_get(mpd, "result")) == WEBSERVICE_RESULT_NOT_FOUND) {
        response->status = 404;
      } else if (json_integer_value(json_object_get(mpd, "result")) == WEBSERVICE_RESULT_OK) {
        result = mpd_playlist_get_list((struct _carleon_config *)user_data, json_object_get(mpd, "element"));
        if (result == NULL || json_integer_value(json_object_get(result, "result")) == WEBSERVICE_RESULT_ERROR) {
          response->status = 500;
        } else {
          ulfius_set_json_body_response(response, 200, json_object_get(result, "list"));
        }
        json_decref(result);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_playlist_get_list - Error getting mpd");
        response->status = 500;
      }
    }
    json_decref(mpd);
  }
  return U_CALLBACK_CONTINUE;
}

/**
 * Get the song specified at the position pos
 */
json_t * mpd_playlist_get_pos(struct _carleon_config * config, json_t * mpd, unsigned int pos) {
  struct mpd_connection *conn;
  struct mpd_song * song;
  json_t * to_return;
  int auth = 1;

  UNUSED(config);
  conn = mpd_connection_new(json_string_value(json_object_get(mpd, "host")), json_integer_value(json_object_get(mpd, "port")), MPD_CONNECT_TIMEOUT);
  
  if (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS) {
    y_log_message(Y_LOG_LEVEL_ERROR, "mpd_playlist_get_pos - Error mpd_connection_new, message is %s", mpd_connection_get_error_message(conn));
    to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  } else {
    if (json_object_get(mpd, "password") != NULL && json_is_string(json_object_get(mpd, "password")) && json_string_length(json_object_get(mpd, "password")) > 0) {
      if (!mpd_run_password(conn, json_string_value(json_object_get(mpd, "password")))) {
        auth = 0;
      }
    }
    if (auth) {
      song = mpd_run_get_queue_song_pos(conn, pos);
      if (song != NULL) {
        to_return = json_pack("{siss}", "result", WEBSERVICE_RESULT_OK, "uri", mpd_song_get_uri(song));
        mpd_song_free(song);
      } else {
        to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_NOT_FOUND);
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "mpd_playlist_get_pos - Error mpd_run_password, message is %s", mpd_connection_get_error_message(conn));
      to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
    }
  }
  mpd_connection_free(conn);
  return to_return;
}

/**
 * Callback function
 * Clear the current playlist, add a song, then play it
 */
int callback_service_mpd_playlist_get_pos (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * result, * mpd;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_playlist_get_pos - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    mpd = mpd_get((struct _carleon_config *)user_data, u_map_get(request->map_url, "name"));
    if (mpd == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_playlist_get_pos - Error getting mpd");
      response->status = 500;
    } else {
      if (json_integer_value(json_object_get(mpd, "result")) == WEBSERVICE_RESULT_NOT_FOUND) {
        response->status = 404;
      } else if (json_integer_value(json_object_get(mpd, "result")) == WEBSERVICE_RESULT_OK) {
        result = mpd_playlist_get_pos((struct _carleon_config *)user_data, json_object_get(mpd, "element"), strtol(u_map_get(request->map_url, "pos"), NULL, 10));
        if (result == NULL || json_integer_value(json_object_get(result, "result")) == WEBSERVICE_RESULT_ERROR) {
          response->status = 500;
        } else {
          json_object_del(result, "result");
          ulfius_set_json_body_response(response, 200, result);
        }
        json_decref(result);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_playlist_get_pos - Error getting mpd");
        response->status = 500;
      }
    }
    json_decref(mpd);
  }
  return U_CALLBACK_CONTINUE;
}

/**
 * Clear the current playlist, add a song, then play it
 */
json_t * mpd_playlist_clear(struct _carleon_config * config, json_t * mpd) {
  struct mpd_connection *conn;
  json_t * to_return;
  int auth = 1;

  UNUSED(config);
  conn = mpd_connection_new(json_string_value(json_object_get(mpd, "host")), json_integer_value(json_object_get(mpd, "port")), MPD_CONNECT_TIMEOUT);
  
  if (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS) {
    y_log_message(Y_LOG_LEVEL_ERROR, "mpd_playlist_clear - Error mpd_connection_new, message is %s", mpd_connection_get_error_message(conn));
    to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  } else {
    if (json_object_get(mpd, "password") != NULL && json_is_string(json_object_get(mpd, "password")) && json_string_length(json_object_get(mpd, "password")) > 0) {
      if (!mpd_run_password(conn, json_string_value(json_object_get(mpd, "password")))) {
        auth = 0;
      }
    }
    if (auth) {
      if (mpd_run_clear(conn)) {
        to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "mpd_playlist_clear - Error cler playlist, message is %s", mpd_connection_get_error_message(conn));
        to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "mpd_playlist_clear - Error mpd_run_password, message is %s", mpd_connection_get_error_message(conn));
      to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
    }
  }
  mpd_connection_free(conn);
  return to_return;
}

/**
 * Callback function
 * Clear the current playlist, add a song, then play it
 */
int callback_service_mpd_playlist_clear (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * result, * mpd;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_playlist_clear - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    mpd = mpd_get((struct _carleon_config *)user_data, u_map_get(request->map_url, "name"));
    if (mpd == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_playlist_clear - Error getting mpd");
      response->status = 500;
    } else {
      if (json_integer_value(json_object_get(mpd, "result")) == WEBSERVICE_RESULT_NOT_FOUND) {
        response->status = 404;
      } else if (json_integer_value(json_object_get(mpd, "result")) == WEBSERVICE_RESULT_OK) {
        result = mpd_playlist_clear((struct _carleon_config *)user_data, json_object_get(mpd, "element"));
        if (result == NULL || json_integer_value(json_object_get(result, "result")) == WEBSERVICE_RESULT_ERROR) {
          response->status = 500;
        }
        json_decref(result);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_playlist_clear - Error getting mpd");
        response->status = 500;
      }
    }
    json_decref(mpd);
  }
  return U_CALLBACK_CONTINUE;
}

/**
 * Clear the current playlist, then push a list of songs in it
 */
json_t * mpd_playlist_set(struct _carleon_config * config, json_t * mpd, json_t * playlist) {
  struct mpd_connection *conn;
  json_t * to_return, * j_element;
  int auth = 1;
  size_t index;

  UNUSED(config);
  conn = mpd_connection_new(json_string_value(json_object_get(mpd, "host")), json_integer_value(json_object_get(mpd, "port")), MPD_CONNECT_TIMEOUT);
  
  if (playlist == NULL || !json_is_array(playlist)) {
    to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_PARAM);
  } else if (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS) {
    y_log_message(Y_LOG_LEVEL_ERROR, "mpd_playlist_set - Error mpd_connection_new, message is %s", mpd_connection_get_error_message(conn));
    to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  } else {
    if (json_object_get(mpd, "password") != NULL && json_is_string(json_object_get(mpd, "password")) && json_string_length(json_object_get(mpd, "password")) > 0) {
      if (!mpd_run_password(conn, json_string_value(json_object_get(mpd, "password")))) {
        auth = 0;
      }
    }
    if (auth) {
      if (mpd_run_clear(conn)) {
        json_array_foreach(playlist, index, j_element) {
          if (json_is_string(j_element)) {
            if (!mpd_run_add(conn, json_string_value(j_element))) {
              y_log_message(Y_LOG_LEVEL_ERROR, "mpd_playlist_set - Error adding song, message is %s", mpd_connection_get_error_message(conn));
            }
          }
        }
        to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "mpd_playlist_set - Error clear playlist, message is %s", mpd_connection_get_error_message(conn));
        to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "mpd_playlist_set - Error mpd_run_password, message is %s", mpd_connection_get_error_message(conn));
      to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
    }
  }
  mpd_connection_free(conn);
  return to_return;
}

/**
 * Callback function
 * Clear the current playlist, add a song, then play it
 */
int callback_service_mpd_playlist_set (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * result, * mpd, * json_body = ulfius_get_json_body_request(request, NULL);
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_playlist_set - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    mpd = mpd_get((struct _carleon_config *)user_data, u_map_get(request->map_url, "name"));
    if (mpd == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_playlist_set - Error getting mpd");
      response->status = 500;
    } else {
      if (json_integer_value(json_object_get(mpd, "result")) == WEBSERVICE_RESULT_NOT_FOUND) {
        response->status = 404;
      } else if (json_integer_value(json_object_get(mpd, "result")) == WEBSERVICE_RESULT_OK) {
        result = mpd_playlist_set((struct _carleon_config *)user_data, json_object_get(mpd, "element"), json_body);
        if (result == NULL || json_integer_value(json_object_get(result, "result")) == WEBSERVICE_RESULT_ERROR) {
          response->status = 500;
        }
        json_decref(result);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_playlist_set - Error getting mpd");
        response->status = 500;
      }
    }
    json_decref(mpd);
  }
  json_decref(json_body);
  return U_CALLBACK_CONTINUE;
}

/**
 * Push a list of songs in the current playlist
 */
json_t * mpd_playlist_append(struct _carleon_config * config, json_t * mpd, json_t * playlist) {
  struct mpd_connection *conn;
  json_t * to_return, * j_element;
  int auth = 1;
  size_t index;

  UNUSED(config);
  conn = mpd_connection_new(json_string_value(json_object_get(mpd, "host")), json_integer_value(json_object_get(mpd, "port")), MPD_CONNECT_TIMEOUT);
  
  if (playlist == NULL || !json_is_array(playlist)) {
    to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_PARAM);
  } else if (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS) {
    y_log_message(Y_LOG_LEVEL_ERROR, "mpd_playlist_append - Error mpd_connection_new, message is %s", mpd_connection_get_error_message(conn));
    to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  } else {
    if (json_object_get(mpd, "password") != NULL && json_is_string(json_object_get(mpd, "password")) && json_string_length(json_object_get(mpd, "password")) > 0) {
      if (!mpd_run_password(conn, json_string_value(json_object_get(mpd, "password")))) {
        auth = 0;
      }
    }
    if (auth) {
      json_array_foreach(playlist, index, j_element) {
        if (json_is_string(j_element)) {
          if (!mpd_run_add(conn, json_string_value(j_element))) {
            y_log_message(Y_LOG_LEVEL_ERROR, "mpd_playlist_append - Error adding song, message is %s", mpd_connection_get_error_message(conn));
          }
        }
      }
      to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "mpd_playlist_append - Error mpd_run_password, message is %s", mpd_connection_get_error_message(conn));
      to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
    }
  }
  mpd_connection_free(conn);
  return to_return;
}

/**
 * Callback function
 * Clear the current playlist, add a song, then play it
 */
int callback_service_mpd_playlist_append (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * result, * mpd, * json_body = ulfius_get_json_body_request(request, NULL);
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_playlist_append - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    mpd = mpd_get((struct _carleon_config *)user_data, u_map_get(request->map_url, "name"));
    if (mpd == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_playlist_append - Error getting mpd");
      response->status = 500;
    } else {
      if (json_integer_value(json_object_get(mpd, "result")) == WEBSERVICE_RESULT_NOT_FOUND) {
        response->status = 404;
      } else if (json_integer_value(json_object_get(mpd, "result")) == WEBSERVICE_RESULT_OK) {
        result = mpd_playlist_append((struct _carleon_config *)user_data, json_object_get(mpd, "element"), json_body);
        if (result == NULL || json_integer_value(json_object_get(result, "result")) == WEBSERVICE_RESULT_ERROR) {
          response->status = 500;
        }
        json_decref(result);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_playlist_append - Error getting mpd");
        response->status = 500;
      }
    }
    json_decref(mpd);
  }
  json_decref(json_body);
  return U_CALLBACK_CONTINUE;
}

/**
 * Delete the song at position pos in the current playlist
 */
json_t * mpd_playlist_delete(struct _carleon_config * config, json_t * mpd, unsigned int pos) {
  struct mpd_connection *conn;
  json_t * to_return;
  int auth = 1;

  UNUSED(config);
  conn = mpd_connection_new(json_string_value(json_object_get(mpd, "host")), json_integer_value(json_object_get(mpd, "port")), MPD_CONNECT_TIMEOUT);
  
  if (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS) {
    y_log_message(Y_LOG_LEVEL_ERROR, "mpd_playlist_delete - Error mpd_connection_new, message is %s", mpd_connection_get_error_message(conn));
    to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  } else {
    if (json_object_get(mpd, "password") != NULL && json_is_string(json_object_get(mpd, "password")) && json_string_length(json_object_get(mpd, "password")) > 0) {
      if (!mpd_run_password(conn, json_string_value(json_object_get(mpd, "password")))) {
        auth = 0;
      }
    }
    if (auth) {
      if (mpd_run_delete(conn, pos)) {
        to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "mpd_playlist_delete - Error running play, message is %s", mpd_connection_get_error_message(conn));
        to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "mpd_playlist_delete - Error mpd_run_password, message is %s", mpd_connection_get_error_message(conn));
      to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
    }
  }
  mpd_connection_free(conn);
  return to_return;
}

/**
 * Callback function
 * Delete the song at position pos in the current playlist
 */
int callback_service_mpd_playlist_delete (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * result, * mpd, * json_body = ulfius_get_json_body_request(request, NULL);
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_playlist_delete - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    mpd = mpd_get((struct _carleon_config *)user_data, u_map_get(request->map_url, "name"));
    if (mpd == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_playlist_delete - Error getting mpd");
      response->status = 500;
    } else {
      if (json_integer_value(json_object_get(mpd, "result")) == WEBSERVICE_RESULT_NOT_FOUND) {
        response->status = 404;
      } else if (json_integer_value(json_object_get(mpd, "result")) == WEBSERVICE_RESULT_OK) {
        result = mpd_playlist_delete((struct _carleon_config *)user_data, json_object_get(mpd, "element"), strtol(u_map_get(request->map_url, "pos"), NULL, 10));
        if (result == NULL || json_integer_value(json_object_get(result, "result")) == WEBSERVICE_RESULT_ERROR) {
          response->status = 500;
        }
        json_decref(result);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_playlist_delete - Error getting mpd");
        response->status = 500;
      }
    }
    json_decref(mpd);
  }
  json_decref(json_body);
  return U_CALLBACK_CONTINUE;
}

/**
 * Get all the playlists available for the specified mpd
 */
json_t * mpd_get_playlists(struct _carleon_config * config, json_t * mpd) {
  struct mpd_connection * conn;
  struct mpd_playlist * playlist;
  json_t * to_return, * j_list;
  int auth = 1;

  UNUSED(config);
  conn = mpd_connection_new(json_string_value(json_object_get(mpd, "host")), json_integer_value(json_object_get(mpd, "port")), MPD_CONNECT_TIMEOUT);
  
  if (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS) {
    y_log_message(Y_LOG_LEVEL_ERROR, "mpd_get_playlists - Error mpd_connection_new, message is %s", mpd_connection_get_error_message(conn));
    to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  } else {
    if (json_object_get(mpd, "password") != NULL && json_is_string(json_object_get(mpd, "password")) && json_string_length(json_object_get(mpd, "password")) > 0) {
      if (!mpd_run_password(conn, json_string_value(json_object_get(mpd, "password")))) {
        auth = 0;
      }
    }
    if (auth) {
      if (!mpd_send_list_playlists(conn)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "mpd_get_playlists - Error mpd_send_list_playlists, message is %s", mpd_connection_get_error_message(conn));
        to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
      } else {
        j_list = json_array();
        if (j_list == NULL) {
          y_log_message(Y_LOG_LEVEL_ERROR, "mpd_get_playlists - Error allocating resources for j_list");
          to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
        } else {
          while ((playlist = mpd_recv_playlist(conn)) != NULL) {
            json_array_append_new(j_list, json_string(mpd_playlist_get_path(playlist)));
            mpd_playlist_free(playlist);
          }
          to_return = json_pack("{siso}", "result", WEBSERVICE_RESULT_OK, "list", j_list);
        }
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "mpd_get_playlists - Error mpd_run_password, message is %s", mpd_connection_get_error_message(conn));
      to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
    }
  }
  mpd_connection_free(conn);
  return to_return;
}

/**
 * Callback function
 * Get all the playlists available for the specified mpd
 */
int callback_service_mpd_playlists_get (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * result, * mpd;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_playlists_get - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    mpd = mpd_get((struct _carleon_config *)user_data, u_map_get(request->map_url, "name"));
    if (mpd == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_playlists_get - Error getting mpd");
      response->status = 500;
    } else {
      if (json_integer_value(json_object_get(mpd, "result")) == WEBSERVICE_RESULT_NOT_FOUND) {
        response->status = 404;
      } else if (json_integer_value(json_object_get(mpd, "result")) == WEBSERVICE_RESULT_OK) {
        result = mpd_get_playlists((struct _carleon_config *)user_data, json_object_get(mpd, "element"));
        if (result == NULL || json_integer_value(json_object_get(result, "result")) != WEBSERVICE_RESULT_OK) {
          response->status = 500;
        } else {
					ulfius_set_json_body_response(response, 200, json_object_get(result, "list"));
        }
        json_decref(result);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_playlists_get - Error getting mpd");
        response->status = 500;
      }
    }
    json_decref(mpd);
  }
  return U_CALLBACK_CONTINUE;
}

/**
 * Load the specified playlist of the specified mpd
 */
json_t * mpd_load_playlist(struct _carleon_config * config, json_t * mpd, const char * playlist) {
  struct mpd_connection *conn;
  struct mpd_playlist * m_playlist;
  json_t * to_return;
  int found = 0, auth = 1;

  UNUSED(config);
  conn = mpd_connection_new(json_string_value(json_object_get(mpd, "host")), json_integer_value(json_object_get(mpd, "port")), MPD_CONNECT_TIMEOUT);
  
  if (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS) {
    y_log_message(Y_LOG_LEVEL_ERROR, "mpd_load_playlist - Error mpd_connection_new, message is %s", mpd_connection_get_error_message(conn));
    to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  } else {
    if (json_object_get(mpd, "password") != NULL && json_is_string(json_object_get(mpd, "password")) && json_string_length(json_object_get(mpd, "password")) > 0) {
      if (!mpd_run_password(conn, json_string_value(json_object_get(mpd, "password")))) {
        auth = 0;
      }
    }
    if (auth) {
      if (!mpd_send_list_playlists(conn)) {
        y_log_message(Y_LOG_LEVEL_ERROR, "mpd_load_playlist - Error mpd_send_list_playlists, message is %s", mpd_connection_get_error_message(conn));
        to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
      } else {
        while ((m_playlist = mpd_recv_playlist(conn)) != NULL) {
          if (0 == o_strcmp(mpd_playlist_get_path(m_playlist), playlist)) {
            found = 1;
          }
          mpd_playlist_free(m_playlist);
        }
        if (found) {
          if (mpd_run_clear(conn) && mpd_run_load(conn, playlist) && mpd_run_play(conn)) {
            to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "mpd_load_playlist - Error mpd_run_load, message is %s", mpd_connection_get_error_message(conn));
            to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
          }
        } else {
          to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_NOT_FOUND);
        }
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "mpd_load_playlist - Error mpd_run_password, message is %s", mpd_connection_get_error_message(conn));
      to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
    }
  }
  mpd_connection_free(conn);
  return to_return;
}

/**
 * Callback function
 * Load the specified playlist of the specified mpd
 */
int callback_service_mpd_playlists_load (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * result, * mpd;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_playlists_load - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    mpd = mpd_get((struct _carleon_config *)user_data, u_map_get(request->map_url, "name"));
    if (mpd == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_playlists_load - Error getting mpd");
      response->status = 500;
    } else {
      if (json_integer_value(json_object_get(mpd, "result")) == WEBSERVICE_RESULT_NOT_FOUND) {
        response->status = 404;
      } else if (json_integer_value(json_object_get(mpd, "result")) == WEBSERVICE_RESULT_OK) {
        result = mpd_load_playlist((struct _carleon_config *)user_data, json_object_get(mpd, "element"), u_map_get(request->map_url, "playlist_name"));
        if (result == NULL || json_integer_value(json_object_get(result, "result")) == WEBSERVICE_RESULT_ERROR) {
          response->status = 500;
        } else if (json_integer_value(json_object_get(result, "result")) == WEBSERVICE_RESULT_NOT_FOUND) {
          response->status = 404;
        }
        json_decref(result);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_playlists_load - Error getting mpd");
        response->status = 500;
      }
    }
    json_decref(mpd);
  }
  return U_CALLBACK_CONTINUE;
}

/**
 * change the volume of the specified mpd
 */
json_t * mpd_set_volume(struct _carleon_config * config, json_t * mpd, uint volume) {
  struct mpd_connection * conn;
  json_t * to_return;
  int auth = 1;

  UNUSED(config);
  conn = mpd_connection_new(json_string_value(json_object_get(mpd, "host")), json_integer_value(json_object_get(mpd, "port")), MPD_CONNECT_TIMEOUT);
  
  if (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS) {
    y_log_message(Y_LOG_LEVEL_ERROR, "mpd_set_volume - Error mpd_connection_new, message is %s", mpd_connection_get_error_message(conn));
    to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  } else {
    if (json_object_get(mpd, "password") != NULL && json_is_string(json_object_get(mpd, "password")) && json_string_length(json_object_get(mpd, "password")) > 0) {
      if (!mpd_run_password(conn, json_string_value(json_object_get(mpd, "password")))) {
        auth = 0;
      }
    }
    if (auth) {
      if (mpd_run_set_volume(conn, volume)) {
        to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "mpd_set_volume - Error mpd_run_set_volume, message is %s", mpd_connection_get_error_message(conn));
        to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "mpd_set_volume - Error mpd_run_password, message is %s", mpd_connection_get_error_message(conn));
      to_return = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
    }
  }
  mpd_connection_free(conn);
  return to_return;
}

/**
 * Callback function
 * change the volume of the specified mpd
 */
int callback_service_mpd_volume_set (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * result, * mpd;
  uint volume;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_volume_set - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    mpd = mpd_get((struct _carleon_config *)user_data, u_map_get(request->map_url, "name"));
    if (mpd == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_volume_set - Error getting mpd");
      response->status = 500;
    } else {
      if (json_integer_value(json_object_get(mpd, "result")) == WEBSERVICE_RESULT_NOT_FOUND) {
        response->status = 404;
      } else if (json_integer_value(json_object_get(mpd, "result")) == WEBSERVICE_RESULT_OK) {
        char * ptr;
        volume = strtol(u_map_get(request->map_url, "volume"), &ptr, 10);
        if (ptr == u_map_get(request->map_url, "volume")) {
          set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "Volume invalid"));
        } else if (volume > 100) {
          set_response_json_body_and_clean(response, 400, json_pack("{ss}", "error", "Volume invalid"));
        } else {
          result = mpd_set_volume((struct _carleon_config *)user_data, json_object_get(mpd, "element"), volume);
          if (result == NULL || json_integer_value(json_object_get(result, "result")) != WEBSERVICE_RESULT_OK) {
            response->status = 500;
          }
          json_decref(result);
        }
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_volume_set - Error getting mpd");
        response->status = 500;
      }
    }
    json_decref(mpd);
  }
  return U_CALLBACK_CONTINUE;
}

#ifndef U_DISABLE_WEBSOCKET
static long random_at_most(long max) {
  unsigned long
  // max <= RAND_MAX < ULONG_MAX, so this is okay.
  num_bins = (unsigned long) max + 1,
  num_rand = (unsigned long) RAND_MAX + 1,
  bin_size = num_rand / num_bins,
  defect   = num_rand % num_bins;

  long x;
  do {
   x = random();
  }
  // This is carefully written not to overflow
  while (num_rand - defect <= (unsigned long)x);

  // Truncated division is intentional
  return x/bin_size;
}

/**
 * Generates a random string and store it in str
 */
static char * rand_string(char * str, size_t str_size) {
	const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	size_t n;
	
	if (str_size > 0 && str != NULL) {
		for (n = 0; n < str_size; n++) {
			long key = random_at_most((sizeof(charset)) - 2);
			str[n] = charset[key];
		}
		str[str_size] = '\0';
		return str;
	} else {
		return NULL;
	}
}

static json_t * add_websocket_session(struct _carleon_config * config, json_t * mpd) {
	char websocket_session[MPD_WEBSOCKET_NAME_SIZE + 1], * escape, * clause;
	json_t * j_query, * j_result;
	int res;
	
	if (rand_string(websocket_session, MPD_WEBSOCKET_NAME_SIZE) != NULL) {
		escape = h_escape_string(config->conn, json_string_value(json_object_get(mpd, "name")));
		clause = msprintf("(SELECT `cpmd_id` FROM "MPD_TABLE_NAME" WHERE `cmpd_name` = '%s')", escape);
		j_query = json_pack("{sss{sss{ss}}}",
												"table",
												MPD_WEBSOCKET_TABLE_NAME,
												"values",
													"cmw_name",
													websocket_session,
													"cmpd_id",
														"raw",
														clause);
		o_free(escape);
		o_free(clause);
		res = h_insert(config->conn, j_query, NULL);
		json_decref(j_query);
		if (res == H_OK) {
			j_result = json_pack("{sis{ss}}", "result", WEBSERVICE_RESULT_OK, "websocket_session", "name", websocket_session);
		} else {
			y_log_message(Y_LOG_LEVEL_ERROR, "add_websocket_session - Error h_insert");
			j_result = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
		}
	} else {
		y_log_message(Y_LOG_LEVEL_ERROR, "add_websocket_session - Error rand_string");
		j_result = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
	}
	return j_result;
}

static int get_websocket_session(struct _carleon_config * config, json_t * mpd, const char * websocket_session) {
	json_t * j_query, * j_result;
	int res, ret;
	char * escape_ws, * escape_name, * clause;
	
	escape_ws = h_escape_string(config->conn, websocket_session);
	escape_name = h_escape_string(config->conn, json_string_value(json_object_get(mpd, "name")));
	clause = msprintf("=(SELECT `cmpd_id` FROM "MPD_TABLE_NAME" WHERE `cmpd_name`='%s')", escape_name);
	j_query = json_pack("{sss{sss{ssss}s{ssss}}}",
											"table",
											MPD_WEBSOCKET_TABLE_NAME,
											"where",
												"cmw_name",
												escape_ws,
												"cmpd_id",
													"operator",
													"raw",
													"value",
													clause,
												"cmw_creation",
													"operator",
													"raw",
													"value",
													config->conn->type==HOEL_DB_TYPE_MARIADB?"< (NOW() - INTERVAL 30 SECOND)":"< date('now','-30 second')");
	o_free(escape_ws);
	o_free(escape_name);
	o_free(clause);
	res = h_select(config->conn, j_query, &j_result, NULL);
	json_decref(j_query);
	if (res == H_OK) {
		if (json_array_size(j_result) > 0) {
			ret = WEBSERVICE_RESULT_OK;
		} else {
			ret = WEBSERVICE_RESULT_NOT_FOUND;
		}
		json_decref(j_result);
	} else {
		y_log_message(Y_LOG_LEVEL_ERROR, "get_websocket_session - Error h_delete");
		ret = WEBSERVICE_RESULT_ERROR;
	}
	return ret;
}

static int delete_websocket_session(struct _carleon_config * config, json_t * mpd, const char * websocket_session) {
	json_t * j_query;
	int res, ret;
	char * escape_ws, * escape_name, * clause;
	
	escape_ws = h_escape_string(config->conn, websocket_session);
	escape_name = h_escape_string(config->conn, json_string_value(json_object_get(mpd, "name")));
	clause = msprintf("=(SELECT `cmpd_id` FROM "MPD_TABLE_NAME" WHERE `cmpd_name`='%s')", escape_name);
	j_query = json_pack("{sss{sss{ssss}}}",
											"table",
											MPD_WEBSOCKET_TABLE_NAME,
											"where",
												"cmw_name",
												escape_ws,
												"cmpd_id",
													"operator",
													"raw",
													"value",
													clause);
	o_free(escape_ws);
	o_free(escape_name);
	o_free(clause);
	res = h_delete(config->conn, j_query, NULL);
	json_decref(j_query);
	if (res == H_OK) {
		ret = WEBSERVICE_RESULT_OK;
	} else {
		y_log_message(Y_LOG_LEVEL_ERROR, "delete_websocket_session - Error h_delete");
		ret = WEBSERVICE_RESULT_ERROR;
	}
	return ret;
}

static int delete_websocket_session_obsolete(struct _carleon_config * config) {
	json_t * j_query;
	int res, ret;
	
	j_query = json_pack("{sss{s{ssss}}}",
											"table",
											MPD_WEBSOCKET_TABLE_NAME,
											"where",
												"cmw_creation",
													"operator",
													"raw",
													"value",
													config->conn->type==HOEL_DB_TYPE_MARIADB?"< (NOW() - INTERVAL 30 SECOND)":"< date('now','-30 second')");
	res = h_delete(config->conn, j_query, NULL);
	json_decref(j_query);
	if (res == H_OK) {
		ret = WEBSERVICE_RESULT_OK;
	} else {
		y_log_message(Y_LOG_LEVEL_ERROR, "delete_websocket_session_obsolete - Error h_delete");
		ret = WEBSERVICE_RESULT_ERROR;
	}
	return ret;
}

/**
 * Callback function
 * Request a websocket session start: send a websocket name
 */
int callback_service_mpd_start_ws (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * result, * mpd;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_start_ws - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
	} else if (delete_websocket_session_obsolete((struct _carleon_config *)user_data) != WEBSERVICE_RESULT_OK) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_start_ws - Error delete_websocket_session_obsolete");
    return U_CALLBACK_ERROR;
  } else {
    mpd = mpd_get((struct _carleon_config *)user_data, u_map_get(request->map_url, "name"));
    if (mpd == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_start_ws - Error getting mpd");
      response->status = 500;
    } else {
      if (json_integer_value(json_object_get(mpd, "result")) == WEBSERVICE_RESULT_NOT_FOUND) {
        response->status = 404;
      } else if (json_integer_value(json_object_get(mpd, "result")) == WEBSERVICE_RESULT_OK) {
        result = add_websocket_session((struct _carleon_config *)user_data, json_object_get(mpd, "element"));
        if (result == NULL || json_integer_value(json_object_get(result, "result")) == WEBSERVICE_RESULT_ERROR) {
          response->status = 500;
        } else {
					ulfius_set_json_body_response(response, 200, json_object_get(result, "websocket_session"));
        }
        json_decref(result);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_start_ws - Error getting mpd");
        response->status = 500;
      }
    }
    json_decref(mpd);
  }
  return U_CALLBACK_CONTINUE;
}

struct _websocket_mpd_config {
	struct _carleon_config * config;
  json_t                 * mpd;
  struct mpd_connection  * conn;
};

void websocket_onclose_mpd (const struct _u_request * request,
                                struct _websocket_manager * websocket_manager,
                                void * websocket_onclose_user_data) {
  UNUSED(websocket_manager);
  UNUSED(request);
  struct _websocket_mpd_config * wmc = (struct _websocket_mpd_config *)websocket_onclose_user_data;
  json_decref(wmc->mpd);
  mpd_connection_free(wmc->conn);
  o_free(wmc);
}

void websocket_manager_mpd(const struct _u_request * request,
                               struct _websocket_manager * websocket_manager,
                               void * websocket_manager_user_data) {
	struct _websocket_mpd_config * wmc = (struct _websocket_mpd_config *)websocket_manager_user_data;
	json_t * status;
	char * message;
	
  UNUSED(request);
	if (mpd_send_idle_mask(wmc->conn, MPD_IDLE_QUEUE | MPD_IDLE_PLAYER | MPD_IDLE_MIXER | MPD_IDLE_OPTIONS)) {
		while (websocket_manager->connected) {
			if (mpd_recv_idle(wmc->conn, false) && websocket_manager->connected) {
				status = mpd_get_status(wmc->config, wmc->mpd);
				if (status != NULL && json_integer_value(json_object_get(status, "result")) == WEBSERVICE_RESULT_OK) {
					message = json_dumps(json_object_get(status, "result"), JSON_COMPACT);
					if (message != NULL) {
						if (ulfius_websocket_send_message(websocket_manager, U_WEBSOCKET_OPCODE_TEXT, o_strlen(message), message) != U_OK) {
							y_log_message(Y_LOG_LEVEL_ERROR, "websocket_manager_mpd - Error ulfius_websocket_send_message");
							break;
						}
						o_free(message);
					}
				} else {
					y_log_message(Y_LOG_LEVEL_ERROR, "websocket_manager_mpd - Error mpd_get_status");
					break;
				}
				json_decref(status);
			}
		}
	} else {
		y_log_message(Y_LOG_LEVEL_ERROR, "websocket_manager_mpd - Error mpd_send_idle_mask, exit websocket");
	}
}

/**
 * Callback function
 * Run a websocket session
 */
int callback_service_mpd_run_ws (const struct _u_request * request, struct _u_response * response, void * user_data) {
  json_t * mpd;
	int res;
  struct _websocket_mpd_config * wmc;
  int auth = 1;
  
  if (user_data == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_run_ws - Error, user_data is NULL");
    return U_CALLBACK_ERROR;
  } else {
    mpd = mpd_get((struct _carleon_config *)user_data, u_map_get(request->map_url, "name"));
    if (mpd == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_run_ws - Error getting mpd");
      response->status = 500;
    } else {
      if (json_integer_value(json_object_get(mpd, "result")) == WEBSERVICE_RESULT_NOT_FOUND) {
        response->status = 404;
      } else if (json_integer_value(json_object_get(mpd, "result")) == WEBSERVICE_RESULT_OK) {
				if ((res = get_websocket_session((struct _carleon_config *)user_data, mpd, u_map_get(request->map_url, "session"))) == WEBSERVICE_RESULT_OK) {
					if (delete_websocket_session((struct _carleon_config *)user_data, mpd, u_map_get(request->map_url, "session")) == WEBSERVICE_RESULT_OK) {
            wmc = o_malloc(sizeof(struct _websocket_mpd_config));
            if (wmc != NULL) {
              wmc->config = (struct _carleon_config *)user_data;
              wmc->mpd = json_deep_copy(mpd);
							wmc->conn = mpd_connection_new(json_string_value(json_object_get(mpd, "host")), json_integer_value(json_object_get(mpd, "port")), MPD_CONNECT_TIMEOUT);
							
							if (mpd_connection_get_error(wmc->conn) != MPD_ERROR_SUCCESS) {
								y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_run_ws - Error mpd_connection_new, message is %s", mpd_connection_get_error_message(wmc->conn));
								json_decref(wmc->mpd);
								mpd_connection_free(wmc->conn);
								o_free(wmc);
								response->status = 500;
							} else {
								if (json_object_get(mpd, "password") != NULL && json_is_string(json_object_get(mpd, "password")) && json_string_length(json_object_get(mpd, "password")) > 0) {
									if (!mpd_run_password(wmc->conn, json_string_value(json_object_get(mpd, "password")))) {
										auth = 0;
									}
								}
								if (auth) {
									if (ulfius_set_websocket_response(response, NULL, NULL, &websocket_manager_mpd, wmc, NULL, NULL, &websocket_onclose_mpd, wmc) != U_OK) {
										y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_run_ws - Error ulfius_set_websocket_response");
										json_decref(wmc->mpd);
										mpd_connection_free(wmc->conn);
										o_free(wmc);
										response->status = 500;
									}
								} else {
									y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_run_ws - Error mpd_run_password, message is %s", mpd_connection_get_error_message(wmc->conn));
									json_decref(wmc->mpd);
									mpd_connection_free(wmc->conn);
									o_free(wmc);
									response->status = 500;
								}
							}
            } else {
							y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_run_ws - Error allocating resources for wmc");
							response->status = 500;
            }
					} else {
						y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_run_ws - Error delete_websocket_session");
						response->status = 500;
					}
				} else if (res != WEBSERVICE_RESULT_NOT_FOUND) {
					y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_run_ws - Error get_websocket_session");
					response->status = 500;
				} else {
					response->status = 404;
				}
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "callback_service_mpd_run_ws - Error getting mpd");
        response->status = 500;
      }
    }
    json_decref(mpd);
  }
	return U_CALLBACK_CONTINUE;
}
#endif

/**
 * Initialize the mpd service
 */
json_t * c_service_init(struct _carleon_config * config) {
  if (config != NULL) {
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix, "/service-mpd/", 2, &callback_service_mpd_get, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix, "/service-mpd/@name", 2, &callback_service_mpd_get, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "POST", config->url_prefix, "/service-mpd/", 2, &callback_service_mpd_add, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "PUT", config->url_prefix, "/service-mpd/@name", 2, &callback_service_mpd_set, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "DELETE", config->url_prefix, "/service-mpd/@name", 2, &callback_service_mpd_remove, (void*)config);

    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix, "/service-mpd/@name/status", 2, &callback_service_mpd_status, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "PUT", config->url_prefix, "/service-mpd/@name/action/@action_name", 2, &callback_service_mpd_action, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "PUT", config->url_prefix, "/service-mpd/@name/action/@action_name/@action_param", 2, &callback_service_mpd_action, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "POST", config->url_prefix, "/service-mpd/@name/play/@uri", 2, &callback_service_mpd_play_song, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "PUT", config->url_prefix, "/service-mpd/@name/playpos/@pos", 2, &callback_service_mpd_play_pos, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix, "/service-mpd/@name/playlists", 2, &callback_service_mpd_playlists_get, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "PUT", config->url_prefix, "/service-mpd/@name/playlist/@playlist_name", 2, &callback_service_mpd_playlists_load, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix, "/service-mpd/@name/playlist/", 2, &callback_service_mpd_playlist_get_list, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix, "/service-mpd/@name/playlist/@pos", 2, &callback_service_mpd_playlist_get_pos, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "DELETE", config->url_prefix, "/service-mpd/@name/playlist/", 2, &callback_service_mpd_playlist_clear, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "POST", config->url_prefix, "/service-mpd/@name/playlist/", 2, &callback_service_mpd_playlist_set, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "PUT", config->url_prefix, "/service-mpd/@name/playlist/", 2, &callback_service_mpd_playlist_append, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "DELETE", config->url_prefix, "/service-mpd/@name/playlist/@pos", 2, &callback_service_mpd_playlist_delete, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "PUT", config->url_prefix, "/service-mpd/@name/volume/@volume", 2, &callback_service_mpd_volume_set, (void*)config);
#ifndef U_DISABLE_WEBSOCKET
    ulfius_add_endpoint_by_val(config->instance, "PUT", config->url_prefix, "/service-mpd/@name/startws", 2, &callback_service_mpd_start_ws, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix, "/service-mpd/@name/ws/@session", 0, &callback_service_mpd_run_ws, (void*)config);
#endif
    return json_pack("{sissss}", 
                      "result", WEBSERVICE_RESULT_OK,
                      "name", "service-mpd",
                      "description", "Control a distant MPD");
  } else {
    return json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  }
}

/**
 * Closes the mpd service
 */
json_t * c_service_close(struct _carleon_config * config) {
  if (config != NULL) {
    ulfius_remove_endpoint_by_val(config->instance, "GET", config->url_prefix, "/service-mpd/");
    ulfius_remove_endpoint_by_val(config->instance, "GET", config->url_prefix, "/service-mpd/@name");
    ulfius_remove_endpoint_by_val(config->instance, "POST", config->url_prefix, "/service-mpd/");
    ulfius_remove_endpoint_by_val(config->instance, "PUT", config->url_prefix, "/service-mpd/@name");
    ulfius_remove_endpoint_by_val(config->instance, "DELETE", config->url_prefix, "/service-mpd/@name");

    ulfius_remove_endpoint_by_val(config->instance, "GET", config->url_prefix, "/service-mpd/@name/status");
    ulfius_remove_endpoint_by_val(config->instance, "PUT", config->url_prefix, "/service-mpd/@name/action/@action_name");
    ulfius_remove_endpoint_by_val(config->instance, "PUT", config->url_prefix, "/service-mpd/@name/action/@action_name/@action_param");
    ulfius_remove_endpoint_by_val(config->instance, "POST", config->url_prefix, "/service-mpd/@name/play/@uri");
    ulfius_remove_endpoint_by_val(config->instance, "POST", config->url_prefix, "/service-mpd/@name/add/@uri");
    ulfius_remove_endpoint_by_val(config->instance, "PUT", config->url_prefix, "/service-mpd/@name/playpos/@pos");
    ulfius_remove_endpoint_by_val(config->instance, "GET", config->url_prefix, "/service-mpd/@name/playlists");
    ulfius_remove_endpoint_by_val(config->instance, "GET", config->url_prefix, "/service-mpd/@name/playlist/");
    ulfius_remove_endpoint_by_val(config->instance, "GET", config->url_prefix, "/service-mpd/@name/playlist/@pos");
    ulfius_remove_endpoint_by_val(config->instance, "PUT", config->url_prefix, "/service-mpd/@name/playlist/@playlist_name");
    ulfius_remove_endpoint_by_val(config->instance, "DELETE", config->url_prefix, "/service-mpd/@name/playlist/");
    ulfius_remove_endpoint_by_val(config->instance, "POST", config->url_prefix, "/service-mpd/@name/playlist/");
    ulfius_remove_endpoint_by_val(config->instance, "PUT", config->url_prefix, "/service-mpd/@name/volume/@volume");
    ulfius_remove_endpoint_by_val(config->instance, "PUT", config->url_prefix, "/service-mpd/@name/startws");
    ulfius_remove_endpoint_by_val(config->instance, "GET", config->url_prefix, "/service-mpd/@name/ws/@session");

    return json_pack("{si}", "result", WEBSERVICE_RESULT_OK);
  } else {
    return json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  }
}

/**
 * send the available commands
 */
json_t * c_service_command_get_list(struct _carleon_config * config) {
  json_t * to_return = json_pack("{siso}", "result", WEBSERVICE_RESULT_OK, "commands", json_object());
  
  UNUSED(config);
  if (to_return != NULL) {
    json_object_set_new(json_object_get(to_return, "commands"), "action", json_pack("{s{s{ssso}s{ssso}}}", "parameters",
                          "action", "type", "string", "required", json_true(),
                          "parameter", "type", "string", "required", json_false()));
    json_object_set_new(json_object_get(to_return, "commands"), "play_song", json_pack("{s{s{ssso}}}", "parameters",
                          "uri", "type", "string", "required", json_true()));
    json_object_set_new(json_object_get(to_return, "commands"), "clear_playlist", json_object());
    json_object_set_new(json_object_get(to_return, "commands"), "load_playlist", json_pack("{s{s{ssso}}}", "parameters",
                          "playlist", "type", "string", "required", json_true()));
    json_object_set_new(json_object_get(to_return, "commands"), "set_playlist", json_pack("{s{s{ssso}}}", "parameters",
                          "playlist", "type", "array", "required", json_true()));
    json_object_set_new(json_object_get(to_return, "commands"), "set_volume", json_pack("{s{s{ssso}}}", "parameters",
                          "volume", "type", "integer", "required", json_true()));
  }
  return to_return;
}

/**
 * Execute a command
 */
json_t * c_service_exec(struct _carleon_config * config, const char * command, const char * element, json_t * parameters) {
  json_t * mpd, * result = NULL;

  if (command != NULL) {
    mpd = mpd_get(config, element);
    if (mpd == NULL || json_integer_value(json_object_get(mpd, "result")) != WEBSERVICE_RESULT_OK) {
      result = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
    } else {
      if (0 == o_strcmp(command, "action")) {
        result = mpd_set_action(config, json_object_get(mpd, "element"), json_string_value(json_object_get(parameters, "action")), json_string_value(json_object_get(parameters, "parameter")));
      } else if (0 == o_strcmp(command, "play_song")) {
        result = mpd_play_song(config, json_object_get(mpd, "element"), json_string_value(json_object_get(parameters, "uri")));
      } else if (0 == o_strcmp(command, "clear_playlist")) {
        result = mpd_playlist_clear(config, json_object_get(mpd, "element"));
      } else if (0 == o_strcmp(command, "load_playlist")) {
        result = mpd_load_playlist(config, json_object_get(mpd, "element"), json_string_value(json_object_get(parameters, "playlist")));
      } else if (0 == o_strcmp(command, "set_playlist")) {
        result = mpd_playlist_set(config, json_object_get(mpd, "element"), json_object_get(parameters, "playlist"));
      } else if (0 == o_strcmp(command, "set_volume")) {
        result = mpd_set_volume(config, json_object_get(mpd, "element"), json_integer_value(json_object_get(parameters, "volume")));
      } else {
        result = json_pack("{si}", "result", WEBSERVICE_RESULT_NOT_FOUND);
      }
    }
    json_decref(mpd);
  } else {
    result = json_pack("{si}", "result", WEBSERVICE_RESULT_ERROR);
  }
  return result;
}

/**
 * Get the list of available elements
 */
json_t * c_service_element_get_list(struct _carleon_config * config) {
  return mpd_get(config, NULL);
}
