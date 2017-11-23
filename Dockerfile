FROM node:9

USER root

RUN mkdir /root/.npm-global \
  && npm config set prefix '/root/.npm-global'

ENV PATH /root/.npm-global/bin:$PATH

RUN npm install -g bs-platform