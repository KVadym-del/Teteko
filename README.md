# Teteko
**Teteko** is a simple cli application for messaging from client to server.

## Build
To build the **Teteko** you need to run `./run.sh` file.

## Usage 
To use **Teteko** you can:
### For server
- use `./run.sh` with arguments:
```bash
./run.sh server --port=8080
```
- use application directly:
```bash
./build/Teteko server --port=8080
```

### For client
- use `./run.sh` with arguments:
```bash
./run.sh client --port=8080 --ip=127.0.0.1
```
- use application directly:
```bash
./build/Teteko client --port=8080 --ip=127.0.0.1
```
  
