// function prototypes
void init_mcp();
void init_connection();
void CAN_RECEIVE();
void MQTT_PUSH();
void callback(char*, uint8_t*, unsigned int);
void crank(bool stop = false);
void set_cranking(bool);
void pump(bool stop = false);
void set_pumping(bool);
