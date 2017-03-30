/* #################################################################################################################
   LightTelemetry protocol (LTM)

   Ghettostation one way telemetry protocol for really low bitrates (1200/2400 bauds).

   Protocol details: 3 different frames, little endian.
     G Frame (GPS position) (2hz @ 1200 bauds , 5hz >= 2400 bauds): 18BYTES
      0x24 0x54 0x47 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF  0xFF   0xC0
       $     T    G  --------LAT-------- -------LON---------  SPD --------ALT-------- SAT/FIX  CRC
     A Frame (Attitude) (5hz @ 1200bauds , 10hz >= 2400bauds): 10BYTES
       0x24 0x54 0x41 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF 0xC0
        $     T   A   --PITCH-- --ROLL--- -HEADING-  CRC
     S Frame (Sensors) (2hz @ 1200bauds, 5hz >= 2400bauds): 11BYTES
       0x24 0x54 0x53 0xFF 0xFF  0xFF 0xFF    0xFF    0xFF      0xFF       0xC0
        $     T   S   VBAT(mv)  Current(ma)   RSSI  AIRSPEED  ARM/FS/FMOD   CRC
   ################################################################################################################# */

#if defined(PROTOCOL_LIGHTTELEMETRY)
#include "LightTelemetry.h"
static uint8_t LTMserialBuffer[LIGHTTELEMETRY_GFRAMELENGTH - 4];
static uint8_t LTMreceiverIndex;
static uint8_t LTMcmd;
static uint8_t LTMrcvChecksum;
static uint8_t LTMreadIndex;
static uint8_t LTMframelength;



uint8_t ltmread_u8()  {
  return LTMserialBuffer[LTMreadIndex++];
}

uint16_t ltmread_u16() {
  uint16_t t = ltmread_u8();
  t |= (uint16_t)ltmread_u8() << 8;
  return t;
}

uint32_t ltmread_u32() {
  uint32_t t = ltmread_u16();
  t |= (uint32_t)ltmread_u16() << 16;
  return t;
}


void ltm_read() {
  
  uint8_t c;

  static enum _serial_state {
    IDLE,
    HEADER_START1,
    HEADER_START2,
    HEADER_MSGTYPE,
    HEADER_DATA
  }
  c_state = IDLE;

  while (ltmSerial.available()) {
    c = char(ltmSerial.read());
    //Serial.print(c);;

    if (c_state == IDLE) {
      c_state = (c == '$') ? HEADER_START1 : IDLE;
      
    }
    else if (c_state == HEADER_START1) {
      c_state = (c == 'T') ? HEADER_START2 : IDLE;
      
    }
    else if (c_state == HEADER_START2) {
      switch (c) {
        case 'G':
          LTMframelength = LIGHTTELEMETRY_GFRAMELENGTH;
          c_state = HEADER_MSGTYPE;
          break;
        case 'A':
          LTMframelength = LIGHTTELEMETRY_AFRAMELENGTH;
          c_state = HEADER_MSGTYPE;
          
          break;
        case 'S':
          LTMframelength = LIGHTTELEMETRY_SFRAMELENGTH;
          c_state = HEADER_MSGTYPE;
          
          break;
        case 'O':
          LTMframelength = LIGHTTELEMETRY_OFRAMELENGTH;
          c_state = HEADER_MSGTYPE;
          
          break;
        case 'N':
          LTMframelength = LIGHTTELEMETRY_NFRAMELENGTH;
          c_state = HEADER_MSGTYPE;
          
          break;
        case 'X':
          LTMframelength = LIGHTTELEMETRY_XFRAMELENGTH;
          c_state = HEADER_MSGTYPE;
          ;
          break;
        default:
          c_state = IDLE;
      }
      LTMcmd = c;
      LTMreceiverIndex = 0;
    }
    else if (c_state == HEADER_MSGTYPE) {
      if (LTMreceiverIndex == 0) {
        LTMrcvChecksum = c;
      }
      else {
        LTMrcvChecksum ^= c;
      }
      if (LTMreceiverIndex == LTMframelength - 4) { // received checksum byte
        if (LTMrcvChecksum == 0) {
          //telemetry_ok = true;
          LTM_pkt_ok++;   //increase packet ok counter
          //lastpacketreceived = millis();
          ltm_check();
          c_state = IDLE;
        }
        else {                                                   // wrong checksum, drop packet
          LTM_pkt_ko++;       //increase packet dropped counter
          c_state = IDLE;
          

        }
      }
      else LTMserialBuffer[LTMreceiverIndex++] = c;
    }
  }
}

