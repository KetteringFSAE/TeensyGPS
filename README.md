##Teensy GPS Shield Library for GPS Data Logging

###Introduction
This is a fork of ![TeensyGPS](https://github.com/smokingresistor/TeensyGPS), for the board sold by ![Smoking Resistor](http://www.smokingresistor.com/product/teensy-3x-gps-shield-can/). It has been modified and simplified for our use case. We have removed support for SD Card logging and configuration, IMU, and the Kalman filter. The raw GPS data is filtered to some extent, and the resulting data is output in J1939 compliant CAN messages. 
