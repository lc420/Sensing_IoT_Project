
///// LIBRARIES
#include <Wire.h>
#include <WiFi.h>

const char *ssid = "Hyperoptic Fibre A2C0";
const char *password = "UNz2PhQXy36YG9";
const char* host = "192.168.1.125";  // Slave's IP address


#define SAMPLING_FREQ_HZ    4                      // Sampling frequency (Hz)
#define SAMPLING_PERIOD_MS  1000 / SAMPLING_FREQ_HZ   // Sampling period (ms) 10 ms between samples
#define NUM_SAMPLES         300                       // 300 samples at 100 Hz is 3s in total

/////// VARIABLES
const int MPU = 0x68; // MPU6050 I2C address
float AccX, AccY, AccZ;
float GyroX, GyroY, GyroZ;
float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ;
float roll, pitch, yaw;
float AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ;
float elapsedTime, currentTime, previousTime;
int c = 0;
int error_number = 500; 
unsigned long initial_timestamp;
unsigned long final_timestamp;
unsigned long timestamp = 0;



int fsrPin = 32;    // select the input pin for the potentiometer
float force_value;

const int flexPin = 33; //pin A0 to read analog input
float flex; 

WiFiClient client;


void setup() {
  Serial.begin(115200);
  Wire.begin();                      // Initialize comunication
  Wire.beginTransmission(MPU);       // Start communication with MPU6050 // MPU=0x68
  Wire.write(0x6B);                  // Talk to the register 6B
  Wire.write(0x00);                  // Make reset - place a 0 into the 6B register
  Wire.endTransmission(true);        //end the transmission
  /*
  // Configure Accelerometer Sensitivity - Full Scale Range (default +/- 2g)
  Wire.beginTransmission(MPU);
  Wire.write(0x1C);                  //Talk to the ACCEL_CONFIG register (1C hex)
  Wire.write(0x10);                  //Set the register bits as 00010000 (+/- 8g full scale range)
  Wire.endTransmission(true);
  // Configure Gyro Sensitivity - Full Scale Range (default +/- 250deg/s)
  Wire.beginTransmission(MPU);
  Wire.write(0x1B);                   // Talk to the GYRO_CONFIG register (1B hex)
  Wire.write(0x10);                   // Set the register bits as 00010000 (1000deg/s full scale)
  Wire.endTransmission(true);
  delay(20);
  */
  // GET IMU ERROR VALUES
  /* 
  Serial.println("START");
  delay(5000);
  Serial.println("Errors being calculated...");
  delay(1000);
  IMU_error();
  delay(1000);
  Serial.println("Errors done!");
  */ 
  IMU_error();

  Serial.println();
  Serial.println("Connecting to WiFi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }

  Serial.println("Connected to WiFi");
  WiFi.setSleep(false);
}

///////////////          LOOP 


void loop() {
  // Connect to Slave
  if (!client.connect(host, 80)) {
  Serial.println("Connection failed");
  delay(5000);
  return;
  }

  timestamp = 0; 
  initial_timestamp = millis();
  Serial.println("timestamp,roll,pitch,yaw,force,flex");

  for (int i = 0; i < NUM_SAMPLES; i++) {
  IMU_calculations();
  force_sensor();
  flex_sensor();
  data_output();
  conditions(); 
  while (millis() < timestamp + SAMPLING_PERIOD_MS);
  final_timestamp = millis(); 
  timestamp = final_timestamp - initial_timestamp; 
  }
  Serial.println();
  timestamp = 0; 
  delay(100);
}

///////////////          FUNCTIONS 
void IMU_error() {
  // We can call this funtion in the setup section to calculate the accelerometer and gyro data error. From here we will get the error values used in the above equations printed on the Serial Monitor.
  // Note that we should place the IMU flat in order to get the proper values, so that we then can the correct values
  // Read accelerometer values 200 times
  while (c < error_number) {
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    AccX = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    AccY = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    // Sum all readings
    AccErrorX = AccErrorX + ((atan((AccY) / sqrt(pow((AccX), 2) + pow((AccZ), 2))) * 180 / PI));
    AccErrorY = AccErrorY + ((atan(-1 * (AccX) / sqrt(pow((AccY), 2) + pow((AccZ), 2))) * 180 / PI));
    c++;
  }

  //Divide the sum by 200 to get the error value
  AccErrorX = AccErrorX / error_number;
  AccErrorY = AccErrorY / error_number;
  c = 0;
  // Read gyro values 200 times
  while (c < error_number) {
    Wire.beginTransmission(MPU);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    GyroX = Wire.read() << 8 | Wire.read();
    GyroY = Wire.read() << 8 | Wire.read();
    GyroZ = Wire.read() << 8 | Wire.read();
    // Sum all readings
    GyroErrorX = GyroErrorX + (GyroX / 131.0);
    GyroErrorY = GyroErrorY + (GyroY / 131.0);
    GyroErrorZ = GyroErrorZ + (GyroZ / 131.0);
    c++;
  }
  //Divide the sum by 200 to get the error value
  GyroErrorX = GyroErrorX / error_number;
  GyroErrorY = GyroErrorY / error_number;
  GyroErrorZ = GyroErrorZ / error_number;
  // Print the error values on the Serial Monitor
  /*
  Serial.print("AccErrorX: ");
  Serial.println(AccErrorX);
  Serial.print("AccErrorY: ");
  Serial.println(AccErrorY);
  Serial.print("GyroErrorX: ");
  Serial.println(GyroErrorX);
  Serial.print("GyroErrorY: ");
  Serial.println(GyroErrorY);
  Serial.print("GyroErrorZ: ");
  Serial.println(GyroErrorZ);
  */ 
}

void IMU_calculations () {
  // === Read acceleromter data === //
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 6 registers total, each axis value is stored in 2 registers


  //For a range of +-2g, we need to divide the raw values by 16384, according to the datasheet
  AccX = (Wire.read() << 8 | Wire.read()) / 16384.0; // X-axis value
  AccY = (Wire.read() << 8 | Wire.read()) / 16384.0; // Y-axis value
  AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0; // Z-axis value
  // Calculating Roll and Pitch from the accelerometer data
  accAngleX = (atan(AccY / sqrt(pow(AccX, 2) + pow(AccZ, 2))) * 180 / PI) - AccErrorX; // AccErrorX ~(0.58) See the calculate_IMU_error()custom function for more details
  accAngleY = (atan(-1 * AccX / sqrt(pow(AccY, 2) + pow(AccZ, 2))) * 180 / PI) - AccErrorY; // AccErrorY ~(-1.58)


  // === Read gyroscope data === //
  previousTime = currentTime;        // Previous time is stored before the actual time read
  currentTime = millis();            // Current time actual time read
  elapsedTime = (currentTime - previousTime) / 1000; // Divide by 1000 to get seconds
  Wire.beginTransmission(MPU);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 4 registers total, each axis value is stored in 2 registers
  GyroX = (Wire.read() << 8 | Wire.read()) / 131.0; // For a 250deg/s range we have to divide first the raw value by 131.0, according to the datasheet
  GyroY = (Wire.read() << 8 | Wire.read()) / 131.0;
  GyroZ = (Wire.read() << 8 | Wire.read()) / 131.0;
  // Correct the outputs with the calculated error values
  GyroX = GyroX- GyroErrorX; // GyroErrorX ~(-0.56)
  GyroY = GyroY - GyroErrorY; // GyroErrorY ~(2)
  GyroZ = GyroZ - GyroErrorZ; // GyroErrorZ ~ (-0.8)
  // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by sendonds (s) to get the angle in degrees
  gyroAngleX = gyroAngleX + GyroX * elapsedTime; // deg/s * s = deg
  gyroAngleY = gyroAngleY + GyroY * elapsedTime;
  yaw =  yaw + GyroZ * elapsedTime;
  // Complementary filter - combine acceleromter and gyro angle values
  gyroAngleX = 0.90 * gyroAngleX + 0.1  * accAngleX;
  gyroAngleY = 0.90 * gyroAngleY + 0.1 * accAngleY;

  roll = gyroAngleX;
  pitch = gyroAngleY;

}

void force_sensor() {
  force_value = analogRead(fsrPin);  
  force_value = map(force_value, 0, 4095, 0, 100);//Map value 0-1023 to 0-255 (PWM)       
}

void flex_sensor (){
  flex = analogRead(flexPin);         //Read and save analog value from potentiometer      
  flex = map(flex, 0, 4095, 0, 100);//Map value 0-1023 to 0-255 (PWM)       
}

void data_output () { 
  // Print IMU values on the serial monitor
  //Serial.print("Roll: "); 
  Serial.print(timestamp);
  Serial.print(",");
  Serial.print(roll);
  Serial.print(",");
  //Serial.print("Pitch: "); 
  Serial.print(pitch);
  Serial.print(",");
  //Serial.print("Yaw: "); 
  Serial.print(yaw);
  Serial.print(",");
  Serial.print(force_value);
  Serial.print(",");
  Serial.print(flex); 
  Serial.println(); 
}

void conditions () { 
  if (flex > 50) { 
    Serial.println("Too far right");
  // Send data to Slave
  client.print(1);
  }

  if (force_value > 50) { 
    Serial.println("Too far forward");
  // Send data to Slave
  client.print(2);
  }

  if (roll < (-50) ) { 
    Serial.println("Too far back");
  // Send data to Slave
  client.print(3);
  }

}





