# http-service

Example of HTTP service in C++ using Boost Beast.

### Requirements

- CMake
- Conan
- Make
- Docker (optional to build deployment image)

### Building

1. Create directory to build project, for example:

```bash
rm -rf ./build
mkdir build && cd build
```

2. Install dependencies, and generate build files:

```bash
conan install ..
cmake ..
```

3. Build the project:

```bash
make
```

### Running

The binary requires address and port to listen on provided as arguments:

```bash
GLOG_logtostderr=1 ./build/bin/http-service 0.0.0.0 8080
```

### Deployment (as Docker container)

1. Build new image using Dockerfile. For example:

```bash
docker build -t aakatev/http-service .
```

2. Run the resulting image, mapped to desired port:

```bash
docker run --rm --init -p 8000:8080 aakatev/http-service
```

3. Test that service is responding. For example using `curl`:

```bash
curl -i localhost:8000/api/status

## Should return something similar to:
HTTP/1.1 200 OK
Connection: close
Content-Type: application/json
Content-Length: 60

{"status":"OK","request_count":"1","timestamp":"1607196274"}
```

4. Tag image, and push it to Docker registry. Pull in your application.
