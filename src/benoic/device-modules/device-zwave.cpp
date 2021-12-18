/**
 *
 * Benoic House Automation service
 *
 * Command house automation devices via an HTTP REST interface
 *
 * Zwave device module
 * Provides all the commands for a zwave dongle
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

/**
 * List of Command classes id
 * (0x20) COMMAND_CLASS_BASIC
 * (0x21) COMMAND_CLASS_CONTROLLER_REPLICATION
 * (0x25) COMMAND_CLASS_SWITCH_BINARY
 * (0x26) COMMAND_CLASS_SWITCH_MULTILEVEL
 * (0x27) COMMAND_CLASS_SWITCH_ALL
 * (0x28) COMMAND_CLASS_SWITCH_TOGGLE_BINARY
 * (0x29) COMMAND_CLASS_SWITCH_TOGGLE_MULTILEVEL
 * (0x30) COMMAND_CLASS_SENSOR_BINARY
 * (0x31) COMMAND_CLASS_SENSOR_MULTILEVEL
 * (0x32) COMMAND_CLASS_METER
 * (0x35) COMMAND_CLASS_METER_PULSE
 * (0x40) COMMAND_CLASS_THERMOSTAT_MODE
 * (0x42) COMMAND_CLASS_THERMOSTAT_OPERATING_STATE
 * (0x43) COMMAND_CLASS_THERMOSTAT_SETPOINT
 * (0x44) COMMAND_CLASS_THERMOSTAT_FAN_MODE
 * (0x45) COMMAND_CLASS_THERMOSTAT_FAN_STATE
 * (0x46) COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE
 * (0x4C) COMMAND_CLASS_DOOR_LOCK_LOGGING
 * (0x50) COMMAND_CLASS_BASIC_WINDOW_COVERING
 * (0x5B) COMMAND_CLASS_CENTRAL_SCENE
 * (0x62) COMMAND_CLASS_DOOR_LOCK
 * (0x63) COMMAND_CLASS_USER_CODE
 * (0x71) COMMAND_CLASS_ALARM
 * (0x73) COMMAND_CLASS_POWERLEVEL
 * (0x75) COMMAND_CLASS_PROTECTION
 * (0x76) COMMAND_CLASS_LOCK
 * (0x80) COMMAND_CLASS_BATTERY
 * (0x81) COMMAND_CLASS_CLOCK
 * (0x84) COMMAND_CLASS_WAKE_UP
 * (0x86) COMMAND_CLASS_VERSION
 * (0x87) COMMAND_CLASS_INDICATOR
 * (0x89) COMMAND_CLASS_LANGUAGE
 * (0x8B) COMMAND_CLASS_TIME_PARAMETERS
 * (0x90) COMMAND_CLASS_ENERGY_PRODUCTION
 * (0x9b) COMMAND_CLASS_ASSOCIATION_COMMAND_CONFIGURATION
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <jansson.h>
#include <math.h>
#include <time.h>
#include <yder.h>
#include <orcania.h>
#include <ulfius.h>

#ifdef __cplusplus
}
#endif

#include "Options.h"
#include "Manager.h"
#include "Driver.h"
#include "Node.h"
#include "Group.h"
#include "Notification.h"
#include "ValueStore.h"
#include "Value.h"
#include "ValueBool.h"
#include "Log.h"

#define RESULT_ERROR     0
#define RESULT_OK        1
#define RESULT_NOT_FOUND 2
#define RESULT_TIMEOUT   3

#define ELEMENT_TYPE_NONE   0
#define ELEMENT_TYPE_SENSOR 1
#define ELEMENT_TYPE_SWITCH 2
#define ELEMENT_TYPE_DIMMER 3
#define ELEMENT_TYPE_HEATER 4

#define UNDEFINED_HOME_ID 0

#define DEFAULT_CONFIG_PATH  "/usr/local/etc/openzwave"
#define DEFAULT_USER_PATH    "/usr/local/var/log/angharad"
#define DEFAULT_COMMAND_LINE ""
#define DEFAULT_LOG_PATH     "openzwave.log"

/**
 * COMMAND_CLASS used
 */
#define COMMAND_CLASS_SWITCH_BINARY               0x25
#define COMMAND_CLASS_SWITCH_MULTILEVEL           0x26
#define COMMAND_CLASS_SENSOR_BINARY               0x30
#define COMMAND_CLASS_SENSOR_MULTILEVEL           0x31
#define COMMAND_CLASS_THERMOSTAT_MODE             0x40
#define COMMAND_CLASS_THERMOSTAT_OPERATING_STATE  0x42
#define COMMAND_CLASS_THERMOSTAT_SETPOINT         0x43
#define COMMAND_CLASS_ALARM                       0x71

#define COMMAND_CLASS_THERMOSTAT_OPERATING_STATE_IDLE     "Idle"
#define COMMAND_CLASS_THERMOSTAT_OPERATING_STATE_HEATING  "Heating"
#define COMMAND_CLASS_THERMOSTAT_MODE_HEAT                "Heat"
#define COMMAND_CLASS_THERMOSTAT_MODE_COOL                "Cool"
#define COMMAND_CLASS_THERMOSTAT_MODE_OFF                 "Off"

#define BENOIC_ELEMENT_HEATER_MODE_OFF     "off"
#define BENOIC_ELEMENT_HEATER_MODE_HEAT    "heat"
#define BENOIC_ELEMENT_HEATER_MODE_COOL    "cool"

extern "C" using namespace OpenZWave;
extern "C" using namespace std;

/**
 * node structure
 * identity a zwave connected device
 */
typedef struct _node {
  uint32        home_id;
  uint8         node_id;
  bool          polled;
  list<ValueID> values;
} node;

struct zwave_context {
  uint32          home_id;
  int             init_failed;
  list<node*>   * nodes_list;
  char            uri[256];
  char            usb_file[512];     // filename pattern of the usb dongle
  char            config_path[256];
  char            user_path[256];
  char            command_line[256];
  char            log_path[256];
  char          * alert_url;
  char          * device_name;
  struct _u_map * alarms;
  struct _u_map * dimmer_values;
};

