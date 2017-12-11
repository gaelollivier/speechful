open Utils;

type state = {
  userText: string,
  users: list(User.t),
  speaking: option(User.id)
};

type action =
  | EventReceived(Event.t)
  | UserTextChanged(string)
  | Say(string)
  | SpeechEnd;

let component = ReasonReact.reducerComponent("Room");

let make = (~setMessageHandler, ~sendMessage, ~room: string, ~currentUser: User.t, _children) => {
  ...component,
  initialState: () => {userText: "", users: [currentUser], speaking: None},
  didMount: (_self) =>
    /* set message handler */
    ReasonReact.SideEffects(
      ({reduce}) => {
        setMessageHandler(reduce((e) => EventReceived(e)));
        /* set username */
        switch currentUser.name {
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
      | RoomJoined(_room, users) => ReasonReact.Update({...state, users: [currentUser, ...users]})
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
      | MessageSent(senderId, txt) when senderId != currentUser.id =>
        ReasonReact.UpdateWithSideEffects(
          {...state, speaking: Some(senderId)},
          (
            /* Say incoming message ! */
            (self) => {
              let user = List.find((u: User.t) => u.id == senderId, state.users);
              let username = default("anonymous", user.name);
              Js.log2(username ++ ":", txt);
              let ut = SpeechSynthesis.Utterance.create(txt);
              SpeechSynthesis.Utterance.on_end(ut, self.reduce((_) => SpeechEnd));
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
    | SpeechEnd => ReasonReact.Update({...state, speaking: None})
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
            state.users
            |> List.sort(
                 (a: User.t, b: User.t) =>
                   switch (a.name, b.name) {
                   | (Some(a), Some(b)) => String.compare(a, b)
                   | (Some(_), None) => (-1)
                   | (None, Some(_)) => 1
                   | (None, None) => 0
                   }
               )
            |> List.map(
                 (user: User.t) =>
                   <li key=user.id>
                     {
                       let username = default("Anonymous", user.name);
                       let me =
                         if (user.id == currentUser.id) {
                           " (me)"
                         } else {
                           ""
                         };
                       let username = username ++ me;
                       switch state.speaking {
                       | Some(id) when id == user.id => <strong> (textEl(username)) </strong>
                       | Some(_)
                       | None => textEl(username)
                       }
                     }
                   </li>
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