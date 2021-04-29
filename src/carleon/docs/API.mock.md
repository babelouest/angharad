# Mock service API

This service does nothing for real, but behaves as a real carleon service. It's designed to help create a client program and create other services.

This service use carleon prefix as prefix. All endpoints described here assume to have carleon prefix before the url.

### Get all mock service elements

#### URL

`/mock-service/`

#### Method

`GET`

#### Success response

Code 200

Content
```javascript
[ array of moc-service elements

    {
        "name": string, name of the element
        "description": string, description of the element
    }

]
```

#### Error Response

Code 500

Internal Error

### Get a moc-service element by its name

#### URL

`/mock-service/@element_id`

#### Method

`GET`

#### URL Parameters

**Required**

`@element_id`: element name

#### Success response

Code 200

Content
```javascript
{
	"name": string, name of the element
	"description": string, description of the element
}

```

#### Error Response

Code 500

Internal Error

OR

Code 404

Element not found

### Add a new mock-service element

#### URL

`/mock-service/`

#### Method

`POST`

#### Data Parameters

```javascript
{
	"name": string, name of the element, max 64 chars
	"description": string, description of the element, max 128 chars
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

### Modify an existing mock-service element

#### URL

`/mock-service/@element_id`

#### Method

`PUT`

#### URL Parameters

**Required**

`@element_id`: element name

#### Data Parameters

```javascript
{
	"description": string, description of the element, max 128 chars
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

### Delete an existing mock-service element

#### URL

`/mock-service/@element_id`

#### Method

`DELETE`

#### URL Parameters

**Required**

`@element_id`: element name

#### Success response

Code 200

#### Error Response

Code 500

Internal Error

OR

Code 404

Element not found

### Execute a mock-service command exec1 on an element

#### URL

`/mock-service/@element_id/command/exec1/@param1/@param2/@param3`

#### Method

`GET`

#### URL Parameters

**Required**

`@element_id`: element name

`@param1`: string

`@param2`: integer

`@param3`: real

#### Success response

Code 200

Content
```javascript
{
	"value1": string
	"value2": integer
}
```

#### Error Response

Code 500

Internal Error

OR

Code 404

Element not found

### Execute a mock-service command exec2 on an element

#### URL

`/mock-service/@element_id/command/exec2/@param1`

#### Method

`GET`

#### URL Parameters

**Required**

`@element_id`: element name

`@param1`: string

#### Success response

Code 200

Content
```javascript
{
	"value": string
}
```

#### Error Response

Code 500

Internal Error

OR

Code 404

Element not found

### Execute a mock-service command exec3 on an element

#### URL

`/mock-service/@element_id/command/exec3`

#### Method

`GET`

#### Success response

Code 200

Content
```javascript
{
	"value": string
}
```

#### Error Response

Code 500

Internal Error

OR

Code 404

Element not found

### Execute a mock-service command exec4 on an element

#### URL

`/mock-service/@element_id/command/exec4`

#### Method

`GET`

#### Success response

Code 200

Content
```javascript
{
	"value1": integer
	"value2": boolean
}
```

#### Error Response

Code 500

Internal Error

OR

Code 404

Element not found
