open WsServer;

/* State + selectors */
type t = {
  clients: list(Client.t),
  rooms: list(string)
};

let get_client = (state: t, id: Socket.id) =>
  List.find((c: Client.t) => c.id == id, state.clients);

let clientsInRoom = (state: t, room: string) =>
  List.filter((c: Client.t) => c.room == Some(room), state.clients);