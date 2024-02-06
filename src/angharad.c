/**
 *
 * Angharad system
 *
 * Manage all subsystems and handles the house automation server:
 * - Benoic
 * - Carleon
 *
 * Angharad server entry point
 *
 * Copyright 2016-2021 Nicolas Mora <mail@babelouest.org>
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
#include <libconfig.h>
#include <getopt.h>
#include <signal.h>

#include "angharad.h"

pthread_mutex_t global_handler_close_lock;
pthread_cond_t  global_handler_close_cond;

/**
 * Main function
 * Benoic application entry point
 * Initialise the database connection
 * Starts the webservice
 * Infinite loop until an end signal is triggered
 */
int main(int argc, char ** argv) {
  struct config_elements * config = o_malloc(sizeof(struct config_elements));
  int res;
  json_t * submodule, * j_tmp;
#ifndef DISABLE_OAUTH2
  char * str_jwks;
  json_t * j_jwks;
#endif

  if (config == NULL) {
    fprintf(stderr, "Memory error - config\n");
    return 1;
  }

  config->config_file = NULL;
  config->url_prefix_angharad = NULL;
  config->url_prefix_benoic = NULL;
  config->url_prefix_carleon = NULL;
  config->allow_origin = NULL;
  config->log_mode = Y_LOG_MODE_NONE;
  config->log_level = Y_LOG_LEVEL_NONE;
  config->log_file = NULL;
  config->angharad_status = ANGHARAD_STATUS_STOP;
  config->alert_url = NULL;
  config->config_content = NULL;
  config->instance = o_malloc(sizeof(struct _u_instance));
  config->c_config = o_malloc(sizeof(struct _carleon_config));
  config->b_config = o_malloc(sizeof(struct _benoic_config));
#ifndef DISABLE_OAUTH2
  config->use_oidc_authentication = 0;
  config->oidc_server_remote_config = NULL;
  config->oidc_server_remote_config_verify_cert = 1;
  config->oidc_server_public_jwks = NULL;
  config->oidc_scope = NULL;
  config->oidc_iss = NULL;
  config->oidc_realm = NULL;
  config->oidc_aud = NULL;
  config->oidc_dpop_max_iat = 0;
  config->iddawc_resource_config = o_malloc(sizeof (struct _iddawc_resource_config));
  if (config->iddawc_resource_config == NULL) {
    fprintf(stderr, "Memory error - config->iddawc_resource_config == NULL\n");
    return 1;
  }
#endif
  config->static_file_config = o_malloc(sizeof(struct _u_compressed_inmemory_website_config));
  if (config->instance == NULL || config->c_config == NULL || config->b_config == NULL || config->static_file_config == NULL) {
    fprintf(stderr, "Memory error - config->instance || config->c_config || config->b_config || config->static_file_config == NULL\n");
    return 1;
  }
  config->c_config->services_path = NULL;
  config->c_config->conn = NULL;
  config->c_config->service_list = NULL;
  config->b_config->modules_path = NULL;
  config->b_config->conn = NULL;
  config->b_config->device_type_list = NULL;
  config->b_config->device_data_list = NULL;
  config->b_config->benoic_status = BENOIC_STATUS_STOP;

  if (u_init_compressed_inmemory_website_config(config->static_file_config) != U_OK) {
    fprintf(stderr, "Error u_init_compressed_inmemory_website_config\n");
    exit_server(&config, ANGHARAD_ERROR);
  }
  u_map_put(&config->static_file_config->mime_types, "*", "application/octet-stream");

#ifndef DISABLE_OAUTH2
  i_global_init();
#endif
  ulfius_init_instance(config->instance, ANGHARAD_DEFAULT_PORT, NULL, NULL);

  if (pthread_mutex_init(&global_handler_close_lock, NULL) ||
      pthread_cond_init(&global_handler_close_cond, NULL)) {
    fprintf(stderr, "init - Error initializing global_handler_close_lock or global_handler_close_cond\n");
    return 1;
  }
  // Catch end signals to make a clean exit
  if (signal (SIGQUIT, exit_handler) == SIG_ERR ||
      signal (SIGINT, exit_handler) == SIG_ERR ||
      signal (SIGTERM, exit_handler) == SIG_ERR ||
      signal (SIGHUP, exit_handler) == SIG_ERR) {
    fprintf(stderr, "init - Error initializing end signal\n");
    return 1;
  }

  // First we parse command line arguments
  if (!build_config_from_args(argc, argv, config)) {
    fprintf(stderr, "Error reading command-line parameters\n");
    print_help(stderr);
    exit_server(&config, ANGHARAD_ERROR);
  }

  // Then we parse configuration file
  // They have lower priority than command line parameters
  if (!build_config_from_file(config)) {
    fprintf(stderr, "Error config file\n");
    exit_server(&config, ANGHARAD_ERROR);
  }

  // Check if all mandatory configuration variables are present and correctly typed
  if (!check_config(config)) {
    fprintf(stderr, "Error initializing configuration\n");
    exit_server(&config, ANGHARAD_ERROR);
  }

  // Setting connection pointer for carleon and benoic
  config->b_config->conn = config->conn;
  config->c_config->conn = config->conn;
  config->alert_url = get_alert_url(config);
  config->b_config->alert_url = config->alert_url;
  config->c_config->alert_url = config->alert_url;
  config->c_config->url_prefix = config->url_prefix_carleon;
  config->c_config->instance = config->instance;

  // Initialize benoic webservice if enabled
  submodule = submodule_get(config, ANGHARAD_SUBMODULE_BENOIC);
  if (submodule != NULL && json_integer_value(json_object_get(submodule, "result")) == A_OK && json_object_get(json_object_get(submodule, "submodule"), "enabled") == json_true()) {
    res = init_benoic(config->instance, config->url_prefix_benoic, config->b_config);
    if (res != B_OK) {
      y_log_message(Y_LOG_LEVEL_ERROR, "Error initializing benoic webservice: %d", res);
      json_decref(submodule);
      exit_server(&config, ANGHARAD_ERROR);
    }
  }
  json_decref(submodule);

  // Initialize carleon webservice if enabled
  submodule = submodule_get(config, ANGHARAD_SUBMODULE_CARLEON);
  if (submodule != NULL && json_integer_value(json_object_get(submodule, "result")) == A_OK && json_object_get(json_object_get(submodule, "submodule"), "enabled") == json_true()) {
    res = init_carleon(config->c_config);
    if (res != C_OK) {
      y_log_message(Y_LOG_LEVEL_ERROR, "Error initializing carleon webservice: %d", res);
      json_decref(submodule);
      exit_server(&config, ANGHARAD_ERROR);
    }
  }
  json_decref(submodule);

#ifndef DISABLE_OAUTH2
  if (config->use_oidc_authentication) {
    if (i_jwt_profile_access_token_init_config(config->iddawc_resource_config, I_METHOD_HEADER, NULL, NULL, config->oidc_scope, NULL, config->oidc_dpop_max_iat) == I_TOKEN_OK) {
      if (config->oidc_server_remote_config != NULL) {
        if (!i_jwt_profile_access_token_load_config(config->iddawc_resource_config, config->oidc_server_remote_config, (int)config->oidc_server_remote_config_verify_cert)) {
          y_log_message(Y_LOG_LEVEL_ERROR, "OIDC authentication - Error i_jwt_profile_access_token_load_config");
          exit_server(&config, ANGHARAD_ERROR);
        }
        y_log_message(Y_LOG_LEVEL_INFO, "OIDC authentication - Load remote authentification config: %s", config->oidc_server_remote_config);
      } else if (config->oidc_server_public_jwks != NULL) {
        res = 1;
        if ((str_jwks = get_file_content(config->oidc_server_public_jwks)) != NULL) {
          if ((j_jwks = json_loads(str_jwks, JSON_DECODE_ANY, NULL)) != NULL) {
            if (!i_jwt_profile_access_token_load_jwks(config->iddawc_resource_config, j_jwks, config->oidc_iss)) {
              y_log_message(Y_LOG_LEVEL_ERROR, "OIDC authentication - Error i_jwt_profile_access_token_load_jwks");
            }
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "OIDC authentication - Error parsing jwks");
            res = 0;
          }
          json_decref(j_jwks);
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "OIDC authentication - Error reading jwks file");
          res = 0;
        }
        o_free(str_jwks);
        if (!res) {
          exit_server(&config, ANGHARAD_ERROR);
        }
        y_log_message(Y_LOG_LEVEL_INFO, "OIDC authentication - Load signature key from file: %s", config->oidc_server_public_jwks);
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "OIDC authentication - Error oidc config");
        exit_server(&config, ANGHARAD_ERROR);
      }
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "OIDC authentication - Error i_jwt_profile_access_token_init_config");
      exit_server(&config, ANGHARAD_ERROR);
    }
    j_tmp = json_pack("{ss ss ss so ss* ss*}", 
                      "angharad_endpoint", config->url_prefix_angharad,
                      "benoic_endpoint", config->url_prefix_benoic,
                      "carleon_endpoint", config->url_prefix_carleon,
                      "oidc", json_true(),
                      "oauth_scope", config->oidc_scope,
                      "oidc_server_remote_config", config->oidc_server_remote_config);
  } else {
    j_tmp = json_pack("{ss ss ss so}", 
                      "angharad_endpoint", config->url_prefix_angharad,
                      "benoic_endpoint", config->url_prefix_benoic,
                      "carleon_endpoint", config->url_prefix_carleon,
                      "oidc", json_false());
  }
