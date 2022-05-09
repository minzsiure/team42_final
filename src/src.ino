#include <WiFi.h> //Connect to WiFi Network
#include <SPI.h>
#include <TFT_eSPI.h>
#include <mpu6050_esp32.h>
#include <math.h>
#include <string.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <Adafruit_LSM303DLH_Mag.h>
#include <Adafruit_LSM303_Accel.h>
#include <Adafruit_Sensor.h>
#include <ArduCAM.h>
#include "memorysaver.h"
#include <Wire.h>
#include <DFRobotDFPlayerMini.h>

#define NEEDLE_LENGTH 40         // Visible length
#define NEEDLE_WIDTH 5           // Width of needle - make it an odd number
#define NEEDLE_RADIUS 60         // Radius at tip
#define NEEDLE_COLOR1 TFT_MAROON // Needle periphery colour
#define NEEDLE_COLOR2 TFT_RED    // Needle centre colour
#define SCREEN_CENTRE_X 64
#define SCREEN_CENTRE_Y 80

// TFT-related definitions

char fake_photo_base64[] = "/9j/4AAQSkZJRgABAQEAAAAAAAD/2wBDAAwICQsJCAwLCgsODQwOEh4UEhEREiUaHBYeLCYuLSsmKikwNkU7MDNBNCkqPFI9QUdKTU5NLzpVW1RLWkVMTUr/2wBDAQ0ODhIQEiMUFCNKMioySkpKSkpKSkpKSkpKSkpKSkpKSkpKSkpKSkpKSkpKSkpKSkpKSkpKSkpKSkpKSkpKSkr/xAAfAAABBQEBAQEBAQAAAAAAAAAAAQIDBAUGBwgJCgv/xAC1EAACAQMDAgQDBQUEBAAAAX0BAgMABBEFEiExQQYTUWEHInEUMoGRoQgjQrHBFVLR8CQzYnKCCQoWFxgZGiUmJygpKjQ1Njc4OTpDREVGR0hJSlNUVVZXWFlaY2RlZmdoaWpzdHV2d3h5eoOEhYaHiImKkpOUlZaXmJmaoqOkpaanqKmqsrO0tba3uLm6wsPExcbHyMnK0tPU1dbX2Nna4eLj5OXm5%2Bjp6vHy8/T19vf4%2Bfr/xAAfAQADAQEBAQEBAQEBAAAAAAAAAQIDBAUGBwgJCgv/xAC1EQACAQIEBAMEBwUEBAABAncAAQIDEQQFITEGEkFRB2FxEyIygQgUQpGhscEJIzNS8BVictEKFiQ04SXxFxgZGiYnKCkqNTY3ODk6Q0RFRkdISUpTVFVWV1hZWmNkZWZnaGlqc3R1dnd4eXqCg4SFhoeIiYqSk5SVlpeYmZqio6Slpqeoqaqys7S1tre4ubrCw8TFxsfIycrS09TV1tfY2dri4%2BTl5ufo6ery8/T19vf4%2Bfr/wAARCAB4AKADASEAAhEBAxEB/9oADAMBAAIRAxEAPwDKtrH5izDJNX0sIW6xitDK5MumW/8AcpWsIMf6taVh8zMOe2RWnG0cNmuf1S38uTI70mCKIqSXmGpKFtfu1YiOJ6AOosoLeW2FxFK0V0vbHBrbsL5opPs9117GmSbHFIcUhXI2dfUVBLcxRruZwB60CuZ1zrNmEO24Gaof8JBbRtlpWk9sVoqcguWbXUxfgeTGdpbac1rmA4%2B7USVgRlJzyBUodlqxi%2Be1NMpNIZmTYa5cH%2BNaxtTUPbf7S0gMTZkZqNz8lSWLE1SxvzQB23hIrcRMp9a6e%2BsI5bYs/VBxTMxdItGmRPNztf7prfj0GPHzAUgHjQ4/9mq%2BpeF4dQtGt2k2A91pxdncLMyE%2BGmmD7007fjTv%2BFdaaDwuR/tMa3%2BsyJ9mJceHbbS7yyhtl2pK3NdR/Zdv6VjJ31Gonn6xhErC1nUZLe6jjicYP3gOtDLRae/jWAyE44pNLuZruHfLGF9KB20C9TY6Se9ZepJtWT0YUEnORdxUL9KksRKEPNAHa%2BBM7pnP3RwK6fVr0xWJXPzPwKCDpvDQC6eitW7QOIUUFiblHUionu7dPvTIPxpE3Od1/V7QX1i6zoRE2TzWxDrNpcxl7eTzVHdaZNzz6ebceKzW02GS7W4/jFWMs3VotxbtF93d3osLMWUHlKxYe9IL9Av0zbPWXf4bTNx9KQjmY12kNUDDMW6kWRDpTVoA7jwe6xae7H%2B/WjE/wDaeomQn9zD933NBJ3Xh9vk2UzxZJf2lol1YN/qj830oJ6nDTeLteYkeeFqpJr%2Bty/evmH0qLmvsynLe6hJ9%2B%2Bk/Oqkjyt9%2B5kP40XHyFOd1jGSzH8a6XwcwaxlZe796ZMkKt9byfdmX86txOCMgitSSYGnCkIjuRmMisO8O6x8s%2BtIDCv/AN2iAelU5FxbJ70iysaBQB0uiM8lr9mj/jbk12FhbrbqFFIk6LS5NhremMU1q8b8q60EniHiM3en6rNb4IAPy8dqyTc3jf3vyqbG3MxpN439%2BmGO5PXNPQWpA6up%2BavR/DWntYaWgf8A1knzNTJZ57ChllVB1aupi02eHS5IhJ85Oc1Vi2znxd3MZ4mf866Gwub63sp57pjjb8maRTsZ48R3Q/1u1xVi0vbe5XdLIFwfumgiUexnapIl3e4iPyKKq6jhFjQdhQSZ7UCgZ2/hKIfYzIR0aumhpEmrZHBrdtHQjaefegk5f4l6MZNOGo2w/eQff91rgLXw5rV/AlxCV8qQZX56LFKVkSnwZrOPmkX8CTT4vAl/K21rtFOM96Wg%2BYz5PDs9ldxib5k3jDetehkPs4joJueYaNHv1GKuyum22cp/2avoaPc4WIb5VHqa9AazjnsxDL92kEzCfRYIpzKv%2BqHY1zsu3exQYXNA4slls2jtEnwfmqgxJNAug1qVBzQI7nw58unoPU5rooaRJfgPNblrKrKPX2oEaO1by2eGZPlYYINZ2l6X/Z1hFajpFwPpUsRK8NZrx/6eR/0zqRnMa%2Bnywe0h/nW9sGwc1Qjyrw0m68LegrodVfZp0x9q06Gv2jldMi86%2BiX/AGq78DIxSCZm6%2B4ttPf1fiuNtIvPuo4%2BxNIPsnRas629l5dct5fVqBELdaltk3PTA7jRI/Lt4x6jNbsdIktRse1a1mQQMnaaBGvC2Oh3Gs7xdNqtvYLPpPzuD8yYzSewkcJd33i%2B8TBhnQe2BS6LZ66%2Bor/an2hbbBz84rL3TW9loVb6K9iuZ/P3eRu/c7mzWTdrr0MvyrMw7bGzVq1hX1KllZX9tOu1SoJ5q5rBvZXMKIxi%2Blaj0uHh/TLlLxZXQoo9a63oKQpbnF63c3s8hSZGCK3HFVNIlit7zfNSLa0L2pLJezAYwOwrPvk8o7fSggzGFauh2/nS0wO0ii8qSJf9itBKRJOprRtNp60gNKG58s4IJrSRlniKY60COcurUQyFC0nH%2B2aqSQxn%2B8f%2BBVz2NzE16JI40KjvVbVfL2RxhimRnitUQ92NUirC10Eko4qRTUiElgEykECs%2Bbw7asAyriQHNIaYr2giDSP1xXOSWkl5KxC8VIzHuYtku2uq8LWW1A7DrVAzoLoYeNvTiplNIkmzVu0fnFAzWXOKt20pDYbigkzPGFpeNZ/arGby3TqMZzXm82vXwnWMakp/vHy%2BlZ2VzRPQhn1aRiVubxZl7YWoLe%2BmvLmXfMGWMfJmqILS3M3XKipftcuN3m1qWWrfUskCQfjWrGcjNBLJ0qbbxUsRVuY9ymq7QiO3OBUlGHFohuJvNlGBnpXR2sCwrhegqhCXfzJRG%2BVqQLAapI5Nppgb1mVuY/l4NWfsq95QKCSyiK8ZieUMCK8z1jwa6X8nk7NrNnnNIaZnXfhR0X5BvrKawu9NupDNY/u26FhkCgdzRhsgB8559qlNkrDhiK1HcYkQQbT%2Bla1hL8vlN1WkM0Uq2P8AV1LJKh5pmN1IY9VxQzUCIJDmo4jjikUWVNTJTJL1tOVPXitRJFxneooEP/tS1jH7yRaxvFrw6npbNY3O25j5XHelcLM8pOt33eeT86P7dv8AH/Hw9MdjUF8Wt%2BPlcetS292TZ/Ock1RVi5byRfZoynDjtVhBILmJ26t1xTEa6VaB4qRELpzUO7b2pDG%2BaackU0v3I2NADXt516xN%2BVQKjCXlTUjLC8CoLjVIrfgfM3tVBFXZVXUL28fZbrj6VtWGizvh7qQn2rM2ska39nqFxgVz3iGKSxtmeBM0hM87a3kVsSrgmpJoV%2BzA4%2BatDI0fsnA7Zo8to16nnhRVDNSytWW3Lyj5iOKusG3QKOopiNJW29asWyvO2EFIk3rTTkjGZOTU8trbOMGMVJqojINPtE/5ZiriiNBhEAqSrEUsn%2ByKrSCF/vxqaQ7FeSysZVIKYzWe3hvTSeMj8aYbGhaWVpZJiJQKsGdRSAha6FUr0pcRMjd6QjzrW08q6VD2zVVyqJGW6VfQy6mntjj%2BYtmrFr5G3zd29vetALC3hjVuAfSp7OR0Tc/32oBlq3DXEu0V1Vhbrbx8daTHBE8s%2BBUKy5NZmxOrU7zcUARTSis2e6xSApve%2B9It/wC9AC/bvemm896AIHu/emG8pCOV8WviaGYc9qzpl32qcgVa2MZbn//Z";

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite needle = TFT_eSprite(&tft); // Sprite object for needle
uint16_t *tft_buffer;
bool buffer_loaded = false;

