# Concurrent web server

- Main thread accepts client connections and put the descriptors in a queue.
- Worker threads consume the queue and handle requests.
- Use synchronization primitives to control access to the queue.

## Usage

```bash
make clean & make

# Run server: wserver [-d basedir] [-p port] [-t <threads>] [-b <buffers>] [-s <schedalg (FIFO|SFF)>]
./wserver -d . -p 8000 -t 10 -b 100 -s FIFO

# Run client: wclient host port filename [-t <threads>]
./wclient localhost 8000 /Makefile -t 10
```

## TODO

- [bug] client didn't receive enough bytes.
