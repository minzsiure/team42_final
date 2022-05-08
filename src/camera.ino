void start_capture() {
  myCAM.clear_fifo_flag();
  myCAM.start_capture();
}

void camCapture(ArduCAM myCAM) {
  uint32_t len  = myCAM.read_fifo_length();
  if (len >= MAX_FIFO_SIZE) //8M
  {
    Serial.println(F("Over size."));
  }
  if (len == 0 ) //0 kb
  {
    Serial.println(F("Size is 0."));
  }
  myCAM.CS_LOW();
  myCAM.set_fifo_burst();
  
  i = 0;
  while ( len-- )
  {
    temp_last = temp;
    temp =  SPI.transfer(0x00);
    //Read JPEG data from FIFO
    if ( (temp == 0xD9) && (temp_last == 0xFF) ) //If find the end ,break while,
    {
      buffer[i++] = temp;  //save the last  0XD9
      //Write the remain bytes in the buffer
      is_header = false;
      myCAM.CS_HIGH();
      break;
    }
    if (is_header == true)
    {
      //Write image data to buffer if not full
      if (i < bufferSize){
        buffer[i++] = temp;
      }
      else
      {
        //Write bufferSize bytes image data to file
        i = 0;
        buffer[i++] = temp;
      }
    }
    else if ((temp == 0xD8) & (temp_last == 0xFF))
    {
      is_header = true;
      buffer[i++] = temp_last;
      buffer[i++] = temp;
    }
  }
  memcpy(holder, buffer, sizeof(buffer));
  memset(image_data, 0, sizeof(image_data));
  base64_encode(image_data, holder, i);
  memset(body, 0, sizeof(body));

  // NOTE: Need to change the variables here!
  sprintf(body,"{\"location\":\'%s\', \"user_id\":\'%s\', \"image_encoding\":\'%s\'}", building_name, user, image_data);
  memset(request_buffer, 0, IN_BUFFER_SIZE);
  memset(response_buffer, 0, OUT_BUFFER_SIZE);
  request_buffer[0] = '\0'; //set 0th byte to null
  int offset = 0; 
  offset += sprintf(request_buffer + offset, "POST /sandbox/sc/team42/608_team42_final/image_request.py HTTP/1.1\r\n");
  offset += sprintf(request_buffer + offset, "Host: 608dev-2.net\r\n");
  offset += sprintf(request_buffer + offset, "Content-Type: application/json\r\n");
  offset += sprintf(request_buffer + offset, "Content-Length: %d\r\n\r\n", strlen(body));
  offset += sprintf(request_buffer + offset, body);
  Serial.println(request_buffer);
  do_http_request("608dev-2.net", cam_request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
  Serial.println("-----------");
  Serial.println(response_buffer);
  Serial.println("-----------");
}

void serverCapture() {
  delay(1000);
  start_capture();
  Serial.println(F("CAM Capturing"));

  int total_time = 0;

  total_time = millis();
  while (!myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK));
  total_time = millis() - total_time;
  Serial.print(F("capture total_time used (in miliseconds):"));
  Serial.println(total_time, DEC);

  total_time = 0;

  Serial.println(F("CAM Capture Done."));
  total_time = millis();
  camCapture(myCAM);
  total_time = millis() - total_time;
  Serial.print(F("send total_time used (in miliseconds):"));
  Serial.println(total_time, DEC);
  Serial.println(F("CAM send Done."));
}

void setup_camera(){
    //Check if the ArduCAM SPI bus is OK
  myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
  temp = myCAM.read_reg(ARDUCHIP_TEST1);
  if (temp != 0x55) {
    Serial.println(F("SPI1 interface Error!"));
    while (1);
  }
  
  //Check if the camera module type is OV2640
  myCAM.wrSensorReg8_8(0xff, 0x01);
  myCAM.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
  myCAM.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
  if ((vid != 0x26 ) && (( pid != 0x41 ) || ( pid != 0x42 )))
    Serial.println(F("Can't find OV2640 module!"));
  else
    Serial.println(F("OV2640 detected."));

  //Change to JPEG capture mode and initialize the OV2640 module
  myCAM.set_format(JPEG);
  myCAM.InitCAM();
  myCAM.OV2640_set_JPEG_size(OV2640_160x120);
  myCAM.clear_fifo_flag();

  // Set up Camera
    uint8_t vid, pid;
    uint8_t temp;
    //set the CS as an output:
    pinMode(CS, OUTPUT);
    pinMode(CAM_POWER_ON , OUTPUT);
    pinMode(KEY, INPUT_PULLUP);
    digitalWrite(CAM_POWER_ON, HIGH);
  #if defined(__SAM3X8E__)
    Wire1.begin();
  #else
    Wire.begin();
  #endif
  //  Serial.begin(115200);
    Serial.println(F("ArduCAM Start!"));
}
