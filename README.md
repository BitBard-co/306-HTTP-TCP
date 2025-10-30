# 306-HTTP-TCP — Simple TCP/HTTP client in C

I wrote a tiny C program that connects to a server over TCP (port 80), sends a simple HTTP/1.1 POST request with a JSON body, receives the response, and prints it. I kept the code straightforward and commented each line so it’s easy to follow.

## What it does (in my own words)

- I resolve a hostname and port, open a TCP socket, and connect to the server.
- build an HTTP/1.1 POST request with these headers: Host, Content-Type, Connection: close, and a correct Content-Length.
- send a JSON body containing three fields: device (UUID), time (ISO-8601 UTC), and temperature (string).
- read the full response from the server until it closes the connection and then print either the full HTTP message (headers + body) or just the body, depending on the handler I choose.

## Where pointers are used (and why)

- Function pointer: I use a function pointer type `response_handler_fn` to switch at runtime between two printing behaviors:
	- `handle_raw`: prints the entire HTTP response as-is.
	- `handle_body`: prints only the part after the `\r\n\r\n` separator (the body).
	This shows how to select behavior with a function pointer.

- Pointer-to-pointer: my receive function has the signature `tcp_recv_into_buf(int fd, char **out, size_t *out_len)`.
	- It allocates a buffer internally and sets `*out` to point at that buffer, and it sets `*out_len` to the number of bytes read.
	- This is a common pattern in C when a function needs to return heap memory and a length.

## How the TCP and HTTP parts work here

- TCP:
	- I call `getaddrinfo(host, port, ...)` to resolve the address.
	- I loop through candidate addresses and try `socket(...)` + `connect(...)` until one works.
	- I send data with `send(...)` in a loop to ensure all bytes are written.
	- I receive data with `recv(...)` in a loop and grow the buffer until the peer closes the connection.

- HTTP:
	- I build the request line `POST <path> HTTP/1.1` and include required headers.
	- I set `Connection: close` so the server will close the socket after the response, which simplifies reading.
	- I compute `Content-Length` from the byte length of the JSON string I send.
	- The response may be a redirect or an HTML/JSON page depending on the host.

## Project layout

- `src/` — C source files
- `libs/` — Header files
- `bin/` — Compiled executable `sensor_client`
- `Makefile` — Builds the project

## Build and run (WSL)

- Build:
	```bash
	make
	```

- Run (full HTTP output: headers + body):
	```bash
	./bin/sensor_client example.com 80 / 22.4 550e8400-e29b-41d4-a716-446655440000 raw
	```

- Run (body only):
	```bash
	./bin/sensor_client example.com 80 / 22.4 550e8400-e29b-41d4-a716-446655440000 body
	```

Notes:
- Many public sites on HTTP (port 80) will redirect to HTTPS or return a small HTML page. That’s totally fine for this exercise. The key is that I can connect, send the POST, and print the response.
- If the server sends no body (e.g., a 301 with Content-Length: 0), the `body` handler will print nothing. Use the `raw` handler to see headers in that case.

## Clean up

- Remove object files and the executable:
	```bash
	make clean
	```
- Also remove the `bin/` directory:
	```bash
	make distclean
	```

## How I know it’s working

- The program prints a valid HTTP status line and headers when I use the `raw` handler. That proves the TCP connection worked and an HTTP response was received.
- The `body` handler prints only the payload if the response has one (useful when the server returns JSON or HTML content).
- Exit code 0 means success; non-zero means something failed (like connect/send/receive/allocation).

## Arguments

```
./bin/sensor_client <host> <port> <path> <temperature> <device-uuid> <handler>
```
- `host`: e.g., `example.com`
- `port`: e.g., `80`
- `path`: e.g., `/post` or `/`
- `temperature`: any string like `22.4`
- `device-uuid`: a UUID string
- `handler`: `raw` or `body`

That’s it. It’s a minimal C client that shows sockets, HTTP, and pointers in a simple way.
