#include "tasks.h"
#include "definitions.h"
#include <TinyGsmClient.h>
#include <TinyGPS++.h>
#include <Wire.h>
#include <HardwareSerial.h>
#include <EEPROM.h>
#include <stdlib.h>

// static - all the definitions are local to this translation unit.

static TinyGsm modem(SerialAT);

// I2C for SIM800 (to keep it running when powered from battery)
static TwoWire I2CPower = TwoWire(0);

// TinyGSM Client for Internet connection
static TinyGsmClient client(modem);

// TinyGPSPlus Client for GPS connection
static TinyGPSPlus gps;

static String date_str , time_str , lat_str="0.00" , lng_str="0.00", speed_str="0.00";
static String systemno_str = "1", date_time_str, longitude_str = "0.00", latitude_str = "0.00";
static String direction_str = "0", locate_str= "0", idlestat_str = "0", milage_str = "0", altitude_str= "0.00", acceleration_str= "0";
static String occupancyStatus_str= "0";

// Server details

static char server[64]; // domain name: example.com, maker.ifttt.com, etc server[] = "192.248.105.25";
static char resource[64]; // resource path, for example: /post-data.php resource[] = "/post-esp-data2.php";
static int  port;         // server port number port = 80;
static char apn[16];

/*
// Server details
// The server variable can be just a domain name or it can have a subdomain. It depends on the service you are using
static char server[64] = "192.248.105.25"; // domain name: example.com, maker.ifttt.com, etc
static char resource[64] = "/index.html";    // "/post-esp-data2.php"; // resource path, for example: /post-data.php
static int port = 80; // server port number.
*/
int occupancyStatus_int = 3;

bool setPowerBoostKeepOn(int en){
  I2CPower.beginTransmission(IP5306_ADDR);
  I2CPower.write(IP5306_REG_SYS_CTL0);
  if (en) {
    I2CPower.write(0x37); // Set bit1: 1 enable 0 disable boost keep on
  } else {
    I2CPower.write(0x35); // 0x37 is default reg value
  }
  return I2CPower.endTransmission() == 0;
}

void batteryPoweredOn(void){
  // Start I2C communication
  I2CPower.begin(I2C_SDA, I2C_SCL, 400000);
  // Keep power when running from battery
  bool isOk = setPowerBoostKeepOn(1);
  SerialMon.println(String("IP5306 KeepOn ") + (isOk ? "OK" : "FAIL"));
}

/*
 * Hardware Trigger
 * The shield comes with a right angle tactile switch situated near the PWR LED indicator. 
 * You need to press that switch for about 2 seconds to power the shield up/down.
 * 
 * Software trigger
 * Instead of manually pressing the PWRKEY every time, you can turn the SIM900 up/down programmatically.
 * First, you need to solder the SMD jumper named R13 on the shield as highlighted in the image.
 * Next, you need to connect D9 pin on the shield to the D9 pin on Arduino.
*/
void SIM900powerON(void){
  pinMode(MODEM_POWER_ON, OUTPUT); //D9
  digitalWrite(MODEM_POWER_ON,LOW); //D9
  delay(1000);
  //delay(5000);
  digitalWrite(MODEM_POWER_ON,HIGH); //D9
  delay(2000);
  digitalWrite(MODEM_POWER_ON,LOW); //D9
  delay(3000);
  //delay(5000);
}

