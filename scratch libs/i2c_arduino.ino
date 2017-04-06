//==========================================================//
//-------------------------WWW.WIDE.HK----------------------//
//-----i2c OLED example-------------------------------------//
//-----Function : show 128x64 Graphic , Word and animation--//
//-----SCL = A5  ,  SDA = A4 , VCC = 3.3V-5V , GND----------//
//==========================================================//


#include <Wire.h>
//---------------FONT + GRAPHIC-----------------------------//
#include <M:\i2c OLED\data.c>
//==========================================================//


#define OLED_address  0x3c

unsigned char fill_OLED=0x55;
unsigned char fill_string1[]="WWW.WIDE.HK";
unsigned char fill_string2[]="OLED 128*64";
unsigned char fill_string3[]="0123456789ABCDEF";
unsigned char fill_string4[]="Arduino 328";
extern unsigned char myFont[][8];
extern unsigned char logo[];
extern unsigned char ip[];

//==========================================================//
void print_a_char(unsigned char ascii=0)
{
  unsigned char i=0;
  for(i=0;i<8;i++)
  {
    SendChar(myFont[ascii-0x20][i]);
  }
}

//==========================================================//
void setup()
{

  Wire.begin();
  init_OLED();
  delay(10);
  clear_display();
  delay(50);
}

//==========================================================//
void loop()
{
  clear_display();
  delay(50);

  sendcommand(0x20);            //Set Memory Addressing Mode
  sendcommand(0x02);            //Set Memory Addressing Mode ab Page addressing mode(RESET)  

  sendcommand(0xa6);            //Set Normal Display (default)
  //====================SHOW FONTS ARRAY[1-4]=======================//
  setXY(2,3);
  sendStr(fill_string1);
  setXY(3,3);
  sendStr(fill_string2);
  setXY(4,3);
  sendStr(fill_string3);
  setXY(5,3);
  sendStr(fill_string4);

  delay(2000);
  sendcommand(0xa7);  //Set Inverse Display  
  delay(2000);

  clear_display();
  delay(50);  

  sendcommand(0xa6);  //Set Normal Display

  sendcommand(0xae);		//display off
  sendcommand(0x20);            //Set Memory Addressing Mode
  sendcommand(0x00);            //Set Memory Addressing Mode ab Horizontal addressing mode


//==========================================================//
  for(int i=0;i<128*8;i++)     // show 128* 64 picture
  {
    SendChar(logo[i]);
  }
//==========================================================//


  sendcommand(0xaf);
  delay(2000);
  sendcommand(0xa7);    //Set Inverse Display
  delay(2000);  

  while(1)
  {
    //sendcommand(0xa6);  Set Normal Display
    sendcommand(0x29);  //Vertical and Horizontal Scroll Setup
    sendcommand(0x00);  //dummy byte
    sendcommand(0x00);  //define page0 as startpage address
    sendcommand(0x00);  //set time interval between each scroll ste as 6 frames
    sendcommand(0x07);  //define page7 as endpage address
    sendcommand(0x01);  //set vertical scrolling offset as 1 row
    sendcommand(0x2f);  //active scrolling
    delay(3000);

  };

}

//==========================================================//
void sendcommand(unsigned char com)
{
  Wire.beginTransmission(OLED_address);     //begin transmitting
  Wire.write(0x80);                          //command mode
  Wire.write(com);
  Wire.endTransmission();                    // stop transmitting
}

//==========================================================//
void clear_display(void)
{
  unsigned char i,k;
  for(k=0;k<8;k++)
  {	
    setXY(k,0);    
    {
      for(i=0;i<128;i++)     //clear all COL
      {
        SendChar(0);         //clear all COL
        //delay(10);
      }
    }
  }
}

//==========================================================//
void SendChar(unsigned char data)
{
  Wire.beginTransmission(OLED_address); // begin transmitting
  Wire.write(0x40);//data mode
  Wire.write(data);
  Wire.endTransmission();    // stop transmitting
}

//==========================================================//
void setXY(unsigned char row,unsigned char col)
{
  sendcommand(0xb0+row);                //set page address
  sendcommand(0x00+(8*col&0x0f));       //set low col address
  sendcommand(0x10+((8*col>>4)&0x0f));  //set high col address
}


//==========================================================//
void sendStr(unsigned char *string)
{
  unsigned char i=0;
  //setXY(0,0);    
  while(*string)
  {
    for(i=0;i<8;i++)
    {
      SendChar(myFont[*string-0x20][i]);

      // SendChar(*string);
      delay(10);
    }
    *string++;
  }
}

//==========================================================//
void init_OLED(void)
{

  sendcommand(0xae);		//display off
  delay(50); 
  //----------------------------REVERSE comments----------------------------//
  //  sendcommand(0xa0);		//seg re-map 0->127(default)
  //  sendcommand(0xa1);		//seg re-map 127->0
  //  sendcommand(0xc8);
  //  delay(1000);
  //----------------------------REVERSE comments----------------------------//
  
  sendcommand(0xaf);		//display on
  delay(50); 

}
