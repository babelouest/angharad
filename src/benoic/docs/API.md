# Benoic API Documentation

Benoic has several endpoints with their own format. Here is the full documentaition of the REST API.

All the return data are in JSON format.

All URL have the prefix called `BENOIC_PREFIX`, default is `/benoic`. All the URLs given in this dcumentation are relative to `BENOIC_PREFIX`.

## Authentication

If used within Angharad application, and except when mentionned otherwise, all endpoints require a valid authentication token located in the header or in the cookies. The header or cookies key must be called `"ANGHARAD_SESSION_ID"` and the value must be a valid token value returned b a previous successfull login.

## Devices management

### Get all device types

Get all the different device types available

#### URL

`/deviceTypes/`

#### Method

`GET`

#### Success response

Code 200

Content
```javascript
[ Array of device types
    {

        "uid": string, uid of the type, max 64 chars
        "name":string, name of the type, max 64 chars
        "description":string, descrition of the type, max 512 chars
        "enabled":boolean
        "options":[ // Array of options avaiable to ad a new device of this type
            {
                "name":string, name of the option
                "type":string, values can be "string", "integer", "float" or "boolean"
                "description":string, description of the option
                "optional":boolean
            }
        ],

    }
]
```

#### Error Response

Code 500

Internal Error

### Get all devices

#### URL

`/device/`

#### Method

`GET`

#### Success response

Code 200

Content
```javascript
[ array of device objects

    {
        "name":string, name of the device, max 64 chars
        "type_uid":string, uid of the device type
        "description":string, description for the device, max 512 chars
        "enabled":boolean
        "connected":boolean
        "last_seen":string, date of last seen device connected, in ISO 8601 format
        "options":{ object containing options for the current device
        },
    }

]
```

#### Error Response

Code 500

Internal Error

### Get a specific device by its name

#### URL

`/device/@device_name`

#### Method

`GET`

#### URL Parameters

**Required**

`@device_name`: device name

#### Success response

Code 200

Content
```javascript
{
    "name":string, name of the device, max 64 chars
    "type_uid":string, uid of the device type
    "description":string, description for the device, max 512 chars
    "enabled":boolean
    "connected":boolean
    "last_seen":string, date of last seen device connected, in ISO 8601 format
    "options":{ object containing options for the current device
    },
}
```

#### Error Response

Code 500

Internal Error

OR

Code 404

Device not found

### Add a new device

#### URL

`/device/`

#### Method

`POST`

#### Data Parameters

```javascript
{
    "name":string, name of the device, max 64 chars, name must not already exist
    "type_uid":string, uid of the device type
    "description":string, description for the device, max 512 chars
    "enabled":boolean
    "connected":boolean
    "options":{ object containing options for the current device
    },
}
```

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

Content: json array containing all errors

### Modify an existing device

#### URL

`/device/@device_name`

#### Method

`PUT`

#### URL Parameters

**Required**

`@device_name`: device name

#### Data Parameters

```javascript
{
    "description":string, description for the device, max 512 chars
    "enabled":boolean
    "connected":boolean
    "options":{ object containing options for the current device
    },
}
```

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

Content: json array containing all errors

OR

Code 404

Device not found

### Delete a device

#### URL

`/device/@device_name`

#### Method

`DELETE`

#### URL Parameters

**Required**

`@device_name`: device name

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

Device not found

### Connect a device

#### URL

`/device/@device_name/connect`

#### Method

`GET`

#### URL Parameters

**Required**

`@device_name`: device name

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

Device not found

### Disconnect a device

#### URL

`/device/@device_name/disconnect`

#### Method

`GET`

#### URL Parameters

**Required**

`@device_name`: device name

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

Device not found

### Ping a device

#### URL

`/device/@device_name/ping`

#### Method

`GET`

#### URL Parameters

**Required**

`@device_name`: device name

#### Success response

Code 200

#### Error Response

Code 503

Device not responding to ping command

OR

Code 500

Internal Error

OR

Code 400

Device disconnected or disabled

OR

Code 404

