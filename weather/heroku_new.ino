char heroku_server[] = "weather-logger.192.168.1.45.xip.io";

// this method makes a HTTP connection to the server:
bool sendDataToHeroku(String data) {
  unsigned long timestamp;
  // if there's a successful connection:
  if (client.connect(heroku_server, 80)) {
    Serial.println("connecting...");
    // send the HTTP PUT request:
    client.println(F("GET /streams HTTP/1.1"));
    client.print(F("Host: "));
    client.println(heroku_server);
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
