type socketState = int;

[@bs.module "ws"] [@bs.val] external stateOpen : socketState = "OPEN";

module SocketMessage = {
  type t;
  [@bs.get] external data : t => string = "";
};

module Socket = {
  type t;
  type id = string;
  let string_of_id: id => string = (id) => id;
  [@bs.set] external onSocketMessage : (t, SocketMessage.t => unit) => unit = "onmessage";
  [@bs.set] external onClose : (t, unit => unit) => unit = "onclose";
  [@bs.get] external readyState : t => socketState = "readyState";
  /* id retrieves the unique id of the socket, so we can store it
     easily in a set */
  [@bs.get] external getId : t => string = "id";
  [@bs.send] external sendUnsafe : (t, string) => unit = "send";
  /* "dirty" safe send: check socket status
   * so we don't write to a closed socket */
  let send = (socket: t, data: string) : unit =>
    if (readyState(socket) == stateOpen) {
      sendUnsafe(socket, data)
    } else {
      Js.log("[ERROR] writting to closed socket")
    };
  let onMessage = (s: t, cb: string => unit) =>
    onSocketMessage(
      s,
      (strMsg) =>
        try (cb(SocketMessage.data(strMsg))) {
        | e => Js.log2("error processing message", e)
        }
    );
};

[@bs.module "./setupServer.js"] [@bs.val]
external create_server : (int, Socket.t => unit) => unit =
  "";