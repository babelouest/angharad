/**
 *
 * Angharad system
 *
 * Manage all subsystems and handles the house automation server:
 * - Benoic
 * - Gareth
 * - Carleon
 * 
 * Events (schedulers and alerts) management functions
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

void * thread_event_run(void * args) {
  struct config_elements * config = (struct config_elements *)args;
  time_t now;
  struct tm ts;
  if (config != NULL) {
    while (config->angharad_status == ANGHARAD_STATUS_RUN) {
      // Run monitoring task every minute
      time(&now);
      ts = *localtime(&now);
      if (ts.tm_sec == 0) {
        // Check schedules and events
        y_log_message(Y_LOG_LEVEL_DEBUG, "thread_event_run - Check schedules and events");
      }
      
      sleep(1);
    }
    config->angharad_status = ANGHARAD_STATUS_STOP;
  }
  
  return NULL;
}
