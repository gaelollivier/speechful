module Message = {
  type t;
  [@bs.get] external data : t => string = "";
};

module Socket = {
  type t;
  [@bs.send] external send : (t, 'a) => unit = "";
  [@bs.set] external onMessage : (t, Message.t => unit) => unit = "onmessage";
};

[@bs.module "./setupServer.js"] [@bs.val]
external create_server : (int, Socket.t => unit) => unit =
  "";