/**
 * return a name based on the value label,
 * by replacing spaces and $ with _
 * and lowercase all the letters
 * returned value must be free'd after use
 */
char * naming_label(const char * label) {
  char * to_return = NULL;
  int i;
  
  if (label != NULL) {
    to_return = o_strdup(label);
    for (i=0; to_return[i] != '\0'; i++) {
      if (to_return[i] == '$' || to_return[i] == ' ') {
        to_return[i] = '_';
      } else {
        to_return[i] = tolower(label[i]);
      }
    }
  }
  return to_return;
}

/**
 * return the node in this device identified by it id
 */
node * get_device_node(zwave_context * zcontext, uint8 node_id) {
  if (zcontext == NULL || zcontext->nodes_list == NULL) {
    return NULL;
  } else {
    for( list<node*>::iterator it = zcontext->nodes_list->begin(); it != zcontext->nodes_list->end(); ++it ) {
      node* cur_node = *it;
      
      if (cur_node->home_id == zcontext->home_id && node_id == cur_node->node_id) {
        return cur_node;
      }
    }
  }
  return NULL;
}

/**
 * return the ValueID of the node identified by the command_class value
 */
ValueID * get_device_value_id(node * cur_node, uint8 command_class) {
  if (cur_node != NULL) {
    for( list<ValueID>::iterator it = cur_node->values.begin(); it != cur_node->values.end(); ++it ) {
      ValueID * v = &(*it);
      if ( v->GetCommandClassId() == command_class ) {
        return v;
      }
    }
  }
  return NULL;
}

/**
 * return the number of ValueID of the node identified by the command_class value
 */
uint get_device_value_id_count(node * cur_node, uint8 command_class) {
  uint count = 0;
  
  if (cur_node != NULL) {
    for( list<ValueID>::iterator it = cur_node->values.begin(); it != cur_node->values.end(); ++it ) {
      ValueID * v = &(*it);
      if ( v->GetCommandClassId() == command_class ) {
        count++;
      }
    }
  }
  return count;
}

/**
 * return the ValueID of the node identified by the command_class value and its index
 */
ValueID * get_device_value_id_by_index(node * cur_node, uint8 command_class, uint index) {
  uint count = 0;
  
  if (cur_node != NULL) {
    for( list<ValueID>::iterator it = cur_node->values.begin(); it != cur_node->values.end(); ++it ) {
      ValueID * v = &(*it);
      if ( v->GetCommandClassId() == command_class ) {
        if (count == index) {
          return v;
        } else {
          count++;
        }
      }
    }
  }
  return NULL;
}

/**
 * return the ValueID of the node identified by the command_class value
 */
ValueID * get_device_value_id_by_label(node * cur_node, uint8 command_class, const char * label) {
  char * named_label;
  
  if (cur_node != NULL) {
    for( list<ValueID>::iterator it = cur_node->values.begin(); it != cur_node->values.end(); ++it ) {
      ValueID * v = &(*it);
      if ( v->GetCommandClassId() == command_class ) {
        named_label = naming_label(Manager::Get()->GetValueLabel(*v).c_str());
        if (0 == o_strcmp(named_label, label)) {
          free(named_label);
          return v;
        }
        free(named_label);
      }
    }
  }
  return NULL;
}

/**
 * return the ValueID of the node identified by the command_class value
 */
ValueID * get_device_value_id_by_element_name(zwave_context * zcontext, const char * element_name) {
  char * str_type, * str_node_id, * str_label, * save_ptr, * dup_name_save, * dup_name = dup_name_save = o_strdup(element_name);
  ValueID * value;
  node * cur_node;
  
  // first token is the type
  str_type = strtok_r(dup_name, "$", &save_ptr);
  
  // Assigning the next 2 tokens
  str_node_id = strtok_r(NULL, "$", &save_ptr);
  str_label = strtok_r(NULL, "$", &save_ptr);
  
  if (str_type == NULL || str_node_id == NULL) {
    o_free(dup_name_save);
    return NULL;
  } else {
    cur_node = get_device_node(zcontext, strtol(str_node_id, NULL, 10));
    if (0 == o_strcmp(str_type, "se")) {
      // This is a sensor
      if (str_label == NULL) {
        o_free(dup_name_save);
        return NULL;
      } else {
        value = get_device_value_id_by_label(cur_node, COMMAND_CLASS_SENSOR_BINARY, str_label);
        if (value == NULL) {
          value = get_device_value_id_by_label(cur_node, COMMAND_CLASS_SENSOR_MULTILEVEL, str_label);
        }
        o_free(dup_name_save);
        return value;
      }
    } else if (0 == o_strcmp(str_type, "sw")) {
      o_free(dup_name_save);
      return get_device_value_id(cur_node, COMMAND_CLASS_SWITCH_BINARY);
    } else if (0 == o_strcmp(str_type, "di")) {
      o_free(dup_name_save);
      return get_device_value_id(cur_node, COMMAND_CLASS_SWITCH_MULTILEVEL);
    } else if (0 == o_strcmp(str_type, "he")) {
      o_free(dup_name_save);
      return get_device_value_id(cur_node, COMMAND_CLASS_THERMOSTAT_SETPOINT);
    } else {
      o_free(dup_name_save);
      return NULL;
    }
  }
}

int send_angharad_alert(zwave_context * zcontext, char * source) {
  struct _u_request request;
  int res;
  
  if (zcontext == NULL || source == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "send_angharad_alert - Error input parameters");
    return RESULT_ERROR;
  } else if (zcontext->alert_url != NULL) {
    ulfius_init_request(&request);
    request.http_verb = o_strdup("GET");
    request.http_url = msprintf(zcontext->alert_url, "benoic", zcontext->device_name, source, "NOTIFICATION");
    res = ulfius_send_http_request(&request, NULL);
    if (res != U_OK) {
      y_log_message(Y_LOG_LEVEL_ERROR, "ZWave device, Error sending http request for alert");
    }
    ulfius_clean_request(&request);
    return RESULT_OK;
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "send_angharad_alert - no alert url");
    return RESULT_OK;
  }
}

