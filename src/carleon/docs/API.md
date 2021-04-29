# Carleon API Documentation

Carleon has several endpoints with their own format. Here is the full documentation of the REST API.

All the return data are in JSON format.

All URL have the prefix called `CARLEON_PREFIX`, default is `/carleon`. All the URLs given in this documentation are relative to `CARLEON_PREFIX`.

This is only carleon API, each carleon services can add their own endpoints, their documentation can be found in [carleon service modules](service-modules/README.md)

## Authentication

Except when mentioned otherwise, all endpoints require a valid oauth2 token in Glewlwyd format. The token must contain the scope `angharad`.

### Get all services available, their description, their elements and their commands

#### URL

`/service/`

#### Method

`GET`

#### Success response

Code 200

Content
```javascript
[

    {
        "name":string, name of the service, max 64 chars
        "description":string, description of the service, max 64 chars
        "enabled":boolean
        "commands":{ object containing all commands
            "exec1":{ command name
                "parameters":{ input parameters
                    "param1":{ parameter name
                        "type":string, values can be "string", "integer", "real" or "boolean"
                        "required":boolean
                    },
                    "param2":{ another parameter name
                        "type":string, values can be "string", "integer", "real" or "boolean"
                        "required":boolean
                    },
                    "param3":{
                        "type":"real"
                    }
                },
                "result":{ Result object, this command has 2 values in its result
                    "value1":{ result name
                        "type":string, values can be "string", "integer", "real" or "boolean"
                    },
                    "value2":{ another result name
                        "type":string, values can be "string", "integer", "real" or "boolean"
                    }
                }
            },
            "exec2":{ another command name
                "result":{ result object, this command has 1 value in its result
                    "type":string, values can be "string", "integer", "real" or "boolean"
                }
            },
        },
        "element":[ Elements list of all the service elements
            {
                "name":string, element name, max 64 chars
                "description":string, element description, max 64 chars
                "tags":[ Array of tags, each tag is astring, max 64 chars, max 128 tags per element
                ]
            }
        ]
    },

]
```

#### Error Response

Code 500

Internal Error

### Enable or disable a service

#### URL

`/service/@service_name/enable/@enable_value`

#### Method

`PUT`

#### URL Parameters

**Required**

`@service_name`: service name

`@enable_value`: 0 (disable) or 1 (enable)

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

Service not found

### Add a tag to a service element

#### URL

`/service/@service_name/@element_id/@tag`

#### Method

`PUT`

#### URL Parameters

**Required**

`@service_name`: service name

`@element_id`: element name

`@tag`: string, tag value

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

Service or element not found

### Remove a tag from a service element

#### URL

`/service/@service_name/@element_id/@tag`

#### Method

`DELETE`

#### URL Parameters

**Required**

`@service_name`: service name

`@element_id`: element name

`@tag`: string, tag value

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

Service or element or tag not found

### Clean all data of a service element

#### URL

`/service/@service_name/@element_id/cleanup`

#### Method

`PUT`

#### URL Parameters

**Required**

`@service_name`: service name

`@element_id`: element name

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

Service or element not found
