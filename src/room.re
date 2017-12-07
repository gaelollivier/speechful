open Utils;

type state = {
  userText: string,
  users: list(User.t)
};

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
  initialState: () => {userText: "", users: []},
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
  reducer: (action, state) =>
    switch action {
    | EventReceived(event) =>
      Js.log2("message received:", event);
      switch event {
      | Moien(_) => ReasonReact.NoUpdate
      | RoomJoined(_room, users) => ReasonReact.Update({...state, users})
      | UserJoined(user, _room) => ReasonReact.Update({...state, users: [user, ...state.users]})
      | UserLeft(userId, _room) =>
        ReasonReact.Update({
          ...state,
          users: List.filter((user: User.t) => user.id != userId, state.users)
        })
      | ChangedUsername(newUser) =>
        ReasonReact.Update({
          ...state,
          users: List.map((user: User.t) => user.id != newUser.id ? user : newUser, state.users)
        })
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
      | MessageSent(_, _) => ReasonReact.NoUpdate
      }
    | UserTextChanged(txt) => ReasonReact.Update({...state, userText: txt})
    | Say(txt) =>
      ReasonReact.UpdateWithSideEffects
        /* Reset text input */
        (
          {...state, userText: ""},
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
        <ul>
          (
            List.map(
              (user: User.t) =>
                <li key=user.id>
                  (
                    switch user.username {
                    | None => textEl("Anonymous")
                    | Some(username) => textEl(username)
                    }
                  )
                </li>,
              state.users
            )
            |> Array.of_list
            |> ReasonReact.arrayToElement
          )
        </ul>
        <Input
          value=state.userText
          onChange=(reduce((v) => UserTextChanged(v)))
          placeholder="Say something..."
        />
      </form>
    </div>
};