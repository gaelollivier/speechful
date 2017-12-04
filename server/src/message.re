open WsServer;

/* outgoing messages */
type t =
  | Moien(Socket.id)
  | RoomJoined(string, list(Client.t))
  | UserLeft(Socket.id, string)
  | UserJoined(Client.t, string)
  | ChangedUsername(Client.t)
  | MessageSent(Socket.id, string);

let encodeMoien = (userId: Socket.id) =>
  Json.Encode.(object_([("type", string("moien")), ("user_id", string(userId))]))
  |> Js.Json.stringify;

let encodeClient = (client: Client.t) =>
  Json.Encode.(
    object_([
      ("id", string(client.id)),
      (
        "username",
        switch client.username {
        | Some(name) => string(name)
        | None => null
        }
      ),
      (
        "room",
        switch client.room {
        | Some(room) => string(room)
        | None => null
        }
      )
    ])
  );

let encodeRoomJoined = (room: string, clients: list(Client.t)) =>
  Json.Encode.(
    object_([
      ("type", string("room_joined")),
      ("room", string(room)),
      ("users", list(encodeClient, clients))
    ])
  )
  |> Js.Json.stringify;

let encodeRoomUserLeft = (clientId: Socket.id, room: string) =>
  Json.Encode.(
    object_([("type", string("user_left")), ("room", string(room)), ("user_id", string(clientId))])
  )
  |> Js.Json.stringify;

let encodeRoomUserJoined = (client: Client.t, room: string) =>
  Json.Encode.(
    object_([
      ("type", string("user_joined")),
      ("room", string(room)),
      ("user", encodeClient(client))
    ])
  )
  |> Js.Json.stringify;

/* called when a user in the room changed his username */
let encodeChangedUsername = (client: Client.t) =>
  Json.Encode.(object_([("type", string("username_changed")), ("user", encodeClient(client))]))
  |> Js.Json.stringify;

let encodeMessageSent = (clientId: Socket.id, message: string) =>
  Json.Encode.(
    object_([
      ("type", string("message_sent")),
      ("user_id", string(clientId)),
      ("message", string(message))
    ])
  )
  |> Js.Json.stringify;

/* encode function for outgoing message */
let encodeJSON = (msg: t) =>
  switch msg {
  | Moien(userId) => encodeMoien(userId)
  | RoomJoined(room, clients) => encodeRoomJoined(room, clients)
  | UserLeft(clientId, room) => encodeRoomUserLeft(clientId, room)
  | UserJoined(client, room) => encodeRoomUserJoined(client, room)
  | ChangedUsername(client) => encodeChangedUsername(client)
  | MessageSent(clientId, message) => encodeMessageSent(clientId, message)
  };