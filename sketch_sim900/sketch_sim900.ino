#include "tasks.h"
#include "definitions.h"

    
TaskHandle_t Task1Handle = NULL;
TaskHandle_t Task2Handle = NULL;
TaskHandle_t Task3Handle = NULL;
TaskHandle_t Task4Handle = NULL;
TaskHandle_t Task5Handle = NULL;
TaskHandle_t Task6Handle = NULL;
TaskHandle_t Task7Handle = NULL;
TaskHandle_t Task8Handle = NULL;

QueueHandle_t xStructQueue;

void setup() {
  // Set serial monitor debugging window baud rate to 115200
  SerialMon.begin(115200);
  SerialMon.println("Version 0.32 \r");
  //batteryPoweredOn();
  
  mcuGPIOconfiguration();
  //SIM900SoftwareSerialOff();
  SIM900powerON();
  initializeModem();   //Initializing modem
  SerialMon.println("2. Initializaing Modem finished - GSM Core# "+ String(xPortGetCoreID()));
  buzzerRingOnceSuperQuick();
  //EEPROM.begin(256);
  initializeEEPROM();
  readServerSettings();
  readAPNSettings();
  xStructQueue = xQueueCreate(1, /* The number of items the queue can hold. */
                         sizeof( occupancyStatusStruct ) ); /* Size of each item is big enough to hold the whole structure. */

  xTaskCreatePinnedToCore(vTask1, "task1", 4096, (void*)NULL, 2, &Task1Handle, 1); // 2048, 1024 
  /* Read GPS data */

  /* xTaskCreatePinnedToCore(vTask2, "task2", 4096, (void*)NULL, 6, &Task2Handle, 0); // 2048, 1024 */
  /* send GSM data. Unnessory since this function is embedded to Task 8 */
  
  /* xTaskCreatePinnedToCore(vTask3, "task3", 2048, (void*)NULL, 1, &Task3Handle, 1); // 2048, 1024 */ 
  /* calling to the agent */
  
  /* xTaskCreatePinnedToCore(vTask4, "task4", 4096, (void*)NULL, 5, &Task4Handle, 1); // 2048, 1024 */
  /* Testing the Modem with AT commands */
   
  /* xTaskCreatePinnedToCore(vTask5, "task5", 8192, (void*)NULL, 6, &Task5Handle, 0); // 2048, 1024*/
  /* Receiving a Call. Unnessory since this function is embedded to Task 8 */

    xTaskCreatePinnedToCore(vTask6, "task6", 2*4096, (void*)NULL, 3, &Task6Handle, 0); // 2048, 1024
   /* Seat Occupancy Reading*/

   /* //xTaskCreatePinnedToCore(vTask7, "task7", 4096, (void*)NULL, 5, &Task7Handle, 1); // 2048, 1024 */
   /* // I2C Communication with ESP32CAM - Stopped at the moment.*/
  
   xTaskCreatePinnedToCore(vTask8, "vTask8", 2*4096, (void*)NULL, 6, &Task8Handle, 1); // 2048, 1024
  /* Server settings SMS receive */
}

void loop() {
  // put your main code here, to run repeatedly:
  // Serial.println("getCpuFrequencyMhz:"+ String(getCpuFrequencyMhz()));
   delay(20000);
}