#else
  j_tmp = json_pack("{ss ss ss so}", 
                    "angharad_endpoint", config->url_prefix_angharad,
                    "benoic_endpoint", config->url_prefix_benoic,
                    "carleon_endpoint", config->url_prefix_carleon,
                    "oidc", json_false());
#endif

  if ((config->config_content = json_dumps(j_tmp, JSON_COMPACT)) == NULL) {
    fprintf(stderr, "Error setting config content\n");
    exit_server(&config, ANGHARAD_ERROR);
  }
  json_decref(j_tmp);
  
  // Initialize angharad webservice
  res = init_angharad(config);
  if (res != A_OK) {
    y_log_message(Y_LOG_LEVEL_ERROR, "Error initializing angharad webservice: %d", res);
    exit_server(&config, ANGHARAD_ERROR);
  }

  // Start the webservice
#ifndef DISABLE_OAUTH2
  y_log_message(Y_LOG_LEVEL_INFO, "Start Angharad on port %d, scope %s", config->instance->port, config->oidc_scope);
#else
  y_log_message(Y_LOG_LEVEL_INFO, "Start Angharad on port %d", config->instance->port);
#endif
  if (ulfius_start_framework(config->instance) == U_OK) {
    // Wait until stop signal is broadcasted
    pthread_mutex_lock(&global_handler_close_lock);
    pthread_cond_wait(&global_handler_close_cond, &global_handler_close_lock);
    pthread_mutex_unlock(&global_handler_close_lock);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "Error starting angharad webserver");
    exit_server(&config, ANGHARAD_ERROR);
  }
  y_log_message(Y_LOG_LEVEL_INFO, "Stop angharad server");
  exit_server(&config, 0);
  return 0;
}

/**
 * Initialize the application configuration based on the command line parameters
 */
