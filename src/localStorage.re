/* Simple wrapper arround local storage */
[@bs.scope "window.localStorage"] [@bs.val] [@bs.return nullable]
external getItem : string => option(string) =
  "";

[@bs.scope "window.localStorage"] [@bs.val] external setItem : (string, string) => unit = "";

[@bs.scope "window.localStorage"] [@bs.val] external removeItem : string => unit = "";