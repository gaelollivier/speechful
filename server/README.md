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

#### Set username response

Sent by server to notify status of "set_username"

```js
{
  "type": "username_set",
  "error": "[ERROR MESSAGE]" | null
}
```