/*
The sketch starts by including a SoftwareSerial.h library and initializing it 
with the Arduino pins to which Tx and Rx of SIM900 shield is connected.
#include <SoftwareSerial.h>
//Create software serial object to communicate with SIM900
// set up a new serial object
SoftwareSerial mySerial (rxPin, txPin);
SoftwareSerial mySerial (SIM900_RX, SIM900_TX);
SoftwareSerial mySerial(7, 8); //SIM900 Tx(D7) & Rx(D8) is connected to Arduino #7 & #8
//Begin serial communication with Arduino and SIM900
mySerial.begin(9600);
mySerial.println("AT"); //Handshaking with SIM900
while(mySerial.available()) 
{
  Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
}
*/
void TestSIM900Connection(void){
     delay(2000);
     SerialAT.println("AT");
     while(SerialAT.available()) { SerialMon.write(SerialAT.read());}
     delay(2000);
     SerialAT.println("ATI");
     while(SerialAT.available()) { SerialMon.write(SerialAT.read());}
     delay(2000);
     SerialAT.println("AT+COPS?");
     while(SerialAT.available()) { SerialMon.write(SerialAT.read());}
     delay(2000);
     SerialAT.println("AT+CREG?");
     while(SerialAT.available()) { SerialMon.write(SerialAT.read());} 
     delay(2000);
     SerialAT.println("AT+CCID");
     while(SerialAT.available()) { SerialMon.write(SerialAT.read());}
     delay(2000);
     SerialAT.println("AT+CSQ");
     while(SerialAT.available()) { SerialMon.write(SerialAT.read());}
     delay(2000);
     SerialAT.println("AT+CBC");
     while(SerialAT.available()) { SerialMon.write(SerialAT.read());}
     delay(2000);
}

void SIM900SoftwareSerialOff(void){ //SIM900 Tx & Rx is connected to Arduino #7 & #8
  //pinMode(SIM900_TX, OUTPUT); 
  //digitalWrite(SIM900_TX,HIGH); 
  //pinMode(SIM900_RX, INPUT_PULLUP); 
}

void unlockSim(void){
  pinMode(MODEM_RX_RX2, INPUT_PULLUP);
  pinMode(MODEM_TX_TX2, OUTPUT);
  
  SerialMon.println("2. Unlocking SIM! ");
  // Set GSM module baud rate and UART pins
  //Serial2.begin(baud-rate, protocol, RX pin, TX pin);
  //
 // SerialAT.begin(115200, SERIAL_8N1, MODEM_RX_RX2, MODEM_TX_TX2);
  SerialAT.begin(9600, SERIAL_8N1, MODEM_RX_RX2, MODEM_TX_TX2);
  delay(3000);

  // Restart SIM800 module, it takes quite some time
  // To skip it, call init() instead of restart()
  SerialMon.println("2. Initializing modem...");
  modem.restart();
  // use modem.init() if you don't need the complete restart

  // Unlock your SIM card with a PIN if needed
  if (strlen(simPIN) && modem.getSimStatus() != 3 ) {
    modem.simUnlock(simPIN);
  }
}

void initializeModem(void){
  // Set modem reset, enable, power pins
  //pinMode(MODEM_PWKEY, OUTPUT);
  //pinMode(MODEM_RST, OUTPUT);
  //digitalWrite(MODEM_PWKEY, LOW);
  //digitalWrite(MODEM_RST, HIGH);
  //pinMode(MODEM_POWER_ON, OUTPUT);
  //digitalWrite(MODEM_POWER_ON, HIGH);
  
  SerialMon.println("2. Modem Pin settings! ");
  unlockSim();
}


void GPRSinitializeSIM900(void){
  //SerialAT.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\""); /* Connection type GPRS */
  SerialAT.println("AT+GPRS=1");
  delay(500);
  updateSerial();
  //delay(5000);
  //SerialAT.println("AT+SAPBR=3,1,\"APN\",\"mobitel\"");
  //delay(500);
  //updateSerial();
  //delay(5000);
  //SerialAT.println("AT+SAPBR=1,1"); /* Open GPRS context */
  //SerialAT.println("AT+SAPBR=2,1");  /* Query the GPRS context */
  //SerialAT.println("AT+SAPBR=0,1"); /* Close GPRS context */
}

