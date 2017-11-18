Utils.requireCSS("./index.css");

ReactDOMRe.renderToElementWithId(<App />, "root");

SpeechSynthesis.onVoicesChanged(() => Js.log(SpeechSynthesis.getVoices()));