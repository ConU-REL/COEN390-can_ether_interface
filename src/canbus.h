// function to receive and process CAN messages
void CAN_RECEIVE(){
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

void CAN_SEND(){
  buff[0] = status;
  CAN.sendMsgBuf(transmit_id, 0, 1, buff);
}