void readGSPdata(void){
   float latitude , longitude, ground_speed;
   int year , month , date, hour , minute , second;
   int pm;
   
   if (SerialGPS.available() > 0){
     SerialMon.println("SerialGPS available");
   }else {
     SerialMon.println("SerialGPS Not available");
   }

   while (SerialGPS.available() > 0)
    if (gps.encode(SerialGPS.read()))
    {
      if (gps.location.isValid())
      {
        latitude = gps.location.lat();
        lat_str = String(latitude , 6);
        longitude = gps.location.lng();
        lng_str = String(longitude , 6);
      }

      if (gps.date.isValid())
      {
        date_str = "";
        date = gps.date.day();
        month = gps.date.month();
        year = gps.date.year();

        if (date < 10)
          date_str = '0';
        date_str += String(date);

        date_str += " / ";

        if (month < 10)
          date_str += '0';
        date_str += String(month);

        date_str += " / ";

        if (year < 10)
          date_str += '0';
        date_str += String(year);
      }

      if (gps.time.isValid())
      {
        time_str = "";
        hour = gps.time.hour();
        minute = gps.time.minute();
        second = gps.time.second();

        minute = (minute + 30);
        if (minute > 59)
        {
          minute = minute - 60;
          hour = hour + 1;
        }
        hour = (hour + 5) ;
        if (hour > 23)
          hour = hour - 24;

        if (hour >= 12)
          pm = 1;
        else
          pm = 0;

        hour = hour % 12;

        if (hour < 10)
          time_str = '0';
        time_str += String(hour);

        time_str += " : ";

        if (minute < 10)
          time_str += '0';
        time_str += String(minute);

        time_str += " : ";

        if (second < 10)
          time_str += '0';
        time_str += String(second);

        if (pm == 1)
          time_str += " PM ";
        else
          time_str += " AM ";
      }

      if (gps.speed.isValid()){
        ground_speed = gps.speed.kmph();
        speed_str = String(ground_speed, 6);
        speed_str = String((int)ground_speed);
      }
      SerialMon.print(" Date:"+date_str);
      SerialMon.println(" Time: " + time_str);
      SerialMon.print(" Latitude:"+lat_str);
      SerialMon.print(" Longitude:"+lng_str);
      SerialMon.println(" Ground Speed:"+speed_str);
      
      direction_str = String((int)1);
      locate_str = String((int)0);
      idlestat_str = String((int)0);
    }

   if (SerialGPS.available() > 0){
      SerialMon.println(time_str);
      SerialMon.println(date_str);
      SerialMon.println(lat_str);
      SerialMon.println(lng_str);
      SerialMon.println(speed_str);
   }else {
   }

//  SerialMon.print("Time:"+time_str);
//  SerialMon.print(" Date:"+date_str);
//  SerialMon.print(" Latitude:"+lat_str);
//  SerialMon.print(" Longitude:"+lng_str);
//  SerialMon.println(" Ground Speed:"+speed_str);
  //SerialMon.print("Connecting to APN: ");
  //SerialMon.println(apn);
  //delay(10000);
}

