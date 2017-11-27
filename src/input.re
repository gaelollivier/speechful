let component = ReasonReact.statelessComponent("Input");

let make = (~value: string, ~onChange: string => unit, ~placeholder=?, _children) => {
  ...component,
  render: (_self) => {
    let handleChange = (event) =>
      onChange(ReactDOMRe.domElementToObj(ReactEventRe.Form.target(event))##value);
    <input className="text-input" _type="text" onChange=handleChange value ?placeholder />
  }
};