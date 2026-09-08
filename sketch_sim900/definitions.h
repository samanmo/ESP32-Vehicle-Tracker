/*
 * A header file in C/C++ contains:
 * Function definitions
 * Data type definitions
 * Macros
GPIO Input Output  Notes
0   pulled up OK  outputs PWM signal at boot
1   TX pin  OK  debug output at boot
2   OK  OK  connected to on-board LED
3   OK  RX pin  HIGH at boot
4   OK  OK  
5   OK  OK  outputs PWM signal at boot
 * 
 */

/*1*/
#include <HardwareSerial.h>
//#include <strings.h>
// Set serial for debug console (to Serial Monitor, default speed 115200)
#define SerialMon Serial
// Set serial for AT commands (to SIM800 module)
#define SerialAT Serial1
// Set serial for GPS module.
#define SerialGPS Serial2

// ESP32 PINs
//23 //  PCB Tracks are not connected (a pin header at the end).
//22 //  PCB Tracks are not connected (a pin header at the end).
// 1 // TX0, Internally connected for programming and debugging. (a pin header at the end).
// 3 // RX0, Internally connected for programming and debugging. (a pin header at the end).
//21 //  PCB Tracks are not connected (a pin header at the end).
//19 // PCB Tracks are not connected (a pin header at the end).
//18 // Confirmed. Connected to IO4 pin of the ESP32CAM (a pin header in the middle).
// 5  // PCB Tracks are not connected (a hole at the end).
//17 2TX// Confirmed. Connected to Hardware RX pin of the SIM900 module.
//16 2RX// Confirmed. Connected to Hardware TX pin of the SIM900 module.
// 4  // PCB Tracks are not connected (a hole at the end).
// 2  // PCB Tracks are not connected (a hole at the end).
//15 // Confirmed. Connected to the plus side of the buzzer.
//GND
//3.3V

//EN
//36 //36 input only // UP// PCB Tracks are not connected.
//39 //39 input only // UN// PCB Tracks are not connected.
//34 //34 input only // PCB Tracks are not connected (a pin header at the end).
//35 //35 input only // PCB Tracks are not connected (a pin header at the end).
//32 // Confirmed. Connected to the D7 of SIM900. (ESP32 must be defined as an input, Software serial not used).
//33 // Confirmed. Connected to the D8 of SIM900. (ESP32 must be defined as an input, Software serial not used).
//25 // Confirmed. Connected to D9 pin of the SIM900 module. (A jumper must be soldered to establish the connection).
//26 // PCB Tracks are not connected (a pin header at the end).
//27 // PCB Tracks are not connected (a pin header at the end).
//14 // PCB Tracks are not connected (a pin header at the end).
//12 // Confirmed. Connected to the Rx of NEO6GPS.
//13 // Confirmed. Connected to the Tx of NEO6GPS.
//GND //
//5V // VIN


//25 // Confirmed. Connected to the D9 of SIM900. (A jumper must be soldered to establish the connection).


// TTGO T-Call pins
/*
#define MODEM_RST           5  // PCB Tracks are not connected (a hole at the end).
#define MODEM_PWKEY         4  // PCB Tracks are not connected (a hole at the end).
*/
#define MODEM_POWER_ON      25 // 25 Both Input and Output OK. // Confirmed. Connected to D9 pin of the SIM900 module.
#define MODEM_RX_RX2        32 // 32 Both Input and Output are OK. // Confirmed. Connected to Hardware TX pin of the SIM900 module.
#define MODEM_TX_TX2        33 // 33 Both Input and Output are OK. // Confirmed. Connected to Hardware RX pin of the SIM900 module.
//
#define I2C_SDA             36 // 36 Input Only. // Battery power control. //36 // PCB Tracks are not connected.
#define I2C_SCL             39 // 39 Input Only. // Battery power control. //39 // PCB Tracks are not connected.
//
#define RXD_GPS             16 // 16 Both Input and Output OK ////13 // Confirmed. Connected to the Tx of NEO6GPS.
#define TXD_GPS             17 // 17 Both Input and Output OK ////12 // Confirmed. Connected to the Rx of NEO6GPS.
//
// 13, 14, 26, 27.
//
//#define SIM900_RX           13 // 13 Both Input and Output OK //32 // Confirmed. Connected to the D7(TX) of SIM900. (ESP32 must be defined as an input, Software serial not used).
// 14 Both Input and Output are OK. Outputs PWM signal at boot.
//#define SIM900_TX           26 // 26 Both Input and Output OK //33 // Confirmed. Connected to the D8(RX) of SIM900. (ESP32 must be defined as an input, Software serial not used).
// 27 Both Input and Output OK.
//
#define BUTTON1             13
#define BUTTON2             14
#define BUTTON3             26
#define BUTTON4             27
//
#define SEAT_OCCUPANCY      18  // 18 Both Input and Output are OK. //18 // Confirmed. Connected to IO4 pin of the ESP32CAM (a pin header in the middle).
#define HANG_OFF_BTN        34  // 34 Input Only // 34 input only // 19 // 19 // PCB Tracks are not connected (a pin header at the end).
#define CALL_BTN            35  // 35 Input only // 26 // PCB Tracks are not connected (a pin header at the end).
#define CALL_STATE          12  // 12 Boot fails if pulled high. Both Input and Output are OK. // PCB Tracks are not connected (a pin header at the end).
#define BUZZER_PLUS         15  // 15 Both Input and Output are OK. Outputs PWM signal at boot //15 // Confirmed. Connected to the plus side of the buzzer.
//
//
#define I2C_ESP32_SDA       22  // 22 Both Input and Output are OK. // PCB Tracks are not connected (a pin header at the end).
#define I2C_ESP32_SCL       23  // 23 Both Input and Output are OK. // PCB Tracks are not connected (a pin header at the end).

#define uS_TO_S_FACTOR 1000000     /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  3600        /* Time ESP32 will go to sleep (in seconds) 3600 seconds = 1 hour */

#define IP5306_ADDR          0x75
#define IP5306_REG_SYS_CTL0  0x00

#define MAX_TXT_LENGTH      128

#define EEPROM_SIZE         256

/*3*/
// Configure TinyGSM library
//#define TINY_GSM_MODEM_SIM800      // Modem is SIM800
#define TINY_GSM_MODEM_SIM900
#define TINY_GSM_RX_BUFFER   1024  // Set RX buffer to 1Kb


/*2*/
//const char apn[]      = "mobitel"; // APN (example: internet.vodafone.pt) use https://wiki.apnchanger.org
const char gprsUser[] = ""; // GPRS User
const char gprsPass[] = ""; // GPRS Password

// SIM card PIN (leave empty, if not defined)
const char simPIN[]   = "";

/*
// Server details
// The server variable can be just a domain name or it can have a subdomain. It depends on the service you are using
const char server[] = "192.248.105.25"; // domain name: example.com, maker.ifttt.com, etc
const char resource[] = "/post-esp-data2.php"; // resource path, for example: /post-data.php
const int  port = 80;                          // server port number
*/

// Keep this API Key value to be compatible with the PHP code provided in the project page.
// If you change the apiKeyValue value, the PHP file /post-data.php also needs to have the same key
const String apiKeyValue = "tPmAT5Ab3j7F9";
const String sensorName  = "NEO5MGPS";
const String sensorLocation = "Office";
