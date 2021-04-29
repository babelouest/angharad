# Motion service API

Motion service is used to display motion pictures taken or real-time streams.

This service use carleon prefix as prefix. All endpoints described here assume to have carleon prefix before the url.

## Motion-service elements management

### Get all motion-service elements

#### URL

`/service-motion/`

#### Method

`GET`

#### Success response

Code 200

Content
```javascript
[ Array of service-motion elements
    {
        "name": string, name of the element, max 64 chars
        "description": string, description of the element, max 128 chars
        "config_uri": string, uri of the motion instance configuration to check if it's still responding, max 512 chars
        "stream_list": [ Array of streams
            {
                "uri": string, uri to access the stream directly from the client, max 512 chars
                "name": string, name of the stream, max 64 chars
                "snapshot_uri": string, uri to run a snapshot command on the stream, max 512 chars
            }
        ],
        "file_list": [ Array of files lists of the pricures taken by this motion instance
			{
				"name": string, name of the file list, max 64 chars
				"path": string, full path of the pictures, max 512 chars
				"thumbnail_path": string, full path of the pictures thumbnails, max 512 chars, optional
			}
		]
    }
]
```

#### Error Response

Code 500

Internal Error

### Get a service-motion element by its name

#### URL

`/service-motion/@name`

#### Method

`GET`

#### URL Parameters

**Required**

`@name`: name of the service-motion element

#### Success response

Code 200

Content
```javascript
{
	"name": string, name of the element, max 64 chars
	"description": string, description of the element, max 128 chars
	"config_uri": string, uri of the motion instance configuration to check if it's still responding, max 512 chars
	"stream_list": [ Array of streams
		{
			"uri": string, uri to access the stream directly from the client, max 512 chars
			"name": string, name of the stream, max 64 chars
			"snapshot_uri": string, uri to run a snapshot command on the stream, max 512 chars
		}
	],
	"file_list": [ Array of files lists of the pricures taken by this motion instance
		{
			"name": string, name of the file list, max 64 chars
			"path": string, full path of the pictures, max 512 chars
			"thumbnail_path": string, full path of the pictures thumbnails, max 512 chars, optional
		}
	]
}
```

#### Error Response

Code 500

Internal Error

OR

Code 404

service-motion element not found

### Add a new service-motion element

#### URL

`/service-motion/`

#### Method

`POST`

#### Data Parameters

```javascript
{
	"name": string, name of the element, max 64 chars, required
	"description": string, description of the element, max 128 chars, optional
	"config_uri": string, uri of the motion instance configuration to check if it's still responding, max 512 chars, required
	"stream_list": [ Array of streams, optional
		{
			"uri": string, uri to access the stream directly from the client, max 512 chars, required
			"name": string, name of the stream, max 64 chars, required
			"snapshot_uri": string, uri to run a snapshot command on the stream, max 512 chars, required
		}
	],
	"file_list": [ Array of files lists of the pricures taken by this motion instance, optional
		{
			"name": string, name of the file list, max 64 chars, required
			"path": string, full path of the pictures, max 512 chars, required
			"thumbnail_path": string, full path of the pictures thumbnails, max 512 chars, optional
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

### Modify an existing service-motion element

#### URL

`/service-motion/@name`

#### Method

`PUT`

#### URL Parameters

**Required**

`@name`: name of the element

#### Data Parameters

```javascript
{
	"description": string, description of the element, max 128 chars, optional
	"config_uri": string, uri of the motion instance configuration to check if it's still responding, max 512 chars, required
	"stream_list": [ Array of streams, optional
		{
			"uri": string, uri to access the stream directly from the client, max 512 chars, required
			"name": string, name of the stream, max 64 chars, required
			"snapshot_uri": string, uri to run a snapshot command on the stream, max 512 chars, required
		}
	],
	"file_list": [ Array of files lists of the pricures taken by this motion instance, optional
		{
			"name": string, name of the file list, max 64 chars, required
			"path": string, full path of the pictures, max 512 chars, required
			"thumbnail_path": string, full path of the pictures thumbnails, max 512 chars, optional
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

OR

Code 404

Service-motion element not found

### Delete an existing service-motion element

#### URL

`/service-motion/@name`

#### Method

`DELETE`

#### URL Parameters

**Required**

`@name`: name of the element

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

Service-motion element not found

## Service-motion element commads

### Get current status of a service-motion element

#### URL

`/service-motion/@name/status`

#### Method

`GET`

#### URL Parameters

**Required**

`@name`: name of the element

**Optional**

`offset`: offset to start file-list

`limit`: limit of a file-list result number of files

#### Success response

Code 200

Content
```javascript
{

    "online":boolean, is the motion instance responding?
    "file_list":{ Object containing lists of files
        "file_list_name_1":[ Array of string, each element is a picture file available
        ],
        "file_list_name_2":[ Array of string, each element is a picture file available
        ],
    },
    "stream_list":[ Array of stream objects
        {
            "name":string, name of the stream object
            "snapshot_uri":string, uri to run a snapshot command on the stream, max 512 chars, required
            "uri":uri to access the stream directly from the client, max 512 chars, required
        }
    ]

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

Element not found

### Get a file from a file list

#### URL

`/service-motion/@name/image/@file_list/@file_name`

#### Method

`GET`

#### URL Parameters

**Required**

`@name`: name of the element

`@file_list`: name of the file list

`@file_name`: name of the file

#### Success response

Code 200

Content

Raw content of the file

#### Error Response

Code 500

Internal Error

OR

Code 404

Element or file list or file not found

### Send a snapshot command to a motion instance

#### URL

`/service-motion/@name/stream/@stream_name/snapshot`

#### Method

`PUT`

#### URL Parameters

**Required**

`@name`: name of the element

`@stream_name`: name of the stream

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

Element or steam not found
