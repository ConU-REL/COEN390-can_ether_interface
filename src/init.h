void init_mcp(){
  // attempt connection to CAN bus module until it works
  while(!(CAN_OK == CAN.begin(CAN_1000KBPS, MCP_CS))){
  	Serial.println("MCP2515 Connection Failed, retrying");
  	delay(100);
  }

  // if connected successfully, print and continue
  Serial.println("MCP2515 Connection Successfull!");

  // initialize CAN bus to listen for messages on recv_id
  CAN.init_Filt(0, 0, recv_id);
}


void init_connection() {
  // initialize ethernet
  Ethernet.begin(mac, ip_mod);

  //Serial.println(ENC28J60ControlCS);
  Serial.print("Link status: ");
  Serial.println(Ethernet.linkStatus() == LinkON);


  // connect to MQTT server
  mqttClient.setClient(ethClient);
  mqttClient.setServer(ip_srv, 1883);
  mqttClient.setCallback(callback);


  while(!mqttClient.connected()){
    Serial.println("Attempting connection to MQTT Broker");
    //Serial.println(mqttClient.connect(CLIENT_ID));
    if(!mqttClient.connect(CLIENT_ID)){
      Serial.println("MQTT Connection failed, retrying");
    }
    delay(100);
  }
  Serial.println("MQTT Connection successful");

  // subscribe to the topic which will have engine instructions
  mqttClient.subscribe("control/engine", 0);
}
