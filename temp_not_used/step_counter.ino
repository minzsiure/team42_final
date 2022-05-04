#include <WiFi.h> //Connect to WiFi Network
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> //Used in support of TFT Display
#include <string.h>  //used for some string handling and processing.
#include <mpu6050_esp32.h>
#include<math.h>

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

char network[] = "MIT";
char password[] = "";

uint8_t channel = 1; //network channel on 2.4 GHz
byte bssid[] = {0x04, 0x95, 0xE6, 0xAE, 0xDB, 0x41}; //6 byte MAC address of AP you're targeting.

const uint8_t LOOP_PERIOD = 10; //milliseconds
int WAITING_PERIOD = 5000; //milliseconds
uint32_t primary_timer = 0;
uint32_t posting_timer = 0;
uint32_t step_timer = 0;
float x, y, z; //variables for grabbing x,y,and z values
const char USER[] = "The Hungry Beaver";

const float ZOOM = 9.81; //for display (converts readings into m/s^2)...used for visualizing only

//Some constants and some resources:
const int RESPONSE_TIMEOUT = 6000; //ms to wait for response from host
const int POSTING_PERIOD = 6000; //periodicity of getting a number fact.
const uint16_t IN_BUFFER_SIZE = 1000; //size of buffer to hold HTTP request
const uint16_t OUT_BUFFER_SIZE = 1000; //size of buffer to hold HTTP response
char request_buffer[IN_BUFFER_SIZE]; //char array buffer to hold HTTP request
char response_buffer[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP response

const uint8_t BUTTON1 = 45; //pin connected to button
const uint8_t BUTTON2 = 39; //pin connected to button
MPU6050 imu; //imu object called, appropriately, imu
float old_acc_mag;  //previous acc mag
float older_acc_mag;  //previous prevoius acc mag

//some suggested variables you can use or delete:

uint8_t state;  //system state for step counting
uint8_t post_state; //state of posting
uint8_t total;
int steps;
uint8_t display_state;

const uint8_t IDLE = 0;  //change if you'd like
const uint8_t DOWN = 1;  //change if you'd like
const uint8_t POST = 2;  //change if you'd like

const int REST = 0;
const int WALK_UP = 1;
const int WALK_DOWN = 2;
unsigned long timer;  //used for storing millis() readings.
const float peak = 9.5;
const float trough = 8.5;
bool new_step = false;

void setup() {
  tft.init();  //init screen
  tft.setRotation(3); //adjust rotation
  tft.setTextSize(1); //default font size
  tft.fillScreen(TFT_BLACK); //fill background
  tft.setTextColor(TFT_GREEN, TFT_BLACK); //set color of font to green foreground, black background
  Serial.begin(115200); //begin serial comms
  delay(100); //wait a bit (100 ms)
  Wire.begin();
  delay(50); //pause to make sure comms get set up
  if (imu.setupIMU(1)) {
    // Serial.println("IMU Connected!");
  } else {
    // Serial.println("IMU Not Connected :/");
    // Serial.println("Restarting");
    ESP.restart(); // restart the ESP (proper way)
  }

  int n = WiFi.scanNetworks();
  // Serial.println("scan done");
  if (n == 0) {
    // Serial.println("no networks found");
  } else {
    // Serial.print(n);
    // Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Serial.printf("%d: %s, Ch:%d (%ddBm) %s ", i + 1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "open" : "");
      uint8_t* cc = WiFi.BSSID(i);
      for (int k = 0; k < 6; k++) {
        // Serial.print(*cc, HEX);
        if (k != 5) Serial.print(":");
        cc++;
      }
      // Serial.println("");
    }
  }
  delay(100); //wait a bit (100 ms)

  //if using regular connection use line below:
  WiFi.begin(network, password);
  //if using channel/mac specification for crowded bands use the following:
  //WiFi.begin(network, password, channel, bssid);

  uint8_t count = 0; //count used for Wifi check times
  // Serial.print("Attempting to connect to ");
  // Serial.println(network);
  while (WiFi.status() != WL_CONNECTED && count < 12) {
    delay(500);
    // Serial.print(".");
    count++;
  }
  delay(2000);
  if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
    // Serial.println("CONNECTED!");
    // Serial.println(WiFi.localIP().toString() + " (" + WiFi.macAddress() + ") (" + WiFi.SSID() + ")");
    delay(500);
  } else { //if we failed to connect just Try again.
    // Serial.println("Failed to Connect :/  Going to restart");
    // Serial.println(WiFi.status());
    ESP.restart(); // restart the ESP (proper way)
  }
  pinMode(BUTTON1, INPUT_PULLUP); //set input pin as an input!
  pinMode(BUTTON2, INPUT_PULLUP); //set input pin as an input!

  state = 0; //
  steps = 0;
  post_state = IDLE; //initialize to something!
  display_state = 0;
}

