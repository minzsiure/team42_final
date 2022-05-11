# Spring 2022 6.08 Team 42 Final Project Overview
## [Main website](https://608dev-2.net/sandbox/sc/team42/608_team42_final/show_photo_website.py)
# PocketBeaver: 6.08 Final Project
### Check out our [main website!](https://608dev-2.net/sandbox/sc/team42/608_team42_final/show_photo_website.py)
Given the increasing demand for campus touring at MIT and the shortage of trained student tour guides, our project aims to implement an open-ended, interactive tourist guide to the MIT campus that can come in handy. Whether they are prospective students or just visiting MIT, they can use our application to explore our dynamic campus and experience firsthand how MIT is making a better world. 

Compared to the plain descriptions on Google Maps or official websites, the product we as MIT students develop is going to feature a unique student perspective and incorporate various additional functions to make the tour more enjoyable and show off MIT student hacker spirit.


<iframe width="560" height="315" src="https://www.youtube.com/embed/6vCF7tuCITA" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>


## Table of Contents
#### 1. [Functionality](#functionality)
#### 2. [Documentation](#documentation)
#### 3. [Detailed Code Layout](#detail)
#### 4. [Milestone Contribution](#milestone)
#### 5. [Future Work](#future)
#### 6. [Team Members](#member)

## 1. Functionality <a id="functionality"></a>

### Overview
The main functionalities of our tour guide include touring route recommendations, location detection, and automated display of descriptions for the current location. To achieve this goal, we created a database with locations and introductions of selected buildings and sites on campus. PocketBeaver is able to locate itself using Google Map API and display the relevant information on LCD. In addition to the core features, we integrated a camera into our product to allow users take and upload pictures, a MP3 speaker to play audio introductions, a pressure sensor to detect altitude and locate the specific floor number. Finally, our product support a vibrant touring community with a comments section and a daily step count leaderboard. 

### 1.1 Tour guiding

As its flag functionality, PocketBeaver is able to direct its user across the MIT campus through various routes (which are sequences of locations on campus). To achieve this goal, the PocketBeaver utilizes on-campus WiFi and used Access Point scans to fetch its current geographical coordinates from the Google Geolocation API, and then use this information to determine which location or building the user is currently located at. PocketBeaver is then able to display a text introduction to the building, and there are audio options for accessibility needs. The screen can also show a compass-like navigation to the next building, which will be described in 1.3.

### 1.2 Locating

PocketBeaver is able to successfully locate itself by first sending a request to the Google Geolocation API, which returns the latitude and longitude of its current location. It then identifies whether this location falls into one of the locations on our tour. We hand-picked a list of coordinates of the polygons that encloses specific locations from Google Map and tested them on-site. To determine if a point is inside a polygon, we use the Jordan Curve Theorem which counts the number of edges we cross along the x-axis. 

Our product has the additional functionality to accurately identify its altitude relative to sea-level and its specific floor number inside a building. We use the `weather.visualcrossing.com` API to request the pressure of the closest sea level location, which is the Logan Airport for our case. The altitude can be calculated from the above information. To be able to know which floor we are at, we specifically measured the floor height of the tour locations. PocketBeaver first recognizes the location, calculates the altitude, and then use the corresonding location to determine floor number. *Note that due to time limits and instability of pressure readings on different floors, this is not integrated into the final product but is separately implemented as a milestone*

### 1.3 Navigation

By integrating the LSM303DLHC Magneto-Accelerometer, we are able to determine the direction from the current user coordinates to the center point coordinates of the next location or building. This is achieved by evaluating the magnetometer reading of earth magnetic flux on the two horizontal directions, the ratio between which corresponds with the actual headings (with regard to magnetic North) of the ESP32 machine when the vertical reading is close to zero (i.e. when the device is held flat). A compass-like, fast-refreshing directing needle is available at all times to help the users navigate to the next location by going at the direction the needle points.

### 1.4 Audio Playing

We generated MP3 recordings of tour location introductions and stored it inside a MicroSD card. PocketBeaver is integrated with a DFPlayer Mini MP3 Player, which decodes the MP3 recordings from the microSD card and drives a speaker. Based on the current location, PockerBeaver selects the corresponding audio introductions to play with a integer index. The audio is default to play once but can be repeated by pressing a button. This functionality makes the tour experience more natural and allows the user to look around a location while listening to its introduction. 

