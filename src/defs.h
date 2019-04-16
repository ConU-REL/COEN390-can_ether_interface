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
DynamicJsonDocument instructions(1024);

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


// can bus status vars
bool curr_can_status = 0;
bool last_can_status = 0;

// variables for receiving CAN messages
uint8_t recv_len;
uint8_t recv_msg [9];

// variables for sending CAN messages
uint8_t status = 0;
uint8_t buff [1] = {status};

// data variables that will contain informatino about the vehicle
float voltage;
int v_int, v_fl, o_temp, o_press, c_temp, f_press, f_temp, gear, rpm;
bool launch, traction, autoshift, remote_start;

// engine crank vars
bool cranking = 0;
unsigned int time_cranking = 0;
unsigned int max_time_cranking = 5000;

// fuel pump vars
bool pumping = 0;
unsigned int time_pumping = 0;
unsigned int max_time_pumping = 5000;


// keepalive
unsigned int time_last_keepalive = 0;
unsigned int time_keepalive_interval = 2000;