void sendGSMdata(void){
  SerialMon.print("Task . Trying to Connect:");
  SerialMon.print("GSM ");
  readServerSettings();
  occupancyStatus_str = String(occupancyStatus_int);
  /*
  if(SerialAT.available()>0){
      SerialMon.println("SerialGSM available");
  }else{
      SerialMon.println("SerialGSM Not available");
      return;
  }
  */
  //SerialAT.println("AT+SAPBR=1,1"); /* Open GPRS context */
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    SerialMon.println(" Task - gprsConnect.fail");
  }
  else {
    SerialMon.println(" Task - gprsConnect.OK");
    //TestSIM900Connection();
    SerialMon.print("Connecting to ");
    SerialMon.print(server);
    if (!client.connect(server, port)) {
      SerialMon.println(" client.connect.fail");
    }else {
      SerialMon.println(" client.connect.OK");

      // Making an HTTP POST request
      SerialMon.println("Performing HTTP POST request...");

      // Prepare your HTTP POST request data
      String httpRequestData = "api_key=" + apiKeyValue +
                             "&systemno=" + systemno_str +
                             "&date_time=" + date_str + " " + time_str +
                             "&latitude=" + lat_str +   //"&value1=" + String("7.098992") +
                             "&longitude=" + lng_str +   //"&value2=" + String("79.898643") +
                             "&speed=" + speed_str + //+ String("99.9999") + "";
                             "&direction=" + direction_str +
                             "&locate=" + locate_str +
                             "&idlestat=" + idlestat_str +
                             "&milage=" + milage_str + 
                             "&altitude=" + altitude_str +
                             "&acceleration=" + acceleration_str +
                             "&occupancystatus=" + occupancyStatus_str;

      client.print(String("POST ") + resource + " HTTP/1.1\r\n");
      client.print(String("Host: ") + server + "\r\n");
      client.println("Connection: close");
      client.println("Content-Type: application/x-www-form-urlencoded");
      client.print("Content-Length: ");
      client.println(httpRequestData.length());
      client.println();
      client.println(httpRequestData);

      unsigned long timeout = millis();
      while (client.connected() && millis() - timeout < 10000L) {
        // Print available data (HTTP response from server)
        while (client.available()) {
          char c = client.read();
          SerialMon.print(c);
          timeout = millis();
        }
      }
      SerialMon.println();

      // Close client and disconnect
      client.stop();
      SerialMon.println(F("Server disconnected"));
      modem.gprsDisconnect();
      SerialMon.println(F("GPRS disconnected"));
    }
  }
  //SerialAT.println("AT+SAPBR=0,1"); /* Close GPRS context */
  
}

void mcuGPIOconfiguration(void)
{
  /*
  //
  //pinMode(SEAT_OCCUPANCY, INPUT);    // sets the digital pin 34 as input
  //pinMode(SEAT_OCCUPANCY, INPUT_PULLUP);
  */
  pinMode(SEAT_OCCUPANCY, INPUT_PULLUP); // This input is for receiving the Seat Occupancy details.
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUTTON3, INPUT_PULLUP);
  pinMode(BUTTON4, INPUT_PULLUP);
}

void clearBufferArray(unsigned char* buffer, int count){              // function to clear buffer array
  for (int i=0; i<count; i++){
    buffer[i]=NULL;                  // clear all index of array with command NULL
  }
}


void ServerURLParsing(const unsigned char* Received_string, const int N ){
    /* http://172.16.7.25/index.html */
/*
// Server details
// The server variable can be just a domain name or it can have a subdomain. It depends on the service you are using
const char server[] = "192.248.105.25"; // domain name: example.com, maker.ifttt.com, etc
const char resource[] = "/post-esp-data2.php"; // resource path, for example: /post-data.php
const int  port = 80;

SERVER,mode,domain/IP,port,0#
E.g. SERVER,1,www.ydpat.com,8011,0#
 SERVER,0,211.154.135.113,8011,0#
mode1 means set with domain name
mode 0 means set with ip address// server port number
*/
  
  int i = 0;
  char server[64];
  char resource[64];
  char port_str[10];
  int ARRAY_START = N + 9;

  for (i = 0; Received_string[i + ARRAY_START] != '/' || i > MAX_TXT_LENGTH/2 ; i++) {
    server[i] = Received_string[i + ARRAY_START];
  }
  server[i] = '\0';
  ARRAY_START = i + ARRAY_START;
 
  
  for (i = 0; Received_string[i + ARRAY_START] != ',' || i > MAX_TXT_LENGTH/2; i++) {
    resource[i] = Received_string[i + ARRAY_START];
  }
  resource[i] = '\0';
  ARRAY_START = i + 1 + ARRAY_START;
  
  for (i = 0; Received_string[i + ARRAY_START] != ',' || i > MAX_TXT_LENGTH/4; i++) {
    port_str[i] = Received_string[i + ARRAY_START];
  }
  port_str[i] = '\0';
  ARRAY_START = i + 1 + ARRAY_START;

  writeCharArrayIntoEEPROM(0, server, sizeof(server));
  writeCharArrayIntoEEPROM(64, resource, sizeof(resource));
  writeCharArrayIntoEEPROM(128, port_str, sizeof(port_str));
  EEPROM.commit();
  
  SerialMon.println(server);
  SerialMon.println(resource);
  SerialMon.println(port_str);
  
}

