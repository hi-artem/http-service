FROM ubuntu:18.04 as build-tools

RUN apt-get update && apt-get install -y \
  build-essential \
  cmake \
  python3-pip \
  && rm -rf /var/lib/apt/lists/*

RUN pip3 install conan

FROM build-tools

WORKDIR /usr/src/service

COPY . .

EXPOSE 8080

WORKDIR /usr/src/service/build

RUN conan install ..
RUN cmake ..
RUN make

WORKDIR /usr/src/service

CMD [ "./build/bin/http-service", "0.0.0.0", "8080" ]