/**
 * on_notification_zwave
 * Callback that is triggered when a value, group or node changes
 */
void on_notification_zwave ( Notification const * _notification, void * _context ) {
  zwave_context * zcontext = (zwave_context *)_context;
  list<node*> * nodes_list = (list<node*> *) zcontext ->nodes_list;
  
  node * cur_node;
  
  switch( _notification->GetType() ) {
  
    // Add the new value to our list if it doesn't already exists
    case Notification::Type_ValueAdded: {
      cur_node = get_device_node( zcontext, _notification->GetNodeId() );
      if( cur_node != NULL ) {
        if (_notification->GetValueID().GetCommandClassId() == COMMAND_CLASS_SWITCH_MULTILEVEL) {
          // Activate ChangeVerified flag to true for dimmers
          Manager::Get()->SetChangeVerified(_notification->GetValueID(), true);
        }
        cur_node->values.push_back( _notification->GetValueID() );
        //y_log_message(Y_LOG_LEVEL_DEBUG, "Adding ValueID type %x to node %d with id %x", _notification->GetValueID().GetCommandClassId(), cur_node->node_id, _notification->GetValueID().GetId());
      }
      break;
    }

    // Remove the value from out list if it exists
    case Notification::Type_ValueRemoved: {
      cur_node = get_device_node( zcontext, _notification->GetNodeId() );
      if( cur_node != NULL ) {
        for( list<ValueID>::iterator it = cur_node->values.begin(); it != cur_node->values.end(); ++it ) {
          if( (*it) == _notification->GetValueID() ) {
            cur_node->values.erase( it );
            //y_log_message(Y_LOG_LEVEL_DEBUG, "Removing ValueID type %x from node %d", _notification->GetValueID().GetCommandClassId(), cur_node->node_id);
            break;
          }
        }
      }
      break;
    }

    case Notification::Type_ValueChanged: {
      // One of the node values has changed
      // Nothing there
      //y_log_message(Y_LOG_LEVEL_DEBUG, "Notification::Type_ValueChanged for NodeID %d and ValueID %d", _notification->GetNodeId(), _notification->GetValueID());
      break;
    }

    case Notification::Type_Group: {
      // One of the node's association groups has changed
      // Nothing there
      //y_log_message(Y_LOG_LEVEL_DEBUG, "Notification::Type_Group for NodeID %d and ValueID %d", _notification->GetNodeId(), _notification->GetValueID());
      break;
    }

    case Notification::Type_NodeAdded: {
      // Add the new node to the device's nodes list
      node * cur_node = new node();
      cur_node->home_id = _notification->GetHomeId();
      cur_node->node_id = _notification->GetNodeId();
      cur_node->polled = false;
      nodes_list->push_back( cur_node );
      //y_log_message(Y_LOG_LEVEL_DEBUG, "Adding Node %d", cur_node->node_id);
      break;
    }

    case Notification::Type_NodeRemoved: {
      // Remove the node from the device's nodes list
      for( list<node*>::iterator it = nodes_list->begin(); it != nodes_list->end(); ++it ) {
        node* cur_node = *it;
        if( get_device_node( zcontext, cur_node->node_id != NULL ) ) {
          //y_log_message(Y_LOG_LEVEL_DEBUG, "Removing Node %d", cur_node->node_id);
          nodes_list->erase( it );
          delete cur_node;
          break;
        }
      }
      break;
    }

    case Notification::Type_NodeEvent: {
      char * alarm_name;
      // Event received
      y_log_message(Y_LOG_LEVEL_DEBUG, "Notification::Type_NodeEvent for NodeID %d, ValueID %x, type %x, send alert", 
                    _notification->GetNodeId(), 
                    _notification->GetValueID().GetId(),
                    _notification->GetValueID().GetCommandClassId());
      alarm_name = msprintf("al$%02d", _notification->GetNodeId());
      // Add alarm name in the alarm list
      u_map_put(zcontext->alarms, alarm_name, alarm_name);
      send_angharad_alert(zcontext, alarm_name);
      break;
    }

    case Notification::Type_PollingDisabled: {
      // Polling is disabled for this node
      cur_node = get_device_node( zcontext, _notification->GetNodeId() );
      if( cur_node != NULL ) {
        //y_log_message(Y_LOG_LEVEL_DEBUG, "Disabling polling for Node %d", cur_node->node_id);
        cur_node->polled = false;
      }
      break;
    }

    case Notification::Type_PollingEnabled: {
      // Polling is enabled for this node
      cur_node = get_device_node( zcontext, _notification->GetNodeId() );
      if( cur_node != NULL ) {
        //y_log_message(Y_LOG_LEVEL_DEBUG, "Enabling polling for Node %d", cur_node->node_id);
        cur_node->polled = true;
      }
      break;
    }

    case Notification::Type_DriverReady: {
      y_log_message(Y_LOG_LEVEL_INFO, "Driver ready");
      zcontext->home_id = _notification->GetHomeId();
      zcontext->init_failed = 0;
      break;
    }

    case Notification::Type_DriverFailed: {
      y_log_message(Y_LOG_LEVEL_WARNING, "Driver failed");
      zcontext->home_id = UNDEFINED_HOME_ID;
      zcontext->init_failed = 1;
      break;
    }

    case Notification::Type_DriverRemoved: {
      //y_log_message(Y_LOG_LEVEL_DEBUG, "Notification::Type_DriverRemoved for NodeID %d and ValueID %d", _notification->GetNodeId(), _notification->GetValueID());
      break;
    }
    
    case Notification::Type_DriverReset: {
      //y_log_message(Y_LOG_LEVEL_DEBUG, "Notification::Type_DriverReset for NodeID %d and ValueID %d", _notification->GetNodeId(), _notification->GetValueID());
      break;
    }
    
    case Notification::Type_Notification: {
      //y_log_message(Y_LOG_LEVEL_DEBUG, "Notification::Type_Notification for NodeID %d and ValueID %d", _notification->GetNodeId(), _notification->GetValueID());
      break;
    }
    
    case Notification::Type_NodeNaming: {
      //y_log_message(Y_LOG_LEVEL_DEBUG, "Notification::Type_NodeNaming for NodeID %d and ValueID %d", _notification->GetNodeId(), _notification->GetValueID());
      break;
    }
    
    case Notification::Type_NodeProtocolInfo: {
      //y_log_message(Y_LOG_LEVEL_DEBUG, "Notification::Type_NodeProtocolInfo for NodeID %d and ValueID %d", _notification->GetNodeId(), _notification->GetValueID());
      break;
    }
    
    default: {
      //y_log_message(Y_LOG_LEVEL_DEBUG, "Uncatch notification: %d for NodeID %d and ValueID %d", _notification->GetType(), _notification->GetNodeId(), _notification->GetValueID());
      break;
    }
  }
}

