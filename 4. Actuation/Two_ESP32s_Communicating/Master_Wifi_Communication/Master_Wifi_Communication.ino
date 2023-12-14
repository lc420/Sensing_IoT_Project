//Libraries
#include <WiFi.h>

// Removed my wifi credentials
const char *ssid = " ";
const char *password = " ";
const char* host = "192.168.1.125";  // Slave's IP address

void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.println();
  Serial.println("Connecting to WiFi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }

  Serial.println("Connected to WiFi");
}

void loop() {
  // Connect to Slave
  WiFiClient client;
  if (!client.connect(host, 80)) {
    Serial.println("Connection failed");
    delay(5000);
    return;
  }

  // Generate random number between 0 and 10
  int numToSend = random(0, 6);
  Serial.print("Sending number to Slave: ");
  Serial.println(numToSend);

  // Send data to Slave
  client.print(numToSend);

  delay(1000);  // Wait before sending the next number
}
