# NodeServer

NodeServer is an IoT monitoring device for ulp (ultra-low-power) sensors communicated using a star network topology based on Android Things on a Raspberry Pi 3.

Decentralized sensor nodes are based on an STM32 platform, providing multiple kinds of sensors on a single device and with simple support for adding new drivers and automatic sensor identification through the corresponding bus. I plan on adding a bootloader system for the STM32 devices to add new drivers via OTA.
Some of the sensors supported by sensor nodes are:

  * Temperature (ST HTS221 and ST LPS25H)
  * Pressure (ST LPS25H)
  * Relative humidity (ST HTS221)
  * Luminosity (LDR analog sensor)
  * Magnetometer (LSM9DS1_M)
  * Accelerometer (LSM9DS1_AG)
  * Gyroscope (LSM9DS1_AG)
  * Battery level monitoring (VBAT analog sensor)

NodeServer will be able to display and monitor surrounding sensor nodes, providing the user a with a familiar User Interface based on Android Things.

The FW for sensor nodes can be found on the following git repository:
https://github.com/Javier-varez/node
