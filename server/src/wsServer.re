type socketState = int;

[@bs.module "ws"] [@bs.val] external stateOpen : socketState = "OPEN";

module Message = {
  type t;
  [@bs.get] external data : t => string = "";
};

module Socket = {
  type t;
  [@bs.set] external onMessage : (t, Message.t => unit) => unit = "onmessage";
  [@bs.set] external onClose : (t, unit => unit) => unit = "onclose";
  [@bs.get] external readyState : t => socketState = "readyState";
  [@bs.send] external sendUnsafe : (t, 'a) => unit = "send";
  /* "dirty" safe send: check socket status
   * so we don't write to a closed socket */
  let send = (socket: t, message: 'a) : unit =>
    if (readyState(socket) == stateOpen) {
      sendUnsafe(socket, message)
    } else {
      Js.log("[ERROR] writting to closed socket")
    };
};

[@bs.module "./setupServer.js"] [@bs.val]
external create_server : (int, Socket.t => unit) => unit =
  "";