#include <WiFi.h>

const char *ssid = "Hyperoptic Fibre A2C0";
const char *password = "UNz2PhQXy36YG9";
int ledPin1 = 2; // Pin for LED if number < 5
int ledPin2 = 3; // Pin for LED if number > 10

WiFiServer server(80);

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

  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    Serial.println("New client connected");
    while (client.connected()) {
      if (client.available()) {
        int receivedNum = client.parseInt();
        Serial.print("Number received: ");
        Serial.println(receivedNum);

        if (receivedNum < 5) {
          Serial.print("Number received less than 5");
          digitalWrite(ledPin1, HIGH);
          //delay(1000); // Wait for 1 second
          digitalWrite(ledPin1, LOW); // Turn OFF the LED
        } else if (receivedNum > 10) {
        Serial.print("Number received more than 5");
        digitalWrite(ledPin1, LOW); // Turn OFF the LED
        }
      }
    }
    client.stop();
    Serial.println("Client disconnected");
  }
}




/*
 Created by Rui Santos
 
 All the resources for this project:
 http://randomnerdtutorials.com/


#include "LedControl.h"
#include "binary.h"


LedControl lc=LedControl(7,5,6,1);

// delay time between faces
unsigned long delaytime=1000;

// HORIZONTAL

// edges

byte leftedge[8]= {B11111111,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000};
byte rightedge[8]= {B11111111,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000};
byte topedge[8]= {B10000000,B10000000,B10000000,B10000000,B10000000,B10000000,B10000000,B10000000};
byte bottomedge[8] = 



byte hf[8]= {B11111111,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B11111111};
// 
byte nf[8]={B00000000, B11111111,B00000000,B00000000,B00000000,B00000000,B11111111,B00000000};
// 
byte sf[8]= {B00000000,B00000000,B11111111,B00000000,B00000000,B11111111,B00000000,B00000000};
// centre
byte lf[8]= {B00000000,B00000000,B00000000,B11111111,B11111111,B00000000, B00000000,B00000000};


// VERTICAL 
//edges
byte af[8]= {B10000001,B10000001,B10000001,B10000001,B10000001,B10000001,B10000001,B10000001};
// 
byte bf[8]={B01000010, B01000010,B01000010,B01000010,B01000010,B01000010,B01000010,B01000010};
// 
byte cf[8]= {B00100100,B00100100,B00100100,B00100100,B00100100,B00100100,B00100100,B00100100};
// centre
byte df[8]= {B00011000,B00011000,B00011000,B00011000,B00011000,B00011000, B00011000,B00011000};




void setup() {
  lc.shutdown(0,false);
  // Set brightness to a medium value
  lc.setIntensity(0,8);
  // Clear the display
  lc.clearDisplay(0);  
  Serial.begin(9600);
}

void illuminate(){

// VERTICAL EDGE
if (receivedNum = 1) {
          // Display happy face
  lc.setRow(0,0,hf[0]);
  lc.setRow(0,1,hf[1]);
  lc.setRow(0,2,hf[2]);
  lc.setRow(0,3,hf[3]);
  lc.setRow(0,4,hf[4]);
  lc.setRow(0,5,hf[5]);
  lc.setRow(0,6,hf[6]);
  lc.setRow(0,7,hf[7]);
  delay(delaytime);
  }


  // Display neutral face
  lc.setRow(0,0,nf[0]);
  lc.setRow(0,1,nf[1]);
  lc.setRow(0,2,nf[2]);
  lc.setRow(0,3,nf[3]);
  lc.setRow(0,4,nf[4]);
  lc.setRow(0,5,nf[5]);
  lc.setRow(0,6,nf[6]);
  lc.setRow(0,7,nf[7]);
  delay(delaytime);

  // Display sad face
  lc.setRow(0,0,sf[0]);
  lc.setRow(0,1,sf[1]);
  lc.setRow(0,2,sf[2]);
  lc.setRow(0,3,sf[3]);
  lc.setRow(0,4,sf[4]);
  lc.setRow(0,5,sf[5]);
  lc.setRow(0,6,sf[6]);
  lc.setRow(0,7,sf[7]);
  delay(delaytime);


  // Display happy face
  lc.setRow(0,0,lf[0]);
  lc.setRow(0,1,lf[1]);
  lc.setRow(0,2,lf[2]);
  lc.setRow(0,3,lf[3]);
  lc.setRow(0,4,lf[4]);
  lc.setRow(0,5,lf[5]);
  lc.setRow(0,6,lf[6]);
  lc.setRow(0,7,lf[7]);
  delay(delaytime);

   // Display happy face
  lc.setRow(0,0,af[0]);
  lc.setRow(0,1,af[1]);
  lc.setRow(0,2,af[2]);
  lc.setRow(0,3,af[3]);
  lc.setRow(0,4,af[4]);
  lc.setRow(0,5,af[5]);
  lc.setRow(0,6,af[6]);
  lc.setRow(0,7,af[7]);
  delay(delaytime);

   // Display happy face
  lc.setRow(0,0,bf[0]);
  lc.setRow(0,1,bf[1]);
  lc.setRow(0,2,bf[2]);
  lc.setRow(0,3,bf[3]);
  lc.setRow(0,4,bf[4]);
  lc.setRow(0,5,bf[5]);
  lc.setRow(0,6,bf[6]);
  lc.setRow(0,7,bf[7]);
  delay(delaytime);

   // Display happy face
  lc.setRow(0,0,cf[0]);
  lc.setRow(0,1,cf[1]);
  lc.setRow(0,2,cf[2]);
  lc.setRow(0,3,cf[3]);
  lc.setRow(0,4,cf[4]);
  lc.setRow(0,5,cf[5]);
  lc.setRow(0,6,cf[6]);
  lc.setRow(0,7,cf[7]);
  delay(delaytime);

    // Display happy face
  lc.setRow(0,0,df[0]);
  lc.setRow(0,1,df[1]);
  lc.setRow(0,2,df[2]);
  lc.setRow(0,3,df[3]);
  lc.setRow(0,4,df[4]);
  lc.setRow(0,5,df[5]);
  lc.setRow(0,6,df[6]);
  lc.setRow(0,7,df[7]);
  delay(delaytime);
}

void loop(){
  drawFaces();
  Serial.print("print");
}
*/ 

