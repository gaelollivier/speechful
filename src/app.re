open Utils;

type state = {
  server: option(Server.t),
  messageHandler: option((string => unit))
};

type action =
  | ConnectionOpened(Server.t)
  | ServerEvent(Server.event)
  | SetMessageHandler(string => unit);

let component = ReasonReact.reducerComponent("App");

let make = (_children) => {
  ...component,
  initialState: () => {server: None, messageHandler: None},
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
        ReasonReact.SideEffects(
          (
            ({state}) =>
              switch state.messageHandler {
              | Some(handler) => handler(msg)
              | None => ()
              }
          )
        )
      }
    },
  render: ({state, reduce}) =>
    <div className="App">
      (
        switch state.server {
        | None => textEl("Loading...")
        | Some(server) =>
          <Room
            setMessageHandler=(reduce((handler) => SetMessageHandler(handler)))
            say=((text) => Server.sendMsg(server, text))
          />
        }
      )
    </div>
};