void loop() {
  char output[100];
  //get IMU information:
  imu.readAccelData(imu.accelCount);
  float x, y, z;
  x = ZOOM * imu.accelCount[0] * imu.aRes;
  y = ZOOM * imu.accelCount[1] * imu.aRes;
  z = ZOOM * imu.accelCount[2] * imu.aRes;
  float acc_mag = sqrt(x * x + y * y + z * z);
  float avg_acc_mag = 1.0 / 3.0 * (acc_mag + old_acc_mag + older_acc_mag);
  older_acc_mag = old_acc_mag;
  old_acc_mag = acc_mag;

  //get button readings:
  uint8_t button1 = digitalRead(BUTTON1);
  uint8_t button2 = digitalRead(BUTTON2);

  if(button2 == 1) {
    if(display_state == 1) {
      tft.fillScreen(TFT_BLACK); //fill background
    }
    display_state = 0;
    sprintf(output, "%d", steps); //render 
    tft.setCursor(0, 0, 4);
    tft.println(output);
  } else {
    display_state = 1;
    tft.setCursor(0, 0, 4);
    tft.println(response_buffer);
  }

  step_reporter_fsm(avg_acc_mag); //run step_reporter_fsm (from lab02a)
  // post_reporter_fsm(button1); //run post_reporter_fsm (written here)

  Serial.printf("%d %d\n", millis() - posting_timer, millis() - posting_timer > WAITING_PERIOD);
  if (new_step && millis() - posting_timer > WAITING_PERIOD) { // when there are new steps and past the waiting period
    Serial.println("hello?");
    char body[100]; //for body
    sprintf(body,"username=%s&step=%d",USER,steps);//generate body, posting to User, 1 step
    int body_len = strlen(body); //calculate body length (for header reporting)
    sprintf(request_buffer,"POST http://608dev-2.net/sandbox/sc/team42/608_team42_final/create_step_db.py HTTP/1.1\r\n");
    strcat(request_buffer,"Host: 608dev-2.net\r\n");
    strcat(request_buffer,"Content-Type: application/x-www-form-urlencoded\r\n");
    sprintf(request_buffer+strlen(request_buffer),"Content-Length: %d\r\n", body_len); //append string formatted to end of request buffer
    strcat(request_buffer,"\r\n"); //new line from header to body
    strcat(request_buffer,body); //body
    strcat(request_buffer,"\r\n"); //new line
    do_http_request("608dev-2.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT,true);
    tft.fillScreen(TFT_BLACK); //black out TFT Screen
    steps = 0;
    new_step = false;
  }
  while (millis() - primary_timer < LOOP_PERIOD);
  primary_timer = millis();
}

//Step Counting FSM from Lab02A.  (bring over and adapt global variables as needed!)
void step_reporter_fsm(float avg_acc_mag) {
  switch(state){
    case REST:     
      if(avg_acc_mag >= peak) {
        state = WALK_UP;
      }
      break;
    
    case WALK_UP:
      if(avg_acc_mag <= trough) {
        state = WALK_DOWN;
        timer = millis();
      }
      break;
    
    case WALK_DOWN:
      if (millis() - timer > 5000) {
        state = REST;
      }
      else if(avg_acc_mag >= peak && millis() - timer > 200) {
        state = WALK_UP;
        steps++;    
        new_step = true;    
        posting_timer = millis();
      }
      break;
  }
}