#include "defs.h"
#include "prototypes.h"
#include "init.h"
#include "canbus.h"
#include "mqtt.h"


void setup(){
  Serial.begin(115200);
  // wait for user input before starting
  while(Serial.read() <= 0);
  Serial.println("Serial connected.");

  // initialize modules
  init_mcp();
  init_connection();


  // start by saying that the ECU is not connected
  mqttClient.publish("status/module", "{\"ecu_conn\":0}");
}


void loop(){
  c_time = millis();
  // when CAN message is available, receive and process it
  if(CAN_MSGAVAIL == CAN.checkReceive()){
    CAN_RECEIVE();
    MQTT_PUSH();
    last_can_update = c_time;

    // send the ecu connection status to MQTT only if it has changed
    curr_can_status = 1;
    if(curr_can_status != last_can_status){
      last_can_status = curr_can_status;
      mqttClient.publish("status/module", "{\"ecu_conn\":1}");
    }
  } else if((c_time - last_can_update) > can_timeout){
    // send the ecu connection status to MQTT only if it has changed
    curr_can_status = 0;
    if(curr_can_status != last_can_status){
      last_can_status = curr_can_status;
      mqttClient.publish("status/module", "{\"ecu_conn\":0}");
    }
  }
  mqttClient.loop();  // keepalive
  if(cranking){
    crank();
  }
}


void crank(bool kill){
  c_time = millis();
  if(kill){
    // stop cranking
    Serial.println("Sending instruction to stop cranking");
    time_cranking = 0;
    cranking = 0;
    set_cranking(0);
    return;
  }
  if(time_cranking == 0){
    // start cranking
    Serial.println("Sending instruction to start cranking");
    time_cranking = c_time;
    set_cranking(1);

  } else if((c_time - time_cranking > max_time_cranking)){
    // stop cranking
    Serial.println("Sending instruction to stop cranking");
    time_cranking = 0;
    cranking = 0;
    set_cranking(0);
  }
}


void set_cranking(bool state){
  status = state ? (status | 0b10000000) : (status & 0b01111111);
  CAN_SEND();
}


void pump(){
  c_time = millis();
  if(time_pumping == 0){
    // start the pump
    Serial.println("Sending instruction to start the pump");
    time_pumping = c_time;
    set_pumping(1);
  } else if(c_time - time_pumping > max_time_pumping){
    // stop the pump
    Serial.println("Sending instruction to stop the pump");
    time_pumping = 0;
    pumping = 0;
    set_pumping(0);
  }
}


void set_pumping(bool state){
  status = state ? (status | 0b00010000) : (status & 11101111);
}