const int SCREEN_HEIGHT = 160;
const int SCREEN_WIDTH = 128;
const int BUTTON_PIN_1 = 45;
const int BUTTON_PIN_2 = 39;
const int BUTTON_PIN_3 = 38;
const int BUTTON_PIN_4 = 34;

const int LOOP_PERIOD = 200;
const int LOCATION_PERIOD = 30000;

const float Pi = 3.1415926;

MPU6050 imu; // imu object called, appropriately, imu

char network[] = "MIT"; // SSID for 6.08 Lab
char password[] = "";   // Password for 6.08 Lab

char TEAM_SERVER[] = "608dev-2.net"; // host for team server HTTP calls

// Some constants and some resources:
const uint16_t RESPONSE_TIMEOUT = 6000; // time for claiming the HTTP response as TIMEOUT
const uint16_t IN_BUFFER_SIZE = 4500;   // size of buffer to hold HTTP request
const uint16_t OUT_BUFFER_SIZE = 4500;  // size of buffer to hold HTTP response
const uint16_t JSON_BODY_SIZE = 1000;   // size of buffer to hold JSON body
char body[IN_BUFFER_SIZE];
char request[IN_BUFFER_SIZE];
char response[OUT_BUFFER_SIZE]; // char array buffer to hold HTTP request
char json_body[JSON_BODY_SIZE];

