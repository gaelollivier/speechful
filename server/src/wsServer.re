type socketState = int;

[@bs.module "ws"] [@bs.val] external stateOpen : socketState = "OPEN";

module Message = {
  type t;
  [@bs.get] external data : t => string = "";
};

module Event = {
  type t =
    | SetUsername(string);
  /* Decode function for individual message types */
  let decodeSetUsername = (jsonMsg) =>
    SetUsername(Json.Decode.(field("username", string, jsonMsg)));
  exception InvalidType(string);
  /* Decode function for incoming message */
  let decode = (m: Message.t) =>
    m
    |> Message.data
    |> Js.Json.parseExn
    |> (
      (jsonMsg) => {
        let eventType = Json.Decode.(field("type", string, jsonMsg));
        switch eventType {
        | "set_username" => decodeSetUsername(jsonMsg)
        | t => raise(InvalidType(t))
        }
      }
    );
};

module Socket = {
  type t;
  type id = string;
  [@bs.set] external onMessage : (t, Message.t => unit) => unit = "onmessage";
  [@bs.set] external onClose : (t, unit => unit) => unit = "onclose";
  [@bs.get] external readyState : t => socketState = "readyState";
  /* id retrieves the unique id of the socket, so we can store it
     easily in a set */
  [@bs.get] external getId : t => id = "id";
  [@bs.send] external sendUnsafe : (t, 'a) => unit = "send";
  /* "dirty" safe send: check socket status
   * so we don't write to a closed socket */
  let send = (socket: t, message: 'a) : unit =>
    if (readyState(socket) == stateOpen) {
      sendUnsafe(socket, message)
    } else {
      Js.log("[ERROR] writting to closed socket")
    };
  let onEvent = (s: t, cb: Event.t => unit) =>
    onMessage(
      s,
      (strMsg) =>
        try (cb(Event.decode(strMsg))) {
        | e => Js.log2("Invalid message received", e)
        }
    );
};

[@bs.module "./setupServer.js"] [@bs.val]
external create_server : (int, Socket.t => unit) => unit =
  "";