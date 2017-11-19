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
#include <string.h>
#include <libconfig.h>
#include <getopt.h>
#include <signal.h>

#include "angharad.h"
#include "glewlwyd_resource.h"

/**
 * Initialize the application configuration based on the command line parameters
 */
int build_config_from_args(int argc, char ** argv, struct config_elements * config) {
  int next_option;
  const char * short_options = "c::p::u::v::w::x::m::l::f::h::b::s";
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
        case 'x':
          if (optarg != NULL) {
            config->url_prefix_gareth = o_strdup(optarg);
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
            free(to_free);
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
    free((*config)->instance);
    free((*config)->config_file);
    free((*config)->url_prefix_angharad);
    free((*config)->url_prefix_benoic);
    free((*config)->url_prefix_carleon);
    free((*config)->url_prefix_gareth);
    free((*config)->static_files_path);
    free((*config)->static_files_prefix);
    free((*config)->alert_url);
    free((*config)->glewlwyd_resource_config->jwt_decode_key);
    free((*config)->glewlwyd_resource_config->oauth_scope);
    free((*config)->glewlwyd_resource_config);
    u_map_clean_full((*config)->mime_types);
    free((*config)->allow_origin);
    free((*config)->log_file);

    free(*config);
    (*config) = NULL;
  }
  if (pthread_mutex_destroy(&global_handler_close_lock) ||
      pthread_cond_destroy(&global_handler_close_cond)) {
    y_log_message(Y_LOG_LEVEL_ERROR, "Error destroying global_handler_close_lock or global_handler_close_cond");
  }
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
  config_setting_t * root, * database, * jwt;
  const char * cur_prefix_angharad, * cur_prefix_benoic, * cur_prefix_carleon, * cur_prefix_gareth, * cur_log_mode, * cur_log_level, * cur_log_file = NULL, * one_log_mode, * carleon_services_path, * benoic_modules_path, * cur_allow_origin, * cur_static_files_prefix, * db_type, * db_sqlite_path, * db_mariadb_host = NULL, * db_mariadb_user = NULL, * db_mariadb_password = NULL, * db_mariadb_dbname = NULL, * cur_static_files_path = NULL, * cur_rsa_pub_file = NULL, * cur_ecdsa_pub_file = NULL, * cur_sha_secret = NULL, * cur_oauth_scope = NULL;
  int db_mariadb_port = 0, cur_key_size = 512, cur_use_rsa = 0, cur_use_sha = 0, cur_use_ecdsa = 0;
  
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

  if (config->url_prefix_gareth == NULL) {
    // Get prefix url for gareth
    if (config_lookup_string(&cfg, "url_prefix_gareth", &cur_prefix_gareth)) {
      config->url_prefix_gareth = o_strdup(cur_prefix_gareth);
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
            config_destroy(&cfg);
            fprintf(stderr, "Error opening sqlite database %s\n", db_sqlite_path);
            return 0;
          } else {
            if (h_exec_query_sqlite(config->conn, "PRAGMA foreign_keys = ON;") != H_OK) {
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

  if (config->static_files_path == NULL) {
    // Get path that serve static files
    if (config_lookup_string(&cfg, "static_files_path", &cur_static_files_path)) {
      config->static_files_path = o_strdup(cur_static_files_path);
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
      config->static_files_prefix = o_strdup(cur_static_files_prefix);
      if (config->static_files_prefix == NULL) {
        fprintf(stderr, "Error allocating config->static_files_prefix, exiting\n");
        config_destroy(&cfg);
        return 0;
      }
    }
  }
  
  jwt = config_setting_get_member(root, "jwt");
  if (jwt != NULL) {
		config_setting_lookup_bool(jwt, "key_size", &cur_key_size);
		
		if (cur_key_size == 256 || cur_key_size == 384 || cur_key_size == 512) {
			config_setting_lookup_bool(jwt, "use_rsa", &cur_use_rsa);
			config_setting_lookup_bool(jwt, "use_sha", &cur_use_sha);
			config_setting_lookup_bool(jwt, "use_ecdsa", &cur_use_ecdsa);
			if (cur_use_rsa) {
				config_setting_lookup_string(jwt, "rsa_pub_file", &cur_rsa_pub_file);
				if (cur_rsa_pub_file != NULL) {
					config->glewlwyd_resource_config->jwt_decode_key = get_file_content(cur_rsa_pub_file);
					if (config->glewlwyd_resource_config->jwt_decode_key == NULL) {
						config_destroy(&cfg);
						fprintf(stderr, "Error, rsa_pub_file content incorrect\n");
						return 0;
					}
					if (cur_key_size == 256) {
						config->glewlwyd_resource_config->jwt_alg = JWT_ALG_RS256;
					} else if (cur_key_size == 384) {
						config->glewlwyd_resource_config->jwt_alg = JWT_ALG_RS384;
					} else if (cur_key_size == 512) {
						config->glewlwyd_resource_config->jwt_alg = JWT_ALG_RS512;
					}
				} else {
					config_destroy(&cfg);
					fprintf(stderr, "Error, rsa_pub_file incorrect\n");
					return 0;
				}
			} else if (cur_use_ecdsa) {
				config_setting_lookup_string(jwt, "ecdsa_pub_file", &cur_ecdsa_pub_file);
				if (cur_ecdsa_pub_file != NULL) {
					config->glewlwyd_resource_config->jwt_decode_key = get_file_content(cur_ecdsa_pub_file);
					if (config->glewlwyd_resource_config->jwt_decode_key == NULL) {
						config_destroy(&cfg);
						fprintf(stderr, "Error, ecdsa_pub_file content incorrect\n");
						return 0;
					}
					if (cur_key_size == 256) {
						config->glewlwyd_resource_config->jwt_alg = JWT_ALG_ES256;
					} else if (cur_key_size == 384) {
						config->glewlwyd_resource_config->jwt_alg = JWT_ALG_ES384;
					} else if (cur_key_size == 512) {
						config->glewlwyd_resource_config->jwt_alg = JWT_ALG_ES512;
					}
				} else {
					config_destroy(&cfg);
					fprintf(stderr, "Error, ecdsa_pub_file incorrect\n");
					return 0;
				}
			} else if (cur_use_sha) {
				config_setting_lookup_string(jwt, "sha_secret", &cur_sha_secret);
				if (cur_sha_secret != NULL) {
					config->glewlwyd_resource_config->jwt_decode_key = o_strdup(cur_sha_secret);
					config->glewlwyd_resource_config->jwt_alg = JWT_ALG_HS512;
				} else {
					config_destroy(&cfg);
					fprintf(stderr, "Error, sha_secret incorrect\n");
					return 0;
				}
			} else {
				config_destroy(&cfg);
				fprintf(stderr, "Error, no jwt algorithm selected\n");
				return 0;
			}
		} else {
			config_destroy(&cfg);
			fprintf(stderr, "Error, key_size incorrect, values available are 256, 384 or 512\n");
			return 0;
		}
  }
  
  if (config_lookup_string(&cfg, "oauth_scope", &cur_oauth_scope)) {
    config->glewlwyd_resource_config->oauth_scope = o_strdup(cur_oauth_scope);
    if (config->glewlwyd_resource_config->oauth_scope == NULL) {
      fprintf(stderr, "Error allocating config->oauth_scope, exiting\n");
      config_destroy(&cfg);
      return 0;
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
  
  if (config->url_prefix_gareth == NULL) {
    config->url_prefix_gareth = o_strdup(GARETH_DEFAULT_PREFIX);
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
    config->allow_origin = o_strdup(ALLOW_ORIGIN_DEFAULT);
  }

  if (config->static_files_prefix == NULL) {
    config->static_files_prefix = o_strdup(STATIC_FILES_PREFIX_DEFAULT);
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
  config->alert_url = NULL;
  config->instance = malloc(sizeof(struct _u_instance));
  config->c_config = malloc(sizeof(struct _carleon_config));
  config->b_config = malloc(sizeof(struct _benoic_config));
  config->mime_types = NULL;
  config->glewlwyd_resource_config = malloc(sizeof (struct _glewlwyd_resource_config));
  if (config->instance == NULL || config->c_config == NULL || config->b_config == NULL || config->glewlwyd_resource_config == NULL) {
    fprintf(stderr, "Memory error - config->instance || config->c_config || config->b_config || config->glewlwyd_resource_config\n");
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
  config->glewlwyd_resource_config->jwt_decode_key = NULL;
  config->glewlwyd_resource_config->oauth_scope = NULL;
  config->glewlwyd_resource_config->method = G_METHOD_HEADER;
  config->glewlwyd_resource_config->realm = NULL;
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
  
  // Initialize benoic webservice if enabled
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
  ulfius_set_default_endpoint(config->instance, &callback_angharad_default, (void*)config);
  
  // Start the webservice
  y_log_message(Y_LOG_LEVEL_INFO, "Start angharad on port %d", config->instance->port);
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
  exit_server(&config, ANGHARAD_STOP);
  return 0;
}

/**
 * Get all submodules or the specified submodule 
 */
json_t * submodule_get(struct config_elements * config, const char * submodule) {
  json_t * j_query, * j_result, * j_submodule;
  int res;
  size_t index;
  
  y_log_message(Y_LOG_LEVEL_DEBUG, "Entering function %s from file %s", __PRETTY_FUNCTION__, __FILE__);
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

/**
 * Set the db flag enabled to true or false, and init or close the specified submodule if the status has changed
 */
int submodule_enable(struct config_elements * config, const char * submodule, int enabled) {
  json_t * j_submodule = submodule_get(config, submodule), * j_query;
  int res;
  
  y_log_message(Y_LOG_LEVEL_DEBUG, "Entering function %s from file %s", __PRETTY_FUNCTION__, __FILE__);
  if (j_submodule != NULL && json_integer_value(json_object_get(j_submodule, "result")) == A_OK) {
    if (enabled && json_object_get(json_object_get(j_submodule, "submodule"), "enabled") == json_false()) {
      json_decref(j_submodule);
      // Enable disabled module
      j_query = json_pack("{sss{si}s{ss}}", "table", ANGHARAD_TABLE_SUBMODULE, "set", "as_enabled", 1, "where" ,"as_name", submodule);
      res = h_update(config->conn, j_query, NULL);
      json_decref(j_query);
      if (res == H_OK) {
        if (0 == o_strcmp(submodule, ANGHARAD_SUBMODULE_BENOIC)) {
          if (init_benoic(config->instance, config->url_prefix_benoic, config->b_config) != B_OK) {
            y_log_message(Y_LOG_LEVEL_ERROR, "submodule_enable - Error init benoic");
            return A_ERROR;
          }
          return A_OK;
        } else if (0 == o_strcmp(submodule, ANGHARAD_SUBMODULE_CARLEON)) {
          if (init_carleon(config->c_config) != C_OK) {
            y_log_message(Y_LOG_LEVEL_ERROR, "submodule_enable - Error init carleon");
            return A_ERROR;
          }
          return A_OK;
        } else if (0 == o_strcmp(submodule, ANGHARAD_SUBMODULE_GARETH)) {
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
        if (0 == o_strcmp(submodule, ANGHARAD_SUBMODULE_BENOIC)) {
          if (close_benoic(config->instance, config->url_prefix_benoic, config->b_config) != B_OK) {
            y_log_message(Y_LOG_LEVEL_ERROR, "submodule_enable - Error closing benoic");
            return A_ERROR;
          }
          return A_OK;
        } else if (0 == o_strcmp(submodule, ANGHARAD_SUBMODULE_CARLEON)) {
          if (close_carleon(config->c_config) != C_OK) {
            y_log_message(Y_LOG_LEVEL_ERROR, "submodule_enable - Error closing carleon");
            return A_ERROR;
          }
          return A_OK;
        } else if (0 == o_strcmp(submodule, ANGHARAD_SUBMODULE_GARETH)) {
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

/**
 * Initializes angharad server
 * Create all the endpoints
 */
int init_angharad(struct config_elements * config) {
  pthread_t thread_scheduler;
  int thread_scheduler_ret = 0, thread_scheduler_detach = 0;

  y_log_message(Y_LOG_LEVEL_DEBUG, "Entering function %s from file %s", __PRETTY_FUNCTION__, __FILE__);
  if (config != NULL && config->instance != NULL && config->url_prefix_angharad) {
    ulfius_add_endpoint_by_val(config->instance, "*", config->url_prefix_angharad, "/submodule/*", 1, &callback_check_glewlwyd_access_token, (void*)config->glewlwyd_resource_config);
    ulfius_add_endpoint_by_val(config->instance, "*", config->url_prefix_angharad, "/script/*", 1, &callback_check_glewlwyd_access_token, (void*)config->glewlwyd_resource_config);
    ulfius_add_endpoint_by_val(config->instance, "*", config->url_prefix_angharad, "/scheduler/*", 1, &callback_check_glewlwyd_access_token, (void*)config->glewlwyd_resource_config);
    ulfius_add_endpoint_by_val(config->instance, "*", config->url_prefix_angharad, "/trigger/*", 1, &callback_check_glewlwyd_access_token, (void*)config->glewlwyd_resource_config);
    ulfius_add_endpoint_by_val(config->instance, "*", config->url_prefix_angharad, "/profile/*", 1, &callback_check_glewlwyd_access_token, (void*)config->glewlwyd_resource_config);
    ulfius_add_endpoint_by_val(config->instance, "*", config->url_prefix_benoic, "*", 1, &callback_check_glewlwyd_access_token, (void*)config->glewlwyd_resource_config);
    ulfius_add_endpoint_by_val(config->instance, "*", config->url_prefix_carleon, "*", 1, &callback_check_glewlwyd_access_token, (void*)config->glewlwyd_resource_config);
    ulfius_add_endpoint_by_val(config->instance, "*", config->url_prefix_gareth, "*", 1, &callback_check_glewlwyd_access_token, (void*)config->glewlwyd_resource_config);
    
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/alert/@submodule_name/@source/@element/@message/", 2, &callback_angharad_alert, (void*)config);
    
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/submodule/", 2, &callback_angharad_submodule_list, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/submodule/@submodule_name", 2, &callback_angharad_submodule_get, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/submodule/@submodule_name/enable/@enabled", 2, &callback_angharad_submodule_enable, (void*)config);

    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/script/", 2, &callback_angharad_script_list, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/script/@script_name", 2, &callback_angharad_script_get, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "POST", config->url_prefix_angharad, "/script/", 2, &callback_angharad_script_add, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "PUT", config->url_prefix_angharad, "/script/@script_name", 2, &callback_angharad_script_modify, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "DELETE", config->url_prefix_angharad, "/script/@script_name", 2, &callback_angharad_script_remove, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "PUT", config->url_prefix_angharad, "/script/@script_name/@tag", 2, &callback_angharad_script_add_tag, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "DELETE", config->url_prefix_angharad, "/script/@script_name/@tag", 2, &callback_angharad_script_remove_tag, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/script/@script_name/run", 2, &callback_angharad_script_run, (void*)config);

    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/scheduler/", 2, &callback_angharad_scheduler_list, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/scheduler/@scheduler_name", 2, &callback_angharad_scheduler_get, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/scheduler/@scheduler_name/enable/@enabled", 2, &callback_angharad_scheduler_enable, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "POST", config->url_prefix_angharad, "/scheduler/", 2, &callback_angharad_scheduler_add, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "PUT", config->url_prefix_angharad, "/scheduler/@scheduler_name", 2, &callback_angharad_scheduler_modify, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "DELETE", config->url_prefix_angharad, "/scheduler/@scheduler_name", 2, &callback_angharad_scheduler_remove, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "PUT", config->url_prefix_angharad, "/scheduler/@scheduler_name/@tag", 2, &callback_angharad_scheduler_add_tag, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "DELETE", config->url_prefix_angharad, "/scheduler/@scheduler_name/@tag",2, &callback_angharad_scheduler_remove_tag, (void*)config);

    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/trigger/", 2, &callback_angharad_trigger_list, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/trigger/@trigger_name", 2, &callback_angharad_trigger_get, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/trigger/@trigger_name/enable/@enabled", 21, &callback_angharad_trigger_enable, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "POST", config->url_prefix_angharad, "/trigger/", 2, &callback_angharad_trigger_add, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "PUT", config->url_prefix_angharad, "/trigger/@trigger_name", 2, &callback_angharad_trigger_modify, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "DELETE", config->url_prefix_angharad, "/trigger/@trigger_name", 2, &callback_angharad_trigger_remove, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "PUT", config->url_prefix_angharad, "/trigger/@trigger_name/@tag", 2, &callback_angharad_trigger_add_tag, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "DELETE", config->url_prefix_angharad, "/trigger/@trigger_name/@tag", 2, &callback_angharad_trigger_remove_tag, (void*)config);

    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/profile", 2, &callback_carleon_profile_list, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "GET", config->url_prefix_angharad, "/profile/@profile_id", 2, &callback_carleon_profile_get, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "PUT", config->url_prefix_angharad, "/profile/@profile_id", 2, &callback_carleon_profile_set, (void*)config);
    ulfius_add_endpoint_by_val(config->instance, "DELETE", config->url_prefix_angharad, "/profile/@profile_id", 2, &callback_carleon_profile_remove, (void*)config);
    
    ulfius_add_endpoint_by_val(config->instance, "GET", config->static_files_prefix, "*", 1, &callback_angharad_static_file, (void*)config);

    ulfius_add_endpoint_by_val(config->instance, "OPTIONS", NULL, "*", 0, &callback_angharad_options, (void*)config);

    ulfius_add_endpoint_by_val(config->instance, "GET", NULL, "/", 2, &callback_angharad_root_url, (void*)config);

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
    u_map_put(config->mime_types, ".json", "application/json");
    u_map_put(config->mime_types, ".map", "application/octet-stream");
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

/**
 * Closes angharad server
 * Close all the endpoints and send a stop signal to all threads
 */
int close_angharad(struct config_elements * config) {
  y_log_message(Y_LOG_LEVEL_DEBUG, "Entering function %s from file %s", __PRETTY_FUNCTION__, __FILE__);
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
    if (strchr(dot, '?') != NULL) {
      *strchr(dot, '?') = '\0';
    }
    return dot;
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
    length = ftell (f);
    fseek (f, 0, SEEK_SET);
    buffer = malloc((length+1)*sizeof(char));
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
