import requests
import sqlite3
import json
ht_db = '/var/jail/home/team42/608_team42_final/building_data.db'


def within_area(point_coord, poly):
    new_poly = [(x - point_coord[0], y - point_coord[1]) for x, y in poly]
    total_cross = 0
    for i in range(len(poly)):
        x1, y1 = new_poly[i]
        if i == len(poly) - 1:
            x2, y2 = new_poly[0]
        else:
            x2, y2 = new_poly[i + 1]
        if sign(y1) != sign(y2):
            if sign(x1) == 1 and sign(x2) == 1:
                total_cross += 1
            elif sign(x1) != sign(x2):
                intersect = (x1 * y2 - y1 * x2) / (y2 - y1)
                if sign(intersect) == 1:
                    total_cross += 1
    if total_cross % 2 == 1:
        return True
    return False


def bounding_box(point_coord, box):
    x = [i[0] for i in box]
    y = [i[1] for i in box]
    min_x = min(x)
    min_y = min(y)
    max_x = max(x)
    max_y = max(y)
    if point_coord[0] >= min_x and point_coord[0] <= max_x and point_coord[1] >= min_y and point_coord[1] <= max_y:
        return True
    return False


def sign(x):
    if x > 0:
        return 1
    elif x == 0:
        return 0
    else:
        return -1


def get_area(point_coord, locations):
    for k, v in locations.items():
        if within_area(point_coord, v):
            return k
    return "Off Campus"


def request_handler(request):
    if 'lat' not in request['values'].keys() or 'lon' not in request['values'].keys():
        return "You must enter an lat and lon value."
    try:
        lat = float(request['values']['lat'])
        lon = float(request['values']['lon'])
    except ValueError:
        return "Both lat and lon must be valid floats."

    with sqlite3.connect(ht_db) as c:
        c.execute(
            """CREATE TABLE IF NOT EXISTS building_table (building_name text, building_id text, coordinates text, text_intro text, audio_data text);""")
        raw_info = c.execute(
            '''SELECT DISTINCT * FROM building_table;''').fetchall()

    # create a dictionary `location` with keys being location_name and values being a list of coordinates

    buildings = {}
    ids = {}
    for loc in raw_info:
        buildings[loc[0]] = eval(loc[2])
        ids[loc[0]] = loc[1]

    result = {}
    building_name = get_area((lat, lon), buildings)
    result["current_building_name"] = building_name
    if building_name == "Off Campus":
        result["current_building_id"] = "-1"
    else:
        result["current_building_id"] = ids[building_name]
    return json.dumps(result)
