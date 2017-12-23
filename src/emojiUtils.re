/* binding to emoji-tree lib */
[@bs.module] external emojiTree : string => array({. "text": string, "_type": string}) =
  "emoji-tree";

type emojiStringChunk =
  | Text(string)
  | Emojis(string);

/*
    split a string containing emojis into a list of chunks of Text/Emoji
    ex: "Hey ! 😂😄 Awesome" becomes [Text("Hey ! "), Emojis("😂😄"), Text(" Awesome")]
 */
let splitString = (str: string) => {
  let chars = emojiTree(str);
  Array.fold_right(
    (curr, acc) => {
      let c = curr##text;
      let cType = curr##_type;
      if (cType == "text") {
        switch acc {
        | [] => [Text(c)]
        | [Text(hd), ...tl] => [Text(c ++ hd), ...tl]
        | tl => [Text(c), ...tl]
        }
      } else {
        switch acc {
        | [] => [Emojis(c)]
        | [Emojis(hd), ...tl] => [Emojis(c ++ hd), ...tl]
        | tl => [Emojis(c), ...tl]
        }
      }
    },
    chars,
    []
  )
};

/* replace common smiley ascii text (like :) ) with emojis */
let replaceAscii = (userText: string) : string =>
  userText
  |> Js.String.replace(":)", {j|🙂|j})
  |> Js.String.replace(":D", {j|😃|j})
  |> Js.String.replace("^^", {j|😊|j})
  |> Js.String.replace(":(", {j|🙁|j})
  |> Js.String.replace(":o", {j|😲|j})
  |> Js.String.replace(":p", {j|😛|j})
  |> Js.String.replace(":/", {j|😕|j});