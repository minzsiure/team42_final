# Spring 2022 6.08 Team 42 Final Project Overview
Given the increasing demand for campus touring at MIT and the shortage of trained student tour guides, our project aims to implement an open-ended, interactive tourist guide to the MIT campus that can come in handy. Whether they are prospective students or just visiting MIT, they can use our application to explore our dynamic campus and experience firsthand how MIT is making a better world.

## ESP32 Functionality Description

All code for the ESP32 is stored in folder ```src/```, consisting of files:
- ```src.ino```, the base file including the state machine class and button class
- ```http_s_helper.ino```, the helper file containing HTTP and HTTPS requests for communicating with team server and outside servers
- ```wifi_locator.ino```, the helper file containing functionalities that returns the ESP32's location based on Wifi APs around it and Google's geolocation API
                    
## Database Structure

### building_data.db
| Variable Name  | Data Type | Description|
| :------------ |:---------------:| -----|
| `building_name`      | string| name of the building |
| `building_id`      | string| id of the building |
| `coordinates`    | list        |   polygon coordinates for the building |
| `text_intro` | string        |  text introduction of the building |
| `audio_data` | list       |  audio version of the introduction |

### user_image.db

| Variable Name  | Data Type | Description|
| :------------ |:---------------:| -----|
| `building_name`      | string| name of the building |
| `user`    | int       |   id of the user who uploaded the image |
| `image_url` | string        |  the url of the uploaded image |

### route_data.db
| Variable Name  | Data Type | Description|
| :------------ |:---------------:| -----|
| `route_name` | string| name of the route |
| `route_id` | int| id of the route |
| `route_building_name`    | list       |   buildings in the route listed in order |
| `route_building_id` | list        |  ids of the buildings in the route listed in order |
