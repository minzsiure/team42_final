#include <Adafruit_BMP280.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <DFRobotDFPlayerMini.h>

Adafruit_BMP280 bmp; // I2C

const char *google_CERT =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDdTCCAl2gAwIBAgILBAAAAAABFUtaw5QwDQYJKoZIhvcNAQEFBQAwVzELMAkG\n"
    "A1UEBhMCQkUxGTAXBgNVBAoTEEdsb2JhbFNpZ24gbnYtc2ExEDAOBgNVBAsTB1Jv\n"
    "b3QgQ0ExGzAZBgNVBAMTEkdsb2JhbFNpZ24gUm9vdCBDQTAeFw05ODA5MDExMjAw\n"
    "MDBaFw0yODAxMjgxMjAwMDBaMFcxCzAJBgNVBAYTAkJFMRkwFwYDVQQKExBHbG9i\n"
    "YWxTaWduIG52LXNhMRAwDgYDVQQLEwdSb290IENBMRswGQYDVQQDExJHbG9iYWxT\n"
    "aWduIFJvb3QgQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDaDuaZ\n"
    "jc6j40+Kfvvxi4Mla+pIH/EqsLmVEQS98GPR4mdmzxzdzxtIK+6NiY6arymAZavp\n"
    "xy0Sy6scTHAHoT0KMM0VjU/43dSMUBUc71DuxC73/OlS8pF94G3VNTCOXkNz8kHp\n"
    "1Wrjsok6Vjk4bwY8iGlbKk3Fp1S4bInMm/k8yuX9ifUSPJJ4ltbcdG6TRGHRjcdG\n"
    "snUOhugZitVtbNV4FpWi6cgKOOvyJBNPc1STE4U6G7weNLWLBYy5d4ux2x8gkasJ\n"
    "U26Qzns3dLlwR5EiUWMWea6xrkEmCMgZK9FGqkjWZCrXgzT/LCrBbBlDSgeF59N8\n"
    "9iFo7+ryUp9/k5DPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNVHRMBAf8E\n"
    "BTADAQH/MB0GA1UdDgQWBBRge2YaRQ2XyolQL30EzTSo//z9SzANBgkqhkiG9w0B\n"
    "AQUFAAOCAQEA1nPnfE920I2/7LqivjTFKDK1fPxsnCwrvQmeU79rXqoRSLblCKOz\n"
    "yj1hTdNGCbM+w6DjY1Ub8rrvrTnhQ7k4o+YviiY776BQVvnGCv04zcQLcFGUl5gE\n"
    "38NflNUVyRRBnMRddWQVDf9VMOyGj/8N7yy5Y0b2qvzfvGn9LhJIZJrglfCm7ymP\n"
    "AbEVtQwdpf5pLGkkeB6zpxxxYu7KyJesF12KwvhHhm4qxFYxldBniYUr+WymXUad\n"
    "DKqC5JlR3XC321Y9YeRq4VzW9v493kHMB65jUr9TU/Qr6cf9tveCX4XSQRjbgbME\n"
    "HMUfpIBvFSDJ3gyICh3WZlXi/EjJKSZp4A==\n"
    "-----END CERTIFICATE-----\n";

const char *Weather_CERT =
    "-----BEGIN CERTIFICATE-----\n"
    "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\n"
    "-----END CERTIFICATE-----\n";

TFT_eSPI tft = TFT_eSPI();

const uint16_t RESPONSE_TIMEOUT = 6000;
const uint16_t IN_BUFFER_SIZE = 3500;  // size of buffer to hold HTTP request
const uint16_t OUT_BUFFER_SIZE = 1000; // size of buffer to hold HTTP response
const uint16_t JSON_BODY_SIZE = 3000;
char request[IN_BUFFER_SIZE];
char response[OUT_BUFFER_SIZE]; // char array buffer to hold HTTP request
char json_body[JSON_BODY_SIZE];
char location[100];

/* CONSTANTS */
// Prefix to POST request:
const char PREFIX[] = "{\"wifiAccessPoints\": [";                 // beginning of json body
const char SUFFIX[] = "]}";                                       // suffix to POST request
const char API_KEY[] = "AIzaSyAQ9SzqkHhV-Gjv-71LohsypXUH447GWX8"; // don't change this and don't share this

uint8_t LCD_CONTROL = 21;
uint8_t LCD_PWM = 0;

double latitude = 0.0;
double longitude = 0.0;

const uint8_t BUTTON45 = 45;
const uint8_t BUTTON39 = 39;
const int MAX_APS = 5;

