vTask1 (GPS Parsing): Reads raw NMEA data from a NEO-6 GPS module and updates strings for latitude, longitude, speed, date, and time using the TinyGPS++ library.
vTask6 (Seat Occupancy Tracking): Monitors a sensor pin (SEAT_OCCUPANCY). 
When triggered, it logs a "1" or "0" and passes this status packet into a FreeRTOS data queue (xStructQueue).
vTask8 (The Main Control Hub):Receives occupancy status data out of the FreeRTOS queue.Listens to the SIM800 modem for incoming SMS commands. 
It parses incoming texts to dynamically reconfigure settings like the APN, Server URL, System Identification Number, and Report Timers directly into the ESP32’s non-volatile EEPROM storage.
Periodically initiates an HTTP POST session over GPRS using the configured server parameters to transmit vehicle location and metadata tracking strings.
