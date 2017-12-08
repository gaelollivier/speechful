open Utils;

type username =
  | NotSet
  | Set(option(string));

type state = {
  server: option(Server.t),
  messageHandler: option((Event.t => unit)),
  userId: option(User.id),
  username
};

type action =
  | ConnectionOpened(Server.t)
  | ServerEvent(Server.event)
  | SetMessageHandler(Event.t => unit);

let component = ReasonReact.reducerComponent("App");

let make = (~room: option(string)=?, _children) => {
  ...component,
  initialState: () => {
    /* check if user has already set its username */
    let username =
      switch (LocalStorage.getItem("username")) {
      | None => NotSet
      | Some(username) when username == "" => Set(None)
      | Some(username) => Set(Some(username))
      };
    {server: None, messageHandler: None, userId: None, username}
  },
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
        | Moien(userId) => ReasonReact.Update({...state, userId: Some(userId)})
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
        switch room {
        | None => textEl("To enter a room, simply enter its url. Ex: '/my-room'")
        | Some(room) =>
          switch state.username {
          | NotSet => <UsernameSelector />
          | Set(username) =>
            switch (state.server, state.userId) {
            | (Some(server), Some(userId)) =>
              <div>
                <UsernameStatus username />
                <Room
                  setMessageHandler=(reduce((handler) => SetMessageHandler(handler)))
                  sendMessage=((msg) => Server.sendMsg(server, Message.encodeJSON(msg)))
                  room
                  currentUser=User.{id: userId, name: username}
                />
              </div>
            | _ => textEl("Loading...")
            }
          }
        }
      )
    </div>
};