void readServerSettings(){
  char str_port[64];
  readCharArrayFromEEPROM(0, server, 64);
  readCharArrayFromEEPROM(64, resource, 64);
  readCharArrayFromEEPROM(128, str_port, 10);
  port = atoi(str_port);
  SerialMon.println(server);
  SerialMon.println(resource);
  SerialMon.println(String(port));
}

void TimerParsing(const unsigned char* Received_string, const int N ){
//TIMER,T1,T2#
  int i = 0;
  char t1_str[6];
  char t2_str[6];
  int ARRAY_START = N + 6;

  for (i = 0; Received_string[i + ARRAY_START] != ',' || i > MAX_TXT_LENGTH/2 ; i++) {
    t1_str[i] = Received_string[i + ARRAY_START];
  }
  t1_str[i] = '\0';
  ARRAY_START = i + 1 + ARRAY_START;
  
  for (i = 0; (Received_string[i + ARRAY_START] != ',') && (i > MAX_TXT_LENGTH/2) && (Received_string[i + ARRAY_START] != '#'); i++) {
    t2_str[i] = Received_string[i + ARRAY_START];
  }
  t2_str[i] = '\0';
  ARRAY_START = i + 1 + ARRAY_START;
  writeCharArrayIntoEEPROM(138, t1_str, sizeof(t1_str)); // 138 - 147
  writeCharArrayIntoEEPROM(148, t2_str, sizeof(t1_str)); // 148 - 157
  
  EEPROM.commit();
  SerialMon.println(t1_str);
  SerialMon.println(t2_str);
}

int getT1value(){
  //char t1_str[10] = "000000000";
  //char t2_str[6];
  char* t1_str = new char[10];
  int t1_int; 
  SerialMon.println("readCharArrayFromEEPROM - start");
  readCharArrayFromEEPROM(138, t1_str, 6);
  SerialMon.println("readCharArrayFromEEPROM - stop");
  //readCharArrayFromEEPROM(148, t2_str, 6);
  t1_int = atoi(t1_str);
  SerialMon.println("readCharArrayFromEEPROM - atoi");
  SerialMon.println("readCharArrayFromEEPROM - t1_int" + String(t1_int));
  //*t2_int = atoi(t2_str);
  //SerialMon.println("getT1value" + String(t1_str));
  //SerialMon.println(t2_str);
  delete t1_str;
  if (t1_int < 10){
    return 10;
  } else if ( t1_int > 18000){
    return 18000;
  } else {
    return t1_int;
  }
  return t1_int;
}

int getT2value(){
  char t2_str[10] = "000000000";
  readCharArrayFromEEPROM(148, t2_str, 6);
  SerialMon.println("getT2value" + String(t2_str));
  if (atoi(t2_str) < 10){
    return 10;
  } else if ( atoi(t2_str) > 18000){
    return 18000;
  } else {
    return atoi(t2_str);
  }
  return atoi(t2_str);
}


void APNParsing(const unsigned char* Received_string, const int N ){
  //APN,apnname#
  //APN,dialogbb#
  //APN,http#
  //APN,mobitel#
  int i = 0;
  char apn_str[16];
  int ARRAY_START = N + 4;
  SerialMon.println("Inside APN Parsing");

  for (i = 0; (Received_string[i + ARRAY_START] != '#') && (i < 15) && (Received_string[i + ARRAY_START] != ',') ; i++) {
    apn_str[i] = Received_string[i + ARRAY_START];
  }
  apn_str[i] = '\0';
  ARRAY_START = i + 1 + ARRAY_START;

  writeCharArrayIntoEEPROM(158, apn_str, sizeof(apn_str)); // 158 - 173
  
  EEPROM.commit();
  SerialMon.println(apn_str);
  SerialMon.println("End APN Parsing");
  //SerialMon.println(t2_str);
}

