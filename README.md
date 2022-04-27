# Spring 2022 6.08 Team 42 Final Project Overview
## [Main website](https://608dev-2.net/sandbox/sc/team42/608_team42_final/show_photo_website.py)

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
| `location_name`      | string| name of the building |
| `user`    | int       |   id of the user who uploaded the image |
| `image_encoding` | string        |  the base64 encoding of the uploaded image |

### route_data.db
| Variable Name  | Data Type | Description|
| :------------ |:---------------:| -----|
| `route_name` | string| name of the route |
| `route_id` | int| id of the route |
| `route_building_name`    | list       |   buildings in the route listed in order |
| `route_building_id` | list        |  ids of the buildings in the route listed in order |
| `route_center_coordinates` | list        |  center coordinates of the buildings in the route listed in order |

### step_data.db
| Variable Name  | Data Type | Description|
| :------------ |:---------------:| -----|
| `username` | string| name of the user |
| `date` | datetime| date |
| `step`    | int       |   number of steps recorded during a day |

## API Request Handlers
| HTTP Verb  | Endpoint | Description|
| :------------ |:---------------:| -----|
| `GET` | /get_route_list.py| This endpoint returns number of routes avaliable and `list` the name of each route as a JSON object with parameters `num_routes` and `name_routes``. |
| `GET` | /get_route_info.py?route_id={id}| This endpoint takes in a route id and return 1. a `list` of buildings in the selected route and 2. a `list` of center coordinates of the buildings in the corresponding route order, and 3. a `list` of building IDs in the selected route as a JSON object with parameters `building_names`, `route_center_coordinates`, and `route_building_id`. |
| `GET` | /get_current_building.py?lat={latitude}&lon={longitude}| This endpoint takes in a latitute value and longitude value, and returns 1.`string` current building name where the user is located in, and 2. `string` corresponding current building id as a JSON object with parameters `current_building_name` and `current_building_id` . |
| `GET` | /get_building_intro.py?building_id={id}&maxlen={number}| This endpoint takes in a building id and a maxlen parameter of the maximum words (in full sentenses) we want to display and return the correpsonding `string` text introduction of the building as a JSON object with parameter `text_intro`. |
| `POST` | /image_request.py | The esp32 could post images in the form of base64 encoding under parameter `image_encoding` with `location` and `user_id`. |
| `GET` | /image_request.py | This endpoint returns all the images stored in the database along with the location and user_id. |
| `GET` | /image_request.py?location={location_name} | This endpoint returns the image filtered based on location name. |
| `GET` | /image_request.py?user_id={user_id} | This endpoint returns the image filtered based on user_id. |
| `GET` | /image_request.py?location={location_name}&user_id={user_id} | This endpoint returns the image filtered based on location name and user id. |