Device not found

## Device elements overview

### Overview Device

#### URL

`/device/@device_name/overview`

#### Method

`GET`

#### URL Parameters

**Required**

`@device_name`: device name

#### Success response

Code 200

Content
```javascript
{

    "sensors":{ Object containing all sensors
        "sensor_name_1":{ string, internal name
            "display":string, element display name, max 128 chars
            "description":string, element description, max 128 chars
            "value":boolean|string|number, sensor value
            "monitored":boolean
            "monitored_every":integer, monitor frequency in seconds
            "options":{ options object
                "unit":string
            },
            "enabled":bolean
        },
        "sensor_name_2":{ string, internal name
            "display":string, element display name, max 128 chars
            "description":string, element description, max 128 chars
            "value":boolean
            "monitored":boolean
            "monitored_every":integer, monitor frequency in seconds
            "options":{ options object
                "unit":string
            },
            "enabled":bolean
        }
    },
    "switches":{ Object containing all switches
        "switch_name_1":{ string, internal name
            "display":string, element display name, max 128 chars
            "description":string, element description, max 128 chars
            "value":integer, 0 (off) or 1 (on)
            "monitored":boolean
            "monitored_every":integer, monitor frequency in seconds
            "options":{ options object
            },
            "enabled":bolean
        },
        "switch_name_2":{ string, internal name
            "display":string, element display name, max 128 chars
            "description":string, element description, max 128 chars
            "value":integer, 0 (off) or 1 (on)
            "monitored":boolean
            "monitored_every":integer, monitor frequency in seconds
            "options":{ options object
            },
            "enabled":bolean
        }
    },
    "dimmers":{ Object containing all dimmers
        "dimmer_name_1":{ string, internal name
            "display":string, element display name, max 128 chars
            "description":string, element description, max 128 chars
            "value":integer, between 0 (off) and 100 (max)
            "monitored":boolean
            "monitored_every":integer, monitor frequency in seconds
            "options":{ options object
            },
            "enabled":bolean
        },
        "dimmer_name_2":{ string, internal name
            "display":string, element display name, max 128 chars
            "description":string, element description, max 128 chars
            "value":integer, between 0 (off) and 100 (max)
            "monitored":boolean
            "monitored_every":integer, monitor frequency in seconds
            "options":{ options object
            },
            "enabled":bolean
        }
    },
    "heaters":{ Object containing all heaters
        "heater_name_1":{
            "display":string, element display name, max 128 chars
            "description":string, element description, max 128 chars
            "options":{ options object
                "unit":string
            },
            "enabled":bolean
            "monitored":bolean
            "monitored_every":integer, monitor frequency in seconds
            "value":{ value object for a heater
                "mode":string, current mode
                "command":number, temperature command
                "on":boolean, indicates if the heat is on or off
                "availableModes":[ array of strings to get all modes available
                ]
            }
        },
        "heater_name_2":{
            "display":string, element display name, max 128 chars
            "description":string, element description, max 128 chars
            "options":{ options object
                "unit":string
            },
            "enabled":bolean
            "monitored":bolean
            "monitored_every":integer, monitor frequency in seconds
            "value":{ value object for a heater
                "mode":string, current mode
                "command":number, temperature command
                "on":boolean, indicates if the heat is on or off
                "availableModes":[ array of strings to get all modes available
                ]
            }
        }
    }

}
```

#### Error Response

Code 500

Internal Error

OR

Code 404

Device not found

## Elements management

"GET", url_prefix, "/device/@device_name/@element_type/@element_name"

### Get a specific element data

#### URL

`/device/@device_name/@element_type/@element_name`

#### Method

`GET`

#### URL Parameters

**Required**

`@device_name`: device name

`@element_type`: string, element type, values are "switch", "dimmer", "sensor" or "heater"

`@element_name`: element name

#### Success response

Code 200

Depending on your element type, the content will vary

