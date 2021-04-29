/**
 *
 * Gareth messenger service
 *
 * Get messages from a REST Webservice
 * Send messages or digested data when previously parametered filters are triggered
 * Send protocols available: http, smtp
 *
 * run a gareth standalone server
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

#include <libconfig.h>
#include <getopt.h>
#include <signal.h>
#include <string.h>

#include "gareth.h"

#define GARETH_RUNNING  0
#define GARETH_STOP     1
#define GARETH_ERROR    2

// Configuration management
#define GARETH_DEFAULT_PREFIX "gareth"
#define GARETH_DEFAULT_PORT 4784

struct config_elements {
  char *                 config_file;
  char *                 url_prefix;
  unsigned long          log_mode;
  unsigned long          log_level;
  char *                 log_file;
  struct _h_connection * conn;
  struct _u_instance   * instance;
};

int global_handler_variable;

int  build_config_from_args(int argc, char ** argv, struct config_elements * config);
int  build_config_from_file(struct config_elements * config);
int  check_config(struct config_elements * config);
void exit_handler(int handler);
void exit_server(struct config_elements ** config, int exit_value);
void print_help(FILE * output);

int callback_default (const struct _u_request * request, struct _u_response * response, void * user_data);

/**
 * Initialize the application configuration based on the command line parameters
 */
int build_config_from_args(int argc, char ** argv, struct config_elements * config) {
  int next_option;
  const char * short_options = "c::p::b::u::d::a::s::m::l::f::r::h::";
  char * tmp = NULL, * to_free = NULL, * one_log_mode = NULL;
  static const struct option long_options[]= {
    {"config-file", optional_argument,NULL, 'c'},
    {"port", optional_argument,NULL, 'p'},
    {"url-prefix", optional_argument,NULL, 'u'},
    {"log-mode", optional_argument,NULL, 'm'},
    {"log-level", optional_argument,NULL, 'l'},
    {"log-file", optional_argument,NULL, 'f'},
    {"help", optional_argument,NULL, 'h'},
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
              exit_server(&config, GARETH_STOP);
            }
          } else {
            fprintf(stderr, "Error!\nNo config file specified\n");
            return 0;
          }
          break;
        case 'p':
          if (optarg != NULL) {
            config->instance->port = strtol(optarg, NULL, 10);
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
            config->url_prefix = o_strdup(optarg);
            if (config->url_prefix == NULL) {
              fprintf(stderr, "Error allocating config->url_prefix, exiting\n");
              exit_server(&config, GARETH_STOP);
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
              exit_server(&config, GARETH_STOP);
            }
            one_log_mode = strtok(tmp, ",");
            while (one_log_mode != NULL) {
              if (0 == strncmp("console", one_log_mode, strlen("console"))) {
                config->log_mode |= Y_LOG_MODE_CONSOLE;
              } else if (0 == strncmp("syslog", one_log_mode, strlen("syslog"))) {
                config->log_mode |= Y_LOG_MODE_SYSLOG;
              } else if (0 == strncmp("file", one_log_mode, strlen("file"))) {
                config->log_mode |= Y_LOG_MODE_FILE;
              }
              one_log_mode = strtok(NULL, ",");
            }
            free(to_free);
          } else {
            fprintf(stderr, "Error!\nNo mode specified\n");
            return 0;
          }
          break;
        case 'l':
          if (optarg != NULL) {
            if (0 == strncmp("NONE", optarg, strlen("NONE"))) {
              config->log_level = Y_LOG_LEVEL_NONE;
            } else if (0 == strncmp("ERROR", optarg, strlen("ERROR"))) {
              config->log_level = Y_LOG_LEVEL_ERROR;
            } else if (0 == strncmp("WARNING", optarg, strlen("WARNING"))) {
              config->log_level = Y_LOG_LEVEL_WARNING;
            } else if (0 == strncmp("INFO", optarg, strlen("INFO"))) {
              config->log_level = Y_LOG_LEVEL_INFO;
            } else if (0 == strncmp("DEBUG", optarg, strlen("DEBUG"))) {
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
              exit_server(&config, GARETH_STOP);
            }
          } else {
            fprintf(stderr, "Error!\nNo log file specified\n");
            return 0;
          }
          break;
        case 'h':
          exit_server(&config, GARETH_STOP);
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
  fprintf(output, "\nGareth Messaging REST Webservice\n");
  fprintf(output, "\n");
  fprintf(output, "Messaging system using a JSON/REST interface\n");
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
  fprintf(output, "-h --help\n");
  fprintf(output, "\tPrint this message\n\n");
}

