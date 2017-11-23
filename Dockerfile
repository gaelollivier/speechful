FROM node:7

RUN npm install -g bs-platform@2.1.0

ADD . /usr/local/src/app

WORKDIR /usr/local/src/app

RUN npm install && npm run build