uint32_t primary_timer;
uint32_t location_timer;

WiFiClientSecure client; // global WiFiClient Secure object
WiFiClient client2;      // global WiFiClient Secure object

Adafruit_LSM303DLH_Mag_Unified mag = Adafruit_LSM303DLH_Mag_Unified(12345);
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);

//ArduCam parameters
#if !(defined ESP32 )
#error Please select the ArduCAM ESP32 UNO board in the Tools/Board
#endif
//This demo can only work on OV2640_MINI_2MP or ARDUCAM_SHIELD_V2 platform.
#if !(defined (OV2640_MINI_2MP)||defined (OV5640_MINI_5MP_PLUS) || defined (OV5642_MINI_5MP_PLUS) \
    || defined (OV5642_MINI_5MP) || defined (OV5642_MINI_5MP_BIT_ROTATION_FIXED) \
    ||(defined (ARDUCAM_SHIELD_V2) && (defined (OV2640_CAM) || defined (OV5640_CAM) || defined (OV5642_CAM))))
#error Please select the hardware platform and camera module in the ../libraries/ArduCAM/memorysaver.h file
#endif

const int CS = 34;
const int CAM_POWER_ON = 10;
const int KEY = 45;

ArduCAM myCAM(OV2640, CS);

static const size_t bufferSize = 8000;
static uint8_t buffer[bufferSize] = {0xFF};
uint8_t temp = 0, temp_last = 0;
int i = 0;
bool is_header = false;

char holder[bufferSize];
char image_data[8000];
char body[6000];
char user[] = "team_42";

