#if defined(ESP32)
  #include <WiFiMulti.h>
  WiFiMulti wifiMulti;
  #define DEVICE "ESP32"
#elif defined(ESP8266)
  #include <ESP8266WiFiMulti.h>
  ESP8266WiFiMulti wifiMulti;
  #define DEVICE "ESP8266"
#endif

#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

// WiFi AP SSID
#define WIFI_SSID "Hyperoptic Fibre A2C0"
// WiFi password
#define WIFI_PASSWORD "UNz2PhQXy36YG9"

#define INFLUXDB_URL "https://eu-central-1-1.aws.cloud2.influxdata.com"
#define INFLUXDB_TOKEN "n3j4dkSPVLsNKoHw_akmxKzpzmAwLbUqftzMnBmW5ifg2jCgQPddO3AvQYMZxXBWUDPnnON58PqKgeUX2KCQrQ=="
#define INFLUXDB_ORG "390dfbd4553f02bd"
#define INFLUXDB_BUCKET "Gyroscope Data"

#define INFLUXDB_CERT "-----BEGIN CERTIFICATE-----\nYourCertificateHere\n-----END CERTIFICATE-----"

// Time zone info
#define TZ_INFO "UTC0"

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, INFLUXDB_CERT);
Point sensor("gyro_status");

void setup() {
  // Initialize Serial
  Serial.begin(115200);

  // Initialize MPU6050
  Wire.begin();
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");
  
  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Other initializations if needed
}

void loop() {
  // Read sensor data
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Prepare InfluxDB data point
  sensor.clearFields();
  sensor.addField("accelX", a.acceleration.x);
  sensor.addField("accelY", a.acceleration.y);
  sensor.addField("accelZ", a.acceleration.z);
  sensor.addField("gyroX", g.gyro.x);
  sensor.addField("gyroY", g.gyro.y);
  sensor.addField("gyroZ", g.gyro.z);

  // Send data to InfluxDB
  if (client.writePoint(sensor)) {
    Serial.println("Data sent to InfluxDB");
  } else {
    Serial.println("Failed to send data to InfluxDB");
  }

  delay(100); // Adjust the delay as per your sampling requirements
}

















/* old
#if defined(ESP32)
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#define DEVICE "ESP32"
#elif defined(ESP8266)
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#define DEVICE "ESP8266"
#endif

#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

// WiFi AP SSID
#define WIFI_SSID "Hyperoptic Fibre A2C0"
// WiFi password
#define WIFI_PASSWORD "UNz2PhQXy36YG9"

#define INFLUXDB_URL "https://eu-central-1-1.aws.cloud2.influxdata.com"
#define INFLUXDB_TOKEN "n3j4dkSPVLsNKoHw_akmxKzpzmAwLbUqftzMnBmW5ifg2jCgQPddO3AvQYMZxXBWUDPnnON58PqKgeUX2KCQrQ=="
#define INFLUXDB_ORG "390dfbd4553f02bd"
#define INFLUXDB_BUCKET "Gyroscope Data"

// Time zone info
#define TZ_INFO "UTC0"

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);
Point sensor("gyro_status");

void setup() {
  // Initialize Serial
  Serial.begin(115200);

  // Initialize MPU6050
  Wire.begin();
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");
  
  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Other initializations if needed
}

void loop() {
  // Read sensor data
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Prepare InfluxDB data point
  sensor.clearFields();
  sensor.addField("accelX", a.acceleration.x);
  sensor.addField("accelY", a.acceleration.y);
  sensor.addField("accelZ", a.acceleration.z);
  sensor.addField("gyroX", g.gyro.x);
  sensor.addField("gyroY", g.gyro.y);
  sensor.addField("gyroZ", g.gyro.z);

  // Send data to InfluxDB
  if (client.writePoint(sensor)) {
    Serial.println("Data sent to InfluxDB");
  } else {
    Serial.println("Failed to send data to InfluxDB");
  }

  delay(100); // Adjust the delay as per your sampling requirements
}

*/ 