float values1[50];
int index1 = 0;
float values2[50];
int index2 = 0;

/* Global variables*/
uint8_t button45_state;     // used for containing button state and detecting edges
int old_button45_state;     // used for detecting button edges
uint8_t button39_state;     // used for containing button state and detecting edges
int old_button39_state;     // used for detecting button edges
uint32_t time_since_sample; // used for microsecond timing

WiFiClientSecure client; // global WiFiClient Secure object
WiFiClient client2;      // global WiFiClient Secure object

HardwareSerial mySoftwareSerial(1);
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

// choose one:
const char NETWORK[] = "EECS_Labs";
const char PASSWORD[] = "";
const char USER[] = "Pearl Li";
// const char NETWORK[] = "TackyManchego";
// const char PASSWORD[] = "BannedRomeo6678";

bool show_altitude = true;
int pressure;
int calibrated = 0;
float hayden_offset = 12.0; // floor 1 of Hayden
float stata_offset = 15.0;  // floor 1 of Hayden
float media_offset = 16.0;  // floor 1 of Hayden
float offset = 12.0;
char hayden[] = "Hayden Library";
char stata[] = "Stata Center";
char media[] = "Media Lab";
bool audio_enabled = false;

char request_buffer[IN_BUFFER_SIZE];   // char array buffer to hold HTTP request
char response_buffer[OUT_BUFFER_SIZE]; // char array buffer to hold HTTP

uint8_t channel = 1; // network channel on 2.4 GHz

char *SERVER = "googleapis.com"; // Server URL
uint32_t timer;

void setup()
{
  mySoftwareSerial.begin(9600, SERIAL_8N1, 18, 17); // speed, type, RX, TX
  Serial.begin(115200);

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  delay(1000);
  if (!myDFPlayer.begin(mySoftwareSerial))
  { // Use softwareSerial to communicate with mp3.

    Serial.println(myDFPlayer.readType(), HEX);
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true)
      ;
  }
  Serial.println(F("DFPlayer Mini online."));

  myDFPlayer.setTimeOut(500); // Set serial communictaion time out 500ms

  myDFPlayer.volume(25);   // Set volume value (0~30).
  myDFPlayer.volumeUp();   // Volume Up
  myDFPlayer.volumeDown(); // Volume Down
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);

  Serial.println(F("readState--------------------"));
  Serial.println(myDFPlayer.readState()); // read mp3 state
  Serial.println(F("readVolume--------------------"));
  Serial.println(myDFPlayer.readVolume()); // read current volume
  Serial.println(F("readFileCounts--------------------"));
  Serial.println(myDFPlayer.readFileCounts()); // read all file counts in SD card
  Serial.println(F("--------------------"));

  if (!bmp.begin())
  {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                     "try a different address!"));
    while (1)
      delay(10);
  }
  pinMode(LCD_CONTROL, OUTPUT);
  ledcSetup(LCD_PWM, 100, 12);         // 12 bits of PWM precision
  ledcWrite(LCD_PWM, 0);               // 0 is a 0% duty cycle for the PFET...increase if you'd like to dim the LCD.
  ledcAttachPin(LCD_CONTROL, LCD_PWM); // Set up serial port

  // SET UP SCREEN:
  tft.init();                            // init screen
  tft.setRotation(2);                    // adjust rotation
  tft.setTextSize(1);                    // default font size, change if you want
  tft.fillScreen(TFT_BLACK);             // fill background
  tft.setTextColor(TFT_PINK, TFT_BLACK); // set color of font to hot pink foreground, black background

  // SET UP BUTTON:
  delay(100); // wait a bit (100 ms)
  pinMode(BUTTON45, INPUT_PULLUP);
  pinMode(BUTTON39, INPUT_PULLUP);

  // PRINT OUT WIFI NETWORKS NEARBY
  int n = WiFi.scanNetworks();
  if (n == 0)
  {
    Serial.println("no networks found");
  }
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      Serial.printf("%d: %s, Ch:%d (%ddBm) %s ", i + 1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i), WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "open" : "");
      uint8_t *cc = WiFi.BSSID(i);
      for (int k = 0; k < 6; k++)
      {
        Serial.print(*cc, HEX);
        if (k != 5)
          Serial.print(":");
        cc++;
      }
      Serial.println("");
    }
  }
  delay(100); // wait a bit (100 ms)

  // if using regular connection use line below:
  WiFi.begin(NETWORK, PASSWORD);
  // if using channel/mac specification for crowded bands use the following:
  // WiFi.begin(network, password, channel, bssid);

  uint8_t count = 0; // count used for Wifi check times
  Serial.print("Attempting to connect to ");
  Serial.println(NETWORK);
  while (WiFi.status() != WL_CONNECTED && count < 12)
  {
    delay(500);
    Serial.print(".");
    count++;
  }
  delay(2000);
  if (WiFi.isConnected())
  { // if we connected then print our IP, Mac, and SSID we're on
    Serial.println("CONNECTED!");
    Serial.printf("%d:%d:%d:%d (%s) (%s)\n", WiFi.localIP()[3], WiFi.localIP()[2],
                  WiFi.localIP()[1], WiFi.localIP()[0],
                  WiFi.macAddress().c_str(), WiFi.SSID().c_str());
    delay(500);
  }
  else
  { // if we failed to connect just Try again.
    Serial.println("Failed to Connect :/  Going to restart");
    Serial.println(WiFi.status());
    ESP.restart(); // restart the ESP (proper way)
  }
  timer = millis();
  old_button45_state = 1;
  old_button39_state = 1;
}

