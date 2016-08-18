# Angharad API Documentation

Angharad has several endpoints with their own format. Here is the full documentaition of the REST API.

All the return data are in JSON format.

Except fo static file API, all URL have the prefix called `ANGHARAD_PREFIX`, default is `/angharad`. All the URLs given in this dcumentation (Except for static files) are relative to `ANGHARAD_PREFIX`.

## Authentication

Except when mentionned otherwise, all endpoints require a valid authentication token located in the header or in the cookies. The header or cookies key must be called `"ANGHARAD_SESSION_ID"` and the value must be a valid token value returned b a previous successfull login.

## Submodules management

### Get all submodules available

#### URL

`/submodule/`

#### Method

`GET`

#### Success response

Code 200

Content
```javascript
[
	{
		"name": string, max 64 characters
		"description": string, max 128 characters
		"enabled": boolean
	}
]
```

#### Error Response

Code 500

Internal Error

### Get submodule specified

#### URL

`/submodule/@submodule_name`

#### Method

`GET`

#### URL Parameters

**Required**

`@submodule_name`: Submodule name to look for

#### Success response

Code 200

Content
```javascript
{
	"name": string, max 64 characters
	"description": string, max 128 characters
	"enabled": boolean
}
```

#### Error Response

Code 500

Internal Error

OR

Code 404

Submodule `@submodule_name`not found

### Enable or disable submodule

#### URL

`/submodule/@submodule_name/enable/@enabled`

#### Method

`GET`

#### URL Parameters

**Required**

`@submodule_name`: Submodule name to look for

`@enabled`: 0 (disable) or 1 (enable)

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

Submodule `@submodule_name`not found

## Scripts management

### Get all scripts available

#### URL

`/script`

#### Method

`GET`

#### Success response

Code 200

Content
```javascript
[
	{

		"name":string, maximum 64 characters, required
		"description":string, maximum 128 characters, optional
		"options":{ optional
			"tags":[
				string, maximum 64 characters
			] Maximum 128 tags
		},
		"actions":[
			{
				"submodule":string, submodule name available (ex, benoic or carleon), required
				"element": string, element name, required
				"parameters":{
					"element_type":string, element type, required if submodule is benoic
					"device":string, device name, required if submodule is benoic
				},
				"command": string|number, command to send to the element, required
			}
		]

	}
]
```

#### Error Response

Code 500

Internal Error

### Get script by its name

#### URL

`/script/@script_name`

#### Method

`GET`

#### URL Parameters

**Required**
`@script_name`: script name

#### Success response

Code 200

Content
```javascript
{

	"name":string, maximum 64 characters, required
	"description":string, maximum 128 characters, optional
	"options":{ optional
		"tags":[
			string, maximum 64 characters
		] Maximum 128 tags
	},
	"actions":[
		{
			"submodule":string, submodule name available (ex, benoic or carleon), required
			"element": string, element name, required
			"parameters":{
				"element_type":string, element type, required if submodule is benoic
				"device":string, device name, required if submodule is benoic
			},
			"command": string|number, command to send to the element, required
		}
	]

}
```

#### Error Response

Code 500

Internal Error

OR

Code 404

Script not found

### Add new script

#### URL

`/script/`

#### Method

`POST`

#### Data Parameters