int build_config_from_args(int argc, char ** argv, struct config_elements * config) {
  int next_option;
  const char * short_options = "c:p:u:v:w:m:l:f:h::b:s:";
  char * tmp = NULL, * to_free = NULL, * one_log_mode = NULL;
  static const struct option long_options[]= {
    {"config-file", optional_argument,NULL, 'c'},
    {"port", optional_argument,NULL, 'p'},
    {"url-prefix-angharad", optional_argument,NULL, 'u'},
    {"url-prefix-benoic", optional_argument,NULL, 'v'},
    {"url-prefix-carleon", optional_argument,NULL, 'w'},
    {"log-mode", optional_argument,NULL, 'm'},
    {"log-level", optional_argument,NULL, 'l'},
    {"log-file", optional_argument,NULL, 'f'},
    {"help", optional_argument,NULL, 'h'},
    {"benoic-modules-path", optional_argument,NULL, 'b'},
    {"carleon-services-path", optional_argument,NULL, 's'},
    {NULL, 0, NULL, 0}
  };

  if (config != NULL) {
    do {
      next_option = getopt_long(argc, argv, short_options, long_options, NULL);

      switch (next_option) {
        case 'c':
          if (optarg != NULL) {
            config->config_file = o_strdup(optarg);
            if (config->config_file == NULL) {
              fprintf(stderr, "Error allocating config->config_file, exiting\n");
              exit_server(&config, ANGHARAD_STOP);
            }
          } else {
            fprintf(stderr, "Error!\nNo config file specified\n");
            return 0;
          }
          break;
        case 'p':
          if (optarg != NULL) {
            config->instance->port = (unsigned int)strtol(optarg, NULL, 10);
            if (config->instance->port <= 0 || config->instance->port > 65535) {
              fprintf(stderr, "Error!\nInvalid TCP Port number\n\tPlease specify an integer value between 1 and 65535");
              return 0;
            }
          } else {
            fprintf(stderr, "Error!\nNo TCP Port number specified\n");
            return 0;
          }
          break;
        case 'u':
          if (optarg != NULL) {
            config->url_prefix_angharad = o_strdup(optarg);
            if (config->url_prefix_angharad == NULL) {
              fprintf(stderr, "Error allocating config->url_prefix_angharad, exiting\n");
              exit_server(&config, ANGHARAD_STOP);
            }
          } else {
            fprintf(stderr, "Error!\nNo URL prefix specified\n");
            return 0;
          }
          break;
        case 'v':
          if (optarg != NULL) {
            config->url_prefix_benoic = o_strdup(optarg);
            if (config->url_prefix_benoic == NULL) {
              fprintf(stderr, "Error allocating config->url_prefix_benoic, exiting\n");
              exit_server(&config, ANGHARAD_STOP);
            }
          } else {
            fprintf(stderr, "Error!\nNo URL prefix specified\n");
            return 0;
          }
          break;
        case 'w':
          if (optarg != NULL) {
            config->url_prefix_carleon = o_strdup(optarg);
            if (config->url_prefix_carleon == NULL) {
              fprintf(stderr, "Error allocating config->url_prefix_carleon, exiting\n");
              exit_server(&config, ANGHARAD_STOP);
            }
          } else {
            fprintf(stderr, "Error!\nNo URL prefix specified\n");
            return 0;
          }
          break;
        case 'm':
          if (optarg != NULL) {
            tmp = o_strdup(optarg);
            if (tmp == NULL) {
              fprintf(stderr, "Error allocating log_mode, exiting\n");
              exit_server(&config, ANGHARAD_STOP);
            }
            one_log_mode = strtok(tmp, ",");
            while (one_log_mode != NULL) {
              if (0 == o_strncmp("console", one_log_mode, strlen("console"))) {
                config->log_mode |= Y_LOG_MODE_CONSOLE;
              } else if (0 == o_strncmp("syslog", one_log_mode, strlen("syslog"))) {
                config->log_mode |= Y_LOG_MODE_SYSLOG;
              } else if (0 == o_strncmp("file", one_log_mode, strlen("file"))) {
                config->log_mode |= Y_LOG_MODE_FILE;
              }
              one_log_mode = strtok(NULL, ",");
            }
            o_free(to_free);
          } else {
            fprintf(stderr, "Error!\nNo mode specified\n");
            return 0;
          }
          break;
        case 'l':
          if (optarg != NULL) {
            if (0 == o_strncmp("NONE", optarg, strlen("NONE"))) {
              config->log_level = Y_LOG_LEVEL_NONE;
            } else if (0 == o_strncmp("ERROR", optarg, strlen("ERROR"))) {
              config->log_level = Y_LOG_LEVEL_ERROR;
            } else if (0 == o_strncmp("WARNING", optarg, strlen("WARNING"))) {
              config->log_level = Y_LOG_LEVEL_WARNING;
            } else if (0 == o_strncmp("INFO", optarg, strlen("INFO"))) {
              config->log_level = Y_LOG_LEVEL_INFO;
            } else if (0 == o_strncmp("DEBUG", optarg, strlen("DEBUG"))) {
              config->log_level = Y_LOG_LEVEL_DEBUG;
            }
          } else {
            fprintf(stderr, "Error!\nNo log level specified\n");
            return 0;
          }
          break;
        case 'f':
          if (optarg != NULL) {
            config->log_file = o_strdup(optarg);
            if (config->log_file == NULL) {
              fprintf(stderr, "Error allocating config->log_file, exiting\n");
              exit_server(&config, ANGHARAD_STOP);
            }
          } else {
            fprintf(stderr, "Error!\nNo log file specified\n");
            return 0;
          }
          break;
        case 'b':
          if (optarg != NULL) {
            config->b_config->modules_path = o_strdup(optarg);
          } else {
            fprintf(stderr, "Error!\nNo benoic modules path specified\n");
            return 0;
          }
          break;
        case 's':
          if (optarg != NULL) {
            config->c_config->services_path = o_strdup(optarg);
          } else {
            fprintf(stderr, "Error!\nNo carleon service path specified\n");
            return 0;
          }
          break;
        case 'h':
          exit_server(&config, ANGHARAD_STOP);
          break;
      }

    } while (next_option != -1);

    // If none exists, exit failure
    if (config->config_file == NULL) {
      fprintf(stderr, "No configuration file found, please specify a configuration file path\n");
      return 0;
    }

    return 1;
  } else {
    return 0;
  }

}

/**
 * Print help message to output file specified
 */
void print_help(FILE * output) {
  fprintf(output, "\nAngharad System\n");
  fprintf(output, "\n");
  fprintf(output, "Version %s\n", _ANGHARAD_VERSION_);
  fprintf(output, "\n");
  fprintf(output, "Command house automation devices and subsystems using a JSON/REST interface\n");
  fprintf(output, "\n");
  fprintf(output, "-c --config-file=PATH\n");
  fprintf(output, "\tPath to configuration file\n");
  fprintf(output, "-p --port=PORT\n");
  fprintf(output, "\tPort to listen to\n");
  fprintf(output, "-u --url-prefix=PREFIX\n");
  fprintf(output, "\tURL prefix\n");
  fprintf(output, "-m --log-mode=MODE\n");
  fprintf(output, "\tLog Mode\n");
  fprintf(output, "\tconsole, syslog or file\n");
  fprintf(output, "\tIf you want multiple modes, separate them with a comma \",\"\n");
  fprintf(output, "\tdefault: console\n");
  fprintf(output, "-l --log-level=LEVEL\n");
  fprintf(output, "\tLog level\n");
  fprintf(output, "\tNONE, ERROR, WARNING, INFO, DEBUG\n");
  fprintf(output, "\tdefault: ERROR\n");
  fprintf(output, "-f --log-file=PATH\n");
  fprintf(output, "\tPath for log file if log mode file is specified\n");
  fprintf(output, "-b --benoic-module-path=PATH\n");
  fprintf(output, "\tPath for benoic modules\n");
  fprintf(output, "-s --carleon-services-path=PATH\n");
  fprintf(output, "\tPath for carleon services\n");
  fprintf(output, "-h --help\n");
  fprintf(output, "\tPrint this message\n\n");
}

/**
 * Exit properly the server by closing opened connections, databases and files
 */
void exit_server(struct config_elements ** config, int exit_value) {
  if (config != NULL && *config != NULL) {
    // Cleaning data

    close_benoic((*config)->instance, (*config)->url_prefix_benoic, (*config)->b_config);
    close_carleon((*config)->c_config);
    close_angharad((*config));
    h_close_db((*config)->conn);
    h_clean_connection((*config)->conn);
    ulfius_stop_framework((*config)->instance);
    ulfius_clean_instance((*config)->instance);
    clean_benoic((*config)->b_config);
    clean_carleon((*config)->c_config);
    o_free((*config)->instance);
    o_free((*config)->config_file);
    o_free((*config)->url_prefix_angharad);
    o_free((*config)->url_prefix_benoic);
    o_free((*config)->url_prefix_carleon);
    o_free((*config)->alert_url);
#ifndef DISABLE_OAUTH2
    o_free((*config)->oidc_server_remote_config);
    o_free((*config)->oidc_scope);
    o_free((*config)->config_content);
    if ((*config)->use_oidc_authentication) {
      i_jwt_profile_access_token_close_config((*config)->iddawc_resource_config);
    }
    o_free((*config)->iddawc_resource_config);
#endif
    o_free((*config)->static_file_config->files_path);
    o_free((*config)->static_file_config->url_prefix);
    u_clean_compressed_inmemory_website_config((*config)->static_file_config);
    o_free((*config)->static_file_config);
    o_free((*config)->allow_origin);
    o_free((*config)->log_file);

    o_free(*config);
    (*config) = NULL;
  }
  if (pthread_mutex_destroy(&global_handler_close_lock) ||
      pthread_cond_destroy(&global_handler_close_cond)) {
    y_log_message(Y_LOG_LEVEL_ERROR, "Error destroying global_handler_close_lock or global_handler_close_cond");
  }
#ifndef DISABLE_OAUTH2
  i_global_close();
#endif
  y_close_logs();
  exit(exit_value);
}

