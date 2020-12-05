FROM conanio/clang9:1.31.2

WORKDIR /usr/src/service

COPY . .

EXPOSE 8080

WORKDIR /usr/src/service/build

RUN sudo /opt/pyenv/shims/conan install ..
RUN sudo cmake ..
RUN sudo make

WORKDIR /usr/src/service

CMD [ "./build/bin/http-service", " 0.0.0.0", "8080" ]
