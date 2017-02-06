/**
 *
 * Angharad system
 *
 * Manage all subsystems and handles the house automation server:
 * - Benoic
 * - Gareth
 * - Carleon
 * 
 * authentication management functions
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
 * Validates if an access_token is valid
 */
json_t * access_token_check(struct config_elements * config, const char * header_value) {
  json_t * j_return, * j_grants;
  jwt_t * jwt = NULL;
  time_t now;
  long expiration;
  char  * grants;
  const char * type, * token_value;
  
  if (header_value != NULL) {
    if (strstr(header_value, PREFIX_BEARER) == header_value) {
      token_value = header_value + strlen(PREFIX_BEARER);
      if (!jwt_decode(&jwt, token_value, (const unsigned char *)config->jwt_decode_key, strlen(config->jwt_decode_key)) && jwt_get_alg(jwt) == config->jwt_alg) {
        time(&now);
        expiration = jwt_get_grant_int(jwt, "iat") + jwt_get_grant_int(jwt, "expires_in");
        type = jwt_get_grant(jwt, "type");
        if (now < expiration && 0 == nstrcmp(type, "access_token")) {
          grants = jwt_get_grants_json(jwt, NULL);
          j_grants = json_loads(grants, JSON_DECODE_ANY, NULL);
          if (j_grants != NULL) {
            j_return = json_pack("{siso}", "result", A_OK, "grants", j_grants);
          } else {
            y_log_message(Y_LOG_LEVEL_ERROR, "access_token_check - Error encoding token grants '%s'", grants);
            j_return = json_pack("{si}", "result", A_ERROR);
          }
          free(grants);
        } else {
          j_return = json_pack("{si}", "result", A_ERROR_UNAUTHORIZED);
        }
      } else {
        j_return = json_pack("{si}", "result", A_ERROR_UNAUTHORIZED);
      }
      jwt_free(jwt);
    } else {
      j_return = json_pack("{si}", "result", A_ERROR_UNAUTHORIZED);
    }
  } else {
    j_return = json_pack("{si}", "result", A_ERROR_UNAUTHORIZED);
  }
  return j_return;
}
