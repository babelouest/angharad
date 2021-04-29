# Benoic modules

Benoic implements an interface to control multiple types of house automation devices. To add a new device type, you have to create a library with the proper functions, then place the library in the folder specified by `modules_path` in your configuration file, then restart the program.

The mock module in the file `device-mock.c` can be used to create your own module.

## Device module interace

A device module is a library loaded with `dlopen/dlsync`. All mandatory functions will return a json object of type `json_t *`, except for `b_device_has_element` which returns a int.

The returned json object must have the following format:

```json
{
  result: number,
  other values
}
```

The `result` value is used to return a status code. The different status codes available are:

```C
#define WEBSERVICE_RESULT_ERROR     0
#define WEBSERVICE_RESULT_OK        1
#define WEBSERVICE_RESULT_NOT_FOUND 2
#define WEBSERVICE_RESULT_TIMEOUT   3
#define WEBSERVICE_RESULT_PARAM     4
```

These functions are mandatory for a device module:

```C
/**
 *
 * Device type initialization
 * 
 * Initialize the device type and return a json object of the following format:
 * {
 *   result: number, result value
 *   uid: string, a unique identifier for your device type
 *   name: string, the name of your device type
 *   description: string
 *   options: json array, contains the name and type of parameters used to instanciate a device
 * }
 *
 * The options array has the following format:
 * {
 *   name: string, // Name of the option
 *   type: "string"|"numeric"|"boolean", // Type of th option, must be one of those values
 *   description: string, // Description for the option
 *   optional: boolean // is the option optional or not?
 * }
 * 
 */
json_t * b_device_type_init ();

/**
 * 
 * Connects the device
 * 
 * Connects a device and initialize its parameters and context
 * device parameter is a json object containing the following values:
 * {
 *   name: string
 *   options: array // options values according to the options format returned in the b_device_type_init function
 * }
 * device_ptr is a pointer you can use to store any other parameter, structure or value you want without having to rebuild it every time the device is called
 * 
 * Return value
 * {
 *   result: number, result value
 * }
 * 
 */
json_t * b_device_connect (json_t * device, void ** device_ptr);

/**
 * 
 * Disconnects the device
 * 
 * Disconnect the device and clean device_ptr
 * 
 * Return value
 * {
 *   result: number, result value
 * }
 * 
 */
json_t * b_device_disconnect (json_t * device, void * device_ptr);

/**
 * 
 * Pings the device
 * 
 * Check if the device is still availble
 * 
 * Return value
 * {
 *   result: number, result value
 * }
 * 
 */
json_t * b_device_ping (json_t * device, void * device_ptr);

/**
 * 
 * Get all the elements and their values for the device
 * 
 * Return value
 * {
 *   result: number, result value
 *   sensors: {
 *     sensor_name_1: {
 *       unit: string,
 *       value: string|number
 *     },
 *     sensor_name_2: {
 *       unit: string,
 *       value: string|number
 *     }, etc
 *   }
 *   switches: {
 *     switch_name_1: number (0 or 1),
 *     switch_name_2: number (0 or 1),
 *     etc.
 *   }
 *   dimmers: {
 *     dimmer_name_1: number (between 0 and 100),
 *     dimmer_name_2: number (between 0 and 100),
 *     etc.
 *   }
 *   heaters: {
 *     heater_name_1: {
 *       mode: string,
 *       command: float,
 *       on: boolean,
 *       availableModes: Array of strings listing all the different modes available
 *     },
 *     heater_name_2: {
 *       mode: string,
 *       command: float,
 *       on: boolean,
 *       availableModes: Array of strings listing all the different modes available
 *     }
 *     etc.
 *   }
 * }
 * 
 */
json_t * b_device_overview (json_t * device, void * device_ptr);

/**
 * 
 * Get the value of the specified sensor on the specified device
 * 
 * Return value
 * {
 *   result: number, result value
 *   value: string|number
 * }
 * 
 */
json_t * b_device_get_sensor (json_t * device, const char * sensor_name, void * device_ptr);

/**
 * 
 * Get the value of the specified switch on the specified device
 * 
 * Return value
 * {
 *   result: number, result value
 *   value: number (0 or 1),
 * }
 * 
 */
json_t * b_device_get_switch (json_t * device, const char * switch_name, void * device_ptr);

/**
 * 
 * Send the command on the specified switch on the specified device
 * 
 * Return value
 * {
 *   result: number, result value
 * }
 * 
 */
json_t * b_device_set_switch (json_t * device, const char * switch_name, const int command, void * device_ptr);

/**
 * 
 * Get the value of the specified dimmer on the specified device
 * 
 * Return value
 * {
 *   result: number, result value
 *   value: number (between 0 and 100),
 * }
 * 
 */
json_t * b_device_get_dimmer (json_t * device, const char * dimmer_name, void * device_ptr);

/**
 * 
 * Send the command on the specified dimmer on the specified device
 * 
 * Return value
 * {
 *   result: number, result value
 * }
 * 
 */
json_t * b_device_set_dimmer (json_t * device, const char * dimmer_name, const int command, void * device_ptr);

/**
 * 
 * Get the value of the specified heater on the specified device
 * 
 * Return value
 * {
 *   result: number, result value
 *   mode: string,
 *   command: float,
 *   on: boolean,
 *   availableModes: Array of strings listing all the different modes available
 * }
 * 
 */
json_t * b_device_get_heater (json_t * device, const char * heater_name, void * device_ptr);

/**
 * 
 * Send the command on the specified heater on the specified device
 * 
 * Return value
 * {
 *   result: number, result value
 * }
 * 
 */
json_t * b_device_set_heater (json_t * device, const char * heater_name, const char * mode, const float command, void * device_ptr);

/**
 * 
 * Return true if the specified element exist on the specified device, false otherwise
 * 
 * element_type can have the following values:
 * 
 * ELEMENT_TYPE_SENSOR 1
 * ELEMENT_TYPE_SWITCH 2
 * ELEMENT_TYPE_DIMMER 3
 * ELEMENT_TYPE_HEATER 4
 * 
 */
int b_device_has_element (json_t * device, int element_type, const char * element_name, void * device_ptr);
```
