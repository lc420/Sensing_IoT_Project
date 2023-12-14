#include <WiFi.h>

// Removed wifi credentials 
const char *ssid = " ";
const char *password = " ";


WiFiServer server(80);

#include <MaxMatrix.h>

int DIN = 23;   // DIN pin of MAX7219 module
int CLK = 18;   // CLK pin of MAX7219 module
int CS = 5;    // CS pin of MAX7219 module
int maxInUse = 1;


MaxMatrix m(DIN, CS, CLK, maxInUse);

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

  server.begin();
  Serial.println("Server started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  WiFi.setSleep(false);


  m.init(); // MAX7219 initialization
  m.setIntensity(8); // initial led matrix intensity, 0-15

  WiFi.setSleep(false); 
}

/* old loop 
void loop() {
  WiFiClient client = server.available();

  if (client) {
    Serial.println("New client connected");
    while (client.connected()) {
      if (client.available()) {
        int receivedNum = client.parseInt();
        Serial.print("Number received: ");
        Serial.println(receivedNum);

        if (receivedNum == 2) {
          Serial.println("Lights: 2 = right");
          m.clear();
          for (int i=0; i<8; i++){ // RIGHT COLUMN
          m.setDot(0,i,true);
          }
          //delay(500);
        } 

        if (receivedNum == 1) {
          Serial.println("Lights: 1 = Left");

          m.clear();
          for (int i=0; i<8; i++){ // LEFT COLUMN 
          m.setDot(7,i,true);
          }
          //delay(500);
        } 

         if (receivedNum == 3) {
          Serial.println("Lights: 3 = Forward");
          m.clear();
          for (int i=0; i<8; i++){ // TOP ROW
          m.setDot(i,7,true);
          }
          //delay(500);
        } 

         if (receivedNum == 4) {
          Serial.println("Lights: 4 = Backward");
          m.clear();
          for (int i=0; i<8; i++){ // TOP ROW
          m.setDot(i,0,true);
          }
          delay(500);
        } 
        
        if (receivedNum == 5) {
          //delay(100);
          m.clear();
          Serial.println("Default");
          m.setDot(3,3,true);
          m.setDot(4,4,true);
          m.setDot(3,4,true); 
          m.setDot(4,3,true);
          //delay(1000);
        }
      }
    }
    //client.stop();
    //Serial.println("Client disconnected");
  }
}
*/ 

// new loop
void loop() {
  WiFiClient client = server.available();

  if (client) {
    Serial.println("New client connected");
    bool clientConnected = true;

    while (clientConnected) {
      if (client.available()) {
        int receivedNum = client.parseInt();
        Serial.print("Number received: ");
        Serial.println(receivedNum);

        if (receivedNum == 2) {
          // Lights: 2 = right
          Serial.println("Lights: 2 = right");
          m.clear();
          for (int i = 0; i < 8; i++) { // RIGHT COLUMN
            m.setDot(0, i, true);
          }
        } 

        if (receivedNum == 1) {
          Serial.println("Lights: 1 = Left");

          m.clear();
          for (int i=0; i<8; i++){ // LEFT COLUMN 
          m.setDot(7,i,true);
          }
          //delay(500);
        } 

         if (receivedNum == 3) {
          Serial.println("Lights: 3 = Forward");
          m.clear();
          for (int i=0; i<8; i++){ // TOP ROW
          m.setDot(i,7,true);
          }
          //delay(500);
        } 

         if (receivedNum == 4) {
          Serial.println("Lights: 4 = Backward");
          m.clear();
          for (int i=0; i<8; i++){ // TOP ROW
          m.setDot(i,0,true);
          }
          delay(500);
        } 
        
        if (receivedNum == 5) {
          //delay(100);
          m.clear();
          Serial.println("Default");
          m.setDot(3,3,true);
          m.setDot(4,4,true);
          m.setDot(3,4,true); 
          m.setDot(4,3,true);
          //delay(1000);
        // Similarly handle other receivedNum cases...
        } 
        // Read the complete request before closing the client connection
        while (client.available()) {
          client.read();
        }
        clientConnected = false; // Close the client connection
      }
    }
    client.stop();
    Serial.println("Client disconnected");
  }
}