void readAPNSettings(){
  SerialMon.println("readCharArrayFromEEPROM - start");
  readCharArrayFromEEPROM(158, apn, 16);
  SerialMon.println("readCharArrayFromEEPROM - completed");
  SerialMon.println(apn);
  SerialMon.println("readCharArrayFromEEPROM - End");
}


void writeCharArrayIntoEEPROM(const int address, char character[], const int arraySize)
{
  char aChar;
  for (int i = 0; i < arraySize; i++)
  {
    EEPROM.write(address + i, character[i]);
    //SerialMon.print(character[i]);
    aChar = EEPROM.read(address + i);
    SerialMon.print(aChar);
  }
  SerialMon.println("\r");
}

void readCharArrayFromEEPROM(const int address, char character[], const int arraySize)
{
  char aChar = EEPROM.read(address);
  for (int i = 0; aChar != '\0'; i++)
  {
    aChar = EEPROM.read(address + i);
    SerialMon.print(aChar);
    character[i] = aChar;
  }
  SerialMon.println("\r");
}


void buzzerRingOnce(){
//15 // Confirmed. Connected to the plus side of the buzzer.
  pinMode(BUZZER_PLUS, OUTPUT); //D9
  digitalWrite(BUZZER_PLUS,LOW); //D9
  delay(1000);
  digitalWrite(BUZZER_PLUS,HIGH); //D9
  delay(10);
  digitalWrite(BUZZER_PLUS,LOW); //D9
  delay(100);
  digitalWrite(BUZZER_PLUS,HIGH); //D9
  delay(10);
  digitalWrite(BUZZER_PLUS,LOW); //D9
  delay(1000);
}

void buzzerRingOncequick(){
//15 // Confirmed. Connected to the plus side of the buzzer.
  pinMode(BUZZER_PLUS, OUTPUT); //D9
  digitalWrite(BUZZER_PLUS,LOW); //D9
  delay(1000);
  digitalWrite(BUZZER_PLUS,HIGH); //D9
  delay(50);
  digitalWrite(BUZZER_PLUS,LOW); //D9
  delay(100);
  digitalWrite(BUZZER_PLUS,HIGH); //D9
  delay(50);
  digitalWrite(BUZZER_PLUS,LOW); //D9
  delay(1000);
}

void buzzerRingOnceSuperQuick(){
//15 // Confirmed. Connected to the plus side of the buzzer.
  pinMode(BUZZER_PLUS, OUTPUT); //D9
  digitalWrite(BUZZER_PLUS,LOW); //D9
  delay(1000);
  digitalWrite(BUZZER_PLUS,HIGH); //D9
  delay(25);
  digitalWrite(BUZZER_PLUS,LOW); //D9
  delay(100);
  digitalWrite(BUZZER_PLUS,HIGH); //D9
  delay(25);
  digitalWrite(BUZZER_PLUS,LOW); //D9
  delay(1000);
}


void updateSerial()
{
  delay(500);
  while (SerialMon.available()) 
  {
    SerialAT.write(SerialMon.read());//Forward what Serial received to Software Serial Port
  }
  while(SerialAT.available()) 
  {
     SerialMon.write(SerialAT.read());//Forward what Software Serial received to Serial Port
     delay(50);
  }
}

void writedataSerialAT()
{
  delay(500);
  while (SerialMon.available()) 
  {
    SerialAT.write(SerialMon.read());//Forward what Serial received to Software Serial Port
  }
}

void printSerialATdata()
{
  delay(500);
  while(SerialAT.available()) 
  {
     SerialMon.write(SerialAT.read());//Forward what Software Serial received to Serial Port
     delay(50);
  }
}

void initializeEEPROM(){
  EEPROM.begin(EEPROM_SIZE);
}
