

// ************************************************************
//            LTM TELEMETRY OLED READER
//
// Simple LTM Telemetry parser and displayer for OLED 128x64 SSD1306
// Based in code from Kipk (Getthostation)and MWOSD. Thanks to Digital Entity, DIvkvu, Bart from EZGUI and all the people
// working and testing  INAV.
// Mainly used for INAV flight controllers
// LTM telemetry is designed for low baudrates, only downlink. Its best goal is
// the aircraft is sending its GPS position (and other data) continuosly, so it is also
// your 'GPS beacon' to find your lost plane.
// This is intended only for OLED telemetry, althought can be resized to be a ground OSD or antennatracker.
//
//
// sppnk 2017
//
//
// ********************************************************
// NOTES:
//
//
// You can mount this project inside your transmitter, or throught a serial bluetooth connection (easier). On first way
// EZGUI can be used at the same time with BT connection.

// Pushbutton  to change screens .
//
// if you use bluetooth, HC-05 must be set to automatic pairing with the transmitter BT module.
// Connect BT module tx pin to pin8 in arduino
// if you make the mod in your transmitter instead serial Bluetooth connection, you can connect directly you TX line to pin8
// be carefull to supply your Arduino through BEC (mine takes BAT+ from Internal pinout for external module)
//
//
//
// TODO:
// - add buzzer to create vario
// - add buzzer to create distance, altitude, battery or RSSI audible alarms
// - add arrow graphic for bearing home instead the numerical value, (to fly by instruments if video is lost).
// - add flight timer
// - add flight report (max distance, minimum rssi, max altitude).
//
//

#ifdef membug
#include <MemoryFree.h>
#endif

//#include <Wire.h>                           //if OLED is CO-16, short pins 13-14 to use this.
//#include <Adafruit_GFX.h>                 //not used in this code
//#include <Adafruit_SSD1306.h>               // memory consumer library. fixme



// #define OLED_RESET 4
// Adafruit_SSD1306 display(OLED_RESET);

//#if (SSD1306_LCDHEIGHT != 64)
//#error("Height incorrect, please fix Adafruit_SSD1306.h!");
//#endif

#define PROTOCOL_LIGHTTELEMETRY
#define BUTTON        4
#define OLED_PANELS   6
#define LTM_BAUDS     9600                       // the lower the better.
#define AVERAGE_ITERATIONS  10

//#include <SoftwareSerial.h>                   //this Software serial library gives many problmems
//SoftwareSerial ltmSerial(8, 9);               //8-RX, 9-TX
//Serial ltmserial;

#include <AltSoftSerial.h>              //  This library works fine
AltSoftSerial ltmSerial(8, 9);          //8-RX, 9-TX

#include <LightTelemetry.cpp>
#include <stdint.h>
#include <i2c_oled.cpp>

//some global variables
uint8_t     displaypage = 0;
uint8_t     button_read = LOW;
uint8_t     button_laststate = LOW;
uint8_t     button_state = LOW;
uint32_t    lastDebounceTime = 0;
uint32_t    debounceDelay = 80;



//////////////////////////////////////////functions

