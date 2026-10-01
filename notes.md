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

I want to make it a complex redis clone that works similar to the true Redis.
For this to work, that means I need an unordered map of a nested unordered map so if say `SET user:100 username "alice"` it will set a key of `user:100` with an unordered_map of possible fields you can add (any as you please)