const uint16_t CAM_IN_BUFFER_SIZE = 8000; //size of buffer to hold HTTP request
const uint16_t CAM_OUT_BUFFER_SIZE = 8000; //size of buffer to hold HTTP response
char request_buffer[CAM_IN_BUFFER_SIZE];
char response_buffer[CAM_OUT_BUFFER_SIZE]; //char array buffer to hold HTTP request


inline int sign_db(double x)
{
    return ((x > 0) ? 1 : ((x < 0) ? -1 : 0));
}

inline bool equal_db(double x, double y)
{
    return abs(x - y) < 1e-8;
}

// used to get cw offset from North (positive y-axis) in degrees from (x00, y00) to (x11, y11)
float direction(double x00, double y00, double x11, double y11)
{
    float degree_from_pos_x_ccw = (atan2(y11 - y00, x11 - x00) * 180) / 3.1415926;
    float degree_from_pos_x_cw_0_360 = 360 - degree_from_pos_x_ccw - ((degree_from_pos_x_ccw < 0.0) ? 360 : 0);
    float degree_from_pos_y_cw_0_360 = 90 + degree_from_pos_x_cw_0_360 - ((degree_from_pos_x_cw_0_360 > 270) ? 360 : 0);
    // Serial.println(degree_from_pos_x_ccw);
    // Serial.println(degree_from_pos_x_cw_0_360);
    // Serial.println(degree_from_pos_y_cw_0_360);
    Serial.printf("Location: %f %f %f %f, Degree %f %f %f\n", x00, y00, x11, y11, degree_from_pos_x_ccw, degree_from_pos_x_cw_0_360, degree_from_pos_y_cw_0_360);
    return degree_from_pos_y_cw_0_360;
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
        long_press_duration = 2000;
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
    double cur_lat; // current latitude of ESP32
    double cur_lon; // current longitude of ESP32
    char cur_building_id[10];
    char cur_building_name[20];
    char cur_building_intro[400]; // introduction for current building (too long to store every introduction locally)
    int cur_building_no;          // current building Number (e.g. 7 = this is the seventh building in this route)
    float heading;

    int total_building;
    char building_names[20][20]; // list of building names in trip (e.g. "Stata Center")
    char building_ids[20][10];   // list of building ID in trip (e.g. "32")
    double building_lat[20];     // list of building center point latitudes in trip
    double building_lon[20];     // list of building center point longtitudes in trip
    char building_intros[20][400];

    char route_name[10][20];
    int total_route;
    int route_selection, old_selection;

    beaver_state state, old_state;

    void get_route_list()
    {
        int req_len = 0;
        req_len += sprintf(request + req_len, "GET http://608dev-2.net/sandbox/sc/team42/608_team42_final/get_route_list.py HTTP/1.1\r\n");
        req_len += sprintf(request + req_len, "Host: 608dev-2.net\r\n\r\n");
        do_http_request(TEAM_SERVER, request, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, Serial);

        DynamicJsonDocument doc(1024);
        deserializeJson(doc, response);

        Serial.printf("Finish deserializing JSON for get_route_list\n");

        total_route = doc["num_routes"];
        for (int i = 0; i < total_route; i++)
        {
            const char *tmp_str = doc["name_routes"][i];
            Serial.println(tmp_str);
            sprintf(route_name[i], "%s", tmp_str);
            Serial.println(route_name[i]);
        }
        Serial.printf("Parsed, total routes: %d, first route: %s\n", total_route, route_name[0]);
    }

    void get_route_info()
    {
        int req_len = 0;
        req_len += sprintf(request + req_len, "GET http://608dev-2.net/sandbox/sc/team42/608_team42_final/get_route_info.py?route_id=%d HTTP/1.1\r\n", route_selection + 1);
        req_len += sprintf(request + req_len, "Host: 608dev-2.net\r\n\r\n");
        do_http_request(TEAM_SERVER, request, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, Serial);

        DynamicJsonDocument doc(1024);
        deserializeJson(doc, response);

        Serial.printf("Finish deserializing JSON for get_route_info\n");

        total_building = doc["building_names"].size();
        const char *tmp_str = "AAAA";
        Serial.println("Finish Initializing tmp_str");
        for (int i = 0; i < total_building; i++)
        {
            tmp_str = doc["building_names"][i];
            sprintf(building_names[i], "%s", tmp_str);
            tmp_str = doc["route_building_id"][i];
            sprintf(building_ids[i], "%s", tmp_str);
            building_lat[i] = doc["route_center_coordinates"][i][0];
            building_lon[i] = doc["route_center_coordinates"][i][1];
        }

        Serial.printf("Parsed, total buildings: %d\n", total_building);
    }

    // bool __within_area(double poly[][2], int len)
    // {
    //     Serial.printf("Begin checking area\n");
    //     for (int i = 0; i < len; i++)
    //     {
    //         poly[i][0] = poly[i][0] - cur_lat;
    //         poly[i][1] = poly[i][1] - cur_lon;
    //     }
    //     Serial.printf("Begin checking area each line\n");
    //     int count = 0;
    //     for (int i = 0; i < len - 1; i++)
    //     {
    //         if (equal_db(poly[i][0], poly[i + 1][0]) || sign_db(poly[i][0]) == sign_db(poly[i + 1][0]))
    //             continue;
    //         double p = (poly[i][1] * poly[i + 1][0] - poly[i + 1][1] * poly[i][0]) / (poly[i + 1][0] - poly[i][0]);
    //         Serial.printf("    get k for line %d: %f\n", i, p);
    //         if (sign_db(p) == 1)
    //             count++;
    //     }
    //     Serial.printf("Finish checking area each line\n");
    //     return ((count & 1) == 1);
    // }

    // void get_current_building()
    // {
    //     double building_7[7][2] = {{42.35955031432619, -71.09356300278161}, {42.359661541063744, -71.09324611026356}, {42.35932248846664, -71.09303546063194}, {42.359386677525606, -71.09283690710286}, {42.35919716678156, -71.09271694767907}, {42.35900612714659, -71.09324021895881}, {42.35955031432619, -71.09356300278161}};
    //     double building_3[5][2] = {{42.35938973414583, -71.09283070230508}, {42.35957924430909, -71.0922660657068}, {42.359398904005594, -71.09214610628298}, {42.35923078969722, -71.09272935727462}, {42.35938973414583, -71.09283070230508}};
    //     double building_10[5][2] = {{42.35985892359337, -71.0923922299534}, {42.36000869668367, -71.09195996237447}, {42.35952575360194, -71.09165799554903}, {42.35937597936053, -71.09212956018057}, {42.35985892359337, -71.0923922299534}};
    //     Serial.printf("Finished defining building polygons\n");
    //     if (__within_area(building_7, 7))
    //         sprintf(cur_building_id, "7"), cur_building_no = 0, strcpy(cur_building_intro, building_intros[0]);
    //     else if (__within_area(building_3, 5))
    //         sprintf(cur_building_id, "3"), cur_building_no = 1, strcpy(cur_building_intro, building_intros[1]);
    //     else if (__within_area(building_10, 5))
    //         sprintf(cur_building_id, "10"), cur_building_no = 2, strcpy(cur_building_intro, building_intros[2]);
    //     Serial.printf("Finished checking buildings, cur_building_no = %d, cur_id = %s\n", cur_building_no, cur_building_id);
    //     return;
    // }

    void get_current_building_new()
    {
        int req_len = 0;
        req_len += sprintf(request + req_len, "GET https://608dev-2.net/sandbox/sc/team42/608_team42_final/get_current_building.py?lat=%f&lon=%f HTTP/1.1\r\n", cur_lat, cur_lon);
        req_len += sprintf(request + req_len, "Host: 608dev-2.net\r\n\r\n");
        do_http_request(TEAM_SERVER, request, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, Serial);

        DynamicJsonDocument doc(1024);
        deserializeJson(doc, response);

        Serial.printf("Finish deserializing JSON for get_current_building\n");
        const char *tmp_str = doc["current_building_id"];
        if (strcmp(tmp_str, "-1") == 0)
        {
            Serial.println("Get off campus, not updating buildings");
            return;
        }
        sprintf(cur_building_id, "%s", tmp_str);
        tmp_str = doc["current_building_name"];
        sprintf(cur_building_name, "%s", tmp_str);
        cur_building_no = -1;
        for (int i = 0; i < total_building; i++)
        {
            Serial.printf("    Comparing %s %s %s\n", cur_building_id, building_ids[i], building_names[i]);
            if (strcmp(cur_building_id, building_ids[i]) == 0)
            {
                cur_building_no = i;
                break;
            }
        }
        Serial.printf("Finished checking buildings, cur_building_no = %d, cur_id = %s, cur_name = %s\n", cur_building_no, cur_building_id, cur_building_name);
    }

    void get_building_intro()
    {
        int req_len = 0;
        req_len += sprintf(request + req_len, "GET https://608dev-2.net/sandbox/sc/team42/608_team42_final/get_building_intro.py?building_id=%s&max_len=400 HTTP/1.1\r\n", cur_building_id);
        req_len += sprintf(request + req_len, "Host: 608dev-2.net\r\n\r\n");
        do_http_request(TEAM_SERVER, request, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, Serial);

        DynamicJsonDocument doc(1024);
        deserializeJson(doc, response);

        Serial.printf("Finish deserializing JSON for get_building_intro\n");
        const char *tmp_str = doc["text_intro"];
        sprintf(cur_building_intro, "%s", tmp_str);
    }
    
    void upload_photo(){

        sprintf(body, "location=%s&user_id=orion&image_encoding=%s", cur_building_id, fake_photo_base64);

        int req_len = 0;
        req_len += sprintf(request + req_len, "POST http://608dev-2.net/sandbox/sc/team42/608_team42_final/image_request.py HTTP/1.1\r\n");
        req_len += sprintf(request + req_len, "Host: 608dev-2.net\r\n");
        req_len += sprintf(request + req_len, "Content-Type: application/x-www-form-urlencoded\r\n");
        // req_len += sprintf(request + req_len, "Connection: keep-alive\r\n");
        req_len += sprintf(request + req_len, "Content-Length: %d\r\n\r\n", strlen(body));
        req_len += sprintf(request + req_len, "%s\r\n", body);

        Serial.println("finishing prepping upload photo");

        do_http_request(TEAM_SERVER, request, response, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);

        Serial.println("finished photo uploading");

    }

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
      //        Serial.println(i);
            }
            else
            {
              //Write bufferSize bytes image data to file
              i = 0;
              buffer[i++] = temp;
      //        Serial.println("Reset");
            }
          }
          else if ((temp == 0xD8) && (temp_last == 0xFF))
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
      
        sprintf(body,"{\"location\":\'%s\', \"user_id\":\'%s\', \"image_encoding\":\'%s\'}", cur_building_name, user, image_data);
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
        do_http_request("608dev-2.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, false);
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

public:
    PocketBeaver()
    {
        state = S0;
        old_state = S111;
        cur_lat = 0;
        cur_lon = 0;
        heading = 0;
        // should be getting all possible routes from team server in S00-S01 transition, using fake data right now
        // total_route = 3;
        // sprintf(route_name[0], "Infinite");
        // sprintf(route_name[1], "30s");
        // sprintf(route_name[2], "Dorm St.");
        // initialize route with fake data right now
        // sprintf(building_ids[0], "7");
        // sprintf(building_ids[1], "3");
        // sprintf(building_ids[2], "10");
        // sprintf(building_intros[0], "MIT Building 7, named Rogers Building and home to the iconic Lobby 7, hosts the Department of Architecture and a series of undergraduate-related offices. It also marks the start of the famous Infinite Corridor.");
        // sprintf(building_intros[1], "MIT Building 3, part of the Maclaurin Buildings, hosts the Department of Mechanical Engineering as well as series of graduate-related offices. It is the second 2nd along the Infinite Corridor (from west to east).");
        // sprintf(building_intros[2], "MIT Building 10, part of the Maclaurin Buildings, is the place where the iconic Great Dome resides, as well as being the home to the Barker Engineering Library. It is the 3rd building along the Infinite Corridor (from west to east).");
        // building_lat[0] = 42.359247834274086;
        // building_lat[1] = 0;
        // building_lat[2] = 0;
        // building_lon[0] = -71.09309433468378;
        // building_lon[1] = 0;
        // building_lon[2] = 0;
        route_selection = 0;
    }
    void update(double lat,
                double lon,
                int button1,
                int button2,
                int button3,
                int button4)
    {

        bool location_changed = false, building_changed = false;
        int before_check_building_no = cur_building_no;
        if (!equal_db(lat, 1000) && !equal_db(lon, 1000))
        {
            Serial.printf("Changing lat/lon: %f %f %f %f\n", lat, lon, cur_lat, cur_lon);
            location_changed = true;
            cur_lat = lat;
            cur_lon = lon;
            get_current_building_new();
            building_changed = (cur_building_no != before_check_building_no);
        }

        sensors_event_t mag_event;
        mag.getEvent(&mag_event);
        sensors_event_t accel_event;
        accel.getEvent(&accel_event);

        if (accel_event.acceleration.z < 9.8 && accel_event.acceleration.z > 9.4)
        {
            heading = (atan2(mag_event.magnetic.y, mag_event.magnetic.x) * 180) / Pi;

            // Normalize to 0-360
            if (heading < 0)
                heading = 360 + heading;

            // Adjust clock direction
            heading = 360 - heading;

            float dir_offset = direction(cur_lon, cur_lat, building_lon[cur_building_no + 1], building_lat[cur_building_no + 1]);

            Serial.printf("Offsets: original %f, direction %f, total %f\n", heading, dir_offset, heading + dir_offset + 180);

            // Add direction offset
            heading = heading + dir_offset + 270;
            while (heading >= 360)
                heading = heading - 360;
        }

        switch (state)
        {
        case S0:
            /*
            S0 = Welcome state, user can press any button to access list of routes
            Will trigger get_route_list and corresponding API call to the team server, will go to S01 after that
            */
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
                route_selection = 0;
                old_selection = 0;
                get_route_list();
            }            
            break;

        case S01:
            /*
            S01 = Route selection state, user can use button 2/3 to slide and button 1 to selection
            Will trigger going to S02, which will trigger get_route_info on selected route and corresponding API call to the team server
            */
            if (old_state != state || old_selection != route_selection)
            {
                tft.fillScreen(TFT_BLACK);
                tft.setCursor(0, 0);
                tft.printf("Please choose between the %d routes below, scrolling with Buttons 2 and 3, confirming with 1:\n");
                for (int i = 0; i < total_route; i++)
                {
                    if (i == route_selection)
                        tft.setTextColor(TFT_BLACK, TFT_WHITE);
                    tft.printf("Route %d: %s\n", i, route_name[i]);
                    if (i == route_selection)
                        tft.setTextColor(TFT_GREEN, TFT_BLACK);
                }
            }
            old_state = state;
            old_selection = route_selection;
            if (button1 == 1)
                state = S02;
            else if (button2 == 1)
                route_selection = (route_selection + 1) % total_route;
            else if (button3 == 1)
                route_selection = (route_selection - 1 + total_route) % total_route;
            break;

        case S02:
            /*
            S02 = state for getting route info
            Only used as transitional state to call server
            Will goto S03 after get_route_info returns
            */
            if (old_state != state)
            {
                tft.fillScreen(TFT_BLACK);
                tft.setCursor(0, 0);
                tft.printf("Selected Route %d! Downloading Route Info ...", route_selection);
                sleep(1); // TO BE DEPRECATED: Dangerous!
                get_route_info();
                cur_building_no = -1;
                old_state = state;
                state = S03;
            }
            break;

        case S03:
            /*
            S03: Direction to start state
            Will Show Compass telling me to get to the starting building
            Will goto S11 when user reaches first building
            */
            if (old_state != state || location_changed)
            {
                // should be printing compass here, printing foo direction instead
                Serial.println("Entered S03!");
                tft.fillScreen(TFT_BLACK);
                tft.setTextColor(TFT_RED, TFT_BLACK);
                tft.setTextDatum(MC_DATUM);
                tft.drawString("N", SCREEN_CENTRE_X, SCREEN_CENTRE_Y, 4);
            }

            plotNeedle(heading, 10);

            if (cur_building_no == 0)
            {
                Serial.println("S03 reached starting point!");
                tft.fillScreen(TFT_BLACK);
                tft.setCursor(0, 0);
                tft.printf("Reached starting point!, Trip starting in 5 seconds......");
                get_building_intro();
                sleep(5); // TO BE DEPRECATED
                old_state = state;
                state = S11;
            }
            else
            {
                old_state = state;
            }
            break;

        case S10:
            if (old_state != state)
            {
                tft.fillScreen(TFT_BLACK);
                tft.setCursor(0, 0);
                tft.printf("Reached new point!");
                get_building_intro();
                sleep(5); // TO BE DEPRECATED
                old_state = state;
                state = S11;
            }
            break;

        case S11:
            if (old_state != state) // Need to add "coming back from no internet" condition
            {
                tft.fillScreen(TFT_BLACK);
                tft.setTextColor(TFT_GREEN, TFT_BLACK);
                tft.setTextDatum(TL_DATUM);
                tft.setCursor(0, 0);
                tft.printf("%s", cur_building_intro);
                Serial.println(cur_building_intro);
            }
            if (building_changed)
            {
                old_state = state;
                state = S10;
            }
            else
            {
                if (button2 == 1)
                {
                    old_state = state;
                    state = S12;
                }
                else if (button3 == 1)
                {
                    old_state = state;
                    state = S13;
                }
                else if (button1 == 2){
                    old_state = state;
                    state = S111;
                }
                else
                    old_state = state;
            }
            break;

        case S111:
            if (old_state != state){
                Serial.printf("Entering S111 %d, playing......\n", cur_building_no+1);
                myDFPlayer.play(cur_building_no+1);
                delay(32000);
                old_state = state;
                state = S11;
            }

        case S12:
            if (old_state != state)
            {
                tft.fillScreen(TFT_BLACK);
                tft.setTextColor(TFT_RED, TFT_BLACK);
                tft.setTextDatum(MC_DATUM);
                tft.drawString("N", SCREEN_CENTRE_X, SCREEN_CENTRE_Y, 4);
            }
            tft.setTextColor(TFT_RED, TFT_BLACK);
            tft.setTextDatum(MC_DATUM);
            tft.drawString("N", SCREEN_CENTRE_X, SCREEN_CENTRE_Y, 4);
            plotNeedle(heading, 10);
            if (building_changed)
            {
                old_state = state;
                state = S10;
            }
            else
            {
                if (button2 == 1)
                {
                    old_state = state;
                    state = S11;
                }
                else if (button3 == 1){
                    old_state = state;
                    state = S13;
                }
                else
                    old_state = state;
            }
            break;

        case S13:
            if (old_state != state)
            {
                serverCapture();
                state = old_state;
            }
            break;

            // case S131:
            //     if (old_state != state)
            //     {
            //         server.handleClient();
            //     }

            //     break;

        default:
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(0, 0);
            tft.printf("Error State: %d", state);
            while (1)
                ; // TO BE DEPRECATED: dangerous!
            break;
        }
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
    tft.init();                             // init screen
    tft.setRotation(2);                     // adjust rotation
    tft.setTextSize(1);                     // default font size, change if you want
    tft.fillScreen(TFT_BLACK);              // fill background
    tft.setTextColor(TFT_GREEN, TFT_BLACK); // set color of font to hot pink foreground, black background
    tft.setCursor(0, 0);
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
        Serial.println("Failed to Connect in the first time:/  Going to restart");
        Serial.println(WiFi.status());
        ESP.restart(); // restart the ESP (proper way)
    }
    // if (imu.setupIMU(1))
    // {
    //     Serial.println("IMU Connected!");
    // }
    // else
    // {
    //     Serial.println("IMU Not Connected :/");
    //     Serial.println("Restarting");
    //     ESP.restart(); // restart the ESP (proper way)
    // }

    setup_camera();

