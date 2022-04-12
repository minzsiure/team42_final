#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

/* CONSTANTS */
// Prefix to POST request:
const char PREFIX[] = "{\"wifiAccessPoints\": [";                 // beginning of json body
const char SUFFIX[] = "]}";                                       // suffix to POST request
const char API_KEY[] = "AIzaSyAQ9SzqkHhV-Gjv-71LohsypXUH447GWX8"; // don't change this and don't share this
const int MAX_APS = 5;

/* Global variables*/
uint32_t time_since_sample; // used for microsecond timing

uint8_t channel = 1;                                 // network channel on 2.4 GHz
byte bssid[] = {0x04, 0x95, 0xE6, 0xAE, 0xDB, 0x41}; // 6 byte MAC address of AP you're targeting.

/*----------------------------------
    wifi_object_builder: generates a json-compatible entry for use with Google's geolocation API
    Arguments:
        * `char* object_string`: a char pointer to a location that can be used to build a c-string with a fully-contained JSON-compatible entry for one WiFi access point
        *  `uint32_t os_len`: a variable informing the function how much  space is available in the buffer
        * `uint8_t channel`: a value indicating the channel of WiFi operation (1 to 14)
        * `int signal_strength`: the value in dBm of the Access point
        * `uint8_t* mac_address`: a pointer to the six long array of `uint8_t` values that specifies the MAC address for the access point in question.

    Return value:
        the length of the object built. If not entry is written,
*/
int wifi_object_builder(char *object_string, uint32_t os_len, uint8_t channel, int signal_strength, uint8_t *mac_address)
{
    char str[100];
    // printf("start sprintf\n");
    sprintf(str, "{\"macAddress\": \"%02x:%02x:%02x:%02x:%02x:%02x\",\"signalStrength\": %d,\"age\": 0,\"channel\": %d}", mac_address[0], mac_address[1], mac_address[2], mac_address[3], mac_address[4], mac_address[5], signal_strength, channel);
    // printf("end sprintf\n");
    if (strlen(str) > os_len)
        return 0;
    else
    {
        strcpy(object_string, str);
        return strlen(str);
    }
}

char *GOOGLE_SERVER = "googleapis.com"; // Server URL

/*----------------------------------
    get_location: writes current location based on wifi into given locations
    Arguments:
        double* lat: pointer to the space to store current latitude
        double* lon: pointer to the space to store current longitude
*/
void get_location(double* lat, double* lon)
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
        {                                                                                                                             // for each valid access point
            uint8_t *mac = WiFi.BSSID(i);                                                                                             // get the MAC Address
            offset += wifi_object_builder(json_body + offset, JSON_BODY_SIZE - offset, WiFi.channel(i), WiFi.RSSI(i), WiFi.BSSID(i)); // generate the query
            if (i != max_aps - 1)
            {
                offset += sprintf(json_body + offset, ","); // add comma between entries except trailing.
            }
        }

        sprintf(json_body + offset, "%s", SUFFIX);
        Serial.println(json_body);
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

        do_https_request(GOOGLE_SERVER, request, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);

        Serial.println("-----------");
        Serial.println(response);
        Serial.println("-----------");

        char *starting = strchr(response, '{');
        char *ending = strrchr(response, '}');
        *(ending + 1) = '\0';
        Serial.printf("%s\n", starting);
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, starting);

        *lat = doc["location"]["lat"];
        *lon = doc["location"]["lng"];
        Serial.printf("Parsed, %f, %f, %f\n", *lat, *lon, doc["accuracy"]);
        /*
        tft.setCursor(0, 0);
        tft.printf("Current Location:\n");
        tft.printf("Lat: %.7f\n", lat);
        tft.printf("Lon: %.7f\n", lon);
        */
    }
}
