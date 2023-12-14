/*
      8x8 LED Matrix MAX7219 Example 01

   by Dejan Nedelkovski, www.HowToMechatronics.com

   Based on the following library:
   GitHub | riyas-org/max7219  https://github.com/riyas-org/max7219
*/

#include <MaxMatrix.h>

int DIN = 23;   // DIN pin of MAX7219 module
int CLK = 18;   // CLK pin of MAX7219 module
int CS = 5;    // CS pin of MAX7219 module
int maxInUse = 1;


MaxMatrix m(DIN, CS, CLK, maxInUse); 

void setup() {
  m.init(); // MAX7219 initialization
  m.setIntensity(8); // initial led matrix intensity, 0-15
}

void loop() {
  /* 
  // Seting the LEDs On or Off at x,y or row,column position
  m.setDot(1,1,true);
  m.setDot(1,2,true); 
  m.setDot(1,3,true);
  delay(1000);
  m.setDot(1,1,true);
  m.setDot(2,2,true); 
  m.setDot(3,3,true);
  delay(1000);
  m.clear(); // Clears the display
  */ 
  // IF YOU HAVE THE CABLES FACING UP
  for (int i=0; i<8; i++){ // RIGHT COLUMN
    m.setDot(0,i,true);
    //delay(100); 
  }
  delay(1000);
  m.clear();

  m.setDot(3,3,true);
  m.setDot(4,4,true);
  m.setDot(3,4,true); 
  m.setDot(4,3,true);
  delay(3000);
  m.clear();


  for (int i=0; i<8; i++){ // BOTTOM ROW 
    m.setDot(i,0,true);
    //delay(100); 
  }
  delay(1000);
  m.clear();

  m.setDot(3,3,true);
  m.setDot(4,4,true);
  m.setDot(3,4,true); 
  m.setDot(4,3,true);
  delay(3000);
  m.clear();

  for (int i=0; i<8; i++){ // LEFT COLUMN 
      m.setDot(7,i,true);
      //delay(100);
  }
  delay(1000);
  m.clear();

  m.setDot(3,3,true);
  m.setDot(4,4,true);
  m.setDot(3,4,true); 
  m.setDot(4,3,true);
  delay(1000);
  m.clear();
  
  for (int i=0; i<8; i++){ // TOP ROW
      m.setDot(i,7,true);
      //delay(100); 
  }
  delay(1000);
  m.clear();

  m.setDot(3,3,true);
  m.setDot(4,4,true);
  m.setDot(3,4,true); 
  m.setDot(4,3,true);
  delay(1000);
  m.clear();
} 