#include "tasks.h"
#include "definitions.h"
#include <string.h>
#include <Wire.h>
#include <HardwareSerial.h>
#include <EEPROM.h>


void vTask8(void *p){
      const char* char_ptr = "ABCDEFGHIJ";
      int count = 0;
      unsigned char buffer[MAX_TXT_LENGTH];
      static unsigned long time_stamp1 = millis();
      int T1value = getT1value()*1000;
      SerialMon.println("T1value" + String(T1value));
      vTaskDelay(10000);
      /* SerialMon.println("8. Initializaing vTask8"); */
      SerialAT.print("AT+CLIP=1\r"); // turn on caller ID notification
      vTaskDelay(500);
      updateSerial();
      SerialAT.println("AT+CMGF=1\r"); // Configuring TEXT mode
      vTaskDelay(500);
      if (SerialAT.available()>0){
        updateSerial();
      }else{
        ESP.restart();
      }
      //SerialAT.println("AT+CNMI=1,2,0,0,0\r"); // Decides how newly arrived SMS messages should be handled
      SerialAT.println("AT+CNMI=2,2,0,0,0\r"); // Decides how newly arrived SMS messages should be handled
      vTaskDelay(500);
      updateSerial();
      vTaskDelay(2000);
      buzzerRingOncequick();
      GPRSinitializeSIM900();
      buzzerRingOnceSuperQuick();
      struct occupancyStatusStruct xRxedStructure;
      while(1){
           if ( (millis() - time_stamp1 > T1value) && (millis() > 3*60*1000)) {
              SerialMon.print("8. vTask8 while loop on Counting 15 seconds, then Reseting the counter ");
              time_stamp1 = millis();
              sendGSMdata();
           }
           if( xQueueReceive( xStructQueue, &( xRxedStructure ), 1000/portTICK_RATE_MS ) == pdPASS )
            {
              /* xRxedStructure now contains a copy of xMessage. */
              Serial.println("xQueueReceive Pass" + String(xRxedStructure.occupancyStatus_int));
              occupancyStatus_int = xRxedStructure.occupancyStatus_int;
            }
           //SerialMon.print("8. vTask8 while loop on Core# " + String(xPortGetCoreID()));
           //delay(1000);
           //vTaskDelay(1000);
           //delay(300);
           // Display any text that the GSM shield sends out on the serial monitor
           // if(SerialAT.available()>0) {}
           writedataSerialAT();
           if (SerialAT.available()>0){         // if date is comming from softwareserial port ==> data is comming from gprs shield
                  SerialMon.println("SerialGSM available");
                  count = 0;
                  SerialMon.print(" Count " + String(count) + "\r");
                  while(SerialAT.available()){          // reading data into char array 
                      buffer[count++]=SerialAT.read();     // writing data into array
                      if(count == (MAX_TXT_LENGTH -2))break;
                      vTaskDelay(10); //if fails, try no delay.
                  }
                  buffer[count++]='\0';
                  buffer[MAX_TXT_LENGTH -1]='\0';
                  SerialMon.print(" Count " + String(count) + "\r");
                  SerialMon.println("Section one");
                  for(int i = 0; buffer[i]!='\0'; i++){
                      if (buffer[i+0]=='R' && buffer[i+1]=='I' && buffer[i+2]=='N' && buffer[i+3]=='G' ){
                          SerialAT.print("ATA\r");
                          SerialMon.print("i is at " + String(i) + " Call answered\r");
                      } else if(buffer[i+0]=='A' && buffer[i+1]=='P' && buffer[i+2]=='N'){
                          SerialMon.print("i is at " + String(i) + " APN answered\r");
                          APNParsing(buffer, i);
                          buzzerRingOnceSuperQuick();
                          readAPNSettings();
                      } else if(buffer[i+0]=='S' && buffer[i+1]=='O' && buffer[i+2]=='S'){
                          SerialMon.print("i is at " + String(i) + " SOS answered\r");
                      } else if(buffer[i+0]=='S' && buffer[i+1]=='E' && buffer[i+2]=='R' && buffer[i+3]=='V' && buffer[i+4]=='E' && buffer[i+5]=='R'){
                          SerialMon.print("i is at " + String(i) + " SERVER answered\r");
                          buzzerRingOncequick();
                          ServerURLParsing(buffer, i);
                          readServerSettings();
                          buzzerRingOncequick();
                      } else if(buffer[i+0]=='T' && buffer[i+1]=='I' && buffer[i+2]=='M' && buffer[i+3]=='E' && buffer[i+4]=='R'){
                          TimerParsing(buffer, i);
                          T1value = getT1value()*1000;
                          SerialMon.print("i is at " + String(i) + " TIMER answered\r");
                      } else if(buffer[i+0]=='C' && buffer[i+1]=='E' && buffer[i+2]=='N' && buffer[i+3]=='T' && buffer[i+4]=='R' && buffer[i+5]=='E'){
                          readServerSettings();
                          SerialMon.print("i is at " + String(i) + " CENTRE answered\r");
                      } else if(buffer[i+0]=='C' && buffer[i+1]=='E' && buffer[i+2]=='N' && buffer[i+3]=='T' && buffer[i+4]=='E' && buffer[i+5]=='R'){
                          readServerSettings();
                          SerialMon.print("i is at " + String(i) + " CENTER answered\r");
                      } else if(buffer[i+0]=='S' && buffer[i+1]=='T' && buffer[i+2]=='A' && buffer[i+3]=='T' && buffer[i+4]=='U' && buffer[i+5]=='S'){
                          SerialMon.print("i is at " + String(i) + " STATUS answered\r");
                      } else if(buffer[i+0]=='P' && buffer[i+1]=='A' && buffer[i+2]=='R' && buffer[i+3]=='A' && buffer[i+4]=='M'){
                          readServerSettings();
                          SerialMon.println("T1value" + String(T1value));
                          SerialMon.print("i is at " + String(i) + " PARAMETERS answered\r");
                      } else if(buffer[i+0]=='U' && buffer[i+1]=='R' && buffer[i+2]=='L'){
                          SerialMon.print("i is at " + String(i) + " URL answered\r");
                      } else {
                          /* SerialMon.println("i is at " + String(i) + "Cannot decode\r"); */
                      }
                  }
                  printSerialATdata();
                  SerialMon.println("Section two");
                  SerialMon.write(buffer,count); // if no data transmission ends, write buffer to hardware serial port
                  clearBufferArray(buffer,count); // call clearBufferArray function to clear the storaged data from the array
                  SerialMon.println("Section three");

                  delay(1000);
            } else {
                  //SerialMon.println("Serial GSM Not available");
            }
            delay(1000);
            //SerialMon.println(" Loop Delay " + String(millis()));
      }
}
