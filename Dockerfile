# Create image with all required build tools
FROM ubuntu:18.04 as build-tools

RUN apt-get update && apt-get install -y \
  build-essential \
  cmake \
  python3-pip \
  && rm -rf /var/lib/apt/lists/*

RUN pip3 install conan


# Build binary
FROM build-tools as builder

WORKDIR /usr/src/service

COPY . .

WORKDIR /usr/src/service/build

RUN conan install ..
RUN cmake ..
RUN make


# Copy binary to fresh ubuntu image
FROM ubuntu:18.04 as runner

RUN groupadd -r ubuntu && useradd --no-log-init -r -g ubuntu ubuntu
USER ubuntu:ubuntu

COPY --from=builder /usr/src/service/build/bin/http-service /usr/bin/http-service
EXPOSE 8080

CMD [ "http-service", "0.0.0.0", "8080" ]