Content
```javascript
sensor value:
{
    "display":string, element display name, max 128 chars
    "description":string, element description, max 128 chars
    "value":boolean|string|number, sensor value
    "monitored":boolean
    "monitored_every":integer, monitor frequency in seconds
    "options":{ options object
        "unit":string
    },
    "enabled":bolean
}

switch value:
{
    "display":string, element display name, max 128 chars
    "description":string, element description, max 128 chars
    "value":integer, 0 (off) or 1 (on)
    "monitored":boolean
    "monitored_every":integer, monitor frequency in seconds
    "options":{ options object
    },
    "enabled":bolean
}

dimmer value:
{
    "display":string, element display name, max 128 chars
    "description":string, element description, max 128 chars
    "value":integer, between 0 (off) and 100 (max)
    "monitored":boolean
    "monitored_every":integer, monitor frequency in seconds
    "options":{ options object
    },
    "enabled":bolean
}

heater value:
{
    "display":string, element display name, max 128 chars
    "description":string, element description, max 128 chars
    "options":{ options object
        "unit":string
    },
    "enabled":bolean
    "monitored":bolean
    "monitored_every":integer, monitor frequency in seconds
    "value":{ value object for a heater
        "mode":string, current mode
        "command":number, temperature command
        "on":boolean, indicates if the heat is on or off
        "availableModes":[ array of strings to get all modes available
        ]
    }
}
```

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

Element type incorrect

OR

Code 404

Device or element not found

### Send a command to an element

#### URL

`/device/@device_name/@element_type/@element_name/@command`

#### Method

`GET`

#### URL Parameters

**Required**

`@device_name`: device name

`@element_type`: string, element type, values are "switch", "dimmer", "sensor" or "heater"

`@element_name`: element name

`@command`: number, command value

**Optional**

`@mode`: string, heater mode (heater only)

#### Success response

Code 200

Content

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

Content: json array containing all errors

OR

Code 404

Device or element not found

### Update an element data

#### URL

`/device/@device_name/@element_type/@element_name`

#### Method

`PUT`

#### URL Parameters

**Required**

`@device_name`: device name

`@element_type`: string, element type, values are "switch", "dimmer", "sensor" or "heater"

`@element_name`: element name

#### Data Parameters

```javascript
{
    "display": string, max 128 chars
    "description": string, max 128 chars
    "enabled": boolean
    "options": object containing options values, example unit value for sensors
    "monitored":bolean
    "monitored_every":integer, monitor frequency in seconds
}
```

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

Content: json array containing all errors

OR

Code 404

Device or element not found

### Add a tag to an element

#### URL

`/device/@device_name/@element_type/@element_name/@tag`

#### Method

`PUT`

#### URL Parameters

**Required**

`@device_name`: device name

`@element_type`: string, element type, values are "switch", "dimmer", "sensor" or "heater"

`@element_name`: element name

`@tag`: string, tag value

#### Success response

Code 200

Content

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

Content: json array containing all errors

OR

Code 404

Device or element not found

### Delete a tag from an element

#### URL

`/device/@device_name/@element_type/@element_name/@tag`

#### Method

`DELETE`

#### URL Parameters

**Required**

`@device_name`: device name

`@element_type`: string, element type, values are "switch", "dimmer", "sensor" or "heater"

`@element_name`: element name

`@tag`: string, tag value

#### Success response

Code 200

Content

#### Error Response

Code 500

Internal Error

OR

Code 404

Device or element not found

### Get an element's monitored values

#### URL

`/monitor/@device_name/@element_type/@element_name/`

#### Method

`GET`

#### URL Parameters

**Required**

`@device_name`: device name

`@element_type`: string, element type, values are "switch", "dimmer", "sensor" or "heater"

`@element_name`: element name

**Optional**

`from`: Start date for the monitoring, in UNX EPOCH format

`to`: End date for the monitoring, in UNX EPOCH format

#### Success response

Code 200

Content
```javascript
[ Array of monitor objects

    {
        "timestamp":integer, date for the monitoring, in UNX EPOCH format
        "value":string, value for the monitoring
    }

]
```

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

Content: json array containing all errors

OR

Code 404

Device or element not found
