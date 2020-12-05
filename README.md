# http-service

Example of HTTP service in C++ using Boost Beast.

### Requirements

- CMake
- Conan
- Make

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
./build/bin/http-service 0.0.0.0 8080
```
