
/*https communication requires certificates to be known ahead of time so each entity will trust one another.
   This is a cert for google's generic servers below, allowing us to connect over https with their resources
*/


/*----------------------------------
   char_append Function:
   Arguments:
      char* buff: pointer to character array which we will append a
      char c:
      uint16_t buff_size: size of buffer buff

   Return value:
      boolean: True if character appended, False if not appended (indicating buffer full)
*/
uint8_t char_append(char* buff, char c, uint16_t buff_size) {
  int len = strlen(buff);
  if (len > buff_size) return false;
  buff[len] = c;
  buff[len + 1] = '\0';
  return true;
}



/*----------------------------------
 * do_http_request Function:
 * Arguments:
 *    char* host: null-terminated char-array containing host to connect to
 *    char* request: null-terminated char-arry containing properly formatted HTTP request
 *    char* response: char-array used as output for function to contain response
 *    uint16_t response_size: size of response buffer (in bytes)
 *    uint16_t response_timeout: duration we'll wait (in ms) for a response from server
 *    uint8_t serial: used for printing debug information to terminal (true prints, false doesn't)
 * Return value:
 *    void (none)
 */
void do_http_request(char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint8_t serial){
  if (client2.connect(host, 80)) { //try to connect to host on port 80
    if (serial) Serial.print(request);//Can do one-line if statements in C without curly braces
    client2.print(request);
    memset(response, 0, response_size); //Null out (0 is the value of the null terminator '\0') entire buffer
    uint32_t count = millis();
    while (client2.connected()) { //while we remain connected read out data coming back
      client2.readBytesUntil('\n',response,response_size);
      if (serial) Serial.println(response);
      if (strcmp(response,"\r")==0) { //found a blank line!
        break;
      }
      memset(response, 0, response_size);
      if (millis()-count>response_timeout) break;
    }
    memset(response, 0, response_size);  
    count = millis();
    while (client2.available()) { //read out remaining text (body of response)
      char_append(response,client2.read(),OUT_BUFFER_SIZE);
    }
    if (serial) Serial.println(response);
    client2.stop();
    if (serial) Serial.println("-----------");  
  }else{
    if (serial) Serial.println("connection failed :/");
    if (serial) Serial.println("wait 0.5 sec...");
    client2.stop();
  }
} 

/*----------------------------------
   do_https_request Function. Similar to do_http_request, but supports https requests
   Arguments:
      char* host: null-terminated char-array containing host to connect to
      char* request: null-terminated char-arry containing properly formatted HTTP request
      char* response: char-array used as output for function to contain response
      uint16_t response_size: size of response buffer (in bytes)
      uint16_t response_timeout: duration we'll wait (in ms) for a response from server
      uint8_t serial: used for printing debug information to terminal (true prints, false doesn't)
   Return value:
      void (none)
*/
void do_https_request(char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint8_t serial, const char* cert) {
  client.setHandshakeTimeout(30);
  client.setCACert(cert); //set cert for https
  if (client.connect(host,443,4000)) { //try to connect to host on port 443
    if (serial) Serial.print(request);//Can do one-line if statements in C without curly braces
    delay(10);
    client.print(request);
    response[0] = '\0';
    //memset(response, 0, response_size); //Null out (0 is the value of the null terminator '\0') entire buffer
    uint32_t count = millis();
    while (client.connected()) { //while we remain connected read out data coming back
      client.readBytesUntil('\n', response, response_size);
      if (serial) Serial.println(response);
      if (strcmp(response, "\r") == 0) { //found a blank line!
        break;
      }
      memset(response, 0, response_size);
      if (millis() - count > response_timeout) break;
    }
    memset(response, 0, response_size);
    count = millis();
    while (client.available()) { //read out remaining text (body of response)
      char_append(response, client.read(), OUT_BUFFER_SIZE);
    }
    if (serial) Serial.println(response);
    client.stop();
    if (serial) Serial.println("-----------");
  } else {
    if (serial) Serial.println("connection failed :/");
    if (serial) Serial.println("wait 0.5 sec...");
    client.stop();
  }
}

int wifi_object_builder(char* object_string, uint32_t os_len, uint8_t channel, int signal_strength, uint8_t* mac_address) {
    char body[100]; //for body
    sprintf(body,"{\"macAddress\": \"%.2x:%.2x:%.2x:%.2x:%.2x:%.2x\",\"signalStrength\": %d,\"age\": %d,\"channel\": %d}",
                    *(mac_address), *(mac_address + 1), *(mac_address + 2), *(mac_address + 3), *(mac_address + 4), *(mac_address + 5), 
                    signal_strength, 0, channel);//
    strcat(body,"\0");
    if(strlen(body) < os_len) {
      strcpy(object_string, body);
      return strlen(body);
    } else {
      return 0;
    }
}

float averaging_filter(float input, float* stored_values, int order, int* index) {
    float sum = 0;
    stored_values[*index] = input;
    *index = *index + 1;
    if(*index > order) *index = 0;
    
    for (int i = 0; i <= order; i++) {
      sum += stored_values[i];
    }
    return sum / float(order + 1);
}