// main body of code
void loop()
{
  button45_state = digitalRead(BUTTON45);
  button39_state = digitalRead(BUTTON39);

  if (!button45_state && button45_state != old_button45_state)
  {
    audio_enabled = true;
    Serial.printf("audio state = %d\n", int(audio_enabled));
  }

  // ------------------ interaction with serial monitor to control speaker ------------------
  if (Serial.available())
  {
    String inData = "";
    inData = Serial.readStringUntil('\n');
    if (inData.startsWith("n"))
    {
      Serial.println(F("next--------------------"));
      myDFPlayer.next();
      Serial.println(myDFPlayer.readCurrentFileNumber()); // read current play file number
    }
    else if (inData.startsWith("p"))
    {
      Serial.println(F("previous--------------------"));
      myDFPlayer.previous();
      Serial.println(myDFPlayer.readCurrentFileNumber()); // read current play file number
    }
    else if (inData.startsWith("+"))
    {
      Serial.println(F("up--------------------"));
      myDFPlayer.volumeUp();
      Serial.println(myDFPlayer.readVolume()); // read current volume
    }
    else if (inData.startsWith("-"))
    {
      Serial.println(F("down--------------------"));
      myDFPlayer.volumeDown();
      Serial.println(myDFPlayer.readVolume()); // read current volume
    }
    else if (inData.startsWith("*"))
    {
      Serial.println(F("pause--------------------"));
      myDFPlayer.pause();
    }
    else if (inData.startsWith(">"))
    {
      Serial.println(F("start--------------------"));
      myDFPlayer.start();
    }
  }

  if (calibrated == 1 && show_altitude)
  {
    float temp = bmp.readTemperature();
    float alt = bmp.readAltitude(pressure);
    float avg_alt = averaging_filter(alt, values1, 5, &index1);
    // Serial.printf("Approx altitude = %f, temp = %f\n", alt, temp);

    tft.setCursor(0, 0);
    tft.fillScreen(TFT_BLACK); // fill background
    tft.printf("lat: %f\nlon: %f\n", latitude, longitude);

    int floor_num = 1;
    tft.printf("raw avg_alt: %.2f\n", avg_alt);

    // ------------------ play audio ------------------
    if (strcmp(hayden, location) == 0)
    {
      tft.printf("adjustsed: %.2f\n", avg_alt + hayden_offset);
      if (avg_alt + hayden_offset > 4.0)
        floor_num = 2;
      if (audio_enabled)
      {
        myDFPlayer.play(1); // Play the first mp3
        delay(20000);
      }
    }
    else if (strcmp(media, location) == 0)
    {
      tft.printf("adjustsed: %.2f\n", avg_alt + media_offset);
      if (avg_alt + media_offset > 6.0)
        floor_num = 3;
      else if (avg_alt + media_offset > 2.0)
        floor_num = 2;
      if (audio_enabled)
      {
        myDFPlayer.play(3); // Play the third mp3
        delay(25000);
      }
    }
    else if (strcmp(stata, location) == 0)
    {
      tft.printf("adjustsed: %.2f\n", avg_alt + stata_offset);
      if (avg_alt + stata_offset > 3.0)
        floor_num = 2;
      if (audio_enabled)
      {
        myDFPlayer.play(4); // Play the fourth mp3
        delay(32000);
      }
    }
    else
    {
      tft.printf("avg_alt: %.2f\n", avg_alt + offset);
      if (audio_enabled)
      {
        myDFPlayer.play(2); // Play the second mp3
        delay(10000);
      }
    }
    audio_enabled = false;

    tft.printf("%s\nfloor %d\n", location, floor_num);
    delay(2000);
  }



  if (calibrated == 0 || (!button39_state && button39_state != old_button39_state))
  {
    int offset = sprintf(json_body, "%s", PREFIX);
    int n = WiFi.scanNetworks(); // run a new scan. could also modify to use original scan from setup so quicker (though older info)
    Serial.println("scan done");
    if (n == 0)
    {
      Serial.println("no networks found");
    }
    else
    {
      int max_aps = max(min(MAX_APS, n), 1);
      for (int i = 0; i < max_aps; ++i)
      {                                                                                                                           // for each valid access point
        uint8_t *mac = WiFi.BSSID(i);                                                                                             // get the MAC Address
        offset += wifi_object_builder(json_body + offset, JSON_BODY_SIZE - offset, WiFi.channel(i), WiFi.RSSI(i), WiFi.BSSID(i)); // generate the query
        if (i != max_aps - 1)
        {
          offset += sprintf(json_body + offset, ","); // add comma between entries except trailing.
        }
      }
      sprintf(json_body + offset, "%s", SUFFIX);
      int len = strlen(json_body);
      // Make a HTTP request:
      Serial.println("SENDING REQUEST");
      request[0] = '\0'; // set 0th byte to null
      offset = 0;        // reset offset variable for sprintf-ing
      offset += sprintf(request + offset, "POST https://www.googleapis.com/geolocation/v1/geolocate?key=%s  HTTP/1.1\r\n", API_KEY);
      offset += sprintf(request + offset, "Host: googleapis.com\r\n");
      offset += sprintf(request + offset, "Content-Type: application/json\r\n");
      offset += sprintf(request + offset, "cache-control: no-cache\r\n");
      offset += sprintf(request + offset, "Content-Length: %d\r\n\r\n", len);
      offset += sprintf(request + offset, "%s\r\n", json_body);
      do_https_request(SERVER, request, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false, google_CERT);

      // Serial.println(request);
      if (strlen(response) == 0)
      {
        return;
      }

      char *begin;
      char *end;
      begin = strchr(response, '{');
      end = strrchr(response, '}');
      // Serial.println(response);

      char json[200];
      strncpy(json, begin, end - begin + 1);
      json[end - begin + 1] = '\0';

      DynamicJsonDocument doc(2048);
      DeserializationError error = deserializeJson(doc, json);

      // Test if parsing succeeds.
      if (error)
      {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }
      latitude = doc["location"]["lat"];
      longitude = doc["location"]["lng"];
      Serial.printf("lat: %f, lon: %f\n", latitude, longitude);

      response[0] = '\0'; // clear string
      request[0] = '\0';  // set 0th byte to null
      memset(request, 0, IN_BUFFER_SIZE);

      sprintf(request, "GET https://weather.visualcrossing.com/VisualCrossingWebServices/rest/services/timeline/%f,%f?unitGroup=metric&include=current&contentType=json&elements=pressure&key=68VU3DB49RVD69VYQV44KCCF2 HTTP/1.1\r\n", latitude, longitude);
      strcat(request, "Host: weather.visualcrossing.com\r\n");
      strcat(request, "\r\n"); // add blank line!
      do_https_request("weather.visualcrossing.com", request, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false, Weather_CERT);

      DynamicJsonDocument pressure_doc(1000);
      begin = strchr(response, '{');
      end = strrchr(response, '}');
      *(end + 1) = NULL;
      error = deserializeJson(pressure_doc, begin);
      // Test if parsing succeeds.
      if (error)
      {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.f_str());
      }
      else
      {
        pressure = pressure_doc["currentConditions"]["pressure"];
      }

      Serial.printf("pressure: %d\n", pressure);

      response[0] = '\0'; // clear string
      request[0] = '\0';  // set 0th byte to null
      memset(request, 0, IN_BUFFER_SIZE);

      sprintf(request, "GET https://608dev-2.net/sandbox/sc/team42/608_team42_final/location.py?lat=%f&lon=%f HTTP/1.1\r\n", latitude, longitude);
      strcat(request, "Host: 608dev-2.net\r\n"); // add more to the end
      strcat(request, "\r\n");                   // add blank line!
      // submit to function that performs GET.  It will return output using response_buffer char array
      do_http_request("608dev-2.net", request, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);

      location[0] = 0;
      memset(location, 0, 100);
      sprintf(location, response);
      location[strcspn(location, "\n")] = 0;

      calibrated = 1;
    }
  }
  old_button39_state = button39_state;
  old_button45_state = button45_state;
}
