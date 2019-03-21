# CAN Bus to Ethernet Interface
#### This repo contains all the information pertaining to the device that we will be using to convert from CAN Bus to Ethernet in order to transmit data back and forth between the car and the various phones.


  We have elected to use the STM32F103C8T6 (with the STM32duino bootloader) as a microcontroller, the ENC28J60 Ethernet breakout board, and the MCP2515 CAN bus to SPI converter for this project. The following are some useful links that were used in order to get this project to work.


  https://github.com/rogerclarkmelbourne/Arduino_STM32/tree/master/STM32F1/libraries/Serasidis_EtherCard_STM
  
  https://github.com/njh/EtherCard
  
  https://github.com/Seeed-Studio/CAN_BUS_Shield
  
  https://github.com/stm32duino/STM32Examples
  
  https://github.com/UIPEthernet/UIPEthernet
  
  https://github.com/stm32duino/LwIP.git
  
  https://github.com/knolleary/pubsubclient

  https://pubsubclient.knolleary.net/api.html#publish1

  http://mosquitto.org/man/mosquitto-8.html
  
  http://www.scalagent.com/IMG/pdf/Benchmark_MQTT_servers-v1-1.pdf
  
  
