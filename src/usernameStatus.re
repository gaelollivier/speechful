open Utils;

let component = ReasonReact.statelessComponent("UsernameStatus");

let make = (~username: option(string), _children) => {
  ...component,
  render: (_self) => {
    let handleChangeUsername = (e) => {
      ReactEventRe.Mouse.preventDefault(e);
      /* clear username is local storage and refresh page */
      LocalStorage.removeItem("username");
      Location.reload()
    };
    <div className="username-status">
      <div>
        (textEl("Connected as "))
        <strong>
          (
            switch username {
            | None => textEl("Anonymous")
            | Some(username) => textEl(username)
            }
          )
        </strong>
      </div>
      <a onClick=handleChangeUsername> (textEl("Change username")) </a>
    </div>
  }
};