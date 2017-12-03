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
  let clientId = Socket.getId(ws);
  /* called when an event is received from a client */
  let onMessage = (message: string) => {
    let event = Event.decode(message);
    Js.log2("new event", event);
    switch event {
    | Event.SetUsername(newName) => Store.update(Store.SetUsername(clientId, newName))
    | Event.JoinRoom(room) => Store.update(Store.JoinRoom(clientId, room))
    | Event.SendMessage(message) => Store.update(Store.SendMessage(clientId, message))
    }
  };
  let onClose = () => Store.update(Store.ClientDisconnected(clientId));
  Socket.onMessage(ws, onMessage);
  Socket.onClose(ws, onClose)
};

/* Start server */
create_server(port, onConnect);