/**
 * handles signal catch to exit properly when ^C is used for example
 */
void exit_handler(int signal) {
  y_log_message(Y_LOG_LEVEL_INFO, "Caught a stop or kill signal (%d), exiting", signal);
  pthread_mutex_lock(&global_handler_close_lock);
  pthread_cond_signal(&global_handler_close_cond);
  pthread_mutex_unlock(&global_handler_close_lock);
}

/**
 * Initialize the application configuration based on the config file content
 * Read the config file, get mandatory variables and devices
 */
int build_config_from_file(struct config_elements * config) {

  config_t cfg;
  config_setting_t * root, * database, * mime_type_list, * mime_type;
  const char * cur_prefix_angharad, * cur_prefix_benoic, * cur_prefix_carleon, * cur_log_mode, * cur_log_level, * cur_log_file = NULL, * one_log_mode, * carleon_services_path, * benoic_modules_path, * cur_allow_origin, * db_type, * db_sqlite_path, * db_mariadb_host = NULL, * db_mariadb_user = NULL, * db_mariadb_password = NULL, * db_mariadb_dbname = NULL, * cur_static_files_path = NULL, * extension = NULL, * mime_type_value = NULL;
  int db_mariadb_port = 0;
  unsigned int i;
#ifndef DISABLE_OAUTH2
  config_setting_t * oidc_cfg;
  const char * cur_oidc_server_remote_config = NULL, * cur_oidc_server_public_jwks = NULL, * cur_oidc_iss = NULL, * cur_oidc_realm = NULL, * cur_oidc_aud = NULL, * cur_oidc_scope = NULL;
  int cur_oidc_dpop_max_iat = 0, cur_oidc_server_remote_config_verify_cert = 0, cur_use_oidc_authentication = 0;
#endif

  config_init(&cfg);

  if (!config_read_file(&cfg, config->config_file)) {
    fprintf(stderr, "Error parsing config file %s\nOn line %d error: %s\n", config_error_file(&cfg), config_error_line(&cfg), config_error_text(&cfg));
    config_destroy(&cfg);
    return 0;
  }

  if (config->instance->port == ANGHARAD_DEFAULT_PORT) {
    // Get Port number to listen to
    int port;
    config_lookup_int(&cfg, "port", &port);
    config->instance->port = (uint)port;
  }

  if (config->url_prefix_angharad == NULL) {
    // Get prefix url for angharad
    if (config_lookup_string(&cfg, "url_prefix_angharad", &cur_prefix_angharad)) {
      config->url_prefix_angharad = o_strdup(cur_prefix_angharad);
      if (config->url_prefix_angharad == NULL) {
        fprintf(stderr, "Error allocating config->url_prefix_angharad, exiting\n");
        config_destroy(&cfg);
        return 0;
      }
    }
  }

  if (config->url_prefix_benoic == NULL) {
    // Get prefix url for benoic
    if (config_lookup_string(&cfg, "url_prefix_benoic", &cur_prefix_benoic)) {
      config->url_prefix_benoic = o_strdup(cur_prefix_benoic);
      if (config->url_prefix_benoic == NULL) {
        fprintf(stderr, "Error allocating config->url_prefix_benoic, exiting\n");
        config_destroy(&cfg);
        return 0;
      }
    }
  }

  if (config->url_prefix_carleon == NULL) {
    // Get prefix url for carleon
    if (config_lookup_string(&cfg, "url_prefix_carleon", &cur_prefix_carleon)) {
      config->url_prefix_carleon = o_strdup(cur_prefix_carleon);
      if (config->url_prefix_carleon == NULL) {
        fprintf(stderr, "Error allocating config->url_prefix_carleon, exiting\n");
        config_destroy(&cfg);
        return 0;
      }
    }
  }

  if (config->allow_origin == NULL) {
    // Get allow-origin value for CORS
    if (config_lookup_string(&cfg, "allow_origin", &cur_allow_origin)) {
      config->allow_origin = o_strdup(cur_allow_origin);
      if (config->allow_origin == NULL) {
        fprintf(stderr, "Error allocating config->allow_origin, exiting\n");
        config_destroy(&cfg);
        return 0;
      }
    }
  }

  if (config->c_config->services_path == NULL) {
    // Get carleon services path
    if (config_lookup_string(&cfg, "carleon_services_path", &carleon_services_path)) {
      config->c_config->services_path = o_strdup(carleon_services_path);
      if (config->c_config->services_path == NULL) {
        fprintf(stderr, "Error allocating config->c_config->services_path, exiting\n");
        config_destroy(&cfg);
        return 0;
      }
    }
  }

  if (config->b_config->modules_path == NULL) {
    // Get benoic modules path
    if (config_lookup_string(&cfg, "benoic_modules_path", &benoic_modules_path)) {
      config->b_config->modules_path = o_strdup(benoic_modules_path);
      if (config->b_config->modules_path == NULL) {
        fprintf(stderr, "Error allocating config->b_config->modules_path, exiting\n");
        config_destroy(&cfg);
        return 0;
      }
    }
  }

  if (config->log_mode == Y_LOG_MODE_NONE) {
    // Get log mode
    if (config_lookup_string(&cfg, "log_mode", &cur_log_mode)) {
      one_log_mode = strtok((char *)cur_log_mode, ",");
      while (one_log_mode != NULL) {
        if (0 == o_strncmp("console", one_log_mode, strlen("console"))) {
          config->log_mode |= Y_LOG_MODE_CONSOLE;
        } else if (0 == o_strncmp("syslog", one_log_mode, strlen("syslog"))) {
          config->log_mode |= Y_LOG_MODE_SYSLOG;
        } else if (0 == o_strncmp("file", one_log_mode, strlen("file"))) {
          config->log_mode |= Y_LOG_MODE_FILE;
          // Get log file path
          if (config->log_file == NULL) {
            if (config_lookup_string(&cfg, "log_file", &cur_log_file)) {
              config->log_file = o_strdup(cur_log_file);
              if (config->log_file == NULL) {
                fprintf(stderr, "Error allocating config->log_file, exiting\n");
                config_destroy(&cfg);
                return 0;
              }
            }
          }
        }
        one_log_mode = strtok(NULL, ",");
      }
    }
  }

  if (config->log_level == Y_LOG_LEVEL_NONE) {
    // Get log level
    if (config_lookup_string(&cfg, "log_level", &cur_log_level)) {
      if (0 == o_strncmp("NONE", cur_log_level, strlen("NONE"))) {
        config->log_level = Y_LOG_LEVEL_NONE;
      } else if (0 == o_strncmp("ERROR", cur_log_level, strlen("ERROR"))) {
        config->log_level = Y_LOG_LEVEL_ERROR;
      } else if (0 == o_strncmp("WARNING", cur_log_level, strlen("WARNING"))) {
        config->log_level = Y_LOG_LEVEL_WARNING;
      } else if (0 == o_strncmp("INFO", cur_log_level, strlen("INFO"))) {
        config->log_level = Y_LOG_LEVEL_INFO;
      } else if (0 == o_strncmp("DEBUG", cur_log_level, strlen("DEBUG"))) {
        config->log_level = Y_LOG_LEVEL_DEBUG;
      }
    }
  }

  if (!y_init_logs(ANGHARAD_LOG_NAME, config->log_mode, config->log_level, config->log_file, "Starting Angharad system")) {
    fprintf(stderr, "Error initializing logs\n");
    exit_server(&config, ANGHARAD_ERROR);
  }

  root = config_root_setting(&cfg);
  database = config_setting_get_member(root, "database");
  if (database != NULL) {
    if (config_setting_lookup_string(database, "type", &db_type) == CONFIG_TRUE) {
      if (0 == o_strncmp(db_type, "sqlite3", strlen("sqlite3"))) {
        if (config_setting_lookup_string(database, "path", &db_sqlite_path) == CONFIG_TRUE) {
          config->conn = h_connect_sqlite(db_sqlite_path);
          if (config->conn == NULL) {
            fprintf(stderr, "Error opening sqlite database %s\n", db_sqlite_path);
            config_destroy(&cfg);
            return 0;
          } else {
            if (h_execute_query_sqlite(config->conn, "PRAGMA foreign_keys = ON;") != H_OK) {
              y_log_message(Y_LOG_LEVEL_ERROR, "Error executing sqlite3 query 'PRAGMA foreign_keys = ON;'");
              config_destroy(&cfg);
              return 0;
            }
          }
        } else {
          config_destroy(&cfg);
          fprintf(stderr, "Error, no sqlite database specified\n");
          return 0;
        }
      } else if (0 == o_strncmp(db_type, "mariadb", strlen("mariadb"))) {
        config_setting_lookup_string(database, "host", &db_mariadb_host);
        config_setting_lookup_string(database, "user", &db_mariadb_user);
        config_setting_lookup_string(database, "password", &db_mariadb_password);
        config_setting_lookup_string(database, "dbname", &db_mariadb_dbname);
        config_setting_lookup_int(database, "port", &db_mariadb_port);
        config->conn = h_connect_mariadb(db_mariadb_host, db_mariadb_user, db_mariadb_password, db_mariadb_dbname, (unsigned int)db_mariadb_port, NULL);
        if (config->conn == NULL) {
          fprintf(stderr, "Error opening mariadb database %s\n", db_mariadb_dbname);
          config_destroy(&cfg);
          return 0;
        }
      } else {
        config_destroy(&cfg);
        fprintf(stderr, "Error, database type unknown\n");
        return 0;
      }
    } else {
      config_destroy(&cfg);
      fprintf(stderr, "Error, no database type found\n");
      return 0;
    }
  } else {
    config_destroy(&cfg);
    fprintf(stderr, "Error, no database setting found\n");
    return 0;
  }

  if (config->static_file_config->files_path == NULL) {
    // Get path that serve static files
    if (config_lookup_string(&cfg, "app_files_path", &cur_static_files_path)) {
      config->static_file_config->files_path = o_strdup(cur_static_files_path);
      if (config->static_file_config->files_path == NULL) {
        fprintf(stderr, "Error allocating config->static_file_config->files_path, exiting\n");
        config_destroy(&cfg);
        return 0;
      }
    }
  }

  // Populate mime types u_map
  mime_type_list = config_lookup(&cfg, "app_files_mime_types");
  if (mime_type_list != NULL) {
    for (i=0; i<(unsigned int)config_setting_length(mime_type_list); i++) {
      mime_type = config_setting_get_elem(mime_type_list, i);
      if (mime_type != NULL) {
        if (config_setting_lookup_string(mime_type, "extension", &extension) && config_setting_lookup_string(mime_type, "type", &mime_type_value)) {
          u_map_put(&config->static_file_config->mime_types, extension, mime_type_value);
        }
      }
    }
  }

#ifndef DISABLE_OAUTH2
  if (config_lookup_bool(&cfg, "use_oidc_authentication", &cur_use_oidc_authentication) == CONFIG_TRUE) {
    config->use_oidc_authentication = (short unsigned int)cur_use_oidc_authentication;
  }

  if (config->use_oidc_authentication) {
    oidc_cfg = config_lookup(&cfg, "oidc");
    if (config_setting_lookup_string(oidc_cfg, "server_remote_config", &cur_oidc_server_remote_config) == CONFIG_TRUE) {
      if ((config->oidc_server_remote_config = o_strdup(cur_oidc_server_remote_config)) == NULL) {
        fprintf(stderr, "Error allocating config->oidc_server_remote_config, exiting\n");
        config_destroy(&cfg);
        return 0;
      }
    }
    if (config_setting_lookup_bool(oidc_cfg, "server_remote_config_verify_cert", &cur_oidc_server_remote_config_verify_cert) == CONFIG_TRUE) {
      config->oidc_server_remote_config_verify_cert = (unsigned int)cur_oidc_server_remote_config_verify_cert;
    }
    if (config_setting_lookup_string(oidc_cfg, "server_public_jwks", &cur_oidc_server_public_jwks) == CONFIG_TRUE) {
      if ((config->oidc_server_public_jwks = o_strdup(cur_oidc_server_public_jwks)) == NULL) {
        fprintf(stderr, "Error allocating config->oidc_server_public_jwks, exiting\n");
        config_destroy(&cfg);
        return 0;
      }
    }
    if (config_setting_lookup_string(oidc_cfg, "iss", &cur_oidc_iss) == CONFIG_TRUE) {
      if ((config->oidc_iss = o_strdup(cur_oidc_iss)) == NULL) {
        fprintf(stderr, "Error allocating config->oidc_iss, exiting\n");
        config_destroy(&cfg);
        return 0;
      }
    }
    if (config_setting_lookup_string(oidc_cfg, "realm", &cur_oidc_realm) == CONFIG_TRUE) {
      if ((config->oidc_realm = o_strdup(cur_oidc_realm)) == NULL) {
        fprintf(stderr, "Error allocating config->oidc_realm, exiting\n");
        config_destroy(&cfg);
        return 0;
      }
    }
    if (config_setting_lookup_string(oidc_cfg, "aud", &cur_oidc_aud) == CONFIG_TRUE) {
      if ((config->oidc_aud = o_strdup(cur_oidc_aud)) == NULL) {
        fprintf(stderr, "Error allocating config->oidc_aud, exiting\n");
        config_destroy(&cfg);
        return 0;
      }
    }
    if (config_setting_lookup_int(oidc_cfg, "dpop_max_iat", &cur_oidc_dpop_max_iat) == CONFIG_TRUE) {
      config->oidc_dpop_max_iat = (time_t)cur_oidc_dpop_max_iat;
    }
  }

  if (config_lookup_string(&cfg, "oauth_scope", &cur_oidc_scope)) {
    config->oidc_scope = o_strdup(cur_oidc_scope);
    if (config->oidc_scope == NULL) {
      fprintf(stderr, "Error allocating config->oidc_scope, exiting\n");
      config_destroy(&cfg);
      return 0;
    }
  }
#endif

  config_destroy(&cfg);
  return 1;
}

