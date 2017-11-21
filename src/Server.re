type t = {ws: WebSocket.t};

type event =
  | MessageReceived(string);

let serverUrl = "ws://localhost:8080";

let connect = (onConnect: t => unit, onEvent: event => unit) => {
  let ws = WebSocket.create_ws(serverUrl);
  WebSocket.onOpen(ws, () => onConnect({ws: ws}));
  WebSocket.onMsg(ws, (msg) => onEvent(MessageReceived(msg)))
};

let sendMsg = (server: t, msg: string) => WebSocket.send(server.ws, msg);