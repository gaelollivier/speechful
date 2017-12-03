open WsServer;

let notifyClientLeft = (clientId: Socket.id, room: string, remainingClients: list(Client.t), _) =>
  List.iter(
    (c: Client.t) => Socket.send(c.ws, Message.encodeJSON(UserLeft(clientId, room))),
    remainingClients
  );

let removeClient = (state: State.t, clientId: Socket.id, room: string) => {
  let remainingClients =
    List.filter((c: Client.t) => c.id != clientId, State.clientsInRoom(state, room));
  /* if no more clients in prev room: remove it */
  let newRooms =
    List.length(remainingClients) > 0 ? state.rooms : List.filter((r) => r != room, state.rooms);
  (newRooms, [notifyClientLeft(clientId, room, remainingClients)])
};