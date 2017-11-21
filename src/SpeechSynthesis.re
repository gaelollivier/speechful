module Voice = {
  type t;
};

module Utterance = {
  type t;
  [@bs.new] external create : string => t = "SpeechSynthesisUtterance";
  [@bs.get] external get_lang : t => string = "lang";
  [@bs.set] external set_lang : (t, string) => unit = "lang";
  [@bs.get] external get_pitch : t => float = "pitch";
  [@bs.set] external set_pitch : (t, float) => unit = "pitch";
  [@bs.get] external get_rate : t => float = "rate";
  [@bs.set] external set_rate : (t, float) => unit = "rate";
  [@bs.get] external get_text : t => string = "text";
  [@bs.set] external set_text : (t, string) => unit = "text";
  [@bs.get] external get_voice : t => Voice.t = "voice";
  [@bs.set] external set_voice : (t, Voice.t) => unit = "voice";
  [@bs.get] external get_volume : t => float = "volume";
  [@bs.set] external set_volume : (t, float) => unit = "volume";
};

/* Methods on global `speechSynthesis` object */
[@bs.scope "window.speechSynthesis"] [@bs.val] external getVoices : unit => array(Voice.t) = "";

[@bs.scope "window.speechSynthesis"] [@bs.val] external speak : Utterance.t => unit = "";

/* Setters */
type t_globalSpeechSynthesis;

[@bs.val] [@bs.scope "window"] external globalSpeechSynthesis : t_globalSpeechSynthesis =
  "speechSynthesis";

[@bs.set] external setOnVoicesChanged : (t_globalSpeechSynthesis, unit => unit) => unit =
  "onvoiceschanged";

let onVoicesChanged = (cb: unit => unit) => setOnVoicesChanged(globalSpeechSynthesis, cb);