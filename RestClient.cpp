#include "RestClient.h"

RestClient::RestClient(String _host, int _port, String _protocol, String _basePath)
{
    host = _host;
    port = _port;
    basePath = _basePath;
    setProtocol(_protocol);
    num_headers = 0;
    if (contentType == NULL)
    {
        contentType = "application/x-www-form-urlencoded"; // default
    }
}

void RestClient::setHeader(String header)
{
    headers[num_headers] = header;
    num_headers++;
}

void RestClient::setContentType(String contentTypeValue)
{
    contentType = contentTypeValue;
}

void RestClient::setProtocol(String protocol)
{
    ssl = protocol == "https";
}

// The mother- generic request method.
//
int RestClient::request(String method, String path, const char *body, String *response)
{
    if (ssl)
    {
        client = new BearSSL::WiFiClientSecure();
        static_cast<BearSSL::WiFiClientSecure *>(client)->setInsecure();
        static_cast<BearSSL::WiFiClientSecure *>(client)->setCiphersLessSecure();
    }
    else
    {
        client = new WiFiClient();
    }
    if (!client->connect(host, port))
    {
        Serial.println("DEBUG: client->connect() failed");
        return 0;
    }
    String request = method + " " + basePath + path + " HTTP/1.1\r\n";
    for (int i = 0; i < num_headers; i++)
    {
        request += String(headers[i]) + "\r\n";
    }
    request += "Host: " + String(host) + ":" + String(port) + "\r\n";
    request += "Connection: Keep-Alive\r\n"; // To ensure we can read the response before WiFiClient garbles up it's state.

    if (body != NULL)
    {
        char contentLength[30];
        sprintf(contentLength, "Content-Length: %d\r\n", strlen(body));
        request += String(contentLength);

        request += "Content-Type: " + String(contentType) + "\r\n";
    }
    request += "\r\n";
    if (body != NULL)
    {
        request += String(body);
        request += "\r\n\r\n";
    }

    // DEBUG****
    // Serial.println(request);
    // DEBUG****

    client->print(request.c_str());
    int statusCode = _readResponse(response);
    // cleanup
    num_headers = 0;
    client->stop();
    delete client;
    client = NULL;
    return statusCode;
}

int RestClient::_readResponse(String *response)
{
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    boolean httpBody = false;
    boolean inStatus = false;

    char statusCode[4];
    int i = 0;
    int code = 0;

    // DEBUG****
    //Serial.println(String("Reading response [connected:")+String(client->connected())+"][available:"+String(client->available())+"]" );
    // DEBUG****

    int tick = 5000;
    while(!client->available() && tick--)
    {
        delay(1); // Allow network to catch up or we'll time out doing waiting
    }

    // DEBUG****
    //Serial.println(String("Now wait ticks remaining: ")+String(tick));
    //Serial.printf("Now available: %d\n", client->available());
    // DEBUG****

    // read until there's no more and parse the message basics
    while (client->available())
    {
        char c = client->read();

        if (c == ' ' && !inStatus)
        {
            inStatus = true;
        }

        if (inStatus && i < 3 && c != ' ')
        {
            statusCode[i] = c;
            i++;
        }
        if (i == 3)
        {
            statusCode[i] = '\0';
            code = atoi(statusCode);
        }

        if (httpBody)
        {
            // only write response if its not null
            if (response != NULL)
                response->concat(c);
       }
        else
        {
            if (c == '\n' && currentLineIsBlank)
            {
                httpBody = true;
            }

            if (c == '\n')
            {
                // you're starting a new line
                currentLineIsBlank = true;
            }
            else if (c != '\r')
            {
                // you've gotten a character on the current line
                currentLineIsBlank = false;
            }
        }
    }
    return code;
}