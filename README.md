# Redis Clone

A clone of Redis that acts as a server that sends back data based on what the client requests

# Support

Will only support GET, SET & DELETE for now.

It will also only support `SET user:{id} {field} {string_value}` to start. This is useful so any user can have any field with value as the operator wants.

# Testing

Using nc to test the server.

# Future

In the future I would like the value of the field to be a: String, List, or Map. Started as String to get easy part created.