### 1.5 Photo Taking

We were able to integrate ArduCam into our hardware system, which enables us to take picture and upload it along with the user id and current location. The ArduCam would output image data in hexidecimal format, which would then be encoded using base 64 encoding and sent to our server using POST request. We would then be able to retrieve the image by decoding the data from the database and display them on our main website, grouped by either user ids or locations.


### 1.6 Step Counting and Leaderboard

PocketBeaver is integrated with a MPU9255 IMU, which returns its acceleration in all three dimensions. We observe the magnitude of the acceleration vector during walking to pick up a peak and trough value to input to our step counter finite state machine, illustrated in Figure.X. With the state machine, we increment step counts as the user walks along the tour and automatically send post requests to our step counter API, which updates the user step counts in the database. We implement a daily step counting leaderboard on our website. 

![](https://i.imgur.com/uHx72QO.png)
<center> Figure. X: Step Counter Finite State Machine Transition Diagram </center>

## 2. Documentation <a id="documentation"></a>

### 2.1 Full System Design
![](https://i.ibb.co/CwFp09K/6-08-System-Block-Diagram.png)

<center> Full System Block Diagram </center>

### 2.2 Hardware Layout
![](https://i.imgur.com/T3XZFr1.jpg)


### 2.3 Parts List
[ArduCam](https://www.amazon.com/Arducam-Module-Megapixels-Arduino-Mega2560/dp/B012UXNDOY/ref=sr_1_4?dchild=1&keywords=arducam&qid=1617653366&sr=8-4) 
* Price: $25.99
* Use Case: To take pictures and upload them in real time for user to view their pictures based on location or user id.

[MP3 Player](https://www.amazon.com/Anmbest-YX5200-DFPlayer-Supporting-Arduino/dp/B07JGWMPTF/)
* Price: $2.6
* Use Case: To play audio introductions in real time as user visits locations on the tour.
 
[MicroSD Card](https://www.amazon.com/Gigastone-10-Pack-Surveillance-Security-Action/dp/B0876H387X/ref=sr_1_4?dchild=1&keywords=micro+sd+card+8gb&qid=1617653751&sr=8-4)
* Price: $3.5
* Use case: To store audio introductions of tour locations.

[Magneto-Accelerometer LSM303DLHC](https://www.pololu.com/product/2124)
* Price: $10
* Use case: To detect acceleration and earth magnetic flux, calculating direction to next building, and counting steps.

### 2.4 Design Challenges and Decisions

#### 2.4.1 Picture Uploads
We were able to achieve this by using base 64 encoding to encode the image received by ArduCam. The database would receive the encoding of the picture along with the current location and user id, and the API would then display the image with base 64 decoding. One difficulty we encountered is that the ArduCam is not entirely compatible with TFT. We resolved this issue by setting SPI frequency to 4,000,000 instead of the default value.

#### 2.4.2 Pressure Measurement Calibration for Altitude Determination
The pressure reading from the BMP280 was not able to provide an accurate estimation of the altitude. To cope with hardward limitations, we decide to calibrate our measurements manually. When we arrive at a new location, we will press a button on PocketBeaver, which will calculate the current altitude and use that as an offset for future measuresments. If we get -15.00m on the first floor of Hayden Library, we will always add 15.00m to the measurements there. This saves us from storing different offsets for different locations but automizing the process. 

#### 2.4.3 Data Processing for Website
Building a dynamic website that interacts with databases is mostly beyond the scope of the class. It was quite a challenge to decide what tech stacks we may use to have a fully functioning website that fetches photos from the detabase, display it in a form of cards in correspondence to users/locations, and fetch and display in a descending order of the step counts per user. We were able to accomplish all the described functionalities with only html, css, and python script. The essense of our design idea is to use python to interact with the server, process the fetched data, and format them into html code. We then "glued" all html code together using a single python string, and return this string in the request handler. In addtion, we had a css file to polish our website. We included the import statement of css in our python string as well so it took effect.

## 3. Detailed Code Layout <a id="detail"></a>
### 3.1 ESP32 Side

![](https://i.ibb.co/nQSDC0f/6-08-Final-Project-State-Machine-Diagram.png)
<center> Overview of System State Machine Design </center>

The ESP32 code base is stored in ```/src``` folder, and is divided into one main component ```/src/src.ino``` and several separate functionality components. This part will introduce them one by one.

#### 3.1.1 ESP32 Main State Machine Code (src.ino)

The main code component, src.ino, is a 1000+ line Arduino script including four major implementations:

1. Definition of I/O pins, classes, and events;
2. Button state enums and ```Button``` class;
3. ```PocketBeaver``` class which **implements the state machine diagram above**, as well as its state enums;
4. ```setup()``` and ```loop()``` logics.

The first part definitions are put in this src.ino file so that all other ```.ino``` files can access these high-level instances and events. One example is the ```Adafruit_LSM303_Accel_Unified``` instance which can be used to initialize events returning accelerometer readings. This instance is used in ```src.ino```, ```compass.ino```, as well as ```step.ino```.

The ```Button``` class is derived from earlier class exercises, implementing a button class that is able to debounce and distinguish between short and long presses. Three instances of this class are initiated, corresponding to the three controlling buttons on the ESP32 hardware.

The ```PocketBeaver``` class is a large class that defines and implements the main state machine diagram. It first defines:

1. Private instance variables that store the information for a **downloaded route**;
2. Private instance variables that store the information for the **current location** and **user interactions**;
3. Private methods to interact with the **team server** or **external APIs** to fetch information about the physical and interactive environment the ESP32 machine is in; these methods include:
    a. ```get_route_list()``` which is used to get a list of available touring routes from the team server;
    b. ```get_route_info()``` which is used to get a list of building names, building ids (like "W20" for "Stratton Student Center"), and building centerpoint locations (for compass direction);
    c. ```get_current_building_new()``` which is used to get the current building that the ESP32 is in given locations returned from the Google Geolocation API;
    d. ```get_building_intro()``` which is used to get the current building text introductions;
    e. ```upload_photo()``` which is used to upload a photo just taken by the ArduCam into the team server by a POST request in Base 64 encoding;
    f. ```start_capture()``` and ```server_capture()``` which is used to capture a photo using the ArduCam;
4. A main state machine in the public ```update()``` function that reads and write variables, calls methods under certain circumstances or periodically, as well as makes transitions according to the above diagram. *As a note, the stretch-goal states S03, S04, S14, S15, and S151 are not implemented due to time limits.*

#### 3.1.2 HTTP(S) Request Helper Component (http_s_helper.ino)

This file is directly imported from earlier design exercises, in which the teaching team provided a sound implementation of HTTP and HTTPS requests. A CA certificate for accessing the google geolocation API is also included at the top.

#### 3.1.3 WiFi-based Locator Component (wifi_locator.ino)

This file is implemented based on earlier labs. It scans the available WiFi Access Points nearby, and uploads this list by a POST request to Google's Wifi-Based Geolocation API. It then parses the response from Google to identify the coordinates (latitude and longtitude) the ESP32 machine is currently at. The main function ```get_location``` will be called in the state machine every 30 seconds to update the location. It will also filter out results that are erroreous.

#### 3.1.4 Compass Component (compass.ino)

This file implements a magnetic compass identifying the direction of north with regard to the bottom of the ESP32 machine based on magnetometer readings, while also creating and managing a "sprite" copy of a pixel-paint needle object pointing towards the next building.

The direction of north is calculated by evaluating the magnetometer reading of earth magnetic flux on the X and Y direction. Since that the projective correlation between the ratio of X/Y readings only correspond with the actual headings of the machine when the Z reading is close to zero (i.e. when the Z-axis acceleration is close to gravity and the device is flat), the machine will require the user to put the device flat so that the headings will update.

A number of offsets, including the offset from the current screen direction to North, as well as the offset from North to the vector pointing from the user's current location to the next building's center point is then calculated using these magnetometer readings.

After calculating the direction for the next building, the state machine will call the ```plot_needle(angle, ms_delay)``` function to move the needle object to the given ```angle``` in a speed of 1 degree per ```ms_delay``` milliseconds. The needle object is set up in the ```setup_compass()``` function called in the ```setup()``` of the main file. For more references about the "sprite" functionality, [check this hyperlink](https://github.com/Bodmer/TFT_eSPI#sprites).

#### 3.1.5 MP3 Player Component (mp3.ino)

In this file, the ```mp3_setup()``` function sets up the essential software serial ports and I/O functionalities to control the MP3 player. Controls of the MP3 player is included in the state ```S111``` in ```PocketBeaver.update()``` in ```src.ino```.

#### 3.1.6 Camera Helper Component (camera.ino)

In this file, the ```setup_camera()``` function sets up the ArduCam camera's hardware connections and I/O, and is called once in ```setup()``` in ```src.ino```. The helper function ```base64_encode``` implements the Base 64 encoding of a pixel sequence into Base64 results ready to be uploaded into and parsed by the team server scripts.

#### 3.1.7 Step Counter FSM Component (step.ino)

This file implements a separate state machine implementing a step counter derived from the design in an earlier lab. The ```step_reporter_fsm``` function implements a state machine that is able to transition between the user at rest, walking, and being ready to upload to the server of the user's steps. The ```update_step_count()``` function wraps calculations of the acceleration readings from the LSM303 sensor and calls the ```step_reporter_fsm()``` to update the state machine. 

### 3.2 Server Side
                    
### 3.2.1 Database Structure

#### building_data.db
| Variable Name  | Data Type | Description|
| :------------ |:---------------:| -----|
| `building_name`      | string| name of the building |
| `building_id`      | string| id of the building |
| `coordinates`    | list        |   polygon coordinates for the building |
| `text_intro` | string        |  text introduction of the building |
| `audio_data` | list       |  audio version of the introduction |

#### user_image.db

| Variable Name  | Data Type | Description|
| :------------ |:---------------:| -----|
| `location_name`      | string| name of the building |
| `user_id`    | int       |   id of the user who uploaded the image |
| `image_encoding` | string        |  the base64 encoding of the uploaded image |

#### route_data.db
| Variable Name  | Data Type | Description|
| :------------ |:---------------:| -----|
| `route_name` | string| name of the route |
| `route_id` | int| id of the route |
| `route_building_name`    | list       |   buildings in the route listed in order |
| `route_building_id` | list        |  ids of the buildings in the route listed in order |
| `route_center_coordinates` | list        |  center coordinates of the buildings in the route listed in order |

#### step_data.db
| Variable Name  | Data Type | Description|
| :------------ |:---------------:| -----|
| `username` | string| name of the user |
| `date` | datetime| date |
| `step`    | int       |   number of steps recorded during a day |

### 3.2.2 API Request Handlers
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
| `GET` | /create_step_db.py?view={view_index}&username={user_id}&date={datetime} | This endpoint returns step data. When view = 1, return all users and all dates; view = 2, return all users during a specific date; view = 3, return data for specific user for all dates; view = 4, specific user and specific date. |
| `POST` | /create_step_db.py?username={user_id}&step={step} | This endpoint updates step counter database. If user does not exist or user does not have at current date, create new row with user_id and current date. Otherwise, update existing row by incrementing steps.|

### 3.2.3 Website ([show_photo_website.py](https://608dev-2.net/sandbox/sc/team42/608_team42_final/show_photo_website.py))
We were able to utilize the server for GET/POST requests coming from the ESP32 in various states. We wrote various scripts that would allow us to perform tasks like POSTing step counter data photo data from the functionalities integrated on ESP32, retrieving recent entries. Apart from this, we also made a website with adjustable display mode for both PC and phone to accompany our project that featured 4 main secitons:
* **About:** introduce what PocketBeaver is
* **Avaliable users:** a pile of cards of all avaliable users, and each card display photos associated to the corresponding users
* **Avaliable locations:** same as above, but location-specific.
* **Leaderboard:** display a historical stepcount leaderboard by username, step record, and data, ranked from the highest step counts to the lowest.


## 4. Milestone Contribution <a id="milestone"></a>
Here are the weekly milestones seperated into 4 weeks based on how much they contributed to a particular functionality of the system.

### 4.1 Week 1

<iframe width="560" height="315" src="https://www.youtube.com/embed/velMeBtP_0Q" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

*Orion: Basic working state machine that can get location and display hard-wired introductions to buildings based on Google Geolocation API.*


<iframe width="560" height="315" src="https://www.youtube.com/embed/BzF2I-ZFXmY" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

*Eva: Server side script that handles getting location and text introduction based on location.*


<iframe width="560" height="315" src="https://www.youtube.com/embed/yfl0cxAl-Vw" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

*Yan: Demonstrate how to store and query the sample image using location or user id*

<iframe width="560" height="315" src="https://www.youtube.com/embed/JHeFi_Do_IQ" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

*Pearl: Demonstrate that PocketBeaver is able to locate itself on selected locations of the tour*

<iframe width="560" height="315" src="https://www.youtube.com/embed/a0L2tP6h1nI" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

*Pearl: Demonstrate that PocketBeaver can detect relative altitudes on different floors at the Hayden Library*



### 4.2 Week 2

<iframe width="560" height="315" src="https://www.youtube.com/embed/wEfw3RQ63js" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

*Orion: MVP with ability to direct the user with compass and detect the building the user is in, all based on data fetched from the team server*

<iframe width="560" height="315" src="https://www.youtube.com/embed/cYviOVpOFm4" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

*Eva: Major API endpoints for processing route and locations.*

<iframe width="560" height="315" src="https://www.youtube.com/embed/2vFxk0UM9ys" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

*Yan: Show that the ArduCam could be integrated into hardware system by displaying pictures in the web server*

<iframe width="560" height="315" src="https://www.youtube.com/embed/IuChZ1v8GYU" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

*Pearl: Show that PocketBeaver can detect the current floor number at selected locations*

<iframe width="560" height="315" src="https://www.youtube.com/embed/NWT-N-4ALkE" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

*Pearl: Show that PocketBeaver play the correct audio introductions at different locations*


### 4.3 Week 3

<iframe width="560" height="315" src="https://www.youtube.com/embed/Xs7AFuQZgt0" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

*Orion: Enhanced MVP with photo uploading and MP3 introduction playing integrated*

<iframe width="560" height="315" src="https://www.youtube.com/embed/iIxEcPqLo4M" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

*Eva: Dynamic website for displaying pictures taken by Arducam*

<iframe width="560" height="315" src="https://www.youtube.com/embed/7V8CWWDQYhw" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

*Yan: Show that the image taken by the camera can be uploaded into the database and retrieved from the website*


### 4.4 Week 4
<iframe width="560" height="315" src="https://www.youtube.com/embed/jbDTaUVIyxw" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

*Eva: Complete website with step count learderboard*

<iframe width="560" height="315" src="https://www.youtube.com/embed/W5YQ1p-3WuQ" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

*Yan: Show picture can be taken and uploaded in our final product*

<iframe width="560" height="315" src="https://www.youtube.com/embed/JuyrPCaGkog" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

*Pearl: Show PocketBeaver can keep track of step counts, automatically upload to database, and show on website*

## 5. Future Work <a id="future"></a>
After completing a integrated product from ground up sucessfully, there are still two aspects we identified that we would like to improve further in the future. 

First is to improve the stability of network connections. When designing and testing our tour route, we found that the number of networks avaliable differed a lot depending on the locations. For example, Media Lab has over 200 WiFi's. This overwelmingly large amount of network might have confused Google Location API and thus we had difficulties locating ourselves in the Media Lab.

Thus, it follows naturally that we want to improve our https request function. There was a connection issue that we had difficulties connecting to the host in the Media Lab, and that might resulted from the instable WiFi. 

Second, the current TFT screen is apparently not very capable for the high-refresh-rate sprite movements we are asserting on it right now. There is a need to investigate deeper into the white noise appearing around the needles.

## 6. Team Members <a id="member"></a>
* Orion Li
* Pearl Li
* Yan Wu
* Eva Yi XieGiven the increasing demand for campus touring at MIT and the shortage of trained student tour guides, our project aims to implement an open-ended, interactive tourist guide to the MIT campus that can come in handy. Whether they are prospective students or just visiting MIT, they can use our application to explore our dynamic campus and experience firsthand how MIT is making a better world.

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
| `user_id`    | int       |   id of the user who uploaded the image |
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
| `GET` | /create_step_db.py?view={view_index}&username={user_id}&date={datetime} | This endpoint returns step data. When view = 1, return all users and all dates; view = 2, return all users during a specific date; view = 3, return data for specific user for all dates; view = 4, specific user and specific date. |
| `POST` | /create_step_db.py?username={user_id}&step={step} | This endpoint updates step counter database. If user does not exist or user does not have at current date, create new row with user_id and current date. Otherwise, update existing row by incrementing steps.|

