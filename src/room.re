type state = {userText: string};

type action =
  | EventReceived(Event.t)
  | UserTextChanged(string)
  | Say(string);

let component = ReasonReact.reducerComponent("Room");

let make =
    (
      ~setMessageHandler,
      ~sendMessage,
      ~room: string,
      ~username: option(string),
      ~userId: User.id,
      _children
    ) => {
  ...component,
  initialState: () => {userText: ""},
  didMount: (_self) =>
    /* set message handler */
    ReasonReact.SideEffects(
      ({reduce}) => {
        setMessageHandler(reduce((e) => EventReceived(e)));
        /* set username */
        switch username {
        | Some(username) => sendMessage(Message.SetUsername(username))
        | None => ()
        };
        /* join room */
        sendMessage(Message.JoinRoom(room))
      }
    ),
  reducer: (action, _state) =>
    switch action {
    | EventReceived(event) =>
      Js.log2("message received:", event);
      switch event {
      | Moien(userId) =>
        Js.log2("user id", userId);
        ReasonReact.NoUpdate
      | MessageSent(senderId, txt) when senderId != userId =>
        ReasonReact.SideEffects(
          (
            /* Say incoming message ! */
            (_self) => {
              Js.log2("anonymous:", txt);
              let ut = SpeechSynthesis.Utterance.create(txt);
              SpeechSynthesis.speak(ut)
            }
          )
        )
      | _ => ReasonReact.NoUpdate
      }
    | UserTextChanged(txt) => ReasonReact.Update({userText: txt})
    | Say(txt) =>
      ReasonReact.UpdateWithSideEffects
        /* Reset text input */
        (
          {userText: ""},
          /* Send message */
          (
            (_self) => {
              Js.log2("me:", txt);
              sendMessage(Message.SendMessage(txt))
            }
          )
        )
    },
  render: ({state, reduce}) =>
    <div className="App">
      <form
        onSubmit=(
          reduce(
            (e) => {
              ReactEventRe.Synthetic.preventDefault(e);
              Say(state.userText)
            }
          )
        )>
        <Input
          value=state.userText
          onChange=(reduce((v) => UserTextChanged(v)))
          placeholder="Say something..."
        />
      </form>
    </div>
};