// --------------------------------------------------------------------------------------
// Decoded received commands
void ltm_check() {
  
  LTMreadIndex = 0;

  if (LTMcmd == LIGHTTELEMETRY_GFRAME)
  {

    uav_lat = (int32_t)ltmread_u32();
    uav_lon = (int32_t)ltmread_u32();
    uav_groundspeedms = ltmread_u8();
    uav_groundspeed = (uint16_t) round((float)(uav_groundspeedms * 3.6f)); // convert to kmh
    uav_alt = (int32_t)ltmread_u32()/100;//convert to m
    uint8_t ltm_satsfix = ltmread_u8();
    uav_satellites_visible         = (ltm_satsfix >> 2) & 0xFF;
    uav_fix_type                   = ltm_satsfix & 0b00000011;
    memset(LTMserialBuffer, 0, LIGHTTELEMETRY_GFRAMELENGTH - 4);

  }

  if (LTMcmd == LIGHTTELEMETRY_AFRAME)
  {
    uav_pitch = (int16_t)ltmread_u16();
    uav_roll =  (int16_t)ltmread_u16();
    uav_heading = (int16_t)ltmread_u16();
    if (uav_heading < 0 ) uav_heading = uav_heading + 360; //convert from -180/180 to 0/360°
    memset(LTMserialBuffer, 0, LIGHTTELEMETRY_AFRAMELENGTH - 4);
  }
  if (LTMcmd == LIGHTTELEMETRY_SFRAME)
  {
    uav_bat = ltmread_u16();
    uav_amp = ltmread_u16();
    uav_rssi = (ltmread_u8()*100)/255;
    uav_airspeed = ltmread_u8()*100;
    ltm_armfsmode = ltmread_u8();
    uav_arm = ltm_armfsmode & 0b00000001;
    uav_failsafe = (ltm_armfsmode >> 1) & 0b00000001;
    uav_flightmode = (ltm_armfsmode >> 2) & 0b00111111;
    memset(LTMserialBuffer, 0, LIGHTTELEMETRY_SFRAMELENGTH - 4);
  }
  if (LTMcmd == LIGHTTELEMETRY_OFRAME) // origin frame
  {
    uav_homelat = (int32_t)ltmread_u32();
    uav_homelon = (int32_t)ltmread_u32();
    uav_homealt = (int32_t)ltmread_u32();
    uav_osd_on = (int8_t)ltmread_u8(); // always 1
    uav_homefixstatus = (int8_t)ltmread_u8(); // home fix status
    
    memset(LTMserialBuffer, 0, LIGHTTELEMETRY_OFRAMELENGTH - 4);
  }
  if (LTMcmd == LIGHTTELEMETRY_NFRAME)
  {
    uav_gpsmode = ltmread_u8();
    uav_navmode = ltmread_u8();
    uav_navaction = ltmread_u8();
    uav_WPnumber = ltmread_u8();
    ltm_naverror = ltmread_u8();
    ltm_flags = ltmread_u8();
    memset(LTMserialBuffer, 0, LIGHTTELEMETRY_NFRAMELENGTH - 4);
  }
  if (LTMcmd == LIGHTTELEMETRY_XFRAME)
  {
    uav_HDOP = ltmread_u16();
    uav_HWstatus = ltmread_u8();
    uav_spare1 = ltmread_u8();
    uav_spare2 = ltmread_u8();
    ltm_spare3 = ltmread_u8();
    memset(LTMserialBuffer, 0, LIGHTTELEMETRY_XFRAMELENGTH - 4);
  }
}


#endif
