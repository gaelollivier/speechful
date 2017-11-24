const path = require('path');
const express = require('express');
const expressWs = require('express-ws');

const create_server = (port, onConnect) => {
  console.log(`Setting up server on ${port}...`);

  const app = express();
  expressWs(app);

  const webappPath = path.join(__dirname, '../../../../build');

  // Server webapp
  app.use('/', express.static(webappPath));

  // Setup ws server: forward new connections to ReasonML code
  app.ws('/ws', function(ws, req) {
    onConnect(ws);
  });

  // All unknown path -> index.html (for HTML5 routing)
  app.get('*', (req, res) => {
    res.sendFile(path.join(webappPath, 'index.html'));
  });

  app.listen(port, () => {
    console.log(`Server listenning on ${port}`);
  });
};

module.exports = {
  create_server,
};
