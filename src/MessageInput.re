let component = ReasonReact.statelessComponent("MessageInput");

let make = (~value: string, ~onChange: string => unit, _children) => {
  ...component,
  render: (_self) => {
    let handleChange = (newText: string) => onChange(EmojiUtils.replaceAscii(newText));
    <Input onChange=handleChange value placeholder="Say something..." />
  }
};