# Server

## Protocol

Speechful server is a websocket server with a basic JSON-based protocol. Base shape of a message is:

```js
{
  "type": "[MESSAGE TYPE]",
  ...
}
```

### Messages types

#### Set username

Sent by client to set its name

```js
{
  "type": "set_username",
  "username": "[USER NAME]"
}
```

#### Join room

Sent by client to join a room.

* One client can only be in one room at a time
* If the room doesn't exist yet, it's created

```js
{
  "type": "join_room",
  "room": "[ROOM NAME]"
}
```

#### Room joined

Sent by server when client successfuly joined a room. Contains the list of users already in the
room.

```js
{
  "type": "room_joined",
  "users": [
    {
      "id": "[USER ID]",
      "username": "[USER NAME]" | null
    },
    ...
  ]
}
```

#### User joined

Sent by server when a new client joins the room the user is currently in.

```js
{
  "type": "new_user_joined",
  "room": "[ROOM NAME]",
  "user": {
    "id": "[USER ID]",
    "username": "[USER NAME]" | null
  }
}
```

#### User left

Sent by server when a new client leaves the room the user is currently in.

```js
{
  "type": "user_left",
  "room": "[ROOM NAME]",
  "user_id": "[USER ID]"
}
```

#### User changed username

Sent by server when a user in the same room changed his username.

```js
{
  "type": "username_changed",
  "user": {
    "id": "[USER ID]",
    "username": "[USER NAME]" | null
  }
}
```

#### Send message to room

Sent by client to set send a message to all users in current room.

```js
{
  "type": "send_message",
  "message": "[USER MESSAGE]"
}
```

#### Sent message

Sent by server when a user in the same room send a message.

```js
{
  "type": "message_sent",
  "user_id": "[USER ID]",
  "message": "[USER MESSAGE]"
}
```
