void ltm_read();
void ltm_check();

#define LIGHTTELEMETRY_START1 0x24 //$ HEADER '$'
#define LIGHTTELEMETRY_START2 0x54 //T  HEADER 'T'
#define LIGHTTELEMETRY_GFRAME 0x47 //G GPS + Baro altitude data ( Lat, Lon, Speed, Alt, Sats, Sat fix)
#define LIGHTTELEMETRY_AFRAME 0x41 //A Attitude data ( Roll,Pitch, Heading )
#define LIGHTTELEMETRY_SFRAME 0x53 //S Sensors/Status data ( VBat, Consumed current, Rssi, Airspeed, Arm status, Failsafe status, Flight mode )
#define LIGHTTELEMETRY_OFRAME 0x4F //O  Origin data (home lon, home lat, homefix)
#define LIGHTTELEMETRY_NFRAME 0x4E //N  NAVIGATION data
#define LIGHTTELEMETRY_XFRAME 0x58 //X  EXTRA FRAME
#define LIGHTTELEMETRY_GFRAMELENGTH 18
#define LIGHTTELEMETRY_AFRAMELENGTH 10
#define LIGHTTELEMETRY_SFRAMELENGTH 11
#define LIGHTTELEMETRY_OFRAMELENGTH 18
#define LIGHTTELEMETRY_NFRAMELENGTH 10
#define LIGHTTELEMETRY_XFRAMELENGTH 10

void telemetry_off(void);

//Global variables
int32_t       LTM_pkt_ko = 0;   //wrong LTM packet counter
int32_t       LTM_pkt_ok = 0;   //good LTM packet counter
int32_t       uav_homelat = 0; //home latitude
int32_t       uav_homelon = 0; //home longitude
int32_t       uav_homealt = 0; //home altitude
int32_t       ground_course = 0; //course over ground 
int32_t       ground_distance = 0; //distance bettween two consecutive GPS points in actual course (unused) // TODO use this to calc SPEED
uint8_t       uav_osd_on = 0; // allways 1
uint32_t      home_distance = 0; // distante to home
int16_t       home_heading = 0; // heading to home
uint8_t       uav_homefixstatus = 0; // 1=homefix ok
uint8_t       uav_gpsmode = 0;
uint8_t       uav_navmode = 0;
uint8_t       uav_navaction = 0;
uint8_t       uav_WPnumber = 0; // Waypoint number
uint8_t       ltm_naverror = 0;
uint8_t       ltm_flags = 0;
uint16_t      uav_HDOP = 0; // GPS HDOP
uint8_t       uav_HWstatus = 0; // hardware error
uint8_t       uav_spare1 = 0;
uint8_t       uav_spare2 = 0;
uint8_t       ltm_spare3 = 0;

int32_t      uav_lat = 0;                    // latitude
int32_t      uav_lon = 0;                    // longitude
float        lonScaleDown = 0.0;             // longitude scaling
uint8_t      uav_satellites_visible = 0;     // number of satellites
uint8_t      uav_fix_type = 0;               // GPS lock 0-1=no fix, 2=2D, 3=3D
int32_t      uav_alt = 0;                    // altitude (dm)
int32_t      rel_alt = 0;                    // relative altitude to home
uint16_t     uav_groundspeed = 0;            // ground speed in km/h
uint8_t      uav_groundspeedms = 0;          // ground speed in m/s
int16_t      uav_pitch = 0;                  // attitude pitch
int16_t      uav_roll = 0;                   // attitude roll
int16_t      uav_heading = 0;                // attitude heading
int16_t      uav_gpsheading = 0;             // gps heading
uint16_t     uav_bat = 0;                    // battery voltage (mv)
uint16_t     uav_amp = 0;                    // consumed mah.
uint16_t     uav_current = 0;                // actual current
uint8_t      uav_rssi = 0;                   // radio RSSI (%)
uint8_t      uav_linkquality = 0;            // radio link quality
uint8_t      uav_airspeed = 0;               // Airspeed sensor (m/s)
uint8_t      ltm_armfsmode = 0;
uint8_t      uav_arm = 0;                    // 0: disarmed, 1: armed
uint8_t      uav_failsafe = 0;               // 0: normal,   1: failsafe
uint8_t      uav_flightmode = 19;            // Flight mode(0-19): 0: Manual, 1: Rate, 2: Attitude/Angle, 3: Horizon, 4: Acro, 5: Stabilized1, 6: Stabilized2, 7: Stabilized3,
// 8: Altitude Hold, 9: Loiter/GPS Hold, 10: Auto/Waypoints, 11: Heading Hold / headFree, 12: Circle, 13: RTH, 14: FollowMe, 15: LAND,
// 16:FlybyWireA, 17: FlybywireB, 18: Cruise, 19: Unknown



//long lastpacketreceived;



boolean gps_fix      = false;
//boolean btholdstate  = false;
//boolean telemetry_ok = false;
//boolean home_pos     = false;
//boolean home_bear    = false;