/**
 * Check if all mandatory configuration parameters are present and correct
 * Initialize some parameters with default value if not set
 */
int check_config(struct config_elements * config) {

  if (config->url_prefix_angharad == NULL) {
    config->url_prefix_angharad = o_strdup(ANGHARAD_DEFAULT_PREFIX);
    if (config->url_prefix_angharad == NULL) {
      fprintf(stderr, "Error allocating url_prefix_angharad, exit\n");
      return 0;
    }
  }

  if (config->url_prefix_benoic == NULL) {
    config->url_prefix_benoic = o_strdup(BENOIC_DEFAULT_PREFIX);
    if (config->url_prefix_benoic == NULL) {
      fprintf(stderr, "Error allocating url_prefix_benoic, exit\n");
      return 0;
    }
  }

  if (config->url_prefix_carleon == NULL) {
    config->url_prefix_carleon = o_strdup(CARLEON_DEFAULT_PREFIX);
    if (config->url_prefix_carleon == NULL) {
      fprintf(stderr, "Error allocating url_prefix_carleon, exit\n");
      return 0;
    }
  }

  if (config->log_mode == Y_LOG_MODE_NONE) {
    config->log_mode = Y_LOG_MODE_CONSOLE;
  }

  if (config->log_level == Y_LOG_LEVEL_NONE) {
    config->log_level = Y_LOG_LEVEL_ERROR;
  }

  if (config->log_mode == Y_LOG_MODE_FILE && config->log_file == NULL) {
    fprintf(stderr, "Error, you must specify a log file if log mode is set to file\n");
    print_help(stderr);
    return 0;
  }

  if (config->allow_origin == NULL) {
    config->allow_origin = o_strdup(ALLOW_ORIGIN_DEFAULT);
  }

  if (config->b_config->modules_path == NULL) {
    fprintf(stderr, "Error, no modules path specified for benoic\n");
    return 0;
  }

  if (config->c_config->services_path == NULL) {
    fprintf(stderr, "Error, no modules path specified for carleon\n");
    return 0;
  }

  return 1;
}

