char weather_server[] = "mysmarthome.herokuapp.com";
//char weather_server[] = "weather-logger.192.168.1.45.xip.io";


bool postToServer(String reading) {
  unsigned long timestamp;
  if (client.connect(weather_server,80)) {
    Serial.println(F("connected"));
    client.println("GET /save?report="+reading+" HTTP/1.1");
    client.print(F("Host: "));
    client.println(weather_server);
    client.println(F("Connection: close"));
    client.println();
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
    Serial.println(F("finishing send"));
    client.stop();
    return true;

  } else {
    Serial.println(F("connection failed"));
    return false;  
  }
}
