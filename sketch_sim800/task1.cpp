#include "tasks.h"
#include "definitions.h"
#include <string.h>
#include <Wire.h>
#include <HardwareSerial.h>
#include <EEPROM.h>

void vTask1(void *p){
  // Set GPS module
  static unsigned long time_stamp1 = micros();
  //
  //Serial2.begin(baud-rate, protocol, RX pin, TX pin);
  SerialGPS.begin(9600, SERIAL_8N1, RXD_GPS, TXD_GPS); //gps baud
  SerialMon.println("1. vTask1 - GPS Core# " + String(xPortGetCoreID()));
  while(1){
    time_stamp1 = micros();
    SerialMon.print("1. vTask1 while loop on Core# "+ String(xPortGetCoreID()));
    readGSPdata();
    SerialMon.println(" Delay " + String(micros()-time_stamp1));
    vTaskDelay(10000);
  }
}