/**
 * Default callback function
 * return status 404
 */
int callback_default (const struct _u_request * request, struct _u_response * response, void * user_data) {
  UNUSED(user_data);
  set_response_json_body_and_clean(response, 404, json_pack("{ssssss}", "error", "page not found", "message", "The page can not be found, check documentation", "method", request->http_verb, "url", request->http_url));
  return U_CALLBACK_CONTINUE;
}

/**
 * Build the alert_url based oon the pattern: http://[hostname]:[port]/[angharad_prefix]/alert/@submodule_name/@source/@element/@message/
 */
char * get_alert_url(struct config_elements * config) {
  char hostname[256];

  if (config == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "get_alert_url - Error config is NULL");
    return NULL;
  } else {
    if (gethostname(hostname, 255) == 0) {
      return msprintf("http://%s:%d/%s/alert/%%s/%%s/%%s/%%s/", hostname, config->instance->port, config->url_prefix_angharad);
    } else {
      y_log_message(Y_LOG_LEVEL_ERROR, "get_alert_url - Error gethostname");
      return NULL;
    }
  }
}

/**
 * Get all submodules or the specified submodule
 */
json_t * submodule_get(struct config_elements * config, const char * submodule) {
  json_t * j_query, * j_result, * j_submodule;
  int res;
  size_t index;

  if (config == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "submodule_get - Error config is NULL");
    return NULL;
  }

  j_query = json_pack("{sss[sss]}", "table", ANGHARAD_TABLE_SUBMODULE, "columns", "as_name AS name", "as_description AS description", "as_enabled AS enabled");

  if (j_query == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "submodule_get - Error allocating resources for j_query");
    return NULL;
  }

  if (submodule != NULL) {
    json_object_set_new(j_query, "where", json_pack("{ss}", "as_name", submodule));
  }

  res = h_select(config->conn, j_query, &j_result, NULL);
  json_decref(j_query);
  if (res == H_OK) {
    if (submodule != NULL) {
      if (json_array_size(j_result) == 0) {
        json_decref(j_result);
        return json_pack("{si}", "result", A_ERROR_NOT_FOUND);
      } else {
        j_submodule = json_copy(json_array_get(j_result, 0));
        if (json_integer_value(json_object_get(j_submodule, "enabled")) == 1) {
          json_object_del(j_submodule, "enabled");
          json_object_set_new(j_submodule, "enabled", json_true());
        } else {
          json_object_del(j_submodule, "enabled");
          json_object_set_new(j_submodule, "enabled", json_false());
        }
        json_decref(j_result);
        return json_pack("{siso}", "result", A_OK, "submodule", j_submodule);
      }
    } else {
      json_array_foreach(j_result, index, j_submodule) {
        if (json_integer_value(json_object_get(j_submodule, "enabled")) == 1) {
          json_object_del(j_submodule, "enabled");
          json_object_set_new(j_submodule, "enabled", json_true());
        } else {
          json_object_del(j_submodule, "enabled");
          json_object_set_new(j_submodule, "enabled", json_false());
        }
      }
      return json_pack("{siso}", "result", A_OK, "submodules", j_result);
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "submodule_get - Error executing db query");
    return NULL;
  }
}

// TODO: Remove me
int callback_gareth_ghost (const struct _u_request * request, struct _u_response * response, void * user_data) {
  UNUSED(request);
  UNUSED(user_data);
  json_t * j_ar = json_array();
  ulfius_set_json_body_response(response, 200, j_ar);
  json_decref(j_ar);
  return U_CALLBACK_COMPLETE;
}

/**
 * Initializes angharad server
 * Create all the endpoints
 */
