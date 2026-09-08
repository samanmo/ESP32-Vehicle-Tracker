/*
 * A header file in C/C++ contains:
 * Function definitions
 * Data type definitions
 * Macros
 */
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

void vTask1(void *p);
void vTask2(void *p);
void vTask3(void *p);
void vTask4(void *p);
void vTask5(void *p);
void vTask6(void *p);
void vTask7(void *p);
void vTask8(void *p);
extern void readGSPdata(void);
extern void initializeModem(void);
extern void sendGSMdata(void);
extern bool setPowerBoostKeepOn(int en);
extern void batteryPoweredOn(void);
extern void unlockSim(void);
extern void mcuGPIOconfiguration(void);
extern void clearBufferArray(unsigned char* buffer, int count);
extern void SIM900powerON(void);
extern void ServerURLParsing(const unsigned char* Received_string, const int N);
extern void writeCharArrayIntoEEPROM(const int address, char character[], const int arraySize);
extern void readCharArrayFromEEPROM(const int address, char character[], const int arraySize);
extern void readServerSettings(void);
extern void SIM900SoftwareSerialOff(void);
extern void printSerialATdata(void);
extern void buzzerRingOnce(void);
extern void buzzerRingOncequick(void);
extern void buzzerRingOnceSuperQuick(void);
extern void TestSIM900Connection(void);
extern void updateSerial(void);
extern void writedataSerialAT(void);
extern void initializeEEPROM(void);
extern void TimerParsing(const unsigned char* Received_string, const int N );
extern int getT1value(void);
extern int getT2value(void);
extern void GPRSinitializeSIM900(void);
extern void APNParsing(const unsigned char* Received_string, const int N );
extern void readAPNSettings();

struct occupancyStatusStruct{
  int occupancyStatus_int;
};

extern QueueHandle_t xStructQueue;
extern int occupancyStatus_int;
