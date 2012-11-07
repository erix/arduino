#define APIKEY         "gdnQaSsTqBscWNJOFOJHTrkaxv-SAKxTSzVxMzZmM01GTT0g" // your cosm api key
#define FEEDID         84141 // your feed ID
#define USERAGENT      "Cosm Arduino Example (84141)" // user agent is the project name

IPAddress server(173,203,98,29); // api.cosm.com

// this method makes a HTTP connection to the server:
bool sendDataToCosm(String data) {
  unsigned long timestamp;
  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    // send the HTTP PUT request:
    client.print(F("PUT /v2/feeds/"));
    client.print(FEEDID);
    client.println(F(".csv HTTP/1.1"));
    client.println(F("Host: api.cosm.com"));
    client.print(F("X-ApiKey: "));
    client.println(APIKEY);
    client.print(F("User-Agent: "));
    client.println(USERAGENT);
    client.print(F("Content-Length: "));
    client.println(data.length());

    // last pieces of the HTTP PUT request:
    client.println(F("Content-Type: text/csv"));
    client.println(F("Connection: close"));
    client.println();

    // here's the actual content of the PUT request:
    client.println(data);
    
    Serial.println(data);
    
    timestamp = millis();
    
    // wait for the response
    while (client.connected() &&
           (millis() - timestamp < networkTimeout)) {
      if (client.available()) {
        char c = client.read();
        Serial.print(c);
      }
    }
    
    // time to stop, either disconnected or timed out
    // TODO: handle the timeout properly
    Serial.println(F("finishing send"));
    client.stop();
    return true;
  
  } 
  else {
    // if you couldn't make a connection:
    Serial.println(F("connection failed"));
    Serial.println();
    Serial.println(F("disconnecting."));
    client.stop();
    return false;
  }
}
  

