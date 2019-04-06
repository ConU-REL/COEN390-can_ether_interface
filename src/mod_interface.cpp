#include <Arduino.h>
#include <mcp_can_stm.h>
#include <SPI.h>
#include <UIPEthernet.h>
#include "PubSubClient.h"
#define ARDUINOJSON_ENABLE_PROGMEM 0
#include <ArduinoJson.h>

// pinout definitions
#define MCP_CS PB12

// declare listen address for CAN Bus
#define recv_id 0x90
#define transmit_id 0x94

// declare CAN bus object
MCP_CAN CAN;

DynamicJsonDocument critical(1024);
DynamicJsonDocument non_critical(1024);


// ethernet and MQTT defs

#define CLIENT_ID "MOD_CAN"
#define PUBLISH_DELAY 1000

// module mac address
uint8_t mac[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
// module ip addr
IPAddress ip_mod(10, 0, 22, 20);
// MQTT server addr
IPAddress ip_srv(10, 0, 22, 10);
// gateway ip address
IPAddress ip_gw(10, 0, 22, 1);
// domain name server (dns) address
IPAddress ip_dns(10, 0, 22, 1);

EthernetClient ethClient;
PubSubClient mqttClient;


// current time
long unsigned int c_time = millis();
// timer to check if CAN has timed out
long unsigned int last_can_update = 0;
// define what constitutes a timeout
unsigned int can_timeout = 5000;

bool curr_can_status = 0;
bool last_can_status = 1;

// variables for receiving CAN messages
uint8_t recv_len;
uint8_t recv_msg [9];

// data variables that will contain informatino about the vehicle
float voltage;
int v_int, v_fl, o_temp, o_press, c_temp, f_press, f_temp, gear, rpm;
bool launch, traction, autoshift, remote_start;


void setup(){
  Serial.begin(115200);
  // wait for user input before starting
  while(Serial.read() <= 0);
  Serial.println("Serial connected.");

  // attempt connection to CAN bus module until it works
  while(!(CAN_OK == CAN.begin(CAN_1000KBPS, MCP_CS))){
  	Serial.println("MCP2515 Connection Failed, retrying");
  	//Serial.println(CAN.begin(CAN_1000KBPS, MCP_CS));
  	delay(100);
  }

  // if connected successfully, print and continue
  Serial.println("MCP2515 Connection Successfull!");

  // initialize CAN bus to listen for messages on recv_id
  CAN.init_Filt(0, 0, recv_id);

  // code to send messages on CAN, will be used eventually
  //CAN.sendMsgBuf(transmit_id, 0, 4, buff);

  // initialize ethernet
  Ethernet.begin(mac, ip_mod);

  Serial.println(ENC28J60ControlCS);
  Serial.print("Link status: ");
  Serial.println(Ethernet.linkStatus() == LinkON);


  // connect to MQTT server
  mqttClient.setClient(ethClient);
  mqttClient.setServer(ip_srv, 1883);


  while(!mqttClient.connected()){
    Serial.println("Attempting connection to MQTT Broker");
    Serial.println(mqttClient.connect(CLIENT_ID));
    if(mqttClient.connect(CLIENT_ID)){
      Serial.println("MQTT Connection failed, retrying");
    }
    delay(100);
  }
  Serial.println("MQTT Connection successful");

}

// function prototypes
void CAN_RECEIVE();
void MQTT_PUSH();

void loop(){
  c_time = millis();
  // when CAN message is available, receive and process it
  if(CAN_MSGAVAIL == CAN.checkReceive()){
    CAN_RECEIVE();
    MQTT_PUSH();
    /*
    Serial.print(gear); Serial.print("\t");
    Serial.print(launch); Serial.print("\t");
    Serial.print(traction); Serial.print("\t");
    Serial.print(autoshift); Serial.print("\t");
    Serial.print(rpm); Serial.print("\t");
    Serial.print(voltage); Serial.print("\t");
    Serial.println();
    */
    last_can_update = c_time;
    curr_can_status = 1;
    if(curr_can_status != last_can_status){
      last_can_status = curr_can_status;
      mqttClient.publish("status/module", "{\"ecu_conn\":1}");
    }
  } else if((c_time - last_can_update) > can_timeout){
    curr_can_status = 0;
    if(curr_can_status != last_can_status){
      last_can_status = curr_can_status;
      mqttClient.publish("status/module", "{\"ecu_conn\":0}");
    }
  }
  mqttClient.loop();  // keepalive

}

// function to receive and process CAN messages
void CAN_RECEIVE()

{
  // read the buffer, write data into data vars
  CAN.readMsgBuf(&recv_len, recv_msg);

  // verify that the message was meant for us
  if(CAN.getCanId() == recv_id)
  {
    // extract information from the message and store it
    gear = ((recv_msg[0] & 0b11110000) >> 4)-2;
    launch = (recv_msg[0] & 0b00001000) >> 3;
    traction = (recv_msg[0] & 0b00000100) >> 2;
    autoshift = (recv_msg[1] & 0b11110000) >> 4;
    rpm = (recv_msg[2] * 60);
    voltage = (recv_msg[3] / 10.0);
    f_press = (recv_msg[4] * 4);
    o_temp = (recv_msg[5] * 4);
    c_temp = (recv_msg[6] * 4);
    o_press = (recv_msg[7] * 4);
  }
  // extended message
  else if(CAN.getCanId() == recv_id + 2)
  {
    f_temp = (recv_msg[0] * 4);
  }
}


void MQTT_PUSH(){
  String msg;
  critical["rpm"] = rpm;
  critical["oil_temp"]   = o_temp;
  critical["coolant_temp"]   = c_temp;
  critical["fuel_pressure"]   = f_press;
  critical["oil_pressure"]   = o_press;

  serializeJson(critical, msg);
  Serial.println(msg);
  mqttClient.publish("sensors/critical", msg.c_str());

  non_critical["gear"] = gear;
  non_critical["launch"] = launch ? 1 : 0;
  non_critical["voltage"] = voltage;
  msg = "";
  serializeJson(non_critical, msg);
  Serial.println(msg);
  mqttClient.publish("sensors/non_critical", msg.c_str());
}
