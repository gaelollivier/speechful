/* incoming server events */
type t =
  | Moien(User.id)
  | RoomJoined(string, list(User.t))
  | UserJoined(User.t, string)
  | UserLeft(User.id, string)
  | ChangedUsername(User.t)
  | MessageSent(User.id, string);

/* shared decode functions */
let decodeUser = (json) =>
  Json.Decode.(
    User.{id: field("id", string, json), username: optional(field("username", string), json)}
  );

/* decode functions for individual message types */
let decodeMoien = (json) => Json.Decode.(Moien(field("user_id", string, json)));

let decodeRoomJoined = (json) =>
  Json.Decode.(RoomJoined(field("room", string, json), field("users", list(decodeUser), json)));

let decodeUserJoined = (json) =>
  Json.Decode.(UserJoined(field("user", decodeUser, json), field("room", string, json)));

let decodeUserLeft = (json) =>
  Json.Decode.(UserLeft(field("user_id", string, json), field("room", string, json)));

let decodeUsernameChanged = (json) =>
  Json.Decode.(ChangedUsername(field("user", decodeUser, json)));

let decodeMessageSent = (json) =>
  Json.Decode.(MessageSent(field("user_id", string, json), field("message", string, json)));

exception InvalidMessageType(string);

/* decode function for incoming message */
let decode = (m: string) =>
  m
  |> Js.Json.parseExn
  |> (
    (json) => {
      let eventType = Json.Decode.(field("type", string, json));
      switch eventType {
      | "moien" => decodeMoien(json)
      | "room_joined" => decodeRoomJoined(json)
      | "user_joined" => decodeUserJoined(json)
      | "user_left" => decodeUserLeft(json)
      | "username_changed" => decodeUsernameChanged(json)
      | "message_sent" => decodeMessageSent(json)
      | t => raise(InvalidMessageType(t))
      }
    }
  );