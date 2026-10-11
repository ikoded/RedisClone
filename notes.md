RESP is Redis Serialization Protocol

Testing tool example: (Netcat) `echo "GET user:100" | nc -N localhost 8080`

Can also test automated using python with the socket library and create c++ tests to learn testing
Add GoogleTest Primer for real production grade testing

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


EXAMPLE OF WHAT BROWSER SENDS

Browser sent: 
GET HTTP/1.1
Host: 127.0.0.1:8080
Connection: keep-alive
Sec-Fetch-Site: same-origin
Sec-Fetch-Mode: no-cors
Sec-Fetch-Dest: empty
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/154.0.0.0 Safari/537.36
Accept-Encoding: gzip, deflate, br, zstd
Accept-Language: en-US,en;q=0.9