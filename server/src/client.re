type t = {
  ws: WsServer.Socket.t,
  id: WsServer.Socket.id,
  username: option(string),
  room: option(string)
};

let make = (ws: WsServer.Socket.t) => {
  ws,
  id: WsServer.Socket.getId(ws),
  username: None,
  room: None
};