int init_angharad(struct config_elements * config) {
  pthread_t thread_scheduler;
  int thread_scheduler_ret = 0, thread_scheduler_detach = 0;

  if (config != NULL && config->instance != NULL && config->url_prefix_angharad) {
#ifndef DISABLE_OAUTH2
    if (config->use_oidc_authentication) {
      ulfius_add_endpoint_by_val(config->instance, "*", config->url_prefix_angharad, "*", ANGHARAD_CALLBACK_PRIORITY_AUTHENTICATION, &callback_check_jwt_profile_access_token, (void*)config->iddawc_resource_config);
      ulfius_add_endpoint_by_val(config->instance, "*", config->url_prefix_benoic, "*", ANGHARAD_CALLBACK_PRIORITY_AUTHENTICATION, &callback_check_jwt_profile_access_token, (void*)config->iddawc_resource_config);
      ulfius_add_endpoint_by_val(config->instance, "*", config->url_prefix_carleon, "*", ANGHARAD_CALLBACK_PRIORITY_AUTHENTICATION, &callback_check_jwt_profile_access_token, (void*)config->iddawc_resource_config);
    } else {
      ulfius_add_endpoint_by_val(config->instance, "*", config->url_prefix_angharad, "*", ANGHARAD_CALLBACK_PRIORITY_AUTHENTICATION, &callback_oauth2_disabled, (void*)config);
      ulfius_add_endpoint_by_val(config->instance, "*", config->url_prefix_benoic, "*", ANGHARAD_CALLBACK_PRIORITY_AUTHENTICATION, &callback_oauth2_disabled, (void*)config);
      ulfius_add_endpoint_by_val(config->instance, "*", config->url_prefix_carleon, "*", ANGHARAD_CALLBACK_PRIORITY_AUTHENTICATION, &callback_oauth2_disabled, (void*)config);
    }
#endif

    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/submodule/", ANGHARAD_CALLBACK_PRIORITY_APPLICATION, &callback_angharad_submodule_list, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/submodule/@submodule_name", ANGHARAD_CALLBACK_PRIORITY_APPLICATION, &callback_angharad_submodule_get, (void*)config);

    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/script/", ANGHARAD_CALLBACK_PRIORITY_APPLICATION, &callback_angharad_script_list, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/script/@script_name", ANGHARAD_CALLBACK_PRIORITY_APPLICATION, &callback_angharad_script_get, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "POST", config->url_prefix_angharad, "/script/", ANGHARAD_CALLBACK_PRIORITY_APPLICATION, &callback_angharad_script_add, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "PUT", config->url_prefix_angharad, "/script/@script_name", ANGHARAD_CALLBACK_PRIORITY_APPLICATION, &callback_angharad_script_modify, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "DELETE", config->url_prefix_angharad, "/script/@script_name", ANGHARAD_CALLBACK_PRIORITY_APPLICATION, &callback_angharad_script_remove, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "PUT", config->url_prefix_angharad, "/script/@script_name/@tag", ANGHARAD_CALLBACK_PRIORITY_APPLICATION, &callback_angharad_script_add_tag, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "DELETE", config->url_prefix_angharad, "/script/@script_name/@tag", ANGHARAD_CALLBACK_PRIORITY_APPLICATION, &callback_angharad_script_remove_tag, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "PUT", config->url_prefix_angharad, "/script/@script_name/run", ANGHARAD_CALLBACK_PRIORITY_APPLICATION, &callback_angharad_script_run, (void*)config);
// TODO: Remove me
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/script/@script_name/run", ANGHARAD_CALLBACK_PRIORITY_APPLICATION, &callback_angharad_script_run, (void*)config);

    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/scheduler/", ANGHARAD_CALLBACK_PRIORITY_APPLICATION, &callback_angharad_scheduler_list, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/scheduler/@scheduler_name", ANGHARAD_CALLBACK_PRIORITY_APPLICATION, &callback_angharad_scheduler_get, (void*)config);
// TODO: Remove me
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/scheduler/@scheduler_name/enable/@enabled", ANGHARAD_CALLBACK_PRIORITY_APPLICATION, &callback_angharad_scheduler_enable, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "PUT", config->url_prefix_angharad, "/scheduler/@scheduler_name/enable/@enabled", ANGHARAD_CALLBACK_PRIORITY_APPLICATION, &callback_angharad_scheduler_enable, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "POST", config->url_prefix_angharad, "/scheduler/", ANGHARAD_CALLBACK_PRIORITY_APPLICATION, &callback_angharad_scheduler_add, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "PUT", config->url_prefix_angharad, "/scheduler/@scheduler_name", ANGHARAD_CALLBACK_PRIORITY_APPLICATION, &callback_angharad_scheduler_modify, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "DELETE", config->url_prefix_angharad, "/scheduler/@scheduler_name", ANGHARAD_CALLBACK_PRIORITY_APPLICATION, &callback_angharad_scheduler_remove, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "PUT", config->url_prefix_angharad, "/scheduler/@scheduler_name/@tag", ANGHARAD_CALLBACK_PRIORITY_APPLICATION, &callback_angharad_scheduler_add_tag, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "DELETE", config->url_prefix_angharad, "/scheduler/@scheduler_name/@tag", ANGHARAD_CALLBACK_PRIORITY_APPLICATION, &callback_angharad_scheduler_remove_tag, (void*)config);

    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/profile", ANGHARAD_CALLBACK_PRIORITY_APPLICATION, &callback_carleon_profile_list, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/profile/@profile_id", ANGHARAD_CALLBACK_PRIORITY_APPLICATION, &callback_carleon_profile_get, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "PUT", config->url_prefix_angharad, "/profile/@profile_id", ANGHARAD_CALLBACK_PRIORITY_APPLICATION, &callback_carleon_profile_set, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "DELETE", config->url_prefix_angharad, "/profile/@profile_id", ANGHARAD_CALLBACK_PRIORITY_APPLICATION, &callback_carleon_profile_remove, (void*)config);

// TODO: Remove me
    ulfius_add_endpoint_by_val(config->instance, "GET", NULL, "/gareth/filter/", ANGHARAD_CALLBACK_PRIORITY_APPLICATION, &callback_gareth_ghost, NULL);
    ulfius_add_endpoint_by_val(config->instance, "GET", NULL, "/gareth/alert/", ANGHARAD_CALLBACK_PRIORITY_APPLICATION, &callback_gareth_ghost, NULL);
// /TODO: Remove me

    ulfius_add_endpoint_by_val(config->instance, "*", config->url_prefix_angharad, "*", ANGHARAD_CALLBACK_PRIORITY_COMPRESSION, &callback_http_compression, NULL);
    ulfius_add_endpoint_by_val(config->instance, "*", config->url_prefix_benoic, "*", ANGHARAD_CALLBACK_PRIORITY_COMPRESSION, &callback_http_compression, NULL);
    ulfius_add_endpoint_by_val(config->instance, "*", config->url_prefix_carleon, "*", ANGHARAD_CALLBACK_PRIORITY_COMPRESSION, &callback_http_compression, NULL);
    if (!o_strnullempty(config->static_file_config->files_path)) {
      ulfius_add_endpoint_by_val(config->instance, "GET", NULL, "*", ANGHARAD_CALLBACK_PRIORITY_FILES, &callback_static_compressed_inmemory_website, (void*)config->static_file_config);
    }
    ulfius_add_endpoint_by_val(config->instance, "OPTIONS", NULL, "*", ANGHARAD_CALLBACK_PRIORITY_ZERO, &callback_angharad_options, (void*)config);

    ulfius_add_endpoint_by_val(config->instance, "GET", "/.well-known/angharad-configuration", NULL, ANGHARAD_CALLBACK_PRIORITY_APPLICATION, &callback_angharad_server_configuration, (void*)config);
    u_map_put(config->instance->default_headers, "Access-Control-Allow-Origin", config->allow_origin);
    u_map_put(config->instance->default_headers, "Access-Control-Allow-Credentials", "true");
    u_map_put(config->instance->default_headers, "Cache-Control", "no-store");
    u_map_put(config->instance->default_headers, "Pragma", "no-cache");

    // Start event thread
    config->angharad_status = ANGHARAD_STATUS_RUN;
    thread_scheduler_ret = pthread_create(&thread_scheduler, NULL, thread_scheduler_run, (void *)config);
    thread_scheduler_detach = pthread_detach(thread_scheduler);
    if (thread_scheduler_ret || thread_scheduler_detach) {
      y_log_message(Y_LOG_LEVEL_ERROR, "init_angharad - Error creating or detaching event thread, return code: %d, detach code: %d",
                  thread_scheduler_ret, thread_scheduler_detach);
      return A_ERROR_IO;
    }

    y_log_message(Y_LOG_LEVEL_INFO, "Starting angharad core services on prefix %s", config->url_prefix_angharad);
    return A_OK;

  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "init_agharad - Error closing angharad webservices, error input parameters");
    return A_ERROR_PARAM;
  }
}