//  uint8_t vid, pid;
//  uint8_t temp;
//  //set the CS as an output:
//  pinMode(CS, OUTPUT);
//  pinMode(CAM_POWER_ON , OUTPUT);
//  pinMode(KEY, INPUT_PULLUP);
//  digitalWrite(CAM_POWER_ON, HIGH);
//#if defined(__SAM3X8E__)
//  Wire1.begin();
//#else
//  Wire.begin();
//#endif
////  Serial.begin(115200);
//  Serial.println(F("ArduCAM Start!"));
//
//  // initialize SPI:
//  SPI.begin();
//  SPI.setFrequency(4000000); //8MHz
//
//  //Check if the ArduCAM SPI bus is OK
//  myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
//  temp = myCAM.read_reg(ARDUCHIP_TEST1);
//  if (temp != 0x55) {
//    Serial.println(F("SPI1 interface Error!"));
//    while (1);
//  }
//  
//  //Check if the camera module type is OV2640
//  myCAM.wrSensorReg8_8(0xff, 0x01);
//  myCAM.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
//  myCAM.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
//  if ((vid != 0x26 ) && (( pid != 0x41 ) || ( pid != 0x42 )))
//    Serial.println(F("Can't find OV2640 module!"));
//  else
//    Serial.println(F("OV2640 detected."));
//
//  //Change to JPEG capture mode and initialize the OV2640 module
//  myCAM.set_format(JPEG);
//  myCAM.InitCAM();
//  myCAM.OV2640_set_JPEG_size(OV2640_160x120);
//  myCAM.clear_fifo_flag();

    setup_compass();

    delay(1000);

    mp3_setup();

    primary_timer = millis();
    location_timer = 0;
}

void loop()
{

    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Reconnecting to WiFi...");
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(0, 0);
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.println("Lost Internet Connection, Trying to Reconnect, Cannot Operate While Reconnecting......");
        WiFi.disconnect();
        WiFi.reconnect();
        int count = 0;
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
            tft.fillScreen(TFT_BLACK);
        }
        else
        { // if we failed to connect just Try again.
            Serial.println("Failed to Connect :/  Going to retry");
            Serial.println(WiFi.status());
            return;
        }
    }

    int bv1 = button1.update(); // get button1 value
    int bv2 = button2.update(); // get button2 value
    int bv3 = button3.update(); // get button3 value
    int bv4 = button4.update(); // get button4 value

    double lat = 1000, lon = 1000;
    if (location_timer == 0 || millis() - location_timer > LOCATION_PERIOD)
    {
        get_location(&lat, &lon);
        location_timer = millis();
    }

    pb.update(lat, lon, bv1, bv2, bv3, bv4);

    while (millis() - primary_timer < LOOP_PERIOD)
        ;

    primary_timer = millis();
}
