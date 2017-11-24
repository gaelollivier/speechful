open Utils;

type state = {server: option(Server.t)};

type action =
  | ConnectionOpened(Server.t)
  | MessageReceived(string);

let component = ReasonReact.reducerComponent("App");

let make = (_children) => {
  ...component,
  initialState: () => {server: None},
  didMount: (self) => {
    let onConnect = (server) => self.reduce(() => ConnectionOpened(server), ());
    let onEvent = (event) =>
      switch event {
      | Server.MessageReceived(msg) =>
        self.reduce(() => MessageReceived(msg), ());
        ()
      };
    Server.connect(onConnect, onEvent);
    ReasonReact.NoUpdate
  },
  reducer: (action, _state) =>
    switch action {
    | ConnectionOpened(server) => ReasonReact.Update({server: Some(server)})
    | MessageReceived(msg) =>
      ReasonReact.SideEffects(
        (
          (_self) => {
            let ut = SpeechSynthesis.Utterance.create(msg);
            Js.log(msg);
            SpeechSynthesis.speak(ut)
          }
        )
      )
    },
  render: ({state}) =>
    <div className="App">
      (
        switch state.server {
        | None => textEl("Hello world !")
        | Some(_) => textEl("Connected !")
        }
      )
    </div>
};