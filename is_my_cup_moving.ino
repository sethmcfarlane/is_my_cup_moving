#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <Adafruit_NeoPixel.h>

  #define PIN 7
 
Adafruit_NeoPixel strip = Adafruit_NeoPixel(24, PIN);


#define BNO055_SAMPLERATE_DELAY_MS (100)

Adafruit_BNO055 bno = Adafruit_BNO055(55);

void displaySensorDetails(void)
{
  sensor_t sensor;
  bno.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" xxx");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" xxx");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" xxx");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

void displaySensorStatus(void)
{
 
  uint8_t system_status, self_test_results, system_error;
  system_status = self_test_results = system_error = 0;
  bno.getSystemStatus(&system_status, &self_test_results, &system_error);

 
  Serial.println("");
  Serial.print("System Status: 0x");
  Serial.println(system_status, HEX);
  Serial.print("Self Test:     0x");
  Serial.println(self_test_results, HEX);
  Serial.print("System Error:  0x");
  Serial.println(system_error, HEX);
  Serial.println("");
  delay(500);
}


void displayCalStatus(void)
{
  
  uint8_t system, gyro, accel, mag;
  system = gyro = accel = mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);

  
  Serial.print("\t");
  if (!system)
  {
    Serial.print("! ");
  }

  
  Serial.print("Sys:");
  Serial.print(system, DEC);
  Serial.print(" G:");
  Serial.print(gyro, DEC);
  Serial.print(" A:");
  Serial.print(accel, DEC);
  Serial.print(" M:");
  Serial.print(mag, DEC);
}

  unsigned long lastMoveTime = 0;
  int prevX = 0;
  int prevY = 0;
  int prevZ = 0;

void setup(void)
{
  Serial.begin(9600);
  Serial.println("Orientation Sensor Test"); Serial.println("");

  /* Initialise the sensor */
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }

  delay(1000);

  /* Display some basic information on this sensor */
  displaySensorDetails();

  /* Optional: Display current status */
  displaySensorStatus();

  bno.setExtCrystalUse(true);

  /*LED*/
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  strip.setBrightness(10);
}

void loop(void)
{
  /* Get a new sensor event */
  sensors_event_t event;
  bno.getEvent(&event);
  strip.show();

  /* Wait the specified delay before requesting nex data */
  delay(BNO055_SAMPLERATE_DELAY_MS);

  /*LED*/

  int curX = event.orientation.x;
  int curY = event.orientation.y;
  int curZ = event.orientation.z;

  int curSum = abs(curX) + abs(curY) + abs(curZ);
  int prevSum = abs(prevX) + abs(prevY) + abs(prevZ);

  if (prevSum - curSum > 1){
    lastMoveTime = millis();
    Serial.print("moving  ");
  }
  else {
  Serial.print("idle    ");
  }

  int yelVal = 0;
  int magVal = 0;
  
  if (lastMoveTime + 500 < millis()){
    
    for(int i = 0; i < 24; i += 2){ //idle
      strip.setPixelColor(i, 0, 0, 0);
    }
    for(int i = 1; i < 24; i += 2){
      strip.setPixelColor(i, 255, 0, 255); /*magenta*/
    }
  }
  else{

    for(int i = 1; i < 24; i += 1){
      strip.setPixelColor(i, 0, 0, 0);
    }
    for(int i = 0; i < 24; i += 2){
    strip.setPixelColor(i, 255, 255, 0); /*yellow*/
        }
  }
  Serial.print("  curSum: ");
  Serial.print(curSum);
  Serial.print("\tprevSum:  ");
  Serial.print(prevSum);
  Serial.println("");
  prevX = curX;
  prevY = curY;
  prevZ = curZ;
  
}
