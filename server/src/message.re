open WsServer;

/* outgoing messages */
type t =
  | UsernameSet(option(string))
  | RoomJoined(string, list(Client.t));

/* encode functions for common types */
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

/* encode function for individual message types */
let encodeUsernameSet = (res) =>
  Json.Encode.(
    object_([
      ("type", string("username_set")),
      (
        "error",
        switch res {
        | Some(err) => string(err)
        | None => null
        }
      )
    ])
  )
  |> Js.Json.stringify;

let encodeRoomJoined = (room: Socket.id, clients: list(Client.t)) =>
  Json.Encode.(
    object_([
      ("type", string("room_joined")),
      ("room", string(room)),
      ("users", list(encodeClient, clients))
    ])
  )
  |> Js.Json.stringify;

/* encode function for outgoing message */
let encodeJSON = (msg: t) =>
  switch msg {
  | UsernameSet(err) => encodeUsernameSet(err)
  | RoomJoined(room, clients) => encodeRoomJoined(room, clients)
  };