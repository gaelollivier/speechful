open WsServer;

/*
 * This file implements an "home-made" state management system,
 * closely inspired from ReasonReact ReducerComponent
 * (but without the "generic" part)
 */
type client = {
  ws: Socket.t,
  id: Socket.id,
  username: option(string)
};

let make_client = (ws: Socket.t) => {ws, id: Socket.getId(ws), username: None};

type state = {clients: list(client)};

let get_client = (state: state, id: Socket.id) => List.find((c) => c.id == id, state.clients);

let currentState: ref(state) = ref({clients: []});

type action =
  | NewClient(Socket.t)
  | ClientDisconnected(Socket.id)
  | SetUsername(Socket.id, string);

type update =
  | NoUpdate
  | SideEffect(state => unit)
  | Update(state)
  | UpdateWithSideEffect(state, state => unit);

let reduce = (state: state, action: action) : update =>
  switch action {
  | NewClient(c) => Update({clients: [make_client(c), ...state.clients]})
  | ClientDisconnected(clientId) =>
    Update({clients: List.filter((c) => c.id !== clientId, state.clients)})
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
  | SetUsername(clientId, username) =>
    let client = get_client(state, clientId);
    UpdateWithSideEffect(
      {
        clients:
          List.map((c) => c.id != clientId ? c : {...c, username: Some(username)}, state.clients)
      },
      ((_) => Socket.send(client.ws, UsernameSet(None)))
    )
  };

let debug = () => {
  Js.log("clients:");
  List.iter((c) => Js.log4("id:", c.id, "username:", c.username), currentState^.clients)
};

let update = (a: action) => {
  Js.log2("new action:", a);
  switch (reduce(currentState^, a)) {
  | NoUpdate => ()
  | SideEffect(fn) => fn(currentState^)
  | Update(newState) => currentState := newState
  | UpdateWithSideEffect(newState, fn) =>
    currentState := newState;
    fn(currentState^)
  };
  Js.log("new state:");
  debug()
};