FROM node:7

WORKDIR /usr/local/src/app

RUN npm install -g bs-platform@2.1.0

# First add package.json and install dependencies
ADD package.json /usr/local/src/app
ADD server/package.json /usr/local/src/app/server/

RUN npm run prepare && npm install \
  && cd server && npm run prepare && npm install

# Then add sources and build the app
ADD . /usr/local/src/app

# Build webapp & server
RUN npm run build && cd server && npm run build

ENV PORT 8080

# Start server
ENTRYPOINT [ "node", "./server/lib/js/src/index.js" ]