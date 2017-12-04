open WsServer;

/*
 * This file implements an "home-made" state management system,
 * closely inspired from ReasonReact ReducerComponent
 * (but without the "generic" part)
 */
/* Actions */
type action =
  | NewClient(Socket.t)
  | ClientDisconnected(Socket.id)
  | SetUsername(Socket.id, string)
  | JoinRoom(Socket.id, string)
  | SendMessage(Socket.id, string);

type update =
  | NoUpdate
  | SideEffect(State.t => unit)
  | Update(State.t)
  | UpdateWithSideEffect(State.t, State.t => unit);

/* Reducer */
let reduce = (state: State.t, action: action) : update =>
  switch action {
  | NewClient(c) => Update({...state, clients: [Client.make(c), ...state.clients]})
  | SetUsername(clientId, username) =>
    let client = State.getClient(state, clientId);
    let newClient = Client.{...client, username: Some(username)};
    UpdateWithSideEffect(
      {
        ...state,
        clients: List.map((c: Client.t) => c.id != clientId ? c : newClient, state.clients)
      },
      {
        /* broadcast username to clients in room */
        let effects =
          switch newClient.room {
          | Some(room) =>
            let clientsInRoom = State.clientsInRoom(state, room);
            List.map(
              (c: Client.t, _) =>
                Socket.send(c.ws, Message.encodeJSON(ChangedUsername(newClient))),
              clientsInRoom
            )
          | None => [
              ((_) => Socket.send(client.ws, Message.encodeJSON(ChangedUsername(newClient))))
            ]
          };
        ((state) => List.iter((f) => f(state), effects))
      }
    )
  | ClientDisconnected(clientId) =>
    let client = State.getClient(state, clientId);
    let (newRooms, effects) =
      switch client.room {
      | Some(room) => Room.removeClient(state, clientId, room)
      | None => (state.rooms, [])
      };
    UpdateWithSideEffect(
      {clients: List.filter((c: Client.t) => c.id !== clientId, state.clients), rooms: newRooms},
      ((state) => List.iter((f) => f(state), effects))
    )
  | JoinRoom(clientId, room) =>
    let client = State.getClient(state, clientId);
    switch client.room {
    /* client didn't change room: do nothing */
    | Some(prevRoom) when prevRoom == room => NoUpdate
    | _ =>
      /* remove client from its current room */
      let (newRooms, effects) =
        switch client.room {
        | Some(prevRoom) => Room.removeClient(state, clientId, prevRoom)
        | _ => (state.rooms, [])
        };
      /* update client room */
      let newClients =
        List.map((c: Client.t) => c.id != clientId ? c : {...c, room: Some(room)}, state.clients);
      let clientsInRoom = State.clientsInRoom(state, room);
      let effects = [
        /* send list of users already in room */
        ((_) => Socket.send(client.ws, Message.encodeJSON(RoomJoined(room, clientsInRoom)))),
        /* broadcast new user to users already in room */
        (
          (_) =>
            List.iter(
              (c: Client.t) => Socket.send(c.ws, Message.encodeJSON(UserJoined(client, room))),
              clientsInRoom
            )
        ),
        ...effects
      ];
      UpdateWithSideEffect(
        {
          clients: newClients,
          /* append new room to rooms list if it doesn't exist already */
          rooms: List.exists((r) => r == room, newRooms) ? newRooms : [room, ...newRooms]
        },
        ((state) => List.iter((f) => f(state), effects))
      )
    }
  | SendMessage(clientId, message) =>
    let client = State.getClient(state, clientId);
    switch client.room {
    | Some(room) =>
      let clientsInRoom = State.clientsInRoom(state, room);
      let effects =
        List.map(
          (c: Client.t, _) =>
            Socket.send(c.ws, Message.encodeJSON(MessageSent(clientId, message))),
          clientsInRoom
        );
      SideEffect(((state) => List.iter((f) => f(state), effects)))
    /* Nobody in room: do nothing */
    | None => NoUpdate
    }
  };

/* Store  */
let currentState: ref(State.t) = ref(State.{clients: [], rooms: []});

let debug = () => {
  Js.log("clients:");
  List.iter((c: Client.t) => Js.log(Message.encodeClient(c)), currentState^.clients);
  Js.log("rooms:");
  List.iter((r) => Js.log(r), currentState^.rooms)
};

let update = (a: action) => {
  Js.log("new action");
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