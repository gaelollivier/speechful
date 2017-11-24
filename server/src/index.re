open WsServer;

let port =
  switch (Js.Dict.get(Node.Process.process##env, "PORT")) {
  | Some(p) => int_of_string(p)
  | None => 8080
  };

let onConnect = (ws: Socket.t) => {
  let onMessage = (message: Message.t) => {
    let data = Message.data(message);
    Js.log("new message ! " ++ data);
    ()
  };
  Socket.onMessage(ws, onMessage);
  Socket.send(ws, "hello world")
};

create_server(port, onConnect);