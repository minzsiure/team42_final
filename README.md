# team42_final
Spring 2022 6.08 final project 

## ESP32 Functionality Description

All code for the ESP32 is stored in folder ```src/```, consisting of files:
- ```src.ino```, the base file including the state machine class and button class
- ```http_s_helper.ino```, the helper file containing HTTP and HTTPS requests for communicating with team server and outside servers
- ```wifi_locator.ino```, the helper file containing functionalities that returns the ESP32's location based on Wifi APs around it and Google's geolocation API
                    
## Database Structure
                    

| Variable Name  | Data Type | Description|
| :------------ |:---------------:| -----|
| `location_name`      | string| name of the location |
| `coordinates`    | list        |   polygon coordinates for the location |
| `text_intro` | list        |  text introduction of the location |
| `image_url` | list      |    a list of image url's of the location |
| `audio_data` | list       |  audio version of the introduction |
