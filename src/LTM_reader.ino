

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

#include <Wire.h>                           //if OLED is CO-16, short pins 13-14 to use this.
//#include <Adafruit_GFX.h>                 //not used in this code
#include <Adafruit_SSD1306.h>               // memory consumer library. fixme


#define OLED_RESET 4      //FIXME
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

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

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);

  switch (displaypage) {

    case 0:                 //MAIN DATA SCREEN
        display.setTextSize(1);display.print(F("H "));
        display.setTextSize(2);
        display.print(home_distance); display.print(F(" ")); display.println(home_heading - uav_heading); //calculated ground course
      // display.print(F("Hdr  "));  display.println(home_heading); // calculated home heading
      // display.print(F("Uhd  "));  display.println(uav_heading);  //ground course from LTM if there is no mag
        display.setTextSize(1);display.print(F("RSSI "));display.setTextSize(2);  display.print(uav_rssi);
        display.setTextSize(1);display.print(F(" ALT "));display.setTextSize(2);  display.println(uav_alt);
        display.setTextSize(1);display.print(F("VBAT "));  display.setTextSize(2);display.print(uav_bat / 100); //show Volts, not mV
        display.setTextSize(1);display.print(F(" mAh "));  display.println(uav_amp);display.println();
        display.setTextSize(1);display.println();
        display.print(F("Sat ")); display.print(uav_satellites_visible);
        display.print(F(" SPD "));  display.print(uav_groundspeed);display.print(F(" M "));  display.println(uav_flightmode);
      //       display.setTextSize(1);
      //
      //      display.print(F("HOM "));display.setTextSize(1); display.print(home_distance);   display.setTextSize(1);display.print(F(" DEG "));display.println(home_heading - ground_course);display.println();//FIXME uav_heading is attitude heading, not course over ground, so it is useless FIXME
      //      display.setTextSize(1);display.print(F("RSSI "));  display.setTextSize(2);display.print(uav_rssi); display.setTextSize(1);display.print(F(" ALT "));  display.setTextSize(2);display.println(uav_alt);
      //      display.setTextSize(1);display.print(F("VBAT "));  display.setTextSize(2);display.print(uav_bat / 100); display.setTextSize(1);display.print(F(" mAh "));  display.setTextSize(2);display.println(uav_amp);
      //           display.setTextSize(1);display.print(F("SATS "));  display.setTextSize(2);display.print(uav_satellites_visible); display.setTextSize(1);display.print(F(" MODE "));  display.setTextSize(2);display.println(uav_flightmode);

      break;

    case 1:

      display.setTextSize(2);
      display.print(F("RSSI:  "));  display.println(uav_rssi);
      display.print(F("ALT:   "));  display.println(uav_alt);
      display.print(F("VBATT: "));  display.println(uav_bat / 100); //show Volts, not mV
      display.print(F("mAh:   "));  display.println(uav_amp); //this seems to be Amph, not current
      display.setTextSize(1);
      display.print(F("OK: "));  display.println(LTM_pkt_ok);
      display.print(F(" KO: "));  display.println(LTM_pkt_ko);
      break;

    case 2:

      display.print(F("HDOP: "));      display.println(uav_HDOP);
      display.print(F("Lat : "));  display.println(uav_lat);
      display.print(F("Lon : "));  display.println(uav_lon);
      display.print(F("Home: ")); display.println(home_distance);
      display.print(F("HDir: ")); display.print(home_heading); display.print(F(" Udir: ")); display.println(uav_heading);
      display.print(F("HFix: ")); display.print(uav_homefixstatus); display.print(F(" AngH: ")); display.println(home_heading - ground_course);
      display.print(F("HLat: ")); display.println(uav_homelat);
      display.print(F("HLon: ")); display.println(uav_homelon);
      break;
    case 3:
      display.print(F("Sats: ")); display.print(uav_satellites_visible);
      display.print(F(" FS: "));  display.print(uav_failsafe);
      display.print(F(" FM: "));  display.println(uav_flightmode);
      display.print(F("ALT: "));  display.print(uav_alt); display.print(F(" SPD: "));  display.println(uav_groundspeed);
      display.print(F("R: "));  display.print(uav_roll);
      display.print(F(" P: "));  display.print(uav_pitch); display.print(F(" ARM: ")); display.println(uav_arm);
      display.print(F("Lat: "));  display.println(uav_lat);
      display.print(F("Lon: "));  display.println(uav_lon);
      display.setTextSize(2);
      display.print(F("RSSI: "));  display.println(uav_rssi);
      display.setTextSize(1);
      display.print(F("OK: "));  display.print(LTM_pkt_ok);
      display.print(F(" KO: "));  display.println(LTM_pkt_ko);
      break;
    case 4:

      display.setTextSize(1);

      display.print(F("HOM ")); display.setTextSize(1); display.print(home_distance);   display.setTextSize(1); display.print(F(" DEG ")); display.println(home_heading - ground_course);  //FIXME uav_heading is attitude heading, not course over ground, so it is useless FIXME
      display.print(F("COU ")); display.setTextSize(1); display.print(ground_course);   display.setTextSize(1); display.print(F(" HEAD ")); display.println(home_heading);
      display.setTextSize(1); display.print(F("RSSI "));  display.setTextSize(2); display.print(uav_rssi); display.setTextSize(1); display.print(F(" ALT "));  display.setTextSize(2); display.println(uav_alt);
      display.setTextSize(1); display.print(F("VBAT "));  display.setTextSize(2); display.print(uav_bat / 100); display.setTextSize(1); display.print(F(" mAh "));  display.setTextSize(1); display.println(uav_amp);    display.println();
      display.setTextSize(1); display.print(F("SATS "));  display.setTextSize(2); display.print(uav_satellites_visible); display.setTextSize(1); display.print(F(" MODE "));  display.setTextSize(2); display.println(uav_flightmode);


      //      display.print(F("Gmode: ")); display.println(uav_gpsmode);
      //      display.print(F("Nmod:  ")); display.println(uav_navmode);
      //      display.print(F("Nact:  "));  display.println(uav_navaction);
      //      display.print(F("WPnr:  "));  display.println(uav_WPnumber);

      break;
    case 5:

      display.setTextSize(2);

      display.print(F("FS:  ")); display.println(uav_failsafe);
      display.print(F("ARM: ")); display.println(uav_arm);
      display.print(F("Err: "));  display.println(ltm_naverror);
      display.print(F("Flg: "));  display.println(ltm_flags);

      break;

    default:
      break;


  }

  display.display();

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

  //Serial.begin(57600);              // debug port arduino. removed as it is not used it in the final working device
  ltmSerial.begin(LTM_BAUDS);         //telemetry downlink is 9600 now . PLease set your telemetry downlink so.
  //pinMode(LED_BUILTIN, OUTPUT); //debug
  pinMode(BUTTON, INPUT_PULLUP);                     // digital input for pushbutton. Connnect to GND the other end of it.
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  //CRIUS CO-16 soldered pins to use adafruit libraries
  display.setTextColor(WHITE);
  display.clearDisplay();
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