void read_button() { // detect the button has been pushed (debouncing) and increase displaypage counter

  button_read = digitalRead(BUTTON);
  //Serial.print(button_read);

  if (button_read != button_laststate)    //pressed button
  {
    lastDebounceTime = millis();          //reset debouncing timer
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {


    if (button_read != button_state)
    {
      button_state = button_read;

      if (button_state == LOW)
      {

        displaypage++;
        displaypage %= OLED_PANELS;

      }
    }

  }
  button_laststate = button_read;
}

///////////////////////  distance and direction between two points giving coordinates

void GPS_dist_bearing(int32_t* lat1, int32_t* lon1, int32_t* lat2, int32_t* lon2, uint32_t* dist, int32_t* bearing) {

  if (uav_homefixstatus == 1) {        //gps home fix is OK

    float rads = (abs((float) * lat2) / 10000000.0) * 0.0174532925; // latitude to radians
    double scaleLongDown = cos (rads); // calculate longitude scaling **taking lat2 ? why not lat1 ?

    float distLat = *lat2 - *lat1;                                          // difference of latitude in 1/10 000 000 degrees
    float distLon = (float)(*lon2 - *lon1) * scaleLongDown;
    //distance calculation
    *dist = sqrt(sq(distLat) + sq(distLon)) * 1.113195 / 100;          // distance between two GPS points in m
    //direction calculation
    *bearing = (int) round ( 90 + atan2(-distLat, distLon) * 57.2957795);      //bearing, convert the output radians to deg
    if (*bearing < 0) *bearing += 360;
  }
    else                         // we dont have a home fix
    {
      *dist = 0;
      *bearing = 0;
    }

  }




////////////////////////////////////// display OLED

void display_oled() { // display data set in OLED depending on displaypage var. Change at your needs.
  // I have found that textsize 2 causes problems and hangs antire device if the showed numbers are big.

  i2c_clear_OLED();

  for (byte n = 0; n<5 ;n++){

    uint8_t  str[n][] = {"                      "};

  }

  switch (displaypage) {

    case 0:                 //MAIN DATA SCREEN

         str [1] = "HOME" + home_distance;
         str [2] = "RSSI" + uav_rssi;
         str [1] = "ALT" + uav_alt;
         str [1] = "VBAT" + uav_bat ;
         str [1] = "mAh" + uav_amp;


      break;

    case 1:


      break;

    case 2:


    case 3:

      break;
    case 4:


      break;
    case 5:


      break;

    default:
      break;


  }

  for (byte n = 0; n<5 ;n++){  i2c_OLED_send_string(str[n];} //print whole screen

}

//void blinkled() { // led for debugging. removed to save memory
//
//  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
//  delay(100);                       // wait for a second
//  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
//  delay(100);
//
//}

/////////////////SETUP

void setup() {

  //Serial.begin(57600);                       // debug port arduino. removed as it is not used it in the final working device
  ltmSerial.begin(LTM_BAUDS);                 //telemetry downlink is 9600 now . PLease set your telemetry downlink so.
  //pinMode(LED_BUILTIN, OUTPUT);             //debug
  pinMode(BUTTON, INPUT);                     // digital input for pushbutton. Connnect to GND the other end of it.

  i2c_OLED_init();          //init i2c display oled
  i2c_clear_OLED();

  //display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  //CRIUS CO-16 soldered pins to use adafruit libraries
  //display.setTextColor(WHITE);
  //display.clearDisplay();
  //blinkled(); blinkled(); //debug

}

////////////////////////LOOP

void loop() {

  //while (ltmSerial.available()) {Serial.print(ltmSerial.read());}  //debug

  int32_t last_uav_lat = uav_lat; // last values of GPS for calculation of course over ground
  int32_t last_uav_lon = uav_lon; //this only works if uav is moving FIXME

  uint16_t ground_courseRaw = 0;
  static uint8_t sig = 0;

  ltm_read();           //read LTM telemetry

  // distance between two GPS points (last and actual) and course of aircraft
  GPS_dist_bearing(&last_uav_lat, &last_uav_lon, &uav_lat, &uav_lon, &ground_distance, &ground_course);
  // ground_course -= 180; //
  //   if (ground_course < 0){ ground_course += 360;}

  //aveaging ground_course value
  static uint16_t ground_courseRawArray[AVERAGE_ITERATIONS];
  ground_courseRawArray[(sig++) % AVERAGE_ITERATIONS] = ground_course;
  for (uint8_t i = 0; i < AVERAGE_ITERATIONS; i++) ground_courseRaw += ground_courseRawArray[i];
  ground_course = ground_courseRaw / AVERAGE_ITERATIONS;

  //TODO measure millis() just to experiment calculating SPEED and comparing with GPS speed from LTM
  GPS_dist_bearing(&uav_lat, &uav_lon, &uav_homelat, &uav_homelon, &home_distance, &home_heading);        // calculate some variables from LTM data


  read_button();        // check pushbutton and increase page counter

  display_oled();       // display data in oled screen



}