/**
 * Initializes the device type by getting its uid, name and description
 */
extern "C" json_t * b_device_type_init () {
  json_t * options = json_array();
  json_array_append_new(options, json_pack("{ssssssso}", "name", "uri", "type", "string", "description", "uri to connect to the device", "optional", json_false()));
  json_array_append_new(options, json_pack("{ssssssso}", "name", "config_path", "type", "string", "description", "Path to openzwave configuration", "optional", json_true()));
  json_array_append_new(options, json_pack("{ssssssso}", "name", "user_path", "type", "string", "description", "Path to openzwave user files", "optional", json_true()));
  json_array_append_new(options, json_pack("{ssssssso}", "name", "command_line", "type", "string", "description", "Openzwave command line options", "optional", json_true()));
  json_array_append_new(options, json_pack("{ssssssso}", "name", "log_path", "type", "string", "description", "Path to openzwave log files", "optional", json_true()));
  return json_pack("{sissssssso}", 
                    "result", RESULT_OK,
                    "uid", "24-67-99", 
                    "name", "Openzwave Device", 
                    "description", "Openzwave supported device", 
                    "options", options);
}

/**
 * connects the device
 */
extern "C" json_t * b_device_connect (json_t * device, void ** device_ptr) {
  int i;
  char filename[512];
  
  * device_ptr = o_malloc(sizeof(struct zwave_context));
  o_strncpy(((struct zwave_context *) * device_ptr)->uri, json_string_value(json_object_get(json_object_get(device, "options"), "uri")), 256);
  if (json_object_get(json_object_get(device, "options"), "config_path") != NULL) {
    o_strncpy(((struct zwave_context *) * device_ptr)->config_path, json_string_value(json_object_get(json_object_get(device, "options"), "config_path")), 256);
  } else {
    o_strncpy(((struct zwave_context *) * device_ptr)->config_path, DEFAULT_CONFIG_PATH, 256);
  }
  if (json_object_get(json_object_get(device, "options"), "user_path") != NULL) {
    o_strncpy(((struct zwave_context *) * device_ptr)->user_path, json_string_value(json_object_get(json_object_get(device, "options"), "user_path")), 256);
  } else {
    o_strncpy(((struct zwave_context *) * device_ptr)->user_path, DEFAULT_USER_PATH, 256);
  }
  if (json_object_get(json_object_get(device, "options"), "command_line") != NULL) {
    o_strncpy(((struct zwave_context *) * device_ptr)->command_line, json_string_value(json_object_get(json_object_get(device, "options"), "command_line")), 256);
  } else {
    o_strncpy(((struct zwave_context *) * device_ptr)->command_line, DEFAULT_COMMAND_LINE, 256);
  }
  if (json_object_get(json_object_get(device, "options"), "log_path") != NULL) {
    o_strncpy(((struct zwave_context *) * device_ptr)->log_path, json_string_value(json_object_get(json_object_get(device, "options"), "log_path")), 256);
  } else {
    o_strncpy(((struct zwave_context *) * device_ptr)->log_path, DEFAULT_LOG_PATH, 256);
  }
  if (json_object_get(json_object_get(device, "options"), "alert_url") != NULL) {
    ((struct zwave_context *) * device_ptr)->alert_url = o_strdup(json_string_value(json_object_get(json_object_get(device, "options"), "alert_url")));
  } else {
    ((struct zwave_context *) * device_ptr)->alert_url = NULL;
  }
  ((struct zwave_context *) * device_ptr)->device_name = o_strdup(json_string_value(json_object_get(device, "name")));
  
  ((struct zwave_context *) *device_ptr)->nodes_list = new list<node*>();
  ((struct zwave_context *) *device_ptr)->alarms = new struct _u_map;
  u_map_init(((struct zwave_context *) *device_ptr)->alarms);
  ((struct zwave_context *) *device_ptr)->dimmer_values = new struct _u_map;
  u_map_init(((struct zwave_context *) *device_ptr)->dimmer_values);
  
  Options::Create( ((struct zwave_context *) *device_ptr)->config_path, ((struct zwave_context *) *device_ptr)->user_path, ((struct zwave_context *) *device_ptr)->command_line );
  Options::Get()->AddOptionString( "LogFileName", ((struct zwave_context *) *device_ptr)->log_path, false );
#ifdef DEBUG
  Options::Get()->AddOptionInt( "SaveLogLevel", LogLevel_Detail );
  Options::Get()->AddOptionInt( "QueueLogLevel", LogLevel_Debug );
  Options::Get()->AddOptionInt( "DumpTrigger", LogLevel_Error );
#else
  Options::Get()->AddOptionInt( "SaveLogLevel", LogLevel_Warning );
  Options::Get()->AddOptionInt( "QueueLogLevel", LogLevel_Warning  );
  Options::Get()->AddOptionInt( "DumpTrigger", LogLevel_Warning  );
#endif
  Options::Get()->AddOptionInt( "PollInterval", 500 );

  Options::Get()->AddOptionBool( "ConsoleOutput", false );

  Options::Get()->AddOptionBool( "IntervalBetweenPolls", true );
  Options::Get()->AddOptionBool( "ValidateValueChanges", true);
  Options::Get()->Lock();

  Manager::Create();
  Manager::Get()->AddWatcher( on_notification_zwave, *device_ptr );
  
  // Loo into uris to find the good one
  for (i=0; i<128; i++) {
    snprintf(filename, 512*sizeof(char), "%s%d", ((struct zwave_context *) *device_ptr)->uri, i);
    if (Manager::Get()->AddDriver( filename )) {
      snprintf(((struct zwave_context *) *device_ptr)->usb_file, 512*sizeof(char), "%s", filename);
      return json_pack("{si}", "result", RESULT_OK);
    }
  }
  return json_pack("{si}", "result", RESULT_ERROR);
}