/**
 * Closes angharad server
 * Close all the endpoints and send a stop signal to all threads
 */
int close_angharad(struct config_elements * config) {
  if (config != NULL && config->instance != NULL && config->url_prefix_angharad) {
    ulfius_remove_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/alert/@submodule_name/@source/@element/@message/");

    ulfius_remove_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/submodule/");
    ulfius_remove_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/submodule/@submodule_name");

    ulfius_remove_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/script/");
    ulfius_remove_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/script/@script_name");
    ulfius_remove_endpoint_by_val(config->instance, "POST", config->url_prefix_angharad, "/script/");
    ulfius_remove_endpoint_by_val(config->instance, "PUT", config->url_prefix_angharad, "/script/@script_name");
    ulfius_remove_endpoint_by_val(config->instance, "DELETE", config->url_prefix_angharad, "/script/@script_name");
    ulfius_remove_endpoint_by_val(config->instance, "PUT", config->url_prefix_angharad, "/script/@script_name/@tag");
    ulfius_remove_endpoint_by_val(config->instance, "DELETE", config->url_prefix_angharad, "/script/@script_name/@tag");
    ulfius_remove_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/script/@script_name/run");

    ulfius_remove_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/scheduler/");
    ulfius_remove_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/scheduler/@scheduler_name");
    ulfius_remove_endpoint_by_val(config->instance, "POST", config->url_prefix_angharad, "/scheduler/");
    ulfius_remove_endpoint_by_val(config->instance, "PUT", config->url_prefix_angharad, "/scheduler/@scheduler_name");
    ulfius_remove_endpoint_by_val(config->instance, "DELETE", config->url_prefix_angharad, "/scheduler/@scheduler_name");
    ulfius_remove_endpoint_by_val(config->instance, "PUT", config->url_prefix_angharad, "/scheduler/@scheduler_name/@tag");
    ulfius_remove_endpoint_by_val(config->instance, "DELETE", config->url_prefix_angharad, "/scheduler/@scheduler_name/@tag");

    ulfius_remove_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/trigger/");
    ulfius_remove_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/trigger/@trigger_name");
    ulfius_remove_endpoint_by_val(config->instance, "POST", config->url_prefix_angharad, "/trigger/");
    ulfius_remove_endpoint_by_val(config->instance, "PUT", config->url_prefix_angharad, "/trigger/@trigger_name");
    ulfius_remove_endpoint_by_val(config->instance, "DELETE", config->url_prefix_angharad, "/trigger/@trigger_name");
    ulfius_remove_endpoint_by_val(config->instance, "PUT", config->url_prefix_angharad, "/trigger/@trigger_name/@tag");
    ulfius_remove_endpoint_by_val(config->instance, "DELETE", config->url_prefix_angharad, "/trigger/@trigger_name/@tag");

    ulfius_remove_endpoint_by_val(config->instance, "GET", NULL, "*");

    ulfius_remove_endpoint_by_val(config->instance, "GET", NULL, "/");

    if (config->angharad_status == ANGHARAD_STATUS_RUN) {
      config->angharad_status = ANGHARAD_STATUS_STOPPING;
      while (config->angharad_status != ANGHARAD_STATUS_STOP) {
        usleep(50);
      }
    }

    return A_OK;
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "close_agharad - Error closing angharad webservices, error input parameters");
    return A_ERROR_PARAM;
  }
}

/**
 *
 * Read the content of a file and return it as a char *
 * returned value must be free'd after use
 *
 */
char * get_file_content(const char * file_path) {
  char * buffer = NULL;
  size_t length, res;
  FILE * f;

  f = fopen (file_path, "rb");
  if (f) {
    fseek (f, 0, SEEK_END);
    length = (size_t)ftell (f);
    fseek (f, 0, SEEK_SET);
    buffer = o_malloc((length+1)*sizeof(char));
    if (buffer) {
      res = fread (buffer, 1, length, f);
      if (res != length) {
        fprintf(stderr, "fread warning, reading %zu while expecting %zu", res, length);
      }
      // Add null character at the end of buffer, just in case
      buffer[length] = '\0';
    }
    fclose (f);
  }
  
  return buffer;
}

int set_response_json_body_and_clean(struct _u_response * response, uint status, json_t * json_body) {
  int res = ulfius_set_json_body_response(response, status, json_body);
  json_decref(json_body);
  return res;
}

/**
 * Check if the result json object has a "result" element that is equal to value
 */
int check_result_value(json_t * result, const int value) {
  return (json_is_integer(json_object_get(result, "result")) &&
          json_integer_value(json_object_get(result, "result")) == value);
}

/**
 * json_t * json_search(json_t * haystack, json_t * needle)
 * jansson library addon
 * Look for an occurence of needle within haystack
 * If needle is present in haystack, return the reference to the json_t * that is equal to needle
 * If needle is not found, return NULL
 */
json_t * json_search(json_t * haystack, json_t * needle) {
  json_t * value1 = NULL, * value2 = NULL;
  size_t index = 0;
  const char * key = NULL;

  if (!haystack || !needle)
    return NULL;

  if (haystack == needle)
    return haystack;

  // If both haystack and needle are the same type, test them
  if (json_typeof(haystack) == json_typeof(needle) && !json_is_object(haystack))
    if (json_equal(haystack, needle))
      return haystack;

  // If they are not equals, test json_search in haystack elements recursively if it's an array or an object
  if (json_is_array(haystack)) {
    json_array_foreach(haystack, index, value1) {
      if (json_equal(value1, needle)) {
        return value1;
      } else {
        value2 = json_search(value1, needle);
        if (value2 != NULL) {
          return value2;
        }
      }
    }
  } else if (json_is_object(haystack) && json_is_object(needle)) {
    int same = 1;
    json_object_foreach(needle, key, value1) {
      value2 = json_object_get(haystack, key);
      if (!json_equal(value1, value2)) {
        same = 0;
      }
    }
    if (same) {
      return haystack;
    }
  } else if (json_is_object(haystack)) {
    json_object_foreach(haystack, key, value1) {
      if (json_equal(value1, needle)) {
        return value1;
      } else {
        value2 = json_search(value1, needle);
        if (value2 != NULL) {
          return value2;
        }
      }
    }
  }
  return NULL;
}
