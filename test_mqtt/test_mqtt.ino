#include <UIPEthernet.h>
#include "PubSubClient.h"

#define CLIENT_ID "MOD_CAN"
#define PUBLISH_DELAY 1000

// module mac address
uint8_t mac[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};

// module ip addr
IPAddress ip_mod(10, 10, 10, 20);

// MQTT server addr
IPAddress ip_srv(10, 10, 10, 10);
// gateway ip address
IPAddress ip_gw(10, 10, 10, 10);

// domain name server (dns) address
IPAddress ip_dns(10, 10, 10, 10);

EthernetClient ethClient;
PubSubClient mqttClient;

void setup(){
  Serial.begin(115200);
  while(Serial.read() <= 0);
  Serial.println("starting");

  Ethernet.begin(mac, ip_mod);

  Serial.println(Ethernet.localIP().toString());
  Serial.println(ethClient.connect(ip_srv, 1883));

  Serial.println(Ethernet.linkStatus() == LinkON);

  mqttClient.setClient(ethClient);
  mqttClient.setServer(ip_srv, 1883);
}

void loop(){
  //Serial.println(mqttClient.state());

  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect(CLIENT_ID)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
  mqttClient.loop();
  Serial.println("Publishing to topic_test");
  mqttClient.publish("topic_test", "test123");

}