/**
 * disconnects the device
 */
extern "C" json_t * b_device_disconnect (json_t * device, void * device_ptr) {
  if (device_ptr != NULL && strlen(((struct zwave_context *) device_ptr)->usb_file) > 0) {
    Manager::Get()->RemoveWatcher( on_notification_zwave, device_ptr );
    Manager::Destroy();
    Options::Destroy();
    delete ((struct zwave_context *) device_ptr)->nodes_list;
    o_free(((struct zwave_context *) device_ptr)->alert_url);
    o_free(((struct zwave_context *) device_ptr)->device_name);
    u_map_clean(((struct zwave_context *) device_ptr)->alarms);
    u_map_clean(((struct zwave_context *) device_ptr)->dimmer_values);
    delete (((struct zwave_context *) device_ptr)->alarms);
    delete (((struct zwave_context *) device_ptr)->dimmer_values);
    o_free(device_ptr);
  }
  return json_pack("{si}", "result", RESULT_OK);
}

/**
 * Ping the device type
 */
extern "C" json_t * b_device_ping (json_t * device, void * device_ptr) {
  if (device_ptr != NULL) {
    return json_pack("{si}", "result", RESULT_OK);
  } else {
    return json_pack("{si}", "result", RESULT_ERROR);
  }
}

/**
 * Get the sensor value
 */
extern "C" json_t * b_device_get_sensor (json_t * device, const char * sensor_name, void * device_ptr) {
  char * str_type, * str_node_id, * str_label, * save_ptr, * end_ptr_d, * dup_name_save, * dup_name = dup_name_save = o_strdup(sensor_name);
  ValueID * value;
  string s_status;
  bool b_status;
  json_t * result = NULL;
  double d_value;
  int is_binary = 0;
  
  // first token is the type, supposed to be "se" in this case
  str_type = strtok_r(dup_name, "$", &save_ptr);
  
  // Assigning the next 2 tokens
  str_node_id = strtok_r(NULL, "$", &save_ptr);
  str_label = strtok_r(NULL, "$", &save_ptr);
  
  if (u_map_has_key(((zwave_context *)device_ptr)->alarms, sensor_name)) {
    result = json_pack("{sisi}", "result", RESULT_OK, "value", 0);
  } else if (str_type == NULL || str_node_id == NULL || str_label == NULL) {
    result = json_pack("{si}", "result", RESULT_ERROR);
  } else {
    value = get_device_value_id_by_label(get_device_node((zwave_context *)device_ptr, strtol(str_node_id, NULL, 10)), COMMAND_CLASS_SENSOR_BINARY, str_label);
    if (value == NULL) {
      value = get_device_value_id_by_label(get_device_node((zwave_context *)device_ptr, strtol(str_node_id, NULL, 10)), COMMAND_CLASS_SENSOR_MULTILEVEL, str_label);
    } else {
      is_binary = 1;
    }
    if (value != NULL) {
      Manager::Get()->RefreshValue(*value);
      if (!is_binary) {
        if (Manager::Get()->GetValueAsString((*value), &s_status)) {
          d_value = strtof(s_status.c_str(), &end_ptr_d);
          if (end_ptr_d != s_status.c_str()) {
            result = json_pack("{sisf}", "result", RESULT_OK, "value", d_value);
          } else {
            result = json_pack("{siss}", "result", RESULT_OK, "value", s_status.c_str());
          }
        } else {
          result = json_pack("{si}", "result", RESULT_ERROR);
        }
      } else {
        if (Manager::Get()->GetValueAsBool((*value), &b_status)) {
          result = json_pack("{siso}", "result", RESULT_OK, "value", b_status?json_true():json_false());
        } else {
          result = json_pack("{si}", "result", RESULT_ERROR);
        }
      }
    } else {
      result = json_pack("{si}", "result", RESULT_NOT_FOUND);
    }
  }
  free(dup_name_save);
  return result;
}

/**
 * Get the switch value
 */
extern "C" json_t * b_device_get_switch (json_t * device, const char * switch_name, void * device_ptr) {
  ValueID * value = NULL;
  bool b_status;
  json_t * result = NULL;
  
  value = get_device_value_id_by_element_name((zwave_context *)device_ptr, switch_name);
  if (value != NULL) {
    Manager::Get()->RefreshValue(*value);
    if (Manager::Get()->GetValueAsBool(*value, &b_status)) {
      result = json_pack("{sisi}", "result", RESULT_OK, "value", (b_status?1:0));
    } else {
      result = json_pack("{si}", "result", RESULT_ERROR);
    }
  } else {
    result = json_pack("{si}", "result", RESULT_NOT_FOUND);
  }
  return result;
}

/**
 * Set the switch command
 */
extern "C" json_t * b_device_set_switch (json_t * device, const char * switch_name, const int command, void * device_ptr) {
  ValueID * value = NULL;
  json_t * result = NULL;
  
  value = get_device_value_id_by_element_name((zwave_context *)device_ptr, switch_name);
  if (value != NULL) {
    if (Manager::Get()->SetValue((*value), (command?true:false))) {
      result = json_pack("{si}", "result", RESULT_OK);
    } else {
      result = json_pack("{si}", "result", RESULT_ERROR);
    }
  } else {
    result = json_pack("{si}", "result", RESULT_NOT_FOUND);
  }
  return result;
}

