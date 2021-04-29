# MPD service API

MPD service is used to get status and send commands to a MPD instance

This service use carleon prefix as prefix. All endpoints described here assume to have carleon prefix before the url.

## Service MPD elements management

### Get all service-mpd elements

#### URL

`/service-mpd/`

#### Method

`GET`

#### Success response

Code 200

Content
```javascript
[ Array of service-mpd elements
    {
        "name": string, name of the element, max 64 chars
        "description": string, description of the element, max 128 chars
        "host": string, host where the mpd instance is hosted, max 128 chars
        "port": integer, port where mpd instance is hosted, if 0 then default
				"device": string, device which switch is attached to this mpd, max 128 chars
				"switch": string, switch attached to this mpd, max 128 chars
    }
]
```

#### Error Response

Code 500

Internal Error

### Get a service-mpd element by its name

#### URL

`/service-mpd/@name`

#### Method

`GET`

#### URL Parameters

**Required**

`@name`: string, name of the element

#### Success response

Code 200

Content
```javascript
{
	"name": string, name of the element, max 64 chars
	"description": string, description of the element, max 128 chars
	"host": string, host where the mpd instance is hosted, max 128 chars
	"port": integer, port where mpd instance is hosted, if 0 then default
	"device": string, device which switch is attached to this mpd, max 128 chars
	"switch": string, switch attached to this mpd, max 128 chars
}
```

#### Error Response

Code 500

Internal Error

OR

Code 404

service-mpd not found

### Add a new service-mpd element

#### URL

`/service-mpd/`

#### Method

`POST`

#### Data Parameters

```javascript
{
	"name": string, name of the element, max 64 chars, required
	"description": string, description of the element, max 128 chars, optional
	"host": string, host where the mpd instance is hosted, max 128 chars, required
	"port": integer, port where mpd instance is hosted, if 0 then default mpd port, optional
	"password": string, password to connect to the mpd instance, max 128 chars, optional
	"device": string, device which switch is attached to this mpd, max 128 chars, optional
	"switch": string, switch attached to this mpd, max 128 chars, optional
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

### Modify an existing service-mpd element

#### URL

`/service-mpd/@name`

#### Method

`PUT`

#### URL Parameters

**Required**

`@name`: string, name of the element

#### Data Parameters

```javascript
{
	"name": string, name of the element, max 64 chars, required
	"description": string, description of the element, max 128 chars, optional
	"host": string, host where the mpd instance is hosted, max 128 chars, required
	"port": integer, port where mpd instance is hosted, if 0 then default mpd port, optional
	"password": string, password to connect to the mpd instance, max 128 chars, optional
	"device": string, device which switch is attached to this mpd, max 128 chars, optional
	"switch": string, switch attached to this mpd, max 128 chars, optional
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

Element not found

### Delete an existing service-mpd element

#### URL

`/service-mpd/@name`

#### Method

`DELETE`

#### URL Parameters

**Required**

`@name`: string, name of the element

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

Element not found

### Get status and current song details of a service-mpd element

#### URL

`/service-mpd/@name/status`

#### Method

`GET`

#### URL Parameters

**Required**

`@name`, name of the element

#### Success response

Code 200

Content
```javascript
{

    "state":string, state of the player, values can be "stop", "play", "pause", "unknown"
    "volume":integer, volume of the player, between 0 and 100
    "elapsed_time":integer, elapsed time of the current song in seconds
    "total_time":integer, total time of the current song in seconds
		"song_pos":integer, position of the current song in the queue
		"random":boolean, random state of the mpd
		"repeat":boolean, repeat status of the mpd
    "title":string, title of current song, optional
    "artist":string, artist of current song, optional
    "album":string, album of current song, optional
    "date":string, year of current song, optional
    "track":string, track of current song, optional

}
```

#### Error Response

Code 500

Internal Error

OR

Code 404

Element not found

### Send a playback command

#### URL

`/service-mpd/@name/action/@action_name`

#### Method

`GET`

#### URL Parameters

**Required**

`@name`: string, name of the element

`@action_name`, string, name of the action to launch, values can be "stop", "play", "pause", "previous", "next", "random", "repeat"

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

Element not found

### Get playlists available

#### URL

`/service-mpd/@name/playlists`

#### Method

`GET`

#### URL Parameters

**Required**

`@name`: string, name of the element

#### Success response

Code 200

Content
```javascript
[ Array of strings

    "playlist_1",
    "playlist_2",
    "playlist_3",
    ...

]
```

#### Error Response

Code 500

Internal Error

OR

Code 404

Element not found

### Load and play a playlist

#### URL

`/service-mpd/@name/playlists/@playlist_name/load`

#### Method

`GET`

#### URL Parameters

**Required**

`@name`: string, name of the element

`@playlist_name`: string, name of the playlist

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

Element or playlist not found

### Change the volume

#### URL

`/service-mpd/@name/volume/@value`

#### Method

`GET`

#### URL Parameters

**Required**

`@name`: string, name of the element

`@value`: integer, volume to set, value must be between 0 (off) and 100 (max)

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

Element not found
