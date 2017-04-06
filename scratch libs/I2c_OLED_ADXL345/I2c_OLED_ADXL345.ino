//
// i2c probes with several arduinos, an OLED CRIUS CO-16 display 
// and a ADXL345 acc device.
// 
// first probes indicates addresses:
// ADXL345 0x53
// Arduino 0x09 (as programmed as slave- just Wire.begin(0x09);)
// Arduino 0x67 (as programmed as slave- just Wire.begin(0x67);)
// OLED 0x3c (no answer because SDO is not connected to SDI in this
// device)
// NOTE: Wire.h doesnt work with this OLED,so we use other i2c funtions
// extracted from MultiWii software. Pending to test other i2c libraries

//
// next: get X,Y,Z raw values and send to OLED
//
// next send data from arduino Nano slave to UNO Master




#include <i2c_functions>       // susbtitute to Wire.h -from MultiWii
#include <OLED_functions.ino> //direct access OLED i2c - from MultiWii


/*
#define ADXL345_REGISTER_XLSB (0x32)
 #define ADXL345_REG_DATAFORMAT (0x31)
 #define ADXL345_ADDRESS (0x53)
 //Need to set power control bit to wake up the adxl345
 #define ADXL_REGISTER_PWRCTL (0x2D)
 #define ADXL_PWRCTL_MEASURE (1<<3)
 
 //init ADXL345 accelerometer
 //With these definitions in place, we initialize the ADXL345 on startup:
 
 void init_adxl345() {
 byte data = 0;
 
 i2c_write(ADXL345_ADDRESS, ADXL345_REG_DATAFORMAT, 0x01);
 i2c_write(ADXL345_ADDRESS, ADXL_REGISTER_PWRCTL, ADXL_PWRCTL_MEASURE);
 //Check to see if it worked!
 i2c_read(ADXL345_ADDRESS, ADXL_REGISTER_PWRCTL, 1, &data);
 Serial.println((unsigned int)data);
 }
 */

int val=0;

void setup()
{
Serial.begin(9600);
  i2c_OLED_init();
  i2c_clear_OLED(); //init OLED

    //init ADXL345
  //init_adxl345();

}


void loop()
{
  delay(1000);           // wait 8 seconds for next scan
  char *Xmenu[] = {"ONE", "DUE", "TREE","ONE", "DUE", "TREE","ONE", "DUE"};
  int Xdata[] = { 145,2123,334,423,524,6234, 2,2  };

  // init_adxl345(); 

  //i2c_read(ADXL345_ADDRESS, 0x32, 6, Xdata);

  displayvalues (*Xmenu, Xdata, 5); //pass array 
Serial.println(*Xmenu[0]);

}


void displayvalues (char argmenu[], int argdata[], int row){

  for (int row = 0;   row <8;   row++){

    char strval[] = "                  ";
    i2c_OLED_set_XY(0,row); //set cursor at X,Y
    sprintf (strval, "%i : %i", argmenu[row], argdata[row]);
   // Serial.println(argmenu[row]);
    i2c_OLED_send_string(strval);
  };

}


/*
//display 5 values in OLED 128x64
 void displayvalues (int pot, int pos, int sensor, int out, int pset){
 
 int startime =millis(); //check duration of this function
 
 char strval[] = "0000";
 
 
 itoa(pot, strval,10); //set int to string
 i2c_OLED_set_XY(0,0); //set cursor at X,Y
 i2c_OLED_send_string("VAL POT: ");
 i2c_OLED_send_string(strval);
 i2c_OLED_send_string(" ");
 
 itoa(pos, strval, 10);
 i2c_OLED_set_XY(0,1);
 i2c_OLED_send_string("VAL LDR: ");
 i2c_OLED_send_string(strval);
 i2c_OLED_send_string(" ");
 
 itoa(sensor, strval,10); //set int to string
 i2c_OLED_set_XY(0,2);
 i2c_OLED_send_string("VAL SEN: ");
 i2c_OLED_send_string(strval);
 i2c_OLED_send_string(" ");
 
 itoa(out, strval,10); //set int to string
 i2c_OLED_set_XY(0,3);
 i2c_OLED_send_string("VAL OUT: ");
 i2c_OLED_send_string(strval);
 i2c_OLED_send_string(" ");
 
 
 itoa(pset, strval,10); //set int to string
 i2c_OLED_set_XY(0,4);
 i2c_OLED_send_string("VAL SET: ");
 i2c_OLED_send_string(strval);
 i2c_OLED_send_string(" ");
 
 int lapse = millis() - startime;
 
 itoa(lapse, strval,10); //set int to string
 i2c_OLED_set_XY(0,5);
 i2c_OLED_send_string("MILLIS: ");
 i2c_OLED_send_string(strval);
 i2c_OLED_send_string(" ");
 
 
 
 }
 
 */