/**
 * Get the dimmer value
 */
extern "C" json_t * b_device_get_dimmer (json_t * device, const char * dimmer_name, void * device_ptr) {
  ValueID * value = NULL;
  string s_status;
  json_t * result = NULL;
  
  value = get_device_value_id_by_element_name((zwave_context *)device_ptr, dimmer_name);
  if (value != NULL) {
    Manager::Get()->RefreshValue(*value);
    if (Manager::Get()->GetValueAsString((*value), &s_status)) {
      if (s_status.compare("0")) {
        u_map_put(((zwave_context *)device_ptr)->dimmer_values, dimmer_name, s_status.c_str());
      }
      result = json_pack("{sisi}", "result", RESULT_OK, "value", strtol(s_status.c_str(), NULL, 10));
    } else {
      result = json_pack("{si}", "result", RESULT_ERROR);
    }
  } else {
    result = json_pack("{si}", "result", RESULT_NOT_FOUND);
  }
  return result;
}

/**
 * Set the dimmer command
 */
extern "C" json_t * b_device_set_dimmer (json_t * device, const char * dimmer_name, const int command, void * device_ptr) {
  ValueID * value = NULL;
  string s_status;
  json_t * result = NULL;
  char val[4];
  int cur_command;
  
  value = get_device_value_id_by_element_name((zwave_context *)device_ptr, dimmer_name);
  if (value != NULL) {
    if (command < 0) cur_command = 0;
    else if (command > 99) cur_command = 99;
    else cur_command = command;
    
    if (command != 101) {
      snprintf(val, 3*sizeof(char), "%d", cur_command);
      if (command > 0) {
        u_map_put(((zwave_context *)device_ptr)->dimmer_values, dimmer_name, val);
      }
    } else if (u_map_has_key(((zwave_context *)device_ptr)->dimmer_values, dimmer_name)) {
      o_strncpy(val, u_map_get(((zwave_context *)device_ptr)->dimmer_values, dimmer_name), 3*sizeof(char));
    } else {
      o_strcpy(val, "0");
    }
    if (Manager::Get()->SetValue((*value), string(val)) ) {
      result = json_pack("{sisi}", "result", RESULT_OK, "value", strtol(val, NULL, 10));
    } else {
      result = json_pack("{si}", "result", RESULT_ERROR);
    }
  } else {
    result = json_pack("{si}", "result", RESULT_NOT_FOUND);
  }
  return result;
}

/**
 * Get the heater value
 */
extern "C" json_t * b_device_get_heater (json_t * device, const char * heater_name, void * device_ptr) {
  ValueID * value = NULL;
  string s_status;
  uint8 node_id;
  char * end_ptr;
  json_t * heater = json_object();
  
  if (heater == NULL) {
    y_log_message(Y_LOG_LEVEL_ERROR, "b_device_get_heater - Error allocating resources for heater");
    return json_pack("{si}", "result", RESULT_ERROR);
  }

  node_id = strtol(strstr(heater_name, "$") + 1, &end_ptr, 10);
  if (*end_ptr == '\0') {
    value = get_device_value_id(get_device_node((zwave_context *)device_ptr, node_id), COMMAND_CLASS_THERMOSTAT_MODE);
    if (value != NULL) {
      Manager::Get()->RefreshValue(*value);
      vector< string > o_value;
      string s;
      Manager::Get()->GetValueAsString(*value, &s_status);
      Manager::Get()->GetValueListItems(*value, &o_value);
      if (json_object_get(heater, "availableModes") == NULL) {
        json_object_set_new(heater, "availableModes", json_array());
      }
      for( vector<string>::iterator it = o_value.begin(); it != o_value.end(); ++it ) {
        s = *it;
        json_array_append_new(json_object_get(heater, "availableModes"), json_string(s.c_str()));
      }
      json_object_set_new(heater, "mode", json_string(s_status.c_str()));
    }
    
    value = get_device_value_id(get_device_node((zwave_context *)device_ptr, node_id), COMMAND_CLASS_THERMOSTAT_OPERATING_STATE);
    if (value != NULL) {
      Manager::Get()->RefreshValue(*value);
      Manager::Get()->GetValueAsString(*value, &s_status);
      if (o_strcmp(s_status.c_str(), COMMAND_CLASS_THERMOSTAT_OPERATING_STATE_HEATING) == 0) {
        json_object_set_new(heater, "on", json_true());
      } else {
        json_object_set_new(heater, "on", json_false());
      }
    }
    
    value = get_device_value_id(get_device_node((zwave_context *)device_ptr, node_id), COMMAND_CLASS_THERMOSTAT_SETPOINT);
    if (value != NULL) {
      Manager::Get()->RefreshValue(*value);
      Manager::Get()->GetValueAsString(*value, &s_status);
      json_object_set_new(heater, "command", json_real(strtod(s_status.c_str(), NULL)));
    }
    return json_pack("{siso}", "result", RESULT_OK, "value", heater);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "b_device_get_heater - Error node not found");
    return json_pack("{si}", "result", RESULT_NOT_FOUND);
  }
}

/**
 * Set the heater command
 */
extern "C" json_t * b_device_set_heater (json_t * device, const char * heater_name, const char * mode, const float command, void * device_ptr) {
  ValueID * value = NULL;
  string s_status;
  uint8 node_id;
  char * end_ptr;
  uint nb_setpoints;
  uint i;
  char str_command[33];
  
  node_id = strtol(strstr(heater_name, "$") + 1, &end_ptr, 10);
  if (*end_ptr == '\0') {
    value = get_device_value_id(get_device_node((zwave_context *)device_ptr, node_id), COMMAND_CLASS_THERMOSTAT_MODE);
    if (value != NULL) {
      Manager::Get()->SetValue(*value, string(mode));
    }
    
    nb_setpoints = get_device_value_id_count(get_device_node((zwave_context *)device_ptr, node_id), COMMAND_CLASS_THERMOSTAT_SETPOINT);
    for (i=0; i<nb_setpoints; i++) {
      value = get_device_value_id_by_index(get_device_node((zwave_context *)device_ptr, node_id), COMMAND_CLASS_THERMOSTAT_SETPOINT, i);
      if (value != NULL) {
        snprintf(str_command, 32*sizeof(char), "%.2f", command);
        Manager::Get()->SetValue(*value, string(str_command));
      }
    }
    
    return json_pack("{si}", "result", RESULT_OK);
  } else {
    y_log_message(Y_LOG_LEVEL_ERROR, "b_device_get_heater - Error node not found");
    return json_pack("{si}", "result", RESULT_NOT_FOUND);
  }
}