/**
 * Exit properly the server by closing opened connections, databases and files
 */
void exit_server(struct config_elements ** config, int exit_value) {
  
  if (config != NULL && *config != NULL) {
    // Cleaning data

    free((*config)->config_file);
    free((*config)->url_prefix);
    free((*config)->log_file);
    h_close_db((*config)->conn);
    h_clean_connection((*config)->conn);
    ulfius_stop_framework((*config)->instance);
    ulfius_clean_instance((*config)->instance);
    free((*config)->instance);
    y_close_logs();
    
    free(*config);
    (*config) = NULL;
  }
  exit(exit_value);
}

/**
 * handles signal catch to exit properly when ^C is used for example
 */
void exit_handler(int signal) {
  y_log_message(Y_LOG_LEVEL_INFO, "Gareth caught a stop or kill signal (%d), exiting", signal);
  global_handler_variable = GARETH_STOP;
}

/**
 * Initialize the application configuration based on the config file content
 * Read the config file, get mandatory variables and devices
 */
int build_config_from_file(struct config_elements * config) {
  
  config_t cfg;
  config_setting_t * root, * database;
  const char * cur_prefix, * cur_log_mode, * cur_log_level, * cur_log_file = NULL, * one_log_mode, 
             * db_type, * db_sqlite_path, * db_mariadb_host = NULL, * db_mariadb_user = NULL, * db_mariadb_password = NULL, * db_mariadb_dbname = NULL;
  int db_mariadb_port = 0;
  
  config_init(&cfg);
  
  if (!config_read_file(&cfg, config->config_file)) {
    fprintf(stderr, "Error parsing config file %s\nOn line %d error: %s\n", config_error_file(&cfg), config_error_line(&cfg), config_error_text(&cfg));
    config_destroy(&cfg);
    return 0;
  }
  
  if (config->instance->port == -1) {
    // Get Port number to listen to
    int port;
    config_lookup_int(&cfg, "port", &port);
    config->instance->port = port;
  }
  
  if (config->url_prefix == NULL) {
    // Get prefix url
    if (config_lookup_string(&cfg, "url_prefix", &cur_prefix)) {
      config->url_prefix = o_strdup(cur_prefix);
      if (config->url_prefix == NULL) {
        fprintf(stderr, "Error allocating config->url_prefix, exiting\n");
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
        if (0 == strncmp("console", one_log_mode, strlen("console"))) {
          config->log_mode |= Y_LOG_MODE_CONSOLE;
        } else if (0 == strncmp("syslog", one_log_mode, strlen("syslog"))) {
          config->log_mode |= Y_LOG_MODE_SYSLOG;
        } else if (0 == strncmp("file", one_log_mode, strlen("file"))) {
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
      if (0 == strncmp("NONE", cur_log_level, strlen("NONE"))) {
        config->log_level = Y_LOG_LEVEL_NONE;
      } else if (0 == strncmp("ERROR", cur_log_level, strlen("ERROR"))) {
        config->log_level = Y_LOG_LEVEL_ERROR;
      } else if (0 == strncmp("WARNING", cur_log_level, strlen("WARNING"))) {
        config->log_level = Y_LOG_LEVEL_WARNING;
      } else if (0 == strncmp("INFO", cur_log_level, strlen("INFO"))) {
        config->log_level = Y_LOG_LEVEL_INFO;
      } else if (0 == strncmp("DEBUG", cur_log_level, strlen("DEBUG"))) {
        config->log_level = Y_LOG_LEVEL_DEBUG;
      }
    }
  }

  if (!y_init_logs(GARETH_LOG_NAME, config->log_mode, config->log_level, config->log_file, "Starting Gareth alert and messenger service")) {
    fprintf(stderr, "Error initializing logs\n");
    exit_server(&config, GARETH_ERROR);
  }
    
  root = config_root_setting(&cfg);
  database = config_setting_get_member(root, "database");
  if (database != NULL) {
    if (config_setting_lookup_string(database, "type", &db_type) == CONFIG_TRUE) {
      if (0 == strncmp(db_type, "sqlite3", strlen("sqlite3"))) {
        if (config_setting_lookup_string(database, "path", &db_sqlite_path) == CONFIG_TRUE) {
          config->conn = h_connect_sqlite(db_sqlite_path);
          if (config->conn == NULL) {
            config_destroy(&cfg);
            fprintf(stderr, "Error opening sqlite database %s\n", db_sqlite_path);
            return 0;
          }
        } else {
          config_destroy(&cfg);
          fprintf(stderr, "Error, no sqlite database specified\n");
          return 0;
        }
      } else if (0 == strncmp(db_type, "mariadb", strlen("mariadb"))) {
        config_setting_lookup_string(database, "host", &db_mariadb_host);
        config_setting_lookup_string(database, "user", &db_mariadb_user);
        config_setting_lookup_string(database, "password", &db_mariadb_password);
        config_setting_lookup_string(database, "dbname", &db_mariadb_dbname);
        config_setting_lookup_int(database, "port", &db_mariadb_port);
        config->conn = h_connect_mariadb(db_mariadb_host, db_mariadb_user, db_mariadb_password, db_mariadb_dbname, db_mariadb_port, NULL);
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
  
  config_destroy(&cfg);
  return 1;
}

/**
 * Check if all mandatory configuration parameters are present and correct
 * Initialize some parameters with default value if not set
 */
int check_config(struct config_elements * config) {

  if (config->instance->port == 1) {
    config->instance->port = GARETH_DEFAULT_PORT;
  }
  
  if (config->url_prefix == NULL) {
    config->url_prefix = o_strdup(GARETH_DEFAULT_PREFIX);
    if (config->url_prefix == NULL) {
      fprintf(stderr, "Error allocating url_prefix, exit\n");
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
  
  return 1;
}

int callback_default (const struct _u_request * request, struct _u_response * response, void * user_data) {
  set_response_json_body_and_clean(response, 404, json_pack("{ssssss}", "error", "page not found", "message", "The page can not be found, check documentation", "url", request->http_url));
  return H_OK;
}

/**
 * Main function
 * Gareth application entry point
 * Initialise the database connection
 * Starts the webservice
 * Infinite loop until an end signal is triggered
 */
int main(int argc, char ** argv) {
  struct config_elements * config = malloc(sizeof(struct config_elements));
  if (config == NULL) {
    fprintf(stderr, "Memory error - config\n");
    return 1;
  }
  
  config->config_file = NULL;
  config->url_prefix = NULL;
  config->log_mode = Y_LOG_MODE_NONE;
  config->log_level = Y_LOG_LEVEL_NONE;
  config->log_file = NULL;
  config->conn = NULL;
  config->instance = malloc(sizeof(struct _u_instance));
  if (config->instance == NULL) {
    fprintf(stderr, "Memory error - config->instance\n");
    return 1;
  }
  ulfius_init_instance(config->instance, 1, NULL, NULL);

  global_handler_variable = GARETH_RUNNING;
  // Catch end signals to make a clean exit
  signal (SIGQUIT, exit_handler);
  signal (SIGINT, exit_handler);
  signal (SIGTERM, exit_handler);
  signal (SIGHUP, exit_handler);

  // First we parse command line arguments
  if (!build_config_from_args(argc, argv, config)) {
    fprintf(stderr, "Error reading command-line parameters\n");
    print_help(stderr);
    exit_server(&config, GARETH_ERROR);
  }
  
  // Then we parse configuration file
  // They have lower priority than command line parameters
  if (!build_config_from_file(config)) {
    fprintf(stderr, "Error config file\n");
    exit_server(&config, GARETH_ERROR);
  }
  
  // Check if all mandatory configuration variables are present and correctly typed
  if (!check_config(config)) {
    fprintf(stderr, "Error initializing configuration\n");
    exit_server(&config, GARETH_ERROR);
  }
  
  // Initialize gareth webservice
  if (!init_gareth(config->instance, config->url_prefix, config->conn)) {
    fprintf(stderr, "Error initializing gareth webservice\n");
    exit_server(&config, GARETH_ERROR);
  }
  
  // Default endpoint
  ulfius_set_default_endpoint(config->instance, &callback_default, (void*)config->conn);
    
  // Start the webservice
  y_log_message(Y_LOG_LEVEL_INFO, "Start gareth on port %d, prefix: %s", config->instance->port, config->url_prefix);
  if (ulfius_start_framework(config->instance) == U_OK) {
    while (global_handler_variable == GARETH_RUNNING) {
      sleep(1);
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "Error starting gareth webserver");
    exit_server(&config, GARETH_ERROR);
  }
  exit_server(&config, GARETH_STOP);
  return 0;
}

int set_response_json_body_and_clean(struct _u_response * response, uint status, json_t * json_body) {
  int res = ulfius_set_json_body_response(response, status, json_body);
  json_decref(json_body);
  return res;
}
