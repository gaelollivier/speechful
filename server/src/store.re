open WsServer;

/*
 * This file implements an "home-made" state management system,
 * closely inspired from ReasonReact ReducerComponent
 * (but without the "generic" part)
 */
type state = {
  clients: list(Client.t),
  rooms: list(string)
};

let get_client = (state: state, id: Socket.id) =>
  List.find((c: Client.t) => c.id == id, state.clients);

let currentState: ref(state) = ref({clients: [], rooms: []});

type action =
  | NewClient(Socket.t)
  | ClientDisconnected(Socket.id)
  | SetUsername(Socket.id, string)
  | JoinRoom(Socket.id, string);

type update =
  | NoUpdate
  | SideEffect(state => unit)
  | Update(state)
  | UpdateWithSideEffect(state, state => unit);

let reduce = (state: state, action: action) : update =>
  switch action {
  | NewClient(c) => Update({...state, clients: [Client.make(c), ...state.clients]})
  | ClientDisconnected(clientId) =>
    /* TODO: Remove client from room */
    Update({...state, clients: List.filter((c: Client.t) => c.id !== clientId, state.clients)})
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
        ...state,
        clients:
          List.map(
            (c: Client.t) => c.id != clientId ? c : {...c, username: Some(username)},
            state.clients
          )
      },
      /* TODO: broadcast username to users in room */
      ((_) => Socket.send(client.ws, Message.encodeJSON(UsernameSet(None))))
    )
  | JoinRoom(clientId, room) =>
    let client = get_client(state, clientId);
    /* TODO: remove client from its current room */
    /* update client room */
    let newClients =
      List.map((c: Client.t) => c.id != clientId ? c : {...c, room: Some(room)}, state.clients);
    let roomExists = List.exists((r) => r == room, state.rooms);
    if (roomExists) {
      /* room exist: need to send list of existing users + broadcast new user */
      NoUpdate
    } else {
      /* new room */
      UpdateWithSideEffect(
        {clients: newClients, rooms: [room, ...state.rooms]},
        ((_) => Socket.send(client.ws, Message.encodeJSON(RoomJoined(room, []))))
      )
    }
  };

let debug = () => {
  Js.log("clients:");
  List.iter((c: Client.t) => Js.log(Message.encodeClient(c)), currentState^.clients);
  Js.log("rooms:");
  List.iter((r) => Js.log(r), currentState^.rooms)
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