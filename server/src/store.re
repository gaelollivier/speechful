open WsServer;

/*
 * Would be cool to have something like redux-loop for state management
 * This is an attempt to have a redux-like state management, so we avoid mutations.
 * For now, reduce can produce side-effects... would be better to return them instead
 * (but I don't plan to write unit tests for now anyway so... ¯\_(ツ)_/¯)
 */
type client = {
  ws: Socket.t,
  id: Socket.id,
  username: option(string)
};

let make_client = (ws: Socket.t) => {ws, id: Socket.getId(ws), username: None};

type t = {clients: list(client)};

let currentState: ref(t) = ref({clients: []});

type action =
  | NewClient(Socket.t)
  | ClientDisconnected(Socket.id)
  | SetUsername(Socket.id, string);

let reduce = (state: t, action: action) : t =>
  switch action {
  | NewClient(c) => {clients: [make_client(c), ...state.clients]}
  | ClientDisconnected(clientId) => {clients: List.filter((c) => c.id !== clientId, state.clients)}
  /* | NewMessage(client, message) =>
     /* Broadcast to all connected clients */
     List.iter(
       (c) =>
         if (c !== client) {
           Socket.send(c, message)
         },
       state.clients
     );
     state */
  | SetUsername(clientId, username) => {
      clients:
        List.map((c) => c.id != clientId ? c : {...c, username: Some(username)}, state.clients)
    }
  };

let debug = () => {
  Js.log("clients:");
  List.iter((c) => Js.log4("id:", c.id, "username:", c.username), currentState^.clients)
};

let update = (a: action) => {
  Js.log2("new action:", a);
  currentState := reduce(currentState^, a);
  Js.log("new state:");
  debug()
};