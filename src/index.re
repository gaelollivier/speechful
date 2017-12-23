Utils.requireCSS("./index.css");

let renderForRoute = (element) => ReactDOMRe.renderToElementWithId(element, "root");

let router =
  DirectorRe.makeRouter({
    "/": () => renderForRoute(<App />),
    "/:room": (room: string) => renderForRoute(<App room />)
  });

DirectorRe.configure(router, {"html5history": true});

DirectorRe.init(router, "");