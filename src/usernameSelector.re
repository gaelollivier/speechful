open Utils;

type state = {username: string};

type action =
  | UsernameChanged(string)
  | FormSubmit
  | StayAnonymous;

let component = ReasonReact.reducerComponent("UsernameSelector");

let make = (_children) => {
  ...component,
  initialState: () => {username: ""},
  reducer: (action, state) => {
    let setUsername = (username) => {
      LocalStorage.setItem("username", username);
      Location.reload()
    };
    switch action {
    | UsernameChanged(name) => ReasonReact.Update({username: name})
    | FormSubmit =>
      if (state.username == "") {
        ReasonReact.NoUpdate
      } else {
        ReasonReact.SideEffects(((_) => setUsername(state.username)))
      }
    | StayAnonymous => ReasonReact.SideEffects(((_) => setUsername("")))
    }
  },
  render: ({state, reduce}) => {
    let handleStayAnonymous =
      reduce(
        (e) => {
          ReactEventRe.Mouse.preventDefault(e);
          StayAnonymous
        }
      );
    <form
      className="username-form"
      onSubmit=(
        reduce(
          (e) => {
            ReactEventRe.Synthetic.preventDefault(e);
            FormSubmit
          }
        )
      )>
      (textEl("Enter username:"))
      <Input
        value=state.username
        onChange=(reduce((v) => UsernameChanged(v)))
        placeholder="Username..."
      />
      <a onClick=handleStayAnonymous> (textEl("Or stay anonymous...")) </a>
    </form>
  }
};