/**
 * Return true if an element with the specified name and the specified type exist in this device
 */
extern "C" int b_device_has_element (json_t * device, int element_type, const char * element_name, void * device_ptr) {
  char * str_type, * str_node_id, * str_label, * save_ptr, * dup_name_save, * dup_name = dup_name_save = o_strdup(element_name);
  ValueID * value;
  
  if (u_map_has_key(((zwave_context *)device_ptr)->alarms, element_name) && element_type == ELEMENT_TYPE_SENSOR) {
    return 1;
  } else if (element_type == ELEMENT_TYPE_SENSOR) {
    // first token is the type, supposed to be "se" in this case
    str_type = strtok_r(dup_name, "$", &save_ptr);
    
    // Assigning the next 2 tokens
    str_node_id = strtok_r(NULL, "$", &save_ptr);
    str_label = strtok_r(NULL, "$", &save_ptr);
    
    if (str_type == NULL || str_node_id == NULL || str_label == NULL) {
      o_free(dup_name_save);
      return 0;
    } else {
      value = get_device_value_id_by_label(get_device_node((zwave_context *)device_ptr, strtol(str_node_id, NULL, 10)), COMMAND_CLASS_SENSOR_BINARY, str_label);
      if (value == NULL) {
        value = get_device_value_id_by_label(get_device_node((zwave_context *)device_ptr, strtol(str_node_id, NULL, 10)), COMMAND_CLASS_SENSOR_MULTILEVEL, str_label);
      }
      o_free(dup_name_save);
      return (value != NULL);
    }
  } else {
    str_type = strtok_r(dup_name, "$", &save_ptr);
    str_node_id = strtok_r(NULL, "$", &save_ptr);
    if (str_type == NULL || str_node_id == NULL) {
      o_free(dup_name_save);
      return 0;
    } else {
      switch (element_type) {
        case ELEMENT_TYPE_SWITCH:
          value = get_device_value_id(get_device_node((zwave_context *)device_ptr, strtol(str_node_id, NULL, 10)), COMMAND_CLASS_SWITCH_BINARY);
          break;
        case ELEMENT_TYPE_DIMMER:
          value = get_device_value_id(get_device_node((zwave_context *)device_ptr, strtol(str_node_id, NULL, 10)), COMMAND_CLASS_SWITCH_MULTILEVEL);
          break;
        case ELEMENT_TYPE_HEATER:
          value = get_device_value_id(get_device_node((zwave_context *)device_ptr, strtol(str_node_id, NULL, 10)), COMMAND_CLASS_THERMOSTAT_SETPOINT);
          break;
        default:
          value = NULL;
          break;
      }
      o_free(dup_name_save);
      return (value != NULL);
    }
  }
}

/**
 * Get the device overview
 * For my own mental sanity, I will assume that a node has only one element, except for sensors, where they will be identified with their label and node id
 * This seems to work on my devices, where I have switches, dimmers and heaters that are one per node, but I also have a multisensor aeotech, which has multiple values
 */
