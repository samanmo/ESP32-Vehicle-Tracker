#include "tasks.h"
#include "definitions.h"
#include <string.h>
#include <Wire.h>
#include <HardwareSerial.h>
#include <EEPROM.h>

void vTask6(void *p){
  SerialMon.println("6. Initializaing vTask6");
  uint32_t txpos=1;
  occupancyStatusStruct occupancyStatus_obj;
  while(1){
     //delay(2000);
     occupancyStatus_obj.occupancyStatus_int = 0;
     
     if (digitalRead(SEAT_OCCUPANCY))
     {
        occupancyStatus_obj.occupancyStatus_int = 1;
        occupancyStatus_int = 1;
        SerialMon.println("SEAT_OCCUPANCY " + String(digitalRead(SEAT_OCCUPANCY)));
        SerialMon.println("SEAT_OCCUPANCY displays when Face Detection is triggered only! " );
     }
     
     if(xQueueSendToBack(xStructQueue, &occupancyStatus_obj, 1000/portTICK_RATE_MS)!=pdTRUE) {
        Serial.println("xQueueSendToBack Failed"  + String (occupancyStatus_obj.occupancyStatus_int));
     }else{
        Serial.println("xQueueSendToBack Passed" + String(occupancyStatus_obj.occupancyStatus_int));
     }
     vTaskDelay(5000);
     //delay(2000);
  }
}
