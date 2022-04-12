#include <WiFi.h> //Connect to WiFi Network
#include <SPI.h>
#include <TFT_eSPI.h>
#include <mpu6050_esp32.h>
#include <math.h>
#include <string.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

// TFT-related definitions

TFT_eSPI tft = TFT_eSPI();
const int SCREEN_HEIGHT = 160;
const int SCREEN_WIDTH = 128;
const int BUTTON_PIN_1 = 45;
const int BUTTON_PIN_2 = 39;
const int BUTTON_PIN_3 = 38;
const int BUTTON_PIN_4 = 34;

const int LOOP_PERIOD = 40;
const int LOCATION_PERIOD = 5000;

MPU6050 imu; // imu object called, appropriately, imu

char network[] = "MIT"; // SSID for 6.08 Lab
char password[] = "";   // Password for 6.08 Lab

// Some constants and some resources:
const uint16_t RESPONSE_TIMEOUT = 6000; // time for claiming the HTTP response as TIMEOUT
const uint16_t IN_BUFFER_SIZE = 3500;   // size of buffer to hold HTTP request
const uint16_t OUT_BUFFER_SIZE = 1000;  // size of buffer to hold HTTP response
const uint16_t JSON_BODY_SIZE = 3000;   // size of buffer to hold JSON body
char request[IN_BUFFER_SIZE];
char response[OUT_BUFFER_SIZE]; // char array buffer to hold HTTP request
char json_body[JSON_BODY_SIZE];

uint32_t primary_timer;
uint32_t location_timer;

WiFiClientSecure client; // global WiFiClient Secure object
WiFiClient client2;      // global WiFiClient Secure object

// used to get x,y values from IMU accelerometer!
void get_angle(float *x, float *y)
{
    imu.readAccelData(imu.accelCount);
    *x = imu.accelCount[0] * imu.aRes;
    *y = imu.accelCount[1] * imu.aRes;
}

// enum for button states
enum button_state
{
    BS0,
    BS1,
    BS2,
    BS3,
    BS4
};

class Button
{
public:
    uint32_t S2_start_time;
    uint32_t button_change_time;
    uint32_t debounce_duration;
    uint32_t long_press_duration;
    uint8_t pin;
    uint8_t flag;
    uint8_t button_pressed;
    button_state state; // This is public for the sake of convenience
    Button(int p)
    {
        flag = 0;
        state = BS0;
        pin = p;
        S2_start_time = millis();      // init
        button_change_time = millis(); // init
        debounce_duration = 10;
        long_press_duration = 1000;
        button_pressed = 0;
    }
    void read()
    {
        uint8_t button_val = digitalRead(pin);
        button_pressed = !button_val; // invert button
    }
    int update()
    {
        read();
        flag = 0;
        if (state == 0)
        {
            if (button_pressed)
            {
                state = BS1;
                button_change_time = millis();
            }
        }
        else if (state == 1)
        {
            if (button_pressed && millis() - button_change_time > debounce_duration)
            {
                state = BS2;
                S2_start_time = millis();
            }
            else if (!button_pressed && millis() - button_change_time <= debounce_duration)
            {
                state = BS0;
                button_change_time = millis();
            }
        }
        else if (state == 2)
        {
            if (!button_pressed)
            {
                state = BS4;
                button_change_time = millis();
            }
            else if (button_pressed && millis() - S2_start_time > long_press_duration)
            {
                state = BS3;
            }
        }
        else if (state == 3)
        {
            if (!button_pressed)
            {
                state = BS4;
                button_change_time = millis();
            }
        }
        else if (state == 4)
        {
            if (button_pressed && millis() - button_change_time <= debounce_duration)
            {
                if (millis() - S2_start_time > long_press_duration)
                {
                    state = BS3;
                    button_change_time = millis();
                }
                else
                {
                    state = BS2;
                    button_change_time = millis();
                }
            }
            else if (!button_pressed && millis() - button_change_time > debounce_duration)
            {
                state = BS0;
                flag = (millis() - S2_start_time <= long_press_duration) ? 1 : 2;
            }
        }
        return flag;
    }
};