extern "C" json_t * b_device_overview (json_t * device, void * device_ptr) {  
  json_t * overview = json_object(), * value;
  char * name, * unit, * end_ptr_d;
	const char * cur_node = NULL;
  string s_status;
  bool b_status;
  double d_value;
  int i;
  const char ** keys = NULL;
  
  list<node*> * nodes_list = (list<node*> *) ((zwave_context *)device_ptr)->nodes_list;
  for( list<node*>::iterator it = nodes_list->begin(); it != nodes_list->end(); ++it ) {
    node* node = *it;
    for( list<ValueID>::iterator it2 = node->values.begin(); it2 != node->values.end(); ++it2 ) {
      ValueID v = *it2;
      
      // getting switchers (COMMAND_CLASS_SWITCH_BINARY)
      if ( v.GetCommandClassId() == COMMAND_CLASS_SWITCH_BINARY ) { //COMMAND_CLASS_SWITCH_BINARY
        Manager::Get()->RefreshValue(v);
        name = msprintf("sw$%02d", node->node_id);
        cur_node = "switches";
        unit = (char *)Manager::Get()->GetValueUnits(v).c_str();
        try {
          if (Manager::Get()->GetValueAsBool(v, &b_status)) {
            if (json_object_get(overview, cur_node) == NULL) {
              json_object_set_new(overview, cur_node, json_object());
            }
            if (unit != NULL && strlen(unit) >0) {
              json_object_set_new(json_object_get(overview, cur_node), name, json_pack("{sssi}", "unit", unit, "value", b_status?1:0));
            } else {
              json_object_set_new(json_object_get(overview, cur_node), name, json_integer(b_status?1:0));
            }
          }
        } catch (OpenZWave::OZWException &) {
#ifdef DEBUG
          y_log_message(Y_LOG_LEVEL_DEBUG, "Exception refresh value binary (COMMAND_CLASS_SWITCH_BINARY) %s, genre %s, type %s", v.GetAsString().c_str(), v.GetGenreAsString().c_str(), v.GetTypeAsString().c_str());
#endif
        }
        o_free(name);
      } else if ( v.GetCommandClassId() == COMMAND_CLASS_SWITCH_MULTILEVEL ) { // COMMAND_CLASS_SWITCH_MULTILEVEL - Dimmer
        Manager::Get()->RefreshValue(v);
        if (0 == strcasecmp(Manager::Get()->GetValueLabel(v).c_str(), "level")) {
          name = msprintf("di$%02d", node->node_id);
          cur_node = "dimmers";
          unit = (char *)Manager::Get()->GetValueUnits(v).c_str();
          try {
            if (Manager::Get()->GetValueAsString(v, &s_status)) {
              if (s_status.compare("0")) {
                u_map_put(((zwave_context *)device_ptr)->dimmer_values, name, s_status.c_str());
              }
              if (json_object_get(overview, cur_node) == NULL) {
                json_object_set_new(overview, cur_node, json_object());
              }
              if (unit != NULL && strlen(unit) >0) {
                json_object_set_new(json_object_get(overview, cur_node), name, json_pack("{sssi}", "unit", unit, "value", strtol(s_status.c_str(), NULL, 10)));
              } else {
                json_object_set_new(json_object_get(overview, cur_node), name, json_integer(strtol(s_status.c_str(), NULL, 10)));
              }
            }
          } catch (OpenZWave::OZWException &) {
#ifdef DEBUG
            y_log_message(Y_LOG_LEVEL_DEBUG, "Exception refresh value string (COMMAND_CLASS_SWITCH_MULTILEVEL) %s, genre %s, type %s", v.GetAsString().c_str(), v.GetGenreAsString().c_str(), v.GetTypeAsString().c_str());
#endif
          }
          o_free(name);
        }
      } else if ( v.GetCommandClassId() == COMMAND_CLASS_SENSOR_BINARY ) { // COMMAND_CLASS_SENSOR_BINARY - binary sensor
        Manager::Get()->RefreshValue(v);
        char * named_label = naming_label(Manager::Get()->GetValueLabel(v).c_str());
        name = msprintf("se$%02d$%s", node->node_id, named_label);
        cur_node = "sensors";
        unit = (char *)Manager::Get()->GetValueUnits(v).c_str();
        try {
          if (Manager::Get()->GetValueAsBool(v, &b_status)) {
            if (json_object_get(overview, cur_node) == NULL) {
              json_object_set_new(overview, cur_node, json_object());
            }
            if (unit != NULL && strlen(unit) >0) {
              json_object_set_new(json_object_get(overview, cur_node), name, json_pack("{sssi}", "unit", unit, "value", b_status?1:0));
            } else {
              json_object_set_new(json_object_get(overview, cur_node), name, b_status?json_true():json_false());
            }
          }
        } catch (OpenZWave::OZWException &) {
#ifdef DEBUG
          y_log_message(Y_LOG_LEVEL_DEBUG, "Exception refresh value binary (COMMAND_CLASS_SENSOR_BINARY) %s, genre %s, type %s", v.GetAsString().c_str(), v.GetGenreAsString().c_str(), v.GetTypeAsString().c_str());
#endif
        }
        o_free(name);
        free(named_label);
      } else if ( v.GetCommandClassId() == COMMAND_CLASS_SENSOR_MULTILEVEL ) { // COMMAND_CLASS_SENSOR_MULTILEVEL - sensor
        Manager::Get()->RefreshValue(v);
        char * named_label = naming_label(Manager::Get()->GetValueLabel(v).c_str());
        name = msprintf("se$%02d$%s", node->node_id, named_label);
        cur_node = "sensors";
        unit = (char *)Manager::Get()->GetValueUnits(v).c_str();
        try {
          if (Manager::Get()->GetValueAsString(v, &s_status)) {
            d_value = strtof(s_status.c_str(), &end_ptr_d);
            if (end_ptr_d != s_status.c_str()) {
              value = json_real(d_value);
            } else {
              value = json_string(s_status.c_str());
            }
            if (json_object_get(overview, cur_node) == NULL) {
              json_object_set_new(overview, cur_node, json_object());
            }
            if (unit != NULL && strlen(unit) >0) {
              json_object_set_new(json_object_get(overview, cur_node), name, json_pack("{ssso}", "unit", unit, "value", json_copy(value)));
            } else {
              json_object_set_new(json_object_get(overview, cur_node), name, json_copy(value));
            }
            json_decref(value);
          }
        } catch (OpenZWave::OZWException &) {
#ifdef DEBUG
          y_log_message(Y_LOG_LEVEL_DEBUG, "Exception refresh value string (COMMAND_CLASS_SENSOR_MULTILEVEL) %s, genre %s, type %s", v.GetAsString().c_str(), v.GetGenreAsString().c_str(), v.GetTypeAsString().c_str());
#endif
        }
        o_free(name);
        free(named_label);
      } else if ( v.GetCommandClassId() == COMMAND_CLASS_THERMOSTAT_SETPOINT ) { // COMMAND_CLASS_THERMOSTAT_SETPOINT - heater
        Manager::Get()->RefreshValue(v);
        name = msprintf("he$%02d", node->node_id);
        cur_node = "heaters";
        if (json_object_get(json_object_get(overview, cur_node), name) == NULL) {
          value = b_device_get_heater(device, name, device_ptr);
          if (value != NULL && json_integer_value(json_object_get(value, "result")) == RESULT_OK) {
            if (json_object_get(overview, cur_node) == NULL) {
              json_object_set_new(overview, cur_node, json_object());
            }
            json_object_set_new(json_object_get(overview, cur_node), name, json_copy(json_object_get(value, "value")));
          }
          json_decref(value);
        }
        o_free(name);
      }
    }
  }
  // Append notified alarms
  keys = u_map_enum_keys(((zwave_context *)device_ptr)->alarms);
  for (i=0; keys != NULL && keys[i] != NULL; i++) {
    cur_node = "sensors";
    if (json_object_get(overview, cur_node) == NULL) {
      json_object_set_new(overview, cur_node, json_object());
    }
    json_object_set_new(json_object_get(overview, cur_node), keys[i], json_pack("{sosi}", "trigger", json_true(), "value", 0));
  }
  json_object_set_new(overview, "result", json_integer(RESULT_OK));
  return overview;
}
