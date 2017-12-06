open Utils;

type state = {
  server: option(Server.t),
  messageHandler: option((Event.t => unit)),
  user: option(User.t)
};

type action =
  | ConnectionOpened(Server.t)
  | ServerEvent(Server.event)
  | SetMessageHandler(Event.t => unit);

let component = ReasonReact.reducerComponent("App");

let make = (~room: option(string)=?, _children) => {
  ...component,
  initialState: () => {server: None, messageHandler: None, user: None},
  didMount: (self) => {
    let onConnect = (server) => self.reduce(() => ConnectionOpened(server), ());
    let onEvent = self.reduce((event) => ServerEvent(event));
    Server.connect(onConnect, onEvent);
    ReasonReact.NoUpdate
  },
  reducer: (action, state) =>
    switch action {
    | ConnectionOpened(server) => ReasonReact.Update({...state, server: Some(server)})
    | SetMessageHandler(handler) => ReasonReact.Update({...state, messageHandler: Some(handler)})
    | ServerEvent(e) =>
      switch e {
      | Server.MessageReceived(msg) =>
        switch (Event.decode(msg)) {
        | exception e =>
          Js.log2("cannot decode incoming message:", e);
          ReasonReact.NoUpdate
        | Moien(userId) =>
          ReasonReact.Update({...state, user: Some(User.{id: userId, username: None})})
        | event =>
          ReasonReact.SideEffects(
            (
              ({state}) =>
                switch state.messageHandler {
                | Some(handler) => handler(event)
                | None => ()
                }
            )
          )
        }
      }
    },
  render: ({state, reduce}) =>
    <div className="App">
      (
        switch (state.server, state.user) {
        | (Some(server), Some(user)) =>
          switch room {
          | Some(room) =>
            <Room
              setMessageHandler=(reduce((handler) => SetMessageHandler(handler)))
              sendMessage=((msg) => Server.sendMsg(server, Message.encodeJSON(msg)))
              user
              room
            />
          | None => textEl("Select a room")
          }
        | _ => textEl("Loading...")
        }
      )
    </div>
};