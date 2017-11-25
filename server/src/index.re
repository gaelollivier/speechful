open WsServer;

/* Retrieve port from env or default to 8080 */
let port =
  switch (Js.Dict.get(Node.Process.process##env, "PORT")) {
  | Some(p) => int_of_string(p)
  | None => 8080
  };

/* called whenever a client connects */
let onConnect = (ws: Socket.t) => {
  Store.update(Store.NewClient(ws));
  /* called when a message is received from a client */
  let onMessage = (message: Message.t) => {
    let txt = Message.data(message);
    Js.log2("new message", txt);
    Store.update(Store.NewMessage(ws, txt));
    ()
  };
  let onClose = () => Store.update(Store.ClientDisconnected(ws));
  Socket.onMessage(ws, onMessage);
  Socket.onClose(ws, onClose);
  /* Welcome clients with some hello world */
  Socket.send(ws, "hello world")
};

/* Start server */
create_server(port, onConnect);