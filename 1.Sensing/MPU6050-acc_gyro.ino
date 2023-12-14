//combining all sensors

///////////////          INITIALISE         /////////////// 

///// LIBRARIES
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

// Removed my wifi credentials
//const char *ssid = " ";
//const char *password = " ";


#define SAMPLING_FREQ_HZ    100                       // Sampling frequency (Hz)
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
float acc_x, acc_y, acc_z, gyro_x, gyro_y, gyro_z;
int c = 0;
int error_number = 500; 
unsigned long initial_timestamp;
unsigned long final_timestamp;
unsigned long timestamp = 0;


int fsrPin = 32;    // select the input pin for the potentiometer
float force_value;

const int flexPin = 33; //pin A0 to read analog input
float flex; 

int samples = 0;


void setup() {
  Serial.begin(115200);

  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  //Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    //Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  //Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  //Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    //Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    //Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    //Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    //Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  //Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    //Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    //Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    //Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    //Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_260_HZ);
  //Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    //Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    //Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    //Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    //Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    //Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    //Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    //Serial.println("5 Hz");
    break;
  }

  //Serial.println("");
  delay(100);
  IMU_error();
  Serial.println("timestamp,aX,aY,aZ,gX,gY,gZ,force,flex");

}

///////////////          LOOP 


void loop() {
  samples = 0;
  //while (samples < NUM_SAMPLES) {
  //Serial.println("timestamp,roll, pitch, yaw, force, flex");

  while (samples < NUM_SAMPLES) {

  //IMU_calculations();
  IMU_library(); 
  force_sensor();
  flex_sensor();
  data_output();
  samples++;
    //if (samples == NUM_SAMPLES)
      //Serial.println();
  //}
  }
}

///////////////          FUNCTIONS 
void IMU_error() {
  float totX, totY, totZ, totXX, totYY, totZZ;
  float number_of_base_samples; 
  sensors_event_t a, g, temp;
  
  for (int i = 0; i < number_of_base_samples; i++) {
    mpu.getEvent(&a, &g, &temp);
    totX = totX + a.acceleration.x;
    totY = totY + a.acceleration.y;
    totZ = totZ + a.acceleration.z;
    totXX = totXX + g.gyro.x; 
    totYY = totYY + g.gyro.y; 
    totZZ = totZZ + g.gyro.z; 


  AccErrorX = totX / number_of_base_samples;
  AccErrorY = totY / number_of_base_samples;
  GyroErrorX = totXX / number_of_base_samples;
  GyroErrorY = totYY / number_of_base_samples;
  GyroErrorZ = totZZ / number_of_base_samples;
  }


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

void IMU_library () {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  acc_x = a.acceleration.x - AccErrorX; 
  acc_y = a.acceleration.y - AccErrorY; 
  acc_z = a.acceleration.z; 
  gyro_x = g.gyro.x - GyroErrorX; 
  gyro_y = g.gyro.y - GyroErrorY; 
  gyro_z = g.gyro.z - GyroErrorZ; 
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
  Serial.print(samples);
  Serial.print(",");
  Serial.print(acc_x,3);
  Serial.print(",");
  //Serial.print("Pitch: "); 
  Serial.print(acc_y,3);
  Serial.print(",");
  //Serial.print("Yaw: "); 
  Serial.print(acc_z,3);
  Serial.print(",");
    Serial.print(gyro_x,3);
  Serial.print(",");
  //Serial.print("Pitch: "); 
  Serial.print(gyro_y,3);
  Serial.print(",");
  //Serial.print("Yaw: "); 
  Serial.print(gyro_z,3);
  Serial.print(",");
  Serial.print(force_value,3);
  Serial.print(",");
  Serial.print(flex,3); 
  Serial.println(); 
}

void old_sample_collection () { 
  timestamp = 0; 
  initial_timestamp = millis();
  //Serial.println("timestamp,roll, pitch, yaw, force, flex");

  for (int i = 0; i < NUM_SAMPLES; i++) {

  //IMU_calculations();
  IMU_library(); 
  force_sensor();
  flex_sensor();
  data_output();
  while (millis() < timestamp + SAMPLING_PERIOD_MS);
  final_timestamp = millis(); 
  timestamp = final_timestamp - initial_timestamp; 
  }
  //Serial.println();
  delay(1000); // delay between the samples being collected 
  timestamp = 0;
}




