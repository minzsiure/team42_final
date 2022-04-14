# Spring 2022 6.08 Team 42 Final Project Overview
Given the increasing demand for campus touring at MIT and the shortage of trained student tour guides, our project aims to implement an open-ended, interactive tourist guide to the MIT campus that can come in handy. Whether they are prospective students or just visiting MIT, they can use our application to explore our dynamic campus and experience firsthand how MIT is making a better world.

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
| `text_intro` | string        |  text introduction of the location |
| `image_url` | list      |    a list of image url's of the location |
| `audio_data` | list       |  audio version of the introduction |

### user_image.db

| Variable Name  | Data Type | Description|
| :------------ |:---------------:| -----|
| `location_name`      | string| name of the location |
| `user`    | int       |   id of the user who uploaded the image |
| `image_url` | string        |  the url of the uploaded image |
