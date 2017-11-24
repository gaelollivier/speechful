type t = {ws: WebSocket.t};

type event =
  | MessageReceived(string);

[@bs.scope "window.location"] [@bs.val] external locationHost : string = "host";

[@bs.scope "window.location"] [@bs.val] external locationProtocol : string = "protocol";

let buildRelativeURL = (path) => {
  let protocol = locationProtocol == "http:" ? "ws://" : "wss://";
  protocol ++ locationHost ++ path
};

let serverUrl = buildRelativeURL("/ws");

let connect = (onConnect: t => unit, onEvent: event => unit) => {
  let ws = WebSocket.create_ws(serverUrl);
  WebSocket.onOpen(ws, () => onConnect({ws: ws}));
  WebSocket.onMsg(ws, (msg) => onEvent(MessageReceived(msg)))
};

let sendMsg = (server: t, msg: string) => WebSocket.send(server.ws, msg);