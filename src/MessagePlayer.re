open Utils;

let component = ReasonReact.statelessComponent("MessagePlayer");

let make = (~message: string, _children) => {
  ...component,
  render: (_self) => <div> (textEl(message)) </div>
};
/* (
     /* Say incoming message ! */
     (self) => {
       let user = List.find((u: User.t) => u.id == senderId, state.users);
       let username = default("anonymous", user.name);
       Js.log2(username ++ ":", txt);
       let ut = SpeechSynthesis.Utterance.create(txt);
       SpeechSynthesis.Utterance.on_end(ut, self.reduce((_) => SpeechEnd));
       SpeechSynthesis.speak(ut)
     }
   ) */