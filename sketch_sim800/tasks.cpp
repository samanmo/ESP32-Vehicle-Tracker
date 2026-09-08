#include "tasks.h"
#include "definitions.h"
#include <string.h>
#include <Wire.h>
#include <HardwareSerial.h>
#include <EEPROM.h>

static char incoming_char=0;




void vTask2(void *p){
  SerialMon.println("2.  vTask2 - GSM - delay start for settling ");
  vTaskDelay(60000);
  SerialMon.println("2.  vTask2 - GSM - delay finished for settling ");
  /*
  initializeModem();   //Initializing modem
  SerialMon.println("2. Initializaing Modem finished - GSM Core# "+ String(xPortGetCoreID()));
  readServerSettings();
  */
  static unsigned long time_stamp1 = micros();
  while(1){
    time_stamp1 = micros();
    SerialMon.print("2. vTask2 while loop Core# "+ String(xPortGetCoreID()));
    sendGSMdata();
    SerialMon.println(" Delay " + String(micros()-time_stamp1));
    //vTaskDelay(120000);
    vTaskDelay(12000);
    //delay(5000);
  }
}


void vTask3(void *p){
  SerialMon.println("3. Initializaing vTask3");
  pinMode(CALL_BTN, INPUT_PULLUP);
  pinMode(CALL_STATE, OUTPUT);

  int state = HIGH;      // the current state of the output pin
  int reading = LOW;     // the current reading from the pin
  int previous = LOW;    // the previous reading from the pin

  long time = 0;         // the last time the output pin was toggled
  long debounce = 200;   // the debounce time, increase if the output flickers

  pinMode(CALL_BTN, INPUT_PULLUP);
  //pinMode(CALL_STATE, OUTPUT);

  int stateCALL_BTN = HIGH;      // the current state of the output pin
  int readingCALL_BTN = LOW;     // the current reading from the inputCALL_BTN pin
  int previousCALL_BTN = LOW;    // the previous reading from the inputCALL_BTN pin

  long timeCALL_BTN = 0;         // the last time the output pin was toggled
  int calling = 0;
  static unsigned long time_stamp1 = micros();
  
  while(1){
      time_stamp1 = micros();
      vTaskDelay(500);
      //delay(300);
      reading = digitalRead(CALL_BTN);
      // if the input just went from LOW and HIGH and we've waited long enough
      // to ignore any noise on the circuit, toggle the output pin and remember
      // the time
      if (reading == HIGH && previous == LOW && millis() - time > debounce) {
        if (state == HIGH)
            state = LOW;
        else
            state = HIGH;
        time = millis();
      }
      //SerialMon.print("Core#"+ String(xPortGetCoreID()) + "Pin12-"+String(reading)+" ");
      digitalWrite(CALL_STATE, state);
      previous = reading;

      ///////////////////////////////////////////////////////////////
      ///////////////////////////////////////////////////////////////

      readingCALL_BTN = digitalRead(CALL_BTN);
      // if the input just went from LOW and HIGH and we've waited long enough
      // to ignore any noise on the circuit, toggle the output pin and remember
      // the time
      if (readingCALL_BTN == HIGH && previousCALL_BTN == LOW && millis() - time > debounce) {
        if (stateCALL_BTN == HIGH)
            stateCALL_BTN = LOW;
        else
            stateCALL_BTN = HIGH;
        time = millis();
      }
      //SerialMon.println("Core#"+ String(xPortGetCoreID()) + "PinCALL_BTN-" + String(readingCALL_BTN)+ "");
      previousCALL_BTN = readingCALL_BTN;

      if(state==LOW && calling == 0 ){
          SerialAT.println("ATD+94701303505;"); //  change ZZ with country code
          SerialMon.print("calling one");
          SerialMon.println(" Delay " + String(micros()-time_stamp1));
          calling = 1;
          state   = HIGH;
          stateCALL_BTN = HIGH;
      }else if(stateCALL_BTN==LOW){
          SerialAT.println("ATH");
          SerialMon.print("calling zero hang off");
          SerialMon.println(" Delay " + String(micros()-time_stamp1));
          calling = 0;
          stateCALL_BTN = HIGH;
          state   = HIGH;
      }else{
          calling = calling;
          stateCALL_BTN = stateCALL_BTN;
          state   = state;
      }
  }
}

void vTask4(void *p){
  SerialMon.println("4. Initializaing vTask4");
  vTaskDelay(60000);
  static unsigned long time_stamp1 = micros();
  while(1){
     SerialMon.print("4. vTask4 while loop on Core# " + String(xPortGetCoreID()));
     SerialMon.println(" Delay " + String(micros()-time_stamp1));
     time_stamp1 = micros();
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
     
     //vTaskDelay(2000);
     //delay(2000);
  }
}

void vTask5(void *p){
  static unsigned long time_stamp1 = micros();
  vTaskDelay(2000);
  vTaskDelay(60000);
  SerialMon.println("5. Initializaing vTask5");
 
  SerialAT.print("AT+CLIP=1\r"); // turn on caller ID notification
  while(1){
     time_stamp1 = micros();
     SerialMon.print("5. vTask5 while loop on Core# " + String(xPortGetCoreID()));
     //delay(1000);
     //vTaskDelay(1000);
     //delay(300);
     // Display any text that the GSM shield sends out on the serial monitor
      if(SerialAT.available()>0) {
        // Get the character from the cellular serial port
        // With an incomming call, a "RING" message is sent out
        incoming_char=SerialAT.read();
        //  Check if the shield is sending a "RING" message
        if (incoming_char=='R') {
          delay(10); //10
          //Serial.print(incoming_char);
          incoming_char=SerialAT.read();
          if (incoming_char =='I') {
            delay(10); //10
            //Serial.print(incoming_char);
            incoming_char=SerialAT.read();
            if (incoming_char=='N') {
              delay(10); //10
              //Serial.print(incoming_char);
              incoming_char=SerialAT.read();
              if (incoming_char=='G') {
                delay(10); //10
                //Serial.print(incoming_char);
                // If the message received from the shield is RING
                // Send ATA commands to answer the phone
                SerialAT.print("ATA\r");
              }
            }
          }
        }
      }
      SerialMon.println(" Delay " + String(micros()-time_stamp1));
      delay(1000);
  }
}




void vTask7(void *p){
  SerialMon.println("7. Initializaing vTask7");
  /*Wire.setPins(I2C_ESP32_SDA, I2C_ESP32_SCL); // 'class TwoWire' has no member named 'setPins' */
  Wire.setClock(10000);
  Wire.begin(I2C_ESP32_SDA, I2C_ESP32_SCL, (uint32_t)10000); //no matching function for call to 'TwoWire::begin(int, int, int, int)'
  
  while(1){
      uint8_t data[3] = {12, 13, 14};
      delay(2000);
      Wire.beginTransmission(11);
      Wire.write(data, 3);
      Wire.endTransmission();
      delay(1000);
      Wire.requestFrom(11, 3);
      while(Wire.available()){
//        byte a = Wire.read();
        char a = Wire.read();

        Serial.println(a);
      }
     //vTaskDelay(2000);
     //delay(2000);
  }
}
