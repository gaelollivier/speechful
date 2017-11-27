type state = {userText: string};

type action =
  | MessageReceived(string)
  | UserTextChanged(string)
  | Say(string);

let component = ReasonReact.reducerComponent("Room");

let make = (~setMessageHandler, ~say, _children) => {
  ...component,
  initialState: () => {userText: ""},
  didMount: (_self) =>
    /* set message handler */
    ReasonReact.SideEffects(({reduce}) => setMessageHandler(reduce((msg) => MessageReceived(msg)))),
  reducer: (action, _state) =>
    switch action {
    | MessageReceived(txt) =>
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
              say(txt)
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