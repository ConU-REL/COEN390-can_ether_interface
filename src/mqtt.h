// function to push data to the MQTT broker
void MQTT_PUSH(){
  String msg;
  critical["rpm"] = rpm;
  critical["oil_temp"]   = o_temp;
  critical["coolant_temp"]   = c_temp;
  critical["fuel_pressure"]   = f_press;
  critical["oil_pressure"]   = o_press;

  serializeJson(critical, msg);
  //Serial.println(msg);
  mqttClient.publish("sensors/critical", msg.c_str());

  non_critical["gear"] = gear;
  non_critical["launch"] = launch ? 1 : 0;
  non_critical["voltage"] = voltage;
  msg = "";
  serializeJson(non_critical, msg);
  //Serial.println(msg);
  mqttClient.publish("sensors/non_critical", msg.c_str());
}


// subscribed topic callback function
void callback(char* topic, uint8_t* payload, unsigned int length){
  String topic_str = String(topic);
  char msg [length];

  for(int i=0; i < length; i++){
    msg[i] = (char) payload[i];
  }

  DeserializationError error = deserializeJson(instructions, msg);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
  if(topic_str == "control/engine"){
    cranking = instructions["crank"];
    if(!cranking){
      crank(1);
    }
  }

  if(topic_str == "control/fuel_pump"){
    pumping = instructions["pump"];
    if(!pumping){
      pump(1);
    }
  }

  if(topic_str == "control/comms"){
    uint8_t flag = instructions["flag"];
    status = (status & 0b10001111) | (flag << 4);
    Serial.print("Sending flag instruction: ");
    Serial.println(status, BIN);
    CAN_SEND();
  }
}
