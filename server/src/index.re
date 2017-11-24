open Express;

let app = express();

let dirname =
  switch [%bs.node __dirname] {
  | Some(p) => p
  | None => "."
  };

let webappPath = Node.Path.join2(dirname, "../../../../build");

/* Static files */
App.useOnPath(
  app,
  ~path="/",
  {
    let options = Static.defaultOptions();
    Static.make(webappPath, options) |> Static.asMiddleware
  }
);

/* Websocket server */
App.get(
  app,
  ~path="/ws",
  Middleware.from((_req, res, _next) => Response.sendString(res, "This is gonna be a websocket!"))
);

/* All the rest goes to index.html, to support frontend routing */
App.get(
  app,
  ~path="*",
  Middleware.from(
    (_req, res, _next) => Response.sendFile(res, Node.Path.join2(webappPath, "index.html"), ())
  )
);

let port =
  switch (Js.Dict.get(Node.Process.process##env, "PORT")) {
  | Some(p) => int_of_string(p)
  | None => 8080
  };

App.listen(
  app,
  ~port,
  ~onListen=(_) => Js.log("Server listening on port " ++ string_of_int(port)),
  ()
);