```javascript
{

	"name":string, maximum 64 characters, must be a new name, required
	"description":string, maximum 128 characters, optional
	"options":{ optional
		"tags":[
			string, maximum 64 characters
		] Maximum 128 tags
	},
	"actions":[
		{
			"submodule":string, submodule name available (ex, benoic or carleon), required
			"element": string, element name, required
			"parameters":{
				"element_type":string, element type, required if submodule is benoic
				"device":string, device name, required if submodule is benoic
			},
			"command": string|number, command to send to the element, required
		}
	]

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

### Modify a script

#### URL

`/script/@script_name`

#### Method

`PUT`

#### URL Parameters

**Required**

`@script_name`: Script name

#### Data Parameters

```javascript
{

	"description":string, maximum 128 characters, optional
	"options":{ optional
		"tags":[
			string, maximum 64 characters
		] Maximum 128 tags
	},
	"actions":[
		{
			"submodule":string, submodule name available (ex, benoic or carleon), required
			"element": string, element name, required
			"command": string|number, command to send to the element, required
			"parameters":{
				"element_type":string, element type, required if submodule is benoic
				"device":string, device name, required if submodule is benoic
			}
		}
	]

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

### Remove a script

#### URL

`/script/@script_name`

#### Method

`DELETE`

#### URL Parameters

**Required**

`@script_name`: Script name

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

Script not found

### Add tag to script

#### URL

`/script/@script_name/@tag`

#### Method

`PUT`

#### URL Parameters

**Required**

`@tag`: tag value, maximum 64 characters, maximum 128 tags per element

`@script_name`: Script name

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

script not found

### Delete tag from script

#### URL

`/script/@script_name/@tag`

#### Method

`DELETE`

#### URL Parameters

**Required**

`@tag`: tag value, maximum 64 characters, maximum 128 tags per element

`@script_name`: Script name

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

script or tag not found

### Run script

#### URL

`/script/@script_name/run`

#### Method

`GET`

#### URL Parameters

**Required**

`@script_name`: Script name

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404
Script not found

## Schedulers management

### Get all schedulers

#### URL

`/scheduler/`

#### Method

`GET`

#### Success response

Code 200

Content
```javascript
[ Array of scripts

    {
        "name":string, 64 characters max
        "description":string, 128 characters max
        "enabled":boolean
        "repeat":integer, values possible are -1: None, 0: minute, 1: hours, 2: days, 3: day of week, 4: month, 5: year
        "repeat_value":positive integer
        "next_time":integer, next launch in UNIX EPOCH format
        "remove_after":boolean, if true, this scheduler will be autmatically removed when done
        "conditions":[ Array of conditions objects
			{
				"condition":string, values can be '==', '!=', '<', '<=', '>', '>=', 'contains' and 'not contains'
				"value":string|integer|boolean
				"submodule":string, submodule name available (ex, benoic or carleon), required
				"element": string, element name, required
				"parameters":{
					"device":string, device name, required if submodule is benoic
					"element_type":string, element type, required if submodule is benoic
				},
			}
        ],
        "scripts":[ Array of scripts to run
            {
                "name":script name
                "enabled":boolean
            }
        ],
		"options":{ optional
			"tags":[
				string, maximum 64 characters
			] Maximum 128 tags
		},
    }

]
```

#### Error Response

Code 500

Internal Error

### Get scheduler by its name

#### URL

`/scheduler/@scheduler_name`

#### Method

`GET`

#### URL Parameters

**Required**

`@scheduler_name`: Scheduler name

#### Success response

Code 200
Content
```javascript
{
	"name":string, 64 characters max
	"description":string, 128 characters max
	"enabled":boolean
	"repeat":integer, values possible are -1: None, 0: minute, 1: hours, 2: days, 3: day of week, 4: month, 5: year
	"repeat_value":positive integer
	"next_time":integer, next launch in UNIX EPOCH format
	"remove_after":boolean, if true, this scheduler will be autmatically removed when done
	"conditions":[ Array of conditions objects
		{
			"condition":string, values can be '==', '!=', '<', '<=', '>', '>=', 'contains' and 'not contains'
			"value":string|integer|boolean
			"submodule":string, submodule name available (ex, benoic or carleon), required
			"element": string, element name, required
			"parameters":{
				"device":string, device name, required if submodule is benoic
				"element_type":string, element type, required if submodule is benoic
			},
		}
	],
	"scripts":[ Array of scripts to run
		{
			"name":script name
			"enabled":boolean
		}
	],
	"options":{ optional
		"tags":[
			string, maximum 64 characters
		] Maximum 128 tags
	},
}
```

#### Error Response

Code 500

Internal Error

OR

Code 404

Scheduler not found

### Enable or disable a scheduler

#### URL
`/scheduler/@scheduler_name/enable/@enabled`

#### Method

`GET`

#### URL Parameters

**Required**

`@scheduler_name`: Scheduler name

`@enabled`: 1 (true) or 0 (false)

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

OR

Code 404

Scheduler not found

### Add a new scheduler

#### URL

`/scheduler/`

#### Method

`POST`

#### Data Parameters

```javascript
{
	"name":string, 64 characters max, required
	"description":string, 128 characters max
	"enabled":boolean, required
	"repeat":integer, values possible are -1: None, 0: minute, 1: hours, 2: days, 3: day of week, 4: month, 5: year, required
	"repeat_value":positive integer, required
	"remove_after":boolean, if true, this scheduler will be autmatically removed when done
	"conditions":[ Array of conditions objects, conditions are optional
		{
			"condition":string, values can be '==', '!=', '<', '<=', '>', '>=', 'contains' and 'not contains', required
			"value":string|integer|boolean, required
			"submodule":string, submodule name available (ex, benoic or carleon), required
			"element": string, element name, required
			"parameters":{
				"device":string, device name, required if submodule is benoic
				"element_type":string, element type, required if submodule is benoic
			},
		}
	],
	"scripts":[ Array of scripts to run, at least one scrit is required
		{
			"name":script name, required
			"enabled":boolean, required
		}
	],
	"options":{ optional
		"tags":[
			string, maximum 64 characters
		] Maximum 128 tags
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

### Modify an existing scheduler

#### URL

`/scheduler/@scheduler_name`

#### Method

`PUT`

#### URL Parameters

**Required**

`@scheduler_name`: Scheduler name

#### Data Parameters

```javascript
{
	"description":string, 128 characters max
	"enabled":boolean, required
	"repeat":integer, values possible are -1: None, 0: minute, 1: hours, 2: days, 3: day of week, 4: month, 5: year, required
	"repeat_value":positive integer, required
	"remove_after":boolean, if true, this scheduler will be autmatically removed when done
	"conditions":[ Array of conditions objects, conditions are optional
		{
			"condition":string, values can be '==', '!=', '<', '<=', '>', '>=', 'contains' and 'not contains', required
			"value":string|integer|boolean, required
			"submodule":string, submodule name available (ex, benoic or carleon), required
			"element": string, element name, required
			"parameters":{
				"device":string, device name, required if submodule is benoic
				"element_type":string, element type, required if submodule is benoic
			},
		}
	],
	"scripts":[ Array of scripts to run, at least one scrit is required
		{
			"name":script name, required
			"enabled":boolean, required
		}
	],
	"options":{ optional
		"tags":[
			string, maximum 64 characters
		] Maximum 128 tags
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

Scheduler not found

### Remove a scheduler

#### URL

`/scheduler/@scheduler_name`

#### Method

`DELETE`

#### URL Parameters

**Required**

`@scheduler_name`: Scheduler name

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

Scheduler not found

### Add a new tag to a scheduler

#### URL

`/scheduler/@scheduler_name/@tag`

#### Method

`PUT`

#### URL Parameters

**Required**

`@scheduler_name`: Scheduler name

`@tag`: Tag value, maximum 64 characters, maximum 128 tags per scheduler

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

OR

Code 404

Scheduler not found

### Remove a tag from a scheduler

#### URL

`/scheduler/@scheduler_name/@tag`

#### Method

`DELETE`

#### URL Parameters

**Required**

`@scheduler_name`: Scheduler name

`@tag`: Tag value

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

Scheduler or tag not found

## Triggers management

### Send an external message

This endpoint does not require a valid authentication token in the headers or in the cookies.

It's used to send an external message in Angharad. If a trigger matches this message, its scripts will be run.

#### URL

`/alert/@submodule_name/@source/@element/@message/`

#### Method

`GET`

#### URL Parameters

**Required**

`@submodule_name`: string, submodule name of the sender (benoic or carleon)

`@source`: string, source of the sender

`@element`: string, sender element

`@message`: string, message sent

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

### Get all triggers

#### URL

`/trigger/`

#### Method

`GET`

#### Success response

Code 200

Content
```javascript
[ Array of trigger objects

    {
        "name":string, max 64 characters
        "description":string, max 128 characters
        "enabled":boolean
        "submodule":string, submodule name available (ex, benoic or carleon)
        "source":string, source of the trigger (ex device for a benoic element), max 64 characters
        "element":string, element of the trigger (ex element name for a benoic element), max 64 characters
        "message":string, message value to match
        "message_match":integer, message match type, values are 0 none, 1 equal, 2 different, 3 contains, 4 not contains, 5 empty, 6 not empty
		"conditions":[ Array of conditions objects
			{
				"condition":string, values can be '==', '!=', '<', '<=', '>', '>=', 'contains' and 'not contains'
				"value":string|integer|boolean
				"submodule":string, submodule name available (ex, benoic or carleon), required
				"element": string, element name, required
				"parameters":{
					"device":string, device name, required if submodule is benoic
					"element_type":string, element type, required if submodule is benoic
				},
			}
		],
        "scripts":[ Array of scripts to run
            {
                "name":string, script name
                "enabled":boolean
            }
        ],
		"options":{ optional
			"tags":[
				string, maximum 64 characters
			] Maximum 128 tags
		},
    }

]
```

#### Error Response

Code 500

Internal Error

### Get a trigger by its name

#### URL

`/trigger/@trigger_name`

#### Method

`GET`

#### URL Parameters

**Required**

`@trigger_name`: trigger name

#### Success response

Code 200

Content
```javascript
{
	"name":string, max 64 characters
	"description":string, max 128 characters
	"enabled":boolean
	"submodule":string, submodule name available (ex, benoic or carleon)
	"source":string, source of the trigger (ex device for a benoic element), max 64 characters
	"element":string, element of the trigger (ex element name for a benoic element), max 64 characters
	"message":string, message value to match
	"message_match":integer, message match type, values are 0 none, 1 equal, 2 different, 3 contains, 4 not contains, 5 empty, 6 not empty
	"conditions":[ Array of conditions objects
		{
			"condition":string, values can be '==', '!=', '<', '<=', '>', '>=', 'contains' and 'not contains'
			"value":string|integer|boolean
			"submodule":string, submodule name available (ex, benoic or carleon), required
			"element": string, element name, required
			"parameters":{
				"device":string, device name, required if submodule is benoic
				"element_type":string, element type, required if submodule is benoic
			},
		}
	],
	"scripts":[ Array of scripts to run
		{
			"name":string, script name
			"enabled":boolean
		}
	],
	"options":{ optional
		"tags":[
			string, maximum 64 characters
		] Maximum 128 tags
	},
}
```

#### Error Response

Code 500

Internal Error

Code 404

trigger not found

### Enable or disable a trigger

#### URL

`/trigger/@trigger_name/enable/@enabled`

#### Method

`GET`

#### URL Parameters

**Required**

`@trigger_name`: trigger name

`@enabled`: 0 (disabled) or 1 (enabled)

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

OR

Code 404

trigger not found

### Add a new trigger

#### URL

`/trigger/`

#### Method

`POST`

#### Data Parameters

```javascript
{
	"name":string, max 64 characters, required
	"description":string, max 128 characters
	"enabled":boolean, required
	"submodule":string, submodule name available (ex, benoic or carleon), required
	"source":string, source of the trigger (ex device for a benoic element), max 64 characters, required
	"element":string, element of the trigger (ex element name for a benoic element), max 64 characters, required
	"message":string, message value to match, required (can be empty)
	"message_match":integer, message match type, values are 0 none, 1 equal, 2 different, 3 contains, 4 not contains, 5 empty, 6 not empty, required
	"conditions":[ Array of conditions objects, conditions are optional
		{
			"condition":string, values can be '==', '!=', '<', '<=', '>', '>=', 'contains' and 'not contains', required
			"value":string|integer|boolean, required
			"submodule":string, submodule name available (ex, benoic or carleon), required
			"element": string, element name, required
			"parameters":{
				"device":string, device name, required if submodule is benoic
				"element_type":string, element type, required if submodule is benoic
			},
		}
	],
	"scripts":[ Array of scripts to run, at least one script is required
		{
			"name":string, script name, required
			"enabled":boolean, required
		}
	],
	"options":{ optional
		"tags":[
			string, maximum 64 characters
		] Maximum 128 tags
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

### Modify an existing trigger

#### URL

`/trigger/@trigger_name`

#### Method

`PUT`

#### URL Parameters

**Required**

`@trigger_name`: trigger name

#### Data Parameters

```javascript
{
	"description":string, max 128 characters
	"enabled":boolean, required
	"submodule":string, submodule name available (ex, benoic or carleon), required
	"source":string, source of the trigger (ex device for a benoic element), max 64 characters, required
	"element":string, element of the trigger (ex element name for a benoic element), max 64 characters, required
	"message":string, message value to match, required (can be empty)
	"message_match":integer, message match type, values are 0 none, 1 equal, 2 different, 3 contains, 4 not contains, 5 empty, 6 not empty, required
	"conditions":[ Array of conditions objects, conditions are optional
		{
			"condition":string, values can be '==', '!=', '<', '<=', '>', '>=', 'contains' and 'not contains', required
			"value":string|integer|boolean, required
			"submodule":string, submodule name available (ex, benoic or carleon), required
			"element": string, element name, required
			"parameters":{
				"device":string, device name, required if submodule is benoic
				"element_type":string, element type, required if submodule is benoic
			},
		}
	],
	"scripts":[ Array of scripts to run, at least one script is required
		{
			"name":string, script name, required
			"enabled":boolean, required
		}
	],
	"options":{ optional
		"tags":[
			string, maximum 64 characters
		] Maximum 128 tags
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

trigger not found

### Remove an existing trigger

#### URL

`/trigger/@trigger_name`

#### Method

`DELETE`

#### URL Parameters

**Required**

`@trigger_name`: trigger name

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

Trigger not found

### Add a new tag to a trigger

#### URL

`/trigger/@trigger_name/@tag`

#### Method

`PUT`

#### URL Parameters

**Required**

`@trigger_name`: Trigger name

`@tag`: Tag value, maximum 64 characters, maximum 128 tags per scheduler

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

OR

Code 404

Trigger not found

### Remove a tag from a trigger

#### URL

`/trigger/@trigger_name/@tag`

#### Method

`DELETE`

#### URL Parameters

**Required**

`@trigger_name`: Trigger name

`@tag`: Tag value

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

Trigger or tag not found

### Profiles management

### Get all profiles data

#### URL

`/profile`

#### Method

`GET`

#### Success response

Code 200

Content
```javascript
[
	any json value as profile data
]
```

#### Error Response

Code 500

Internal Error

### Get a profile by its name

#### URL

`/profile/@profile_id`

#### Method

`GET`

#### URL Parameters

**Required**

`@profile_id`: id (name) of the profile

#### Success response

Code 200

Content
```javascript
any json value as profile data
```

#### Error Response

Code 500

Internal Error

OR

Code 404

profile not found

### Update all values of a profile

#### URL

`/profile/@profile_id`

#### Method

`method`

#### URL Parameters

**Required**

`@profile_id`: id (name) of the profile

#### Data Parameters

```javascript
any json value as profile data
```

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

profile not found

### Delete a profile by its name

#### URL

`/profile/@profile_id`

#### Method

`DELETE`

#### URL Parameters

**Required**

`@profile_id`: id (name) of the profile

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

profile not found

## Authentication management

### Check authentication token

#### URL

`/auth`

#### Method

`GET`

#### Success response

Code 200

Content
```javascript
{
	"token":string, the token value
	"validity":integer, date of end of validity for the current token in UNIX EPOCH format
}
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

### Check user credentials to receive a valid authentication token

This endpoint does not require a valid authentication token in the headers or the cookies

If the validity sent does not match one in the list below, it will be rounded to the previous value in the list.

#### URL

`/auth`

#### Method

`POST`

#### Data Parameters

```javascript
{
	"user": string, required
	"password": string, required
	"validity": integer, end of validity in seconds for the sent token, possible values are 0: none (end of session), 3600: 1 hour, 43200: 12 hours, 86400: 1 day, 604800: 1 week, above 1 week: 10 years
}
```

#### Success response

Code 200

Content
```javascript
{
	"token":string, the token value
	"validity":integer, date of end of validity for the current token in UNIX EPOCH format
}
```

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

### Revoke current token

#### URL

`/auth`

#### Method

`DELETE`

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 400

Error input parameters

OR

Code 404

## Users management

These endpoints manage database users only, and only if database users authentication is enabled, otherwise all those endpoints will return an error 404.

### Get all users

#### URL

`/user/`

#### Method

`GET`

#### Success response

Code 200

Content
```javascript
[ array of users object
	{
		"login": string
		"enabled", boolean
	}
]
```

#### Error Response

Code 500

Internal Error

### Get a database user by it login

#### URL

`/user/@user_name`

#### Method

`GET`

#### URL Parameters

**Required**

`@user_name`: string, login of the user to look for

#### Success response

Code 200

Content
```javascript
{
	"login": string
	"enabled", boolean
}
```

#### Error Response

Code 500

Internal Error

OR

Code 404

User not found

### Add a new database user

#### URL
`/user/`

#### Method

`POST`

#### Data Parameters

```javascript
{
	"login": string, must be a new login, max 128 characters, required
	"password": string, password to use, max 128 characters, required
	"enabled", boolean, required
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

### Modify an existing database user

#### URL

`/user/@user_name`

#### Method

`PUT`

#### URL Parameters

**Required**

`@user_name`: string, login of the user

#### Data Parameters

```javascript
{
	"password": string, password to use, max 128 characters, optional
	"enabled", boolean, required
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

database user not found

### Delete an existing database user

#### URL

`/user/@user_name`

#### Method

`DELETE`

#### URL Parameters

**Required**

`@user_name`: string, login of the user

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

database user not found

## Tokens management

### Get a list of tokens

#### URL

`/token/`

#### Method

`GET`

#### URL Parameters

**Optional**

`login`: string, tokens assigned to the specified login

`enabled`: string ("true"|"false"), look for enabled or revoked tokens

#### Success response

Code 200

Content
```javascript
[
	{

		"token":string, token value
		"login":string, login value assigned to the token
		"enabled":boolean, token valid or revoked
		"last_seen":string, date of last seen token in ISO 8601 format
		"validity":string, date of end of validity of the token in ISO 8601 format

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

### Revoke a token

#### URL

`/token/`

#### Method

`POST`

#### Data Parameters

```javascript
{

    "token":string, token value

}
```

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

Token not found

## Static files

Static files are used to host a static website within Angharad

### Get a static file

#### URL

`/static_file_prefix/@static_file_path`

#### Method

`GET`

#### URL Parameters

**Required**

`@static_file_path`: string, static file path

#### Success response

Code 200

Content

raw file content and its content-type in the headers.

Currently, the supported content-types are:

```
".html": "text/html"
".css": "text/css"
".js": "application/javascript"
".png": "image/png"
".jpeg": "image/jpeg"
".jpg": "image/jpeg"
".ttf": "font/ttf"
".woff": "font/woff"
".woff2": "font/woff2"
".json": "application/json"
".map": "application/octet-stream"
"*": "application/octet-stream"
```

#### Error Response

Code 500

Internal Error

OR

Code 404

file not found

## Options

This endpoint is used to send OPTIONS parameters for all endpoints

### Send OPTIONS parameters

#### URL
`*`

#### Method

`OPTIONS`

#### Success response

Code 200

Content

Headers contain all options
