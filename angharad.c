/**
 *
 * Angharad system
 *
 * Manage all subsystems and handles the house automation server:
 * - Benoic
 * - Gareth
 * - Carleon
 * 
 * Angharad server entry point
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

/**
 * Initialize the application configuration based on the command line parameters
 */
int build_config_from_args(int argc, char ** argv, struct config_elements * config) {
  int next_option;
  const char * short_options = "c::p::u::m::l::f::h::b::s";
  char * tmp = NULL, * to_free = NULL, * one_log_mode = NULL;
  static const struct option long_options[]= {
    {"config-file", optional_argument,NULL, 'c'},
    {"port", optional_argument,NULL, 'p'},
    {"url-prefix-angharad", optional_argument,NULL, 'u'},
    {"url-prefix-benoic", optional_argument,NULL, 'v'},
    {"url-prefix-carleon", optional_argument,NULL, 'w'},
    {"url-prefix-gareth", optional_argument,NULL, 'x'},
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
            config->config_file = nstrdup(optarg);
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
            config->url_prefix_angharad = nstrdup(optarg);
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
            config->url_prefix_benoic = nstrdup(optarg);
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
            config->url_prefix_carleon = nstrdup(optarg);
            if (config->url_prefix_carleon == NULL) {
              fprintf(stderr, "Error allocating config->url_prefix_carleon, exiting\n");
              exit_server(&config, ANGHARAD_STOP);
            }
          } else {
            fprintf(stderr, "Error!\nNo URL prefix specified\n");
            return 0;
          }
          break;
        case 'x':
          if (optarg != NULL) {
            config->url_prefix_gareth = nstrdup(optarg);
            if (config->url_prefix_gareth == NULL) {
              fprintf(stderr, "Error allocating config->url_prefix_gareth, exiting\n");
              exit_server(&config, ANGHARAD_STOP);
            }
          } else {
            fprintf(stderr, "Error!\nNo URL prefix specified\n");
            return 0;
          }
          break;
        case 'm':
          if (optarg != NULL) {
            tmp = nstrdup(optarg);
            if (tmp == NULL) {
              fprintf(stderr, "Error allocating log_mode, exiting\n");
              exit_server(&config, ANGHARAD_STOP);
            }
            one_log_mode = strtok(tmp, ",");
            while (one_log_mode != NULL) {
              if (0 == nstrncmp("console", one_log_mode, strlen("console"))) {
                config->log_mode |= Y_LOG_MODE_CONSOLE;
              } else if (0 == nstrncmp("syslog", one_log_mode, strlen("syslog"))) {
                config->log_mode |= Y_LOG_MODE_SYSLOG;
              } else if (0 == nstrncmp("file", one_log_mode, strlen("file"))) {
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
            if (0 == nstrncmp("NONE", optarg, strlen("NONE"))) {
              config->log_level = Y_LOG_LEVEL_NONE;
            } else if (0 == nstrncmp("ERROR", optarg, strlen("ERROR"))) {
              config->log_level = Y_LOG_LEVEL_ERROR;
            } else if (0 == nstrncmp("WARNING", optarg, strlen("WARNING"))) {
              config->log_level = Y_LOG_LEVEL_WARNING;
            } else if (0 == nstrncmp("INFO", optarg, strlen("INFO"))) {
              config->log_level = Y_LOG_LEVEL_INFO;
            } else if (0 == nstrncmp("DEBUG", optarg, strlen("DEBUG"))) {
              config->log_level = Y_LOG_LEVEL_DEBUG;
            }
          } else {
            fprintf(stderr, "Error!\nNo log level specified\n");
            return 0;
          }
          break;
        case 'f':
          if (optarg != NULL) {
            config->log_file = nstrdup(optarg);
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
            config->b_config->modules_path = nstrdup(optarg);
          } else {
            fprintf(stderr, "Error!\nNo benoic modules path specified\n");
            return 0;
          }
          break;
        case 's':
          if (optarg != NULL) {
            config->c_config->services_path = nstrdup(optarg);
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
    close_carleon((*config)->instance, (*config)->url_prefix_carleon, (*config)->c_config);
    close_angharad((*config));
    h_close_db((*config)->conn);
    h_clean_connection((*config)->conn);
    ulfius_stop_framework((*config)->instance);
    ulfius_clean_instance((*config)->instance);
    clean_benoic((*config)->b_config);
    clean_carleon((*config)->c_config);
    free((*config)->instance);
    free((*config)->config_file);
    free((*config)->url_prefix_angharad);
    free((*config)->url_prefix_benoic);
    free((*config)->url_prefix_carleon);
    free((*config)->url_prefix_gareth);
    free((*config)->static_files_path);
    free((*config)->static_files_prefix);
    u_map_clean_full((*config)->mime_types);
    free((*config)->allow_origin);
    free((*config)->log_file);
    if ((*config)->auth_ldap != NULL) {
      free((*config)->auth_ldap->uri);
      free((*config)->auth_ldap->bind_dn);
      free((*config)->auth_ldap->bind_passwd);
      free((*config)->auth_ldap->filter);
      free((*config)->auth_ldap->login_property);
      free((*config)->auth_ldap->base_search);
      free((*config)->auth_ldap);
    }
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
  y_log_message(Y_LOG_LEVEL_INFO, "Caught a stop or kill signal (%d), exiting", signal);
  global_handler_variable = ANGHARAD_STOP;
}

/**
 * Initialize the application configuration based on the config file content
 * Read the config file, get mandatory variables and devices
 */
int build_config_from_file(struct config_elements * config) {
  
  config_t cfg;
  config_setting_t * root, * database, * auth;
  const char * cur_prefix_angharad, * cur_prefix_benoic, * cur_prefix_carleon, * cur_prefix_gareth, * cur_log_mode, * cur_log_level, * cur_log_file = NULL, * one_log_mode, * carleon_services_path, * benoic_modules_path, * cur_allow_origin, * cur_static_files_prefix,
             * db_type, * db_sqlite_path, * db_mariadb_host = NULL, * db_mariadb_user = NULL, * db_mariadb_password = NULL, * db_mariadb_dbname = NULL, * cur_static_files_path = NULL,
             * cur_auth_ldap_uri = NULL, * cur_auth_ldap_bind_dn = NULL, * cur_auth_ldap_bind_passwd = NULL, * cur_auth_ldap_filter = NULL, * cur_auth_ldap_login_property = NULL, * cur_auth_ldap_base_search = NULL;
  int db_mariadb_port = 0;
  int cur_database_auth = 0, cur_ldap_auth = 0;
  
  config_init(&cfg);
  
  if (!config_read_file(&cfg, config->config_file)) {
    fprintf(stderr, "Error parsing config file %s\nOn line %d error: %s\n", config_error_file(&cfg), config_error_line(&cfg), config_error_text(&cfg));
    config_destroy(&cfg);
    return 0;
  }
  
  if (config->instance->port == -1) {
    // Get Port number to listen to
    config_lookup_int(&cfg, "port", &(config->instance->port));
  }
  
  if (config->url_prefix_angharad == NULL) {
    // Get prefix url for angharad
    if (config_lookup_string(&cfg, "url_prefix_angharad", &cur_prefix_angharad)) {
      config->url_prefix_angharad = nstrdup(cur_prefix_angharad);
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
      config->url_prefix_benoic = nstrdup(cur_prefix_benoic);
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
      config->url_prefix_carleon = nstrdup(cur_prefix_carleon);
      if (config->url_prefix_carleon == NULL) {
        fprintf(stderr, "Error allocating config->url_prefix_carleon, exiting\n");
        config_destroy(&cfg);
        return 0;
      }
    }
  }

  if (config->url_prefix_gareth == NULL) {
    // Get prefix url for gareth
    if (config_lookup_string(&cfg, "url_prefix_gareth", &cur_prefix_gareth)) {
      config->url_prefix_gareth = nstrdup(cur_prefix_gareth);
      if (config->url_prefix_gareth == NULL) {
        fprintf(stderr, "Error allocating config->url_prefix_gareth, exiting\n");
        config_destroy(&cfg);
        return 0;
      }
    }
  }

  if (config->allow_origin == NULL) {
    // Get allow-origin value for CORS
    if (config_lookup_string(&cfg, "allow_origin", &cur_allow_origin)) {
      config->allow_origin = nstrdup(cur_allow_origin);
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
      config->c_config->services_path = nstrdup(carleon_services_path);
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
      config->b_config->modules_path = nstrdup(benoic_modules_path);
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
        if (0 == nstrncmp("console", one_log_mode, strlen("console"))) {
          config->log_mode |= Y_LOG_MODE_CONSOLE;
        } else if (0 == nstrncmp("syslog", one_log_mode, strlen("syslog"))) {
          config->log_mode |= Y_LOG_MODE_SYSLOG;
        } else if (0 == nstrncmp("file", one_log_mode, strlen("file"))) {
          config->log_mode |= Y_LOG_MODE_FILE;
          // Get log file path
          if (config->log_file == NULL) {
            if (config_lookup_string(&cfg, "log_file", &cur_log_file)) {
              config->log_file = nstrdup(cur_log_file);
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
      if (0 == nstrncmp("NONE", cur_log_level, strlen("NONE"))) {
        config->log_level = Y_LOG_LEVEL_NONE;
      } else if (0 == nstrncmp("ERROR", cur_log_level, strlen("ERROR"))) {
        config->log_level = Y_LOG_LEVEL_ERROR;
      } else if (0 == nstrncmp("WARNING", cur_log_level, strlen("WARNING"))) {
        config->log_level = Y_LOG_LEVEL_WARNING;
      } else if (0 == nstrncmp("INFO", cur_log_level, strlen("INFO"))) {
        config->log_level = Y_LOG_LEVEL_INFO;
      } else if (0 == nstrncmp("DEBUG", cur_log_level, strlen("DEBUG"))) {
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
      if (0 == nstrncmp(db_type, "sqlite3", strlen("sqlite3"))) {
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
      } else if (0 == nstrncmp(db_type, "mariadb", strlen("mariadb"))) {
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

  auth = config_setting_get_member(root, "authentication");
  if (auth != NULL) {
    config_setting_lookup_bool(auth, "database_auth", &cur_database_auth);
    config->has_auth_database = cur_database_auth;
    config_setting_lookup_bool(auth, "ldap_auth", &cur_ldap_auth);
    config->has_auth_ldap = cur_ldap_auth;
    
    if (config->has_auth_ldap) {
      config_setting_lookup_string(auth, "uri", &cur_auth_ldap_uri);
      config_setting_lookup_string(auth, "bind_dn", &cur_auth_ldap_bind_dn);
      config_setting_lookup_string(auth, "bind_passwd", &cur_auth_ldap_bind_passwd);
      config_setting_lookup_string(auth, "filter", &cur_auth_ldap_filter);
      config_setting_lookup_string(auth, "login_property", &cur_auth_ldap_login_property);
      config_setting_lookup_string(auth, "base_search", &cur_auth_ldap_base_search);
      if (cur_auth_ldap_uri != NULL && cur_auth_ldap_bind_dn != NULL && cur_auth_ldap_bind_passwd != NULL && cur_auth_ldap_filter != NULL && cur_auth_ldap_login_property != NULL && cur_auth_ldap_base_search != NULL) {
        config->auth_ldap = malloc(sizeof(struct _auth_ldap));
        if (config->auth_ldap == NULL) {
          config_destroy(&cfg);
          fprintf(stderr, "Error allocating resources for config->auth_ldap\n");
          return 0;
        } else {
          config->auth_ldap->uri = nstrdup(cur_auth_ldap_uri);
          if (config->auth_ldap->uri == NULL) {
            config_destroy(&cfg);
            fprintf(stderr, "Error allocating resources for config->auth_ldap->uri\n");
            return 0;
          }
          config->auth_ldap->bind_dn = nstrdup(cur_auth_ldap_bind_dn);
          if (config->auth_ldap->bind_dn == NULL) {
            config_destroy(&cfg);
            fprintf(stderr, "Error allocating resources for config->auth_ldap->bind_dn\n");
            return 0;
          }
          config->auth_ldap->bind_passwd = nstrdup(cur_auth_ldap_bind_passwd);
          if (config->auth_ldap->bind_passwd == NULL) {
            config_destroy(&cfg);
            fprintf(stderr, "Error allocating resources for config->auth_ldap->bind_passwd\n");
            return 0;
          }
          config->auth_ldap->filter = nstrdup(cur_auth_ldap_filter);
          if (config->auth_ldap->filter == NULL) {
            config_destroy(&cfg);
            fprintf(stderr, "Error allocating resources for config->auth_ldap->filter\n");
            return 0;
          }
          config->auth_ldap->login_property = nstrdup(cur_auth_ldap_login_property);
          if (config->auth_ldap->login_property == NULL) {
            config_destroy(&cfg);
            fprintf(stderr, "Error allocating resources for config->auth_ldap->login_property\n");
            return 0;
          }
          config->auth_ldap->base_search = nstrdup(cur_auth_ldap_base_search);
          if (config->auth_ldap->base_search == NULL) {
            config_destroy(&cfg);
            fprintf(stderr, "Error allocating resources for config->auth_ldap->base_search\n");
            return 0;
          }
        }
      } else {
        config_destroy(&cfg);
        fprintf(stderr, "Error, auth ldap error parameters\n");
        return 0;
      }
    }
  }
  
  if (config->static_files_path == NULL) {
    // Get path that serve static files
    if (config_lookup_string(&cfg, "static_files_path", &cur_static_files_path)) {
      config->static_files_path = nstrdup(cur_static_files_path);
      if (config->static_files_path == NULL) {
        fprintf(stderr, "Error allocating config->static_files_path, exiting\n");
        config_destroy(&cfg);
        return 0;
      }
    }
  }

  if (config->static_files_prefix == NULL) {
    // Get prefix url
    if (config_lookup_string(&cfg, "static_files_prefix", &cur_static_files_prefix)) {
      config->static_files_prefix = nstrdup(cur_static_files_prefix);
      if (config->static_files_prefix == NULL) {
        fprintf(stderr, "Error allocating config->static_files_prefix, exiting\n");
        config_destroy(&cfg);
        return 0;
      }
    }
  }
    
  config_destroy(&cfg);
  return 1;
}

/**
 * Check if all mandatory configuration parameters are present and correct
 * Initialize some parameters with default value if not set
 */
int check_config(struct config_elements * config) {

  if (config->instance->port == -1) {
    config->instance->port = ANGHARAD_DEFAULT_PORT;
  }
  
  if (config->url_prefix_angharad == NULL) {
    config->url_prefix_angharad = nstrdup(ANGHARAD_DEFAULT_PREFIX);
    if (config->url_prefix_angharad == NULL) {
      fprintf(stderr, "Error allocating url_prefix_angharad, exit\n");
      return 0;
    }
  }
  
  if (config->url_prefix_benoic == NULL) {
    config->url_prefix_benoic = nstrdup(BENOIC_DEFAULT_PREFIX);
    if (config->url_prefix_benoic == NULL) {
      fprintf(stderr, "Error allocating url_prefix_benoic, exit\n");
      return 0;
    }
  }
  
  if (config->url_prefix_carleon == NULL) {
    config->url_prefix_carleon = nstrdup(CARLEON_DEFAULT_PREFIX);
    if (config->url_prefix_carleon == NULL) {
      fprintf(stderr, "Error allocating url_prefix_carleon, exit\n");
      return 0;
    }
  }
  
  if (config->url_prefix_gareth == NULL) {
    config->url_prefix_gareth = nstrdup(GARETH_DEFAULT_PREFIX);
    if (config->url_prefix_gareth == NULL) {
      fprintf(stderr, "Error allocating url_prefix_gareth, exit\n");
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
    config->allow_origin = nstrdup(ALLOW_ORIGIN_DEFAULT);
  }

  if (config->static_files_prefix == NULL) {
    config->static_files_prefix = nstrdup(STATIC_FILES_PREFIX_DEFAULT);
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

int callback_default (const struct _u_request * request, struct _u_response * response, void * user_data) {
  response->json_body = json_pack("{ssssss}", "error", "page not found", "message", "The page can not be found, check documentation", "method", request->http_verb, "url", request->http_url);
  response->status = 404;
  return H_OK;
}

/**
 * Main function
 * Benoic application entry point
 * Initialise the database connection
 * Starts the webservice
 * Infinite loop until an end signal is triggered
 */
int main(int argc, char ** argv) {
  struct config_elements * config = malloc(sizeof(struct config_elements));
  int res;
  json_t * submodule;
  
  global_handler_variable = ANGHARAD_RUNNING;
  // Catch end signals to make a clean exit
  signal (SIGQUIT, exit_handler);
  signal (SIGINT, exit_handler);
  signal (SIGTERM, exit_handler);
  signal (SIGHUP, exit_handler);

  if (config == NULL) {
    fprintf(stderr, "Memory error - config\n");
    return 1;
  }
  
  config->config_file = NULL;
  config->url_prefix_angharad = NULL;
  config->url_prefix_benoic = NULL;
  config->url_prefix_carleon = NULL;
  config->url_prefix_gareth = NULL;
  config->static_files_path = NULL;
  config->static_files_prefix = NULL;
  config->allow_origin = NULL;
  config->log_mode = Y_LOG_MODE_NONE;
  config->log_level = Y_LOG_LEVEL_NONE;
  config->log_file = NULL;
  config->angharad_status = ANGHARAD_STATUS_STOP;
  config->has_auth_database = 0;
  config->has_auth_ldap = 0;
  config->auth_ldap = NULL;
  config->instance = malloc(sizeof(struct _u_instance));
  config->c_config = malloc(sizeof(struct _carleon_config));
  config->b_config = malloc(sizeof(struct _benoic_config));
  if (config->instance == NULL || config->c_config == NULL || config->b_config == NULL) {
    fprintf(stderr, "Memory error - config->instance || config->c_config || config->b_config\n");
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
  ulfius_init_instance(config->instance, -1, NULL);

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
  
  // Initialize benoic webservice if enabled
  submodule = submodule_get(config, ANGHARAD_SUBMODULE_CARLEON);
  if (submodule != NULL && json_integer_value(json_object_get(submodule, "result")) == A_OK && json_object_get(json_object_get(submodule, "submodule"), "enabled") == json_true()) {
    res = init_carleon(config->instance, config->url_prefix_carleon, config->c_config);
    if (res != C_OK) {
      y_log_message(Y_LOG_LEVEL_ERROR, "Error initializing carleon webservice: %d", res);
      json_decref(submodule);
      exit_server(&config, ANGHARAD_ERROR);
    }
  }
  json_decref(submodule);

  // Initialize benoic webservice if enabled
  submodule = submodule_get(config, ANGHARAD_SUBMODULE_GARETH);
  if (submodule != NULL && json_integer_value(json_object_get(submodule, "result")) == A_OK && json_object_get(json_object_get(submodule, "submodule"), "enabled") == json_true()) {
    res = init_gareth(config->instance, config->url_prefix_gareth, config->conn);
    if (!res) {
      y_log_message(Y_LOG_LEVEL_ERROR, "Error initializing gareth webservice: %d", res);
      json_decref(submodule);
      exit_server(&config, ANGHARAD_ERROR);
    }
  }
  json_decref(submodule);
  
  // Initialize angharad webservice
  res = init_angharad(config);
  if (res != A_OK) {
    y_log_message(Y_LOG_LEVEL_ERROR, "Error initializing angharad webservice: %d", res);
    exit_server(&config, ANGHARAD_ERROR);
  }
  
  // Default endpoint
  ulfius_set_default_endpoint(config->instance, NULL, NULL, NULL, &callback_angharad_default, (void*)config);
  
  // Start the webservice
  y_log_message(Y_LOG_LEVEL_INFO, "Start angharad on port %d", config->instance->port);
  if (ulfius_start_framework(config->instance) == U_OK) {
    while (global_handler_variable == ANGHARAD_RUNNING) {
      sleep(1);
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "Error starting angharad webserver");
    exit_server(&config, ANGHARAD_ERROR);
  }
  exit_server(&config, ANGHARAD_STOP);
  return 0;
}

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

// Set the db flag enabled to true or false, and init or close the specified submodule if the status has changed
int submodule_enable(struct config_elements * config, const char * submodule, int enabled) {
  json_t * j_submodule = submodule_get(config, submodule), * j_query;
  int res;
  
  if (j_submodule != NULL && json_integer_value(json_object_get(j_submodule, "result")) == A_OK) {
    if (enabled && json_object_get(json_object_get(j_submodule, "submodule"), "enabled") == json_false()) {
      json_decref(j_submodule);
      // Enable disabled module
      j_query = json_pack("{sss{si}s{ss}}", "table", ANGHARAD_TABLE_SUBMODULE, "set", "as_enabled", 1, "where" ,"as_name", submodule);
      res = h_update(config->conn, j_query, NULL);
      json_decref(j_query);
      if (res == H_OK) {
        if (0 == nstrcmp(submodule, ANGHARAD_SUBMODULE_BENOIC)) {
          if (init_benoic(config->instance, config->url_prefix_benoic, config->b_config) != B_OK) {
            y_log_message(Y_LOG_LEVEL_ERROR, "submodule_enable - Error init benoic");
            return A_ERROR;
          }
          return A_OK;
        } else if (0 == nstrcmp(submodule, ANGHARAD_SUBMODULE_CARLEON)) {
          if (init_carleon(config->instance, config->url_prefix_carleon, config->c_config) != C_OK) {
            y_log_message(Y_LOG_LEVEL_ERROR, "submodule_enable - Error init carleon");
            return A_ERROR;
          }
          return A_OK;
        } else if (0 == nstrcmp(submodule, ANGHARAD_SUBMODULE_GARETH)) {
          if (!init_gareth(config->instance, config->url_prefix_gareth, config->conn)) {
            y_log_message(Y_LOG_LEVEL_ERROR, "submodule_enable - Error init gareth");
            return A_ERROR;
          }
          return A_OK;
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "submodule_enable - This shouldn't happen unless something ");
          return A_ERROR;
        }
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "submodule_enable - Error updating submodule status in database");
        return A_ERROR_DB;
      }
    } else if (!enabled && json_object_get(json_object_get(j_submodule, "submodule"), "enabled") == json_true()) {
      json_decref(j_submodule);
      // Disable enabled module
      j_query = json_pack("{sss{si}s{ss}}", "table", ANGHARAD_TABLE_SUBMODULE, "set", "as_enabled", 0, "where" ,"as_name", submodule);
      res = h_update(config->conn, j_query, NULL);
      json_decref(j_query);
      if (res == H_OK) {
        if (0 == nstrcmp(submodule, ANGHARAD_SUBMODULE_BENOIC)) {
          if (close_benoic(config->instance, config->url_prefix_benoic, config->b_config) != B_OK) {
            y_log_message(Y_LOG_LEVEL_ERROR, "submodule_enable - Error closing benoic");
            return A_ERROR;
          }
          return A_OK;
        } else if (0 == nstrcmp(submodule, ANGHARAD_SUBMODULE_CARLEON)) {
          if (close_carleon(config->instance, config->url_prefix_carleon, config->c_config) != C_OK) {
            y_log_message(Y_LOG_LEVEL_ERROR, "submodule_enable - Error closing carleon");
            return A_ERROR;
          }
          return A_OK;
        } else if (0 == nstrcmp(submodule, ANGHARAD_SUBMODULE_GARETH)) {
          if (!close_gareth(config->instance, config->url_prefix_gareth)) {
            y_log_message(Y_LOG_LEVEL_ERROR, "submodule_enable - Error closing gareth");
            return A_ERROR;
          }
          return A_OK;
        } else {
          y_log_message(Y_LOG_LEVEL_ERROR, "submodule_enable - This shouldn't happen unless something ");
          return A_ERROR;
        }
      } else {
        y_log_message(Y_LOG_LEVEL_ERROR, "submodule_enable - Error updating submodule status in database");
        return A_ERROR_DB;
      }
    } else {
      return A_OK;
    }
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "submodule_enable - Error getting submodule %s", submodule);
    json_decref(j_submodule);
    return A_ERROR_NOT_FOUND;
  }
}

int init_angharad(struct config_elements * config) {
  pthread_t thread_scheduler;
  int thread_scheduler_ret = 0, thread_scheduler_detach = 0;

  if (config != NULL && config->instance != NULL && config->url_prefix_angharad) {
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/alert/@submodule_name/@source/@element/@message/", NULL, NULL, NULL, &callback_angharad_alert, (void*)config);
    
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/submodule/", NULL, NULL, NULL, &callback_angharad_submodule_list, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/submodule/@submodule_name", NULL, NULL, NULL, &callback_angharad_submodule_get, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/submodule/@submodule_name/enable/@enabled", NULL, NULL, NULL, &callback_angharad_submodule_enable, (void*)config);

    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/script/", NULL, NULL, NULL, &callback_angharad_script_list, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/script/@script_name", NULL, NULL, NULL, &callback_angharad_script_get, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "POST", config->url_prefix_angharad, "/script/", NULL, NULL, NULL, &callback_angharad_script_add, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "PUT", config->url_prefix_angharad, "/script/@script_name", NULL, NULL, NULL, &callback_angharad_script_modify, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "DELETE", config->url_prefix_angharad, "/script/@script_name", NULL, NULL, NULL, &callback_angharad_script_remove, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "PUT", config->url_prefix_angharad, "/script/@script_name/@tag", NULL, NULL, NULL, &callback_angharad_script_add_tag, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "DELETE", config->url_prefix_angharad, "/script/@script_name/@tag", NULL, NULL, NULL, &callback_angharad_script_remove_tag, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/script/@script_name/run", NULL, NULL, NULL, &callback_angharad_script_run, (void*)config);

    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/scheduler/", NULL, NULL, NULL, &callback_angharad_scheduler_list, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/scheduler/@scheduler_name", NULL, NULL, NULL, &callback_angharad_scheduler_get, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/scheduler/@scheduler_name/enable/@enabled", NULL, NULL, NULL, &callback_angharad_scheduler_enable, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "POST", config->url_prefix_angharad, "/scheduler/", NULL, NULL, NULL, &callback_angharad_scheduler_add, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "PUT", config->url_prefix_angharad, "/scheduler/@scheduler_name", NULL, NULL, NULL, &callback_angharad_scheduler_modify, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "DELETE", config->url_prefix_angharad, "/scheduler/@scheduler_name", NULL, NULL, NULL, &callback_angharad_scheduler_remove, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "PUT", config->url_prefix_angharad, "/scheduler/@scheduler_name/@tag", NULL, NULL, NULL, &callback_angharad_scheduler_add_tag, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "DELETE", config->url_prefix_angharad, "/scheduler/@scheduler_name/@tag", NULL, NULL, NULL, &callback_angharad_scheduler_remove_tag, (void*)config);

    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/trigger/", NULL, NULL, NULL, &callback_angharad_trigger_list, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/trigger/@trigger_name", NULL, NULL, NULL, &callback_angharad_trigger_get, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/trigger/@trigger_name/enable/@enabled", NULL, NULL, NULL, &callback_angharad_trigger_enable, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "POST", config->url_prefix_angharad, "/trigger/", NULL, NULL, NULL, &callback_angharad_trigger_add, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "PUT", config->url_prefix_angharad, "/trigger/@trigger_name", NULL, NULL, NULL, &callback_angharad_trigger_modify, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "DELETE", config->url_prefix_angharad, "/trigger/@trigger_name", NULL, NULL, NULL, &callback_angharad_trigger_remove, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "PUT", config->url_prefix_angharad, "/trigger/@trigger_name/@tag", NULL, NULL, NULL, &callback_angharad_trigger_add_tag, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "DELETE", config->url_prefix_angharad, "/trigger/@trigger_name/@tag", NULL, NULL, NULL, &callback_angharad_trigger_remove_tag, (void*)config);

    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/auth", &callback_angharad_no_auth_function, NULL, NULL, &callback_angharad_auth_get, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "POST", config->url_prefix_angharad, "/auth", &callback_angharad_no_auth_function, NULL, NULL, &callback_angharad_auth_check, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "DELETE", config->url_prefix_angharad, "/auth", &callback_angharad_no_auth_function, NULL, NULL, &callback_angharad_auth_delete, (void*)config);

    ulfius_add_endpoint_by_val(config->instance, "GET", config->static_files_prefix, "*", &callback_angharad_no_auth_function, NULL, NULL, &callback_angharad_static_file, (void*)config);

    ulfius_add_endpoint_by_val(config->instance, "OPTIONS", NULL, "*", &callback_angharad_no_auth_function, NULL, NULL, &callback_angharad_options, (void*)config);

    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/", &callback_angharad_no_auth_function, NULL, NULL, &callback_angharad_root_url, (void*)config);

    ulfius_set_default_auth_function(config->instance, &callback_angharad_auth_function, (void*)config, NULL);
    
    u_map_put(config->instance->default_headers, "Access-Control-Allow-Origin", config->allow_origin);
    u_map_put(config->instance->default_headers, "Access-Control-Allow-Credentials", "true");
    
    config->mime_types = malloc(sizeof(struct _u_map));
    if (config->mime_types == NULL) {
      y_log_message(Y_LOG_LEVEL_ERROR, "init_angharad - Error allocating resources for config->mime_types, aborting");
      return A_ERROR_MEMORY;
    }
    u_map_init(config->mime_types);
    // TODO put this in the config file
    u_map_put(config->mime_types, ".html", "text/html");
    u_map_put(config->mime_types, ".css", "text/css");
    u_map_put(config->mime_types, ".js", "application/javascript");
    u_map_put(config->mime_types, ".png", "image/png");
    u_map_put(config->mime_types, ".jpeg", "image/jpeg");
    u_map_put(config->mime_types, ".jpg", "image/jpeg");
    u_map_put(config->mime_types, ".ttf", "font/ttf");
    u_map_put(config->mime_types, ".woff", "font/woff");
    u_map_put(config->mime_types, ".woff2", "font/woff2");
    u_map_put(config->mime_types, "*", "application/octet-stream");
    
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

    ulfius_remove_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/auth/");
    ulfius_remove_endpoint_by_val(config->instance, "POST", config->url_prefix_angharad, "/auth/");
    ulfius_remove_endpoint_by_val(config->instance, "DELETE", config->url_prefix_angharad, "/auth/");

    ulfius_remove_endpoint_by_val(config->instance, "GET", config->static_files_prefix, "*");
    
    ulfius_remove_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/");

    if (config->angharad_status == ANGHARAD_STATUS_RUN) {
      config->angharad_status = ANGHARAD_STATUS_STOPPING;
      while (config->angharad_status != ANGHARAD_STATUS_STOP) {
        sleep(1);
      }
    }
    
    return A_OK;
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "close_agharad - Error closing angharad webservices, error input parameters");
    return A_ERROR_PARAM;
  }
}

/**
 * return the filename extension
 */
const char * get_filename_ext(const char *path) {
    const char *dot = strrchr(path, '.');
    if(!dot || dot == path) return "*";
    return dot;
}
