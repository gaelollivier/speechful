/* incoming events */
type t =
  | SetUsername(string)
  | JoinRoom(string)
  | SendMessage(string);

/* decode function for individual message types */
let decodeSetUsername = (jsonMsg) => SetUsername(Json.Decode.(field("username", string, jsonMsg)));

let decodeJoinRoom = (jsonMsg) => JoinRoom(Json.Decode.(field("room", string, jsonMsg)));

let decodeSendMessage = (jsonMsg) => SendMessage(Json.Decode.(field("message", string, jsonMsg)));

exception InvalidMessageType(string);

/* decode function for incoming message */
let decode = (m: string) =>
  m
  |> Js.Json.parseExn
  |> (
    (jsonMsg) => {
      let eventType = Json.Decode.(field("type", string, jsonMsg));
      switch eventType {
      | "set_username" => decodeSetUsername(jsonMsg)
      | "join_room" => decodeJoinRoom(jsonMsg)
      | "send_message" => decodeSendMessage(jsonMsg)
      | t => raise(InvalidMessageType(t))
      }
    }
  );