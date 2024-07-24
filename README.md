# esp-restclient
ESP8266 REST Client

** IMPORTANT: the cipher mode during connection is set to less secure to allow connections to be established in reasonable time on ESP8266 and similar low spec targets. Don't use this version of the library or derived versions in situations where this lower bar isn't appropriate.

Supporting Protocols

- HTTP
- HTTPS

Supporting Methods

- GET
- POST
- PUT
- PATCH
- DELETE

This client uses BearSSL Secure wifi client for the HTTPS connections.


## Usage HTTP

```
#include <RestClient.h>
...
void setup()
{
  // Connect to WIFI
  ...
  
  // Create a REST Client for http://reqres.in/api/
  RestClient client("reqres.in", 80, "http", "/api/");
  String response;
  client.get("users/2", &response);
  Serial.println(response);
}

void loop()
{
}
```

## Usage HTTPS - with all new BearSSL Secure client

```
#include <RestClient.h>
...
void setup()
{
  // Connect to WIFI
  ...
  
  // Create a REST Client for https://reqres.in/api/
  RestClient client("reqres.in", 443, "https", "/api/");
  String response;
  client.get("users/2", &response);
  Serial.println(response);
}

void loop()
{
}
```
