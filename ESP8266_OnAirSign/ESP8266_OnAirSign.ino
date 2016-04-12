/*
 *  Simple HTTP get webclient test
 */

#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>

const char* ssid     = "ssid";
const char* password = "password";

#define HOST "api.twitch.tv"
#define PATH "/kraken/streams/adafruit"
#define REFRESH 20  // seconds between refresh
#define LED 13
#define PIN 12
Adafruit_NeoPixel strip = Adafruit_NeoPixel(30, PIN, NEO_GRBW + NEO_KHZ800);

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

int value = 0;

void loop() {
  delay(5000);
  ++value;

  Serial.print("connecting to ");
  Serial.println(HOST);
  
  // Use WiFiClient class to create TCP connections
  WiFiClientSecure client;
  if (!client.connect(HOST, 443)) {
    Serial.println("connection failed");
    return;
  }
  
  // We now create a URI for the request
  Serial.print("Requesting URL: ");
  Serial.println(PATH);
  
  // This will send the request to the server
  client.print(String("GET ") + PATH + " HTTP/1.1\r\n" +
               "Host: " + HOST + "\r\n" + 
               "Connection: close\r\n\r\n");

  int32_t timeout = millis() + 1000;
  while (client.available() == 0) { 
    if (timeout - millis() < 0) { 
      Serial.println(">>> Client Timeout !"); 
      client.stop(); 
      return;
    }
  }
  
  boolean isStreaming = false;
  while (client.connected()) {
    if (client.find("\"stream\":{\"_id\":")) {
      isStreaming = true;
    }
  }

  Serial.print("Streaming status: "); Serial.println(isStreaming);
  digitalWrite(LED, isStreaming);
  if (isStreaming) {
    colorWipe(strip.Color(255, 0, 0), 50); // Red
  } else {
    colorWipe(strip.Color(10, 10, 10), 50); // Off
  }
  Serial.println("disconnecting from server.");
  client.stop();

  delay(REFRESH*1000);
}
