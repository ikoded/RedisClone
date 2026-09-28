RESP is Redis Serialization Protocol

Testing tool example: (Netcat) `echo "GET user:100" | nc -N localhost 8080`

Can also test automated using python with the socket library and create c++ tests to learn testing

Will need a persistent file whenever server shuts down to save the data and load in whenever server starts

DOCS:

https://redis.io/docs/latest/develop/reference/protocol-spec/
https://redis.io/docs/latest/

GET request

1. Client (Browser) connects -> Sends HTTP Request:
   "GET /user:100 HTTP/1.1\r\nHost: localhost:8080\r\n\r\n"

2. Your C++ Server parses the string:
   - Extract the path (e.g., "/user:100")
   - Strip the leading "/" to get your Redis key: "user:100"

3. Your C++ Server looks up the key in your Redis map.

4. Your C++ Server sends back a valid HTTP Response:
   "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: [size]\r\n\r\n[Your Value Here]"