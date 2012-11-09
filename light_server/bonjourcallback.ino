void serviceFound(const char* type, MDNSServiceProtocol proto,
                  const char* name, const byte ipAddr[4],
                  unsigned short port,
                  const char* txtContent)
{
  if (NULL == name) {
	Serial.print(F("Finished discovering services of type "));
	Serial.println(type);
  } else {
    getDMXValues();
    getLEDValues();
    sendRefreshUI((uint8_t *)ipAddr);
  }
}
