/* outgoing messages */
type t =
  | JoinRoom(string)
  | SetUsername(string)
  | SendMessage(string);

let encodeJoinRoom = (room: string) =>
  Json.Encode.(object_([("type", string("join_room")), ("room", string(room))]))
  |> Js.Json.stringify;

let encodeSetUsername = (username: string) =>
  Json.Encode.(object_([("type", string("set_username")), ("username", string(username))]))
  |> Js.Json.stringify;

let encodeSendMessage = (message: string) =>
  Json.Encode.(object_([("type", string("send_message")), ("message", string(message))]))
  |> Js.Json.stringify;

/* encode function for outgoing message */
let encodeJSON = (msg: t) =>
  switch msg {
  | JoinRoom(room) => encodeJoinRoom(room)
  | SetUsername(username) => encodeSetUsername(username)
  | SendMessage(message) => encodeSendMessage(message)
  };