// This is the code of how I would have done live inference with the Arduino Library I generated from Edge Impulse. I tried many different ways but I couldn't upload it, but this file shows how it would have been deployed with a different hardware

///// LIBRARIES
#include <Wire.h>
#include <Pointe_Shoe_IoT_inferencing.h> // Edge Impulse Library

#define DEBUG               1                         // 1 to print out debugging info
#define DEBUG_NN            false                     // Print out EI debugging info
#define ANOMALY_THRESHOLD   0.3                       // Scores above this are an "anomaly"

#define SAMPLING_FREQ_HZ    100                        // Sampling frequency (Hz)
#define SAMPLING_PERIOD_MS  1000 / SAMPLING_FREQ_HZ   // Sampling period (ms) 10 ms between samples
#define NUM_SAMPLES         EI_CLASSIFIER_RAW_SAMPLE_COUNT           
#define READINGS_PER_SAMPLE EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME      

//Means: [1.106, -1.6244, -5.871, 7.6484, 51.4082]
//Std devs: [10.2377, 3.3107, 14.8991, 2.2189, 33.9747]
//Mins: [-52.73, -16.47, -59.45, 0.0, 0.0]
//Ranges: [78.62, 27.79, 78.07, 13.0, 98.0]

float mins[] = { 
  -52.73, -16.47, -59.45, 0.0, 0.0
};


float ranges[] = { 
  78.62, 27.79, 78.07, 13.0, 98.0
};

/////// VARIABLES
const uint8_t MPU=0x68; // MPU6050 I2C address
float AccX, AccY, AccZ;
float GyroX, GyroY, GyroZ;
float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ;
float roll, pitch, yaw;
float AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ;
float elapsedTime, currentTime, previousTime;
int c = 0;
int error_number = 500; 


// Force sensors
int fsrPin = 32;    // select the input pin for the potentiometer
float force_value;

const int flexPin = 33; //pin A0 to read analog input
float flex; 


void setup() {
  Serial.begin(115200);
  Wire.begin();                      // Initialize comunication
  Wire.beginTransmission(MPU);       // Start communication with MPU6050 // MPU=0x68
  Wire.write(0x6B);                  // Talk to the register 6B
  Wire.write(0x00);                  // Make reset - place a 0 into the 6B register
  Wire.endTransmission(true);        //end the transmission
  

  Serial.println("START");
  delay(5000);
  Serial.println("Errors being calculated...");
  delay(1000);
  IMU_error();
  delay(1000);
  Serial.println("Errors done!");
}



void loop() {
  unsigned long timestamp;
  static float raw_buf[NUM_SAMPLES * READINGS_PER_SAMPLE];
  static signal_t signal;
  float normal;
  int max_idx = 0;
  float max_val = 0.0;
  char str_buf[40];

  //Serial.println("timestamp,roll,pitch,yaw,force,flex");

  for (int i = 0; i < NUM_SAMPLES; i++) {
    
     timestamp = millis();

     IMU_calculations();
     force_sensor();
     flex_sensor();
     //data_output();

      raw_buf[(i * READINGS_PER_SAMPLE) + 0] = roll;
      raw_buf[(i * READINGS_PER_SAMPLE) + 1] = pitch;
      raw_buf[(i * READINGS_PER_SAMPLE) + 2] = yaw;
      raw_buf[(i * READINGS_PER_SAMPLE) + 3] = force_value;
      raw_buf[(i * READINGS_PER_SAMPLE) + 4] = flex;


      // Perform preprocessing step (normalization) on all readings in the sample
      for (int j = 0; j < READINGS_PER_SAMPLE; j++) {
        normal = raw_buf[(i * READINGS_PER_SAMPLE) + j] - mins[j];
        raw_buf[(i * READINGS_PER_SAMPLE) + j] = normal / ranges[j];
      }
     
     while (millis() < timestamp + SAMPLING_PERIOD_MS);
    }

          // Print out our preprocessed, raw buffer
      #if DEBUG
        for (int i = 0; i < NUM_SAMPLES * READINGS_PER_SAMPLE; i++) {
          Serial.print(raw_buf[i]);
          if (i < (NUM_SAMPLES * READINGS_PER_SAMPLE) - 1) {
            Serial.print(", ");
          }
        }
        Serial.println();
      #endif
    
      // Turn the raw buffer in a signal which we can the classify
      int err = numpy::signal_from_buffer(raw_buf, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);
      if (err != 0) {
          ei_printf("ERROR: Failed to create signal from buffer (%d)\r\n", err);
          return;
      }
    
      // Run inference
      ei_impulse_result_t result = {0};
      err = run_classifier(&signal, &result, DEBUG_NN);
      if (err != EI_IMPULSE_OK) {
          ei_printf("ERROR: Failed to run classifier (%d)\r\n", err);
          return;
      }
    
      // Print the predictions
      ei_printf("Predictions ");
      ei_printf("(DSP: %d ms., Classification: %d ms., Anomaly: %d ms.)\r\n",
            result.timing.dsp, result.timing.classification, result.timing.anomaly);
      for (int i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
        ei_printf("\t%s: %.3f\r\n", 
                  result.classification[i].label, 
                  result.classification[i].value);
      }
    
      // Print anomaly detection score
      #if EI_CLASSIFIER_HAS_ANOMALY == 1
      ei_printf("\tanomaly acore: %.3f\r\n", result.anomaly);
      #endif
    
      // Find maximum prediction
      for (int i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
        if (result.classification[i].value > max_val) {
          max_val = result.classification[i].value;
          max_idx = i;
        }
      }
      Serial.println("Completed loop");
      delay(1000);
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
    Wire.requestFrom(MPU_addr, (size_t) 6, true);
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
}

void IMU_calculations () {
  // === Read acceleromter data === //
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, (size_t) 6, true); // Read 6 registers total, each axis value is stored in 2 registers


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
  force_value = map(force_value, 0, 1023, 100, 0);//Map value 0-1023 to 0-255 (PWM)       
}

void flex_sensor (){
  flex = analogRead(flexPin);         //Read and save analog value from potentiometer      
  flex = map(flex, 0, 1023, 100, 0);//Map value 0-1023 to 0-255 (PWM)       
}

void data_output () { 
  // Print IMU values on the serial monitor
  //Serial.print("Roll: "); 
  //Serial.print(timestamp);
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