enum beaver_state
{
    S0, // Menu state (initial state)
    S01,
    S02,
    S03, // Starting states
    S04,
    S05, // Statistics states
    S10, // New Building state
    S11,
    S111, // Builidng intro states
    S12,  // Direction to next building state
    S13,
    S131, // Photo states
    S14,  // Step counter state
    S15,
    S151, // Feedback state
    S21,
    S22 // End trip state
};

class PocketBeaver
{
private:
    double cur_lat;      // current latitude of ESP32
    double cur_lon;      // current longitude of ESP32
    char cur_intro[400]; // introduction for current building (too long to store every introduction locally)
    int cur_no;          // current building Number (e.g. 7 = this is the seventh building in this route)

    char building_ids[20][10];     // list of building ID in trip (e.g. "W20")
    double building_lat[20];       // list of building center point latitudes in trip
    double building_lon[20];       // list of building center point longtitudes in trip
    char building_intros[20][400]; // TO BE DEPRECATED: list of building introductions

    char foo_direction[200]; // TO BE DEPRECATED: direction text telling user to do it themselves

    char route_name[10][20];
    int total_route;
    int route_selection;

    beaver_state state, old_state;

    bool compare(double lat, double lon)
    {
        return (abs(cur_lat - lat) < 1e-6 && abs(cur_lon - lon) < 1e-6);
    }

    void get_route_info()
    {
        return;
    }

    void get_current_building()
    {
        return;
    }

    void get_building_intro()
    {
        // this function should query an API in the team server. Fake data and local storage is used for now.
        int pos;
        for (pos = 0; pos < 2; pos++)
        {
            if (strcmp(building_ids[pos], building_ids[cur_no]) == 0)
            {
                sprintf(cur_intro, "%s", building_intros[pos]);
                return;
            }
        }
        sprintf(cur_intro, "Cannot find building introduction, please contact support team!");
    }

    void get_direction()
    {
        // this function should return direction to show compass, based on cur_no and building locations.
        return;
    }

public:
    PocketBeaver()
    {
        state = S0;
        old_state = S111;
        cur_lat = 0;
        cur_lon = 0;
        // should be getting all possible routes from team server, using fake data right now
        total_route = 1;
        sprintf(route_name[0], "Cross Mass Ave");
        route_selection = 0;
        // should be initializing magnetometer and IMU here
        sprintf(foo_direction, "We trust you to find the direction yourself! Next building is: %s", building_ids[0]);
    }
    void update(double lat,
                double lon,
                int button1,
                int button2,
                int button3,
                int button4)
    {

        bool location_changed = false;
        if (compare(lat, lon))
        {
            location_changed = true;
            cur_lat = lat;
            cur_lon = lon;
            get_current_building();
        }

        switch (state)
        {
        case S0:
            if (old_state != state)
            {
                tft.fillScreen(TFT_BLACK);
                tft.setCursor(0, 0);
                tft.println("Welcome to Pocket Beaver! Press Any Button to Start.");
            }
            old_state = state;
            if (button1 == 1 || button2 == 1 || button3 == 1 || button4 == 1)
            {
                state = S01;
            }
            break;

        case S01:
            if (old_state != state)
            {
                tft.fillScreen(TFT_BLACK);
                tft.setCursor(0, 0);
                tft.printf("Please choose between the %d routes below, scrolling with Buttons 2 and 3, confirming with 1:\n");
                for (int i = 0; i < total_route; i++)
                {
                    tft.printf("Route %d: %s", i, route_name[i]);
                }
            }
            old_state = state;
            if (button1 == 1)
                state = S02;
            else if (button2 == 1)
                route_selection = (route_selection + 1) % total_route;
            else if (button3 == 1)
                route_selection = (route_selection - 1 + total_route) % total_route;
            break;

        case S02:
            if (old_state != state)
            {
                tft.fillScreen(TFT_BLACK);
                tft.setCursor(0, 0);
                tft.printf("Selected Route %d! Downloading Route Info ...", route_selection);
                get_route_info();
                cur_no = -1;
                old_state = state;
                state = S03;
            }
            break;

        case S03:
            if (old_state != state || location_changed)
            {
                // should be printing compass here, printing foo direction instead
                tft.fillScreen(TFT_BLACK);
                tft.setCursor(0, 0);
                tft.printf("%s%s", foo_direction, building_ids[0]);
            }
            if (cur_no == 0)
            {
                tft.fillScreen(TFT_BLACK);
                tft.setCursor(0, 0);
                tft.printf("Reached starting point!");
                sleep(10);
                old_state = state;
                state = S0;
            }
            else
            {
                old_state = state;
            }
            break;

        default:
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(0, 0);
            tft.printf("Error State: %d", state);
            while (1)
                ; // TO BE DEPRECATED: dangerous!
            break;
        }
        // initialize route with fake data right now
        sprintf(building_ids[0], "W20");
        sprintf(building_ids[1], "Infinite");
        building_lat[0] = 42.359039377388186;
        building_lon[0] = -71.09481585478274;
        sprintf(building_intros[0], "Centrally located, the Stratton Student Center offers places to gather, study, dine, do errands, and relax. MIT students view the Student Center as their out-of-the-classroom home.");
        building_lat[1] = 42.359299025673785;
        building_lon[1] = -71.09294879617293;
        sprintf(building_intros[1], "The Infinite Corridor is a 251-meter (823 ft) hallway that runs through the main buildings of the Massachusetts Institute of Technology, specifically parts of the buildings numbered 7, 3, 10, 4, and 8 (from west to east).");
    }
};

