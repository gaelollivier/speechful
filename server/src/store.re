open WsServer;

/*
 * Would be cool to have something like redux-loop for state management
 * This is an attempt to have a redux-like state management, so we avoid mutations.
 * For now, reduce can produce side-effects... would be better to return them instead
 * (but I don't plan to write unit tests for now anyway so... ¯\_(ツ)_/¯)
 */
type t = {clients: list(Socket.t)};

let currentState: ref(t) = ref({clients: []});

type action =
  | NewClient(Socket.t)
  | ClientDisconnected(Socket.t)
  | NewMessage(Socket.t, string);

let reduce = (state: t, action: action) : t =>
  switch action {
  | NewClient(c) => {clients: [c, ...state.clients]}
  | ClientDisconnected(client) => {clients: List.filter((c) => c !== client, state.clients)}
  | NewMessage(client, message) =>
    /* Broadcast to all connected clients */
    List.iter(
      (c) =>
        if (c !== client) {
          Socket.send(c, message)
        },
      state.clients
    );
    state
  };

let update = (a: action) => currentState := reduce(currentState^, a);