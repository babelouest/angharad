# Carleon services

Carleon implements an interface to control multiple types of house automation services. To add a new service, you have to create a library with the proper functions, then place the library in the folder specified by `services_path` in your configuration file, then restart the program.

In the service module, you have access to carleon configuration and the ulfius instance, which means you can access the database without instanciating your own database connector:

```C
/**
 *
 * Carleon configuration structure
 *
 */
struct _carleon_config {
  char                    * services_path;
  struct _h_connection    * conn;
  struct _carleon_service * service_list;
  char                    * alert_url;
};
```

The intended behaviour is to create your own webservices that will be available within Carleon. Within your service, the user can create elements and execute commands to those elements. Commands can be automatized via Angharad scripts and events.

The mock service in the file `service-mock.c` can be used to create your own module.

## Service module interace

A service module is a library loaded with `dlopen/dlsync`. All mandatory functions will return a json object of type `json_t *`.

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
 * Initializes the service
 * You can add here your own ulfius endpoints
 * 
 * Return value
 * 
 * {
 *   result: number, result value
 *   name: string, name of the service
 *   description: string, description for the service
 * }
 * 
 */
json_t * c_service_init(struct _u_instance * instance, const char * url_prefix, struct _carleon_config * config);

/**
 * Closes the service
 * You can remove your endpoints here
 * 
 * Return value
 * {
 *   result: number, result value
 * }
 * 
 */
json_t * c_service_close(struct _u_instance * instance, const char * url_prefix);


/**
 * Get the available commands executable within angharad scripts or events
 * 
 * Return value
 * {
 *   result: number, result value
 *   commands: object containing different commands
 * }
 * 
 * A command must have the following format:
 * command_name: {
 *   parameters: {
 *     param_name_1: {
 *       type: "string"|"integer"|"boolean",
 *       required: boolean (default is false)
 *     },
 *     param_name_2: {
 *       type: "string"|"integer"|"boolean",
 *       required: boolean (default is false)
 *     },
 *     etc
 *   },
 *   result: {
 *     type: "string"|"integer"|"boolean"
 *     OR
 *     {
 *       result_value_1: { type: "string"|"integer"|"boolean" },
 *       result_value_2: { type: "string"|"integer"|"boolean" },
 *       etc
 *     }
 *   }
 * }
 */
json_t * c_service_command_get_list(struct _carleon_config * config);

/**
 * Get the available elements of the service
 * 
 * Return value
 * {
 *   result: number, result value
 *   element: array of elements
 * }
 * 
 * An element returned must have at least the following format:
 * {
 *   name: string, name of the element
 * }
 * 
 */
json_t * c_service_element_get_list(struct _carleon_config * config);

/**
 * Execute the specified command to the specified element with the specified parameters
 * Return the value of the command if needed
 * 
 * Return value
 * {
 *   result: number, result value
 *   value: "string"|"integer"|"boolean" OR object containing the result values names and their values
 * }
 * 
 */
json_t * c_service_exec(struct _carleon_config * config, const char * command, const char * element, json_t * parameters);
```

## Services API

Each service implement its own API. The current documentation can be fund here:

- [mock service API](API.mock.md)
- [motion service API](API.motion.md)
- [mpd service API](API.mpd.md)