PocketBeaver pb;              // wikipedia object
Button button1(BUTTON_PIN_1); // button object!
Button button2(BUTTON_PIN_2); // button object!
Button button3(BUTTON_PIN_3); // button object!
Button button4(BUTTON_PIN_4); // button object!

void setup()
{
    Serial.begin(115200); // for debugging if needed.

    // SET UP SCREEN:
    tft.init();                            // init screen
    tft.setRotation(2);                    // adjust rotation
    tft.setTextSize(1);                    // default font size, change if you want
    tft.fillScreen(TFT_BLACK);             // fill background
    tft.setTextColor(TFT_PINK, TFT_BLACK); // set color of font to hot pink foreground, black background
    tft.serCursor(0, 0);
    tft.println("Initializing......");

    // SET UP BUTTON:
    delay(100); // wait a bit (100 ms)
    pinMode(BUTTON_PIN_1, INPUT_PULLUP);
    pinMode(BUTTON_PIN_2, INPUT_PULLUP);
    pinMode(BUTTON_PIN_3, INPUT_PULLUP);
    pinMode(BUTTON_PIN_4, INPUT_PULLUP);

    // PRINT OUT WIFI NETWORKS NEARBY
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
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

    WiFi.begin(network, password); // attempt to connect to wifi

    uint8_t count = 0; // count used for Wifi check times
    Serial.print("Attempting to connect to ");
    Serial.println(network);
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
    if (imu.setupIMU(1))
    {
        Serial.println("IMU Connected!");
    }
    else
    {
        Serial.println("IMU Not Connected :/");
        Serial.println("Restarting");
        ESP.restart(); // restart the ESP (proper way)
    }

    primary_timer = millis();
    location_timer = millis();
}

void loop()
{

    float x, y;
    get_angle(&x, &y);          // get angle values
    int bv1 = button1.update(); // get button1 value
    int bv2 = button2.update(); // get button2 value
    int bv3 = button3.update(); // get button3 value
    int bv4 = button4.update(); // get button4 value

    double lat, lon;
    if (millis() - location_timer > LOCATION_PERIOD)
    {
        get_location(&lat, &lon);
        location_timer = millis();
    }

    pb.update(lat, lon, bv1, bv2, bv3, bv4);

    while (millis() - primary_timer < LOOP_PERIOD)
        ;

